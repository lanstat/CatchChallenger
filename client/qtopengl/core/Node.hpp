// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_CORE_NODE_HPP_
#define CLIENT_QTOPENGL_CORE_NODE_HPP_

#include <QGraphicsWidget>
#include <unordered_map>

class Action;

class Node : public QGraphicsItem {
 public:
  static Node* Create(QGraphicsItem *parent = nullptr);
  ~Node();
  void SetPixmap(const QString &file_name);
  void SetPixmap(const QPixmap pixmap);
  void SetSize(int width, int height);
  void SetPos(int x, int y);
  void SetX(int x);
  void SetY(int y);
  int X();
  int Y();
  void RunAction(Action *action);
  Action* GetAction(int tag);
  void SetOnActionDone(const std::function<void(int)> &callback);
  const std::function<void(int)>& GetOnActionDone() const;

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *) override;
  QRectF boundingRect() const override;

 protected:
  QPixmap cache_;

  int x_;
  int y_;

  explicit Node(QGraphicsItem *parent = nullptr);

 private:
  std::unordered_map<int, Action *> actions_;
  std::function<void(int)> on_action_done_;
};
#endif  // CLIENT_QTOPENGL_CORE_NODE_HPP_
