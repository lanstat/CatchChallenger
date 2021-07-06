// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_CORE_SCENE_HPP_
#define CLIENT_QTOPENGL_CORE_SCENE_HPP_

#include <QGraphicsWidget>
#include <string>
#include <vector>

#include "../ScreenInput.hpp"
#include "ActionManager.hpp"
#include "Node.hpp"

class Scene : public ScreenInput, public Node {
 public:
  ~Scene();

  void RegisterMouseListener(Node *node);
  void RegisterKeyboardListerner(Node *node);

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *) override;
  QRectF boundingRect() const override;
  void mousePressEventXY(const QPointF &p, bool &press,
                         bool &call_parent_class) override;
  void mouseReleaseEventXY(const QPointF &p, bool &press,
                           bool &call_parent_class) override;
  void mouseMoveEventXY(const QPointF &p, bool &press,
                        bool &call_parent_class) override;
  void Draw(QPainter *painter) override;
  void MousePressEventXY(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEventXY(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEventXY(const QPointF &point) override;
  void KeyPressEvent(QKeyEvent * event, bool &event_trigger);
  void KeyReleaseEvent(QKeyEvent *event, bool &event_trigger);

 protected:
  explicit Scene(Node *parent = nullptr);

  virtual void OnScreenSD() = 0;
  virtual void OnScreenHD() = 0;
  virtual void OnScreenHDR() = 0;
  virtual void OnScreenResize() = 0;

  void PrintError(const char* file, int line, const QString &message);

 private:
  std::vector<Node *> mouse_listener_;
  std::vector<Node *> keyboard_listener_;
  ActionManager *action_manager_;
};
#endif  // CLIENT_QTOPENGL_CORE_SCENE_HPP_
