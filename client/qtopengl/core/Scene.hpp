// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_CORE_SCENE_HPP_
#define CLIENT_QTOPENGL_CORE_SCENE_HPP_

#include <QCoreApplication>
#include <QGraphicsWidget>
#include <string>
#include <vector>

#include "../ScreenInput.hpp"
#include "Node.hpp"

class Scene : public Node {
  Q_DECLARE_TR_FUNCTIONS(Scene)

 public:
  ~Scene();

  void RegisterMouseListener(Node *node);
  void RegisterKeyboardListerner(Node *node);
  void Paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *) override;

  void Draw(QPainter *painter) override;
  void MousePressEvent(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEvent(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEvent(const QPointF &point) override;
  void KeyPressEvent(QKeyEvent *event, bool &event_trigger) override;
  void KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) override;

 protected:
  explicit Scene(Node *parent = nullptr);

  virtual void OnScreenSD() = 0;
  virtual void OnScreenHD() = 0;
  virtual void OnScreenHDR() = 0;
  virtual void OnScreenResize() = 0;

  void PrintError(const char *file, int line, const QString &message);
};
#endif  // CLIENT_QTOPENGL_CORE_SCENE_HPP_
