// Copyright 2021 CatchChallenger
#include "SceneManager.hpp"

#include "../../qt/GameLoader.hpp"
#ifdef CATCHCHALLENGER_SOLO
  #include <QStandardPaths>

  #include "../../qt/Settings.hpp"
  #include "../../qt/solo/InternalServer.hpp"
#endif
#include "../../../general/base/CommonSettingsCommon.hpp"
#include "../../../general/base/Version.hpp"
#include "../ConnexionManager.hpp"
#include "GraphicRenderer.hpp"
#include "Scene.hpp"
#ifndef CATCHCHALLENGER_NOAUDIO
  #include "../AudioGL.hpp"
#endif
#include <QGLWidget>
#include <iostream>

#ifdef Q_OS_ANDROID
  #include <QtAndroidExtras>

void keep_screen_on(bool on) {
  QtAndroid::runOnAndroidThread([on] {
    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (activity.isValid()) {
      QAndroidJniObject window =
          activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

      if (window.isValid()) {
        const int FLAG_KEEP_SCREEN_ON = 128;
        if (on) {
          window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
        } else {
          window.callMethod<void>("clearFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
        }
      }
    }
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
      env->ExceptionClear();
    }
  });
}
#endif

SceneManager::SceneManager() : graphic_scene_(new QGraphicsScene(this)) {
  {
    QGLWidget *context = new QGLWidget(QGLFormat(QGL::SampleBuffers));
    // if OpenGL is present, use it
    if (context->isValid()) {
      setViewport(context);
      setRenderHint(QPainter::Antialiasing, true);
      setRenderHint(QPainter::TextAntialiasing, true);
      setRenderHint(QPainter::HighQualityAntialiasing, true);
      setRenderHint(QPainter::SmoothPixmapTransform, false);
      setRenderHint(QPainter::NonCosmeticDefaultPen, true);
    }
    // else use the CPU only
  }
  connection_manager_ = nullptr;
#ifdef CATCHCHALLENGER_SOLO
  internal_server_ = nullptr;
#endif
#ifndef NOTHREADS
  thread_solo_ = new QThread(this);
#endif

  frame_counter_ = 0;
  time_elapsed_.restart();
  wait_render_time_ = 40;
  timer_render_.setSingleShot(true);
  if (!connect(&timer_render_, &QTimer::timeout, this, &SceneManager::Render))
    abort();

  setScene(graphic_scene_);
  graphic_scene_->setSceneRect(QRectF(0, 0, width(), height()));
  setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing |
                       QGraphicsView::DontSavePainterState |
                       QGraphicsView::IndirectPainting);
  setBackgroundBrush(Qt::black);
  setFrameStyle(0);
  viewport()->setAttribute(Qt::WA_StaticContents);
  viewport()->setAttribute(Qt::WA_TranslucentBackground);
  viewport()->setAttribute(Qt::WA_NoSystemBackground);
  setViewportUpdateMode(QGraphicsView::NoViewportUpdate);

#ifdef Q_OS_ANDROID
  keep_screen_on(true);
#endif

  renderer_ = new GraphicRenderer();
  graphic_scene_->addItem(renderer_);

  Render();
}

SceneManager::~SceneManager() {
#ifndef NOTHREADS
  thread_solo_->exit();
  thread_solo_->wait();
  thread_solo_->deleteLater();
#endif
}

void SceneManager::resizeEvent(QResizeEvent *event) {
  graphic_scene_->setSceneRect(QRectF(0, 0, width(), height()));
  (void)event;
}

void SceneManager::mousePressEvent(QMouseEvent *event) {
  bool call_parent_class = false;
  const QPointF &p = mapToScene(event->pos());
  bool temp = false;

  renderer_->MousePressEventXY(p, temp);

  if (!temp || call_parent_class) QGraphicsView::mousePressEvent(event);
}

void SceneManager::mouseReleaseEvent(QMouseEvent *event) {
  bool call_parent_class = false;
  const QPointF &p = mapToScene(event->pos());
  bool press_validated = false;

  renderer_->MouseReleaseEventXY(p, press_validated);

  if (!press_validated || call_parent_class)
    QGraphicsView::mouseReleaseEvent(event);
}

void SceneManager::mouseMoveEvent(QMouseEvent *event) {
  const QPointF &p = mapToScene(event->pos());
  bool press_validated = false;

  renderer_->MouseMoveEventXY(p);

  if (!press_validated) QGraphicsView::mouseMoveEvent(event);
}

void SceneManager::closeEvent(QCloseEvent *event) {
#ifdef CATCHCHALLENGER_SOLO
  if (internal_server_ != nullptr) {
    hide();
    if (connection_manager_ != nullptr)
      if (connection_manager_->client != nullptr)
        connection_manager_->client->disconnectFromHost();
    /// internalServer->stop();
    event->accept();
    return;
  }
#endif
  QWidget::closeEvent(event);
  QCoreApplication::quit();
}

void SceneManager::Render() {
  graphic_scene_->setSceneRect(QRectF(0, 0, width(), height()));
  viewport()->update();
}

void SceneManager::paintEvent(QPaintEvent *event) {
  graphic_scene_->setSceneRect(QRectF(0, 0, width(), height()));
  time_elapsed_.restart();

  QGraphicsView::paintEvent(event);

  uint32_t elapsed = time_elapsed_.elapsed();
  if (wait_render_time_ <= elapsed)
    timer_render_.start(1);
  else
    timer_render_.start(wait_render_time_ - elapsed);

  if (frame_counter_ < 65535) frame_counter_++;
}

void SceneManager::keyPressEvent(QKeyEvent *event) {
  bool event_trigger = false;
  event->setAccepted(false);
  renderer_->KeyPressEvent(event, event_trigger);
  if (event_trigger) {
    QGraphicsView::keyPressEvent(event);
    return;
  }
  if (!event->isAccepted()) QGraphicsView::keyPressEvent(event);
}

void SceneManager::keyReleaseEvent(QKeyEvent *event) {
  bool event_trigger = false;
  event->setAccepted(false);
  renderer_->KeyReleaseEvent(event, event_trigger);
  if (event_trigger) {
    QGraphicsView::keyPressEvent(event);
    return;
  }
  if (!event->isAccepted()) QGraphicsView::keyReleaseEvent(event);
}

void SceneManager::PushScene(Scene *scene) {
  scene_stack_.push_back(scene);
  renderer_->SetScene(scene);
}

Scene *SceneManager::PopScene() {
  Scene *tmp = scene_stack_.back();
  scene_stack_.pop_back();
  renderer_->SetScene(scene_stack_.back());
  return tmp;
}
