// Copyright 2021 CatchChallenger
#include "Node.hpp"

#include <QPainter>
#include <iostream>

#include "../action/Action.hpp"
#include "ActionManager.hpp"

Node::Node(QGraphicsItem *parent) : QGraphicsItem(parent) {
  on_action_done_ = nullptr;
  on_click_ = nullptr;
  bounding_rect_ = QRectF(0.0, 0.0, 0.0, 0.0);
  visible_ = true;
  action_manager_ = ActionManager::GetInstance();
}

Node::~Node() {}

void Node::RunAction(Action *action) {
  action_manager_->AddAction(action, this, false);
}

Action *Node::GetAction(int tag) {
  return action_manager_->GetActionByTag(tag, this);
}

void Node::SetOnActionDone(const std::function<void(int)> &callback) {
  on_action_done_ = callback;
}

const std::function<void(int)> &Node::GetOnActionDone() const {
  return on_action_done_;
}

void Node::StartAllActions() {
  auto actions = action_manager_->GetAllActionsByTarget(this);
  auto length = actions->size();
  uint index = 0;
  while (index < length) {
    Action *action = actions->at(index);
    action->Start();
    index++;
  }
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget) {}

QRectF Node::boundingRect() const { return bounding_rect_; }

void Node::SetPos(int x, int y) { bounding_rect_.moveTo(x, y); }

void Node::SetX(int x) { bounding_rect_.moveLeft(x); }

void Node::SetY(int y) { bounding_rect_.moveTop(y); }

int Node::X() { return bounding_rect_.x(); }

int Node::Y() { return bounding_rect_.y(); }

void Node::SetSize(int w, int h) {
  bounding_rect_.setWidth(w);
  bounding_rect_.setHeight(h);
}

void Node::SetHeight(int h) { bounding_rect_.setHeight(h); }

void Node::SetWidth(int w) { bounding_rect_.setWidth(w); }

int Node::Height() { return bounding_rect_.height(); }

int Node::Width() { return bounding_rect_.width(); }

void Node::SetVisible(bool visible) { visible_ = visible; }

bool Node::IsVisible() { return visible_; }

void Node::SetOnClick(const std::function<void()> &callback) {
  on_click_ = callback;
}

void Node::MousePressEventXY(const QPointF &point, bool &press_validated) {
  if (press_validated) return;
  const QRectF &b = boundingRect();
  const QRectF &t = mapRectToScene(b);
  if (t.contains(point)) {
    press_validated = true;
  }
}

void Node::MouseReleaseEventXY(const QPointF &point, bool &prev_validated) {
  if (prev_validated) return;

  const QRectF &b = boundingRect();
  const QRectF &t = mapRectToScene(b);
  if (!prev_validated && IsVisible()) {
    if (t.contains(point)) {
      if (on_click_) {
        on_click_();
      }
    }
  }
}

void Node::MouseMoveEventXY(const QPointF &point) {}
