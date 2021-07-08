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
#include "ActionManager.hpp"
#include "EventManager.hpp"
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
  draw_times_ = 0;
  action_manager_ = ActionManager::GetInstance();
  event_manager_ = EventManager::GetInstance();
  connection_manager_ = nullptr;
#ifdef CATCHCHALLENGER_SOLO
  internal_server_ = nullptr;
#endif
#ifndef NOTHREADS
  thread_solo_ = new QThread(this);
#endif

  timerUpdateFPS.setSingleShot(true);
  timerUpdateFPS.setInterval(1000);
  timeUpdateFPS.restart();
  frame_counter_ = 0;
  time_elapsed_.restart();
  wait_render_time_ = 40;
  timer_render_.setSingleShot(true);
  if (!connect(&timer_render_, &QTimer::timeout, this, &SceneManager::Render))
    abort();
  if (!connect(&timerUpdateFPS, &QTimer::timeout, this,
               &SceneManager::UpdateFPS))
    abort();
  timerUpdateFPS.start();

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

  imageText = new QGraphicsPixmapItem();
  graphic_scene_->addItem(imageText);
  imageText->setPos(0, 0);
  imageText->setZValue(999);

  Render();

  SetTargetFPS(100);
}

SceneManager::~SceneManager() {
#ifndef NOTHREADS
  thread_solo_->exit();
  thread_solo_->wait();
  thread_solo_->deleteLater();
#endif
  delete action_manager_;
  delete event_manager_;
}

void SceneManager::resizeEvent(QResizeEvent *event) {
  graphic_scene_->setSceneRect(QRectF(0, 0, width(), height()));
  (void)event;
}

void SceneManager::mousePressEvent(QMouseEvent *event) {
  bool call_parent_class = false;
  const QPointF &p = mapToScene(event->pos());
  bool temp = false;

  event_manager_->MousePressEvent(p, temp);

  if (!temp || call_parent_class) QGraphicsView::mousePressEvent(event);
}

void SceneManager::mouseReleaseEvent(QMouseEvent *event) {
  bool call_parent_class = false;
  const QPointF &p = mapToScene(event->pos());
  bool press_validated = false;

  event_manager_->MouseReleaseEvent(p, press_validated);

  if (!press_validated || call_parent_class)
    QGraphicsView::mouseReleaseEvent(event);
}

void SceneManager::mouseMoveEvent(QMouseEvent *event) {
  const QPointF &p = mapToScene(event->pos());
  bool press_validated = false;

  event_manager_->MouseMoveEvent(p);

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
  draw_times_++;
  graphic_scene_->setSceneRect(QRectF(0, 0, width(), height()));
  viewport()->update();
}

void SceneManager::paintEvent(QPaintEvent *event) {
  action_manager_->Update();
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
  event_manager_->KeyPressEvent(event, event_trigger);
  if (event_trigger) {
    QGraphicsView::keyPressEvent(event);
    return;
  }
  if (!event->isAccepted()) QGraphicsView::keyPressEvent(event);
}

void SceneManager::keyReleaseEvent(QKeyEvent *event) {
  bool event_trigger = false;
  event->setAccepted(false);
  event_manager_->KeyReleaseEvent(event, event_trigger);
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

void SceneManager::UpdateFPS() {
  draw_times_ = 0;
  const unsigned int FPS =
      (int)(((float)frame_counter_) * 1000) / timeUpdateFPS.elapsed();
  QImage pix(50, 40, QImage::Format_ARGB32_Premultiplied);
  pix.fill(Qt::transparent);
  QPainter p(&pix);
  p.setFont(QFont("Times", 12, QFont::Bold));

  const int offset = 1;
  p.setPen(QPen(Qt::black));
  p.drawText(pix.rect().x() + offset, pix.rect().y() + offset,
             pix.rect().width(), pix.rect().height(), Qt::AlignCenter,
             QString::number(FPS));
  p.drawText(pix.rect().x() - offset, pix.rect().y() + offset,
             pix.rect().width(), pix.rect().height(), Qt::AlignCenter,
             QString::number(FPS));
  p.drawText(pix.rect().x() - offset, pix.rect().y() - offset,
             pix.rect().width(), pix.rect().height(), Qt::AlignCenter,
             QString::number(FPS));
  p.drawText(pix.rect().x() + offset, pix.rect().y() - offset,
             pix.rect().width(), pix.rect().height(), Qt::AlignCenter,
             QString::number(FPS));

  p.setPen(QPen(Qt::white));
  p.drawText(pix.rect(), Qt::AlignCenter, QString::number(FPS));
  imageText->setPixmap(QPixmap::fromImage(pix));

  frame_counter_ = 0;
  timeUpdateFPS.restart();
  timerUpdateFPS.start();
}

void SceneManager::SetTargetFPS(int targetFPS) {
  if (targetFPS == 0) {
    wait_render_time_ = 0;
  } else {
    wait_render_time_ =
        static_cast<uint8_t>(static_cast<float>(1000.0) / (float)targetFPS);
    if (wait_render_time_ < 1) wait_render_time_ = 1;
  }
}
