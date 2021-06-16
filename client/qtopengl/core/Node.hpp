// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_CORE_NODE_HPP_
#define CLIENT_QTOPENGL_CORE_NODE_HPP_

#include <QGraphicsWidget>
#include <QVariant>
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
  void SetEnabled(bool enabled);
  bool IsEnabled() const;
  int Data(int role);
  void SetData(int role, int value);

  void RunAction(Action *action);
  void StartAllActions();
  Action *GetAction(int tag);
  void SetOnActionDone(const std::function<void(int)> &callback);
  const std::function<void(int)> &GetOnActionDone() const;
  void SetOnClick(const std::function<void(Node *)> &callback);

  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *) = 0;
  QRectF boundingRect() const override;

  virtual void MousePressEventXY(const QPointF &point,
                                 bool &press_validated) = 0;
  virtual void MouseReleaseEventXY(const QPointF &point,
                                   bool &prev_validated) = 0;
  virtual void MouseMoveEventXY(const QPointF &point) = 0;

 protected:
  bool visible_;
  QRectF bounding_rect_;
  ActionManager *action_manager_;
  std::function<void(Node *)> on_click_;

  explicit Node(QGraphicsItem *parent = nullptr);

 private:
  std::function<void(int)> on_action_done_;
  std::unordered_map<int, int> data_;
  uint unique_id_;
};
#endif  // CLIENT_QTOPENGL_CORE_NODE_HPP_
