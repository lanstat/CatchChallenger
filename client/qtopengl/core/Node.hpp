// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_CORE_NODE_HPP_
#define CLIENT_QTOPENGL_CORE_NODE_HPP_

#include <QGraphicsWidget>
#include <unordered_map>

class Action;
class ActionManager;

class Node : public QGraphicsItem {
 public:
  ~Node();

  void SetPos(int x, int y);
  void SetX(int x);
  void SetY(int y);
  int X();
  int Y();
  void SetSize(int width, int height);
  void SetWidth(int w);
  void SetHeight(int h);
  int Width();
  int Height();
  void SetVisible(bool visible);
  bool IsVisible();

  void RunAction(Action *action);
  void StartAllActions();
  Action *GetAction(int tag);
  void SetOnActionDone(const std::function<void(int)> &callback);
  const std::function<void(int)> &GetOnActionDone() const;
  void SetOnClick(const std::function<void()> &callback);

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *) override;
  QRectF boundingRect() const override;

  void MousePressEventXY(const QPointF &point, bool &press_validated);
  void MouseReleaseEventXY(const QPointF &point, bool &prev_validated);
  void MouseMoveEventXY(const QPointF &point);

 protected:
  bool visible_;
  QRectF bounding_rect_;
  ActionManager *action_manager_;

  explicit Node(QGraphicsItem *parent = nullptr);

 private:
  std::function<void(int)> on_action_done_;
  std::function<void()> on_click_;
  uint unique_id_;
};
#endif  // CLIENT_QTOPENGL_CORE_NODE_HPP_
