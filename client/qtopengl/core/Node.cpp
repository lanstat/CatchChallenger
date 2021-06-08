// Copyright 2021 CatchChallenger
#include "Node.hpp"

#include <QPainter>

#include "../action/Action.hpp"

Node::Node(QGraphicsItem *parent)
    : QGraphicsItem(parent), on_action_done_(nullptr) {}

Node::~Node() {}

Node *Node::Create(QGraphicsItem *parent) {
  Node *instance = new (std::nothrow) Node(parent);
  return instance;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                 QWidget *) {
  for (auto action : actions_) {
    action.second->Step();
  }
  painter->drawPixmap(x_, y_, cache_);
}

QRectF Node::boundingRect() const { return QRectF(); }

void Node::SetPixmap(const QString &fileName) {}

void Node::SetPixmap(const QPixmap pixmap) { cache_ = pixmap; }

void Node::SetSize(int width, int height) {
  cache_ = cache_.scaled(width, height);
}

void Node::SetPos(int x, int y) {
  x_ = x;
  y_ = y;
}

void Node::SetX(int x) { x_ = x; }

void Node::SetY(int y) { y_ = y; }

int Node::X() { return x_; }

int Node::Y() { return y_; }

void Node::RunAction(Action *action) {
  action->SetTarget(this);
  actions_[action->Tag()] = action;
}

Action* Node::GetAction(int tag) {
  return actions_[tag];
}

void Node::SetOnActionDone(const std::function<void(int)> &callback) {
  on_action_done_ = callback;
}

const std::function<void(int)> &Node::GetOnActionDone() const {
  return on_action_done_;
}
