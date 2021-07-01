// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_CORE_NODE_HPP_
#define CLIENT_QTOPENGL_CORE_NODE_HPP_

#include <QGraphicsWidget>
#include <QVariant>
#include <unordered_map>
#include <vector>

class Action;
class ActionManager;

class Node {
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
  void SetParent(Node *parent);
  Node *Parent();
  QRectF MapRectToScene(QRectF inner) const;
  void RunAction(Action *action);
  void StartAllActions();
  Action *GetAction(int tag);
  bool HasRunningActions();
  void SetOnClick(const std::function<void(Node *)> &callback);
  void TestMode();
  void SetToolTip(QString text);
  QString ToolTip();
  void AddChild(Node *node);
  void Paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *);
  QRectF BoundingRect() const;

  virtual void Draw(QPainter *painter) = 0;
  virtual void MousePressEventXY(const QPointF &point,
                                 bool &press_validated) = 0;
  virtual void MouseReleaseEventXY(const QPointF &point,
                                   bool &prev_validated) = 0;
  virtual void MouseMoveEventXY(const QPointF &point) = 0;

 protected:
  bool visible_;
  bool enabled_;
  QRectF bounding_rect_;
  ActionManager *action_manager_;
  std::function<void(Node *)> on_click_;
  std::vector<Node *> children_;
  Node *parent_;

  explicit Node(Node *parent = nullptr);

 private:
  std::unordered_map<int, int> data_;
  uint unique_id_;
  bool in_test_;
  QString tooltip_;
};
#endif  // CLIENT_QTOPENGL_CORE_NODE_HPP_
