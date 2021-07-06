// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_CORE_SCENEMANAGER_HPP_
#define CLIENT_QTOPENGL_CORE_SCENEMANAGER_HPP_

#include <QGraphicsView>
#include <QTime>
#include <QTimer>
#include <QWidget>
#include <vector>

#include "../../../general/base/GeneralStructures.hpp"
#include "GraphicRenderer.hpp"
#include "Scene.hpp"

namespace CatchChallenger {
class InternalServer;
}

class ConnexionManager;

class SceneManager : public QGraphicsView {
  Q_OBJECT

 public:
  SceneManager();
  ~SceneManager();

  void PushScene(Scene *scene);
  Scene *PopScene();

 protected:
  void resizeEvent(QResizeEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void closeEvent(QCloseEvent *event) override;

 protected:
  void paintEvent(QPaintEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;

 private:
#ifdef CATCHCHALLENGER_SOLO
  CatchChallenger::InternalServer *internal_server_;
#endif
#ifndef NOTHREADS
  QThread *thread_solo_;
#endif
  GraphicRenderer *renderer_;
  ConnexionManager *connection_manager_;
  QGraphicsScene *graphic_scene_;
  std::vector<Scene *> scene_stack_;

  uint8_t wait_render_time_;
  QTimer timer_render_;
  QTime time_elapsed_;
  uint16_t frame_counter_;

  void Render();
};

#endif  // CLIENT_QTOPENGL_CORE_SCENEMANAGER_HPP_
