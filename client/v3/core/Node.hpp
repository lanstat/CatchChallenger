// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_CORE_NODE_HPP_
#define CLIENT_V3_CORE_NODE_HPP_

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
  virtual void OnResize();
  void SetVisible(bool visible);
  bool IsVisible();
  void SetEnabled(bool enabled);
  bool IsEnabled() const;
  int Data(int role);
  void SetData(int role, int value);
  void SetParent(Node *parent);
  Node *Parent();
  /** Get the root node */
  Node *Root();
  std::string ClassName() const;
  QRectF MapRectToScene(QRectF inner) const;
  /** Start an action */
  void RunAction(Action *action);
  void StartAllActions();
  Action *GetAction(int tag);
  bool HasRunningActions();
  void SetOnClick(const std::function<void(Node *)> &callback);
  void TestMode();
  void SetToolTip(QString text);
  QString ToolTip();
  QString NodeType();
  void SetZValue(int index);
  void ReArrangeChild(Node * node);
  int ZValue();
  void AddChild(Node *node);
  /** Remove a child from the node */
  void RemoveChild(Node *node);
  std::vector<Node *> Children();
  /** Invalidate cache */
  void ReDraw();
  void ShouldCache(bool cache);
  void SetScale(qreal value);
  qreal Scale();
  virtual void Paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *);
  virtual QRectF BoundingRect() const;

  virtual void Draw(QPainter *painter);
  virtual void MousePressEvent(const QPointF &point, bool &press_validated);
  virtual void MouseReleaseEvent(const QPointF &point, bool &prev_validated);
  virtual void MouseMoveEvent(const QPointF &point);
  virtual void KeyPressEvent(QKeyEvent *event, bool &event_trigger);
  virtual void KeyReleaseEvent(QKeyEvent *event, bool &event_trigger);
  virtual void OnEnter();
  virtual void OnExit();
  virtual void RegisterEvents();
  virtual void UnRegisterEvents();

 protected:
  bool visible_;
  bool enabled_;
  QRectF bounding_rect_;
  ActionManager *action_manager_;
  std::function<void(Node *)> on_click_;
  std::vector<Node *> children_;
  Node *parent_;
  std::string class_name_;
  QString node_type_;

  explicit Node(Node *parent = nullptr);

 private:
  QPixmap *node_cache_;
  std::unordered_map<int, int> data_;
  uint unique_id_;
  bool in_test_;
  QString tooltip_;
  bool should_cache_;
  qreal scale_;
  std::vector<Node *> node_to_delete_;
  int z_;

  void CleanNodeToDelete();
};
#endif  // CLIENT_V3_CORE_NODE_HPP_
