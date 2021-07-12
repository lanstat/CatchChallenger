// Copyright 2021 CatchChallenger
#include "Node.hpp"

#include <QPainter>
#include <QVariant>
#include <algorithm>
#include <iostream>

#include "../action/Action.hpp"
#include "ActionManager.hpp"

Node::Node(Node *parent) {
  on_click_ = nullptr;
  bounding_rect_ = QRectF(0.0, 0.0, 0.0, 0.0);
  visible_ = true;
  enabled_ = true;
  action_manager_ = ActionManager::GetInstance();
  in_test_ = false;
  parent_ = nullptr;

  if (parent) {
    parent->AddChild(this);
  }
}

Node::~Node() {}

void Node::SetParent(Node *parent) {
  parent_ = parent;
}

Node *Node::Parent() { return parent_; }

Node *Node::Root() {
  Node *parent = parent_;
  while (parent->Parent() != nullptr) {
    parent = parent->Parent();
  }
  return parent; 
}

void Node::AddChild(Node *child) {
  child->SetParent(this);
  children_.push_back(child);
}

void Node::RemoveChild(Node *child) {
  if (child == nullptr) return;
  child->SetParent(nullptr);
  children_.erase(std::remove(children_.begin(), children_.end(), child),
                  children_.end());
}

QRectF Node::MapRectToScene(const QRectF inner) const {
  QRectF tmp = inner;
  Node *parent = parent_;
  while (parent != nullptr) {
    tmp =
        QRectF(tmp.x() + parent->BoundingRect().x(),
               tmp.y() + parent->BoundingRect().y(), tmp.width(), tmp.height());
    parent = parent->Parent();
  }
  return tmp;
}

void Node::RunAction(Action *action) {
  action_manager_->AddAction(action, this, false);
}

Action *Node::GetAction(int tag) {
  return action_manager_->GetActionByTag(tag, this);
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

QRectF Node::BoundingRect() const { return bounding_rect_; }

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

void Node::SetEnabled(bool enabled) { enabled_ = enabled; }

bool Node::IsEnabled() const { return enabled_; }

void Node::SetOnClick(const std::function<void(Node *)> &callback) {
  on_click_ = callback;
}

void Node::SetData(int role, int value) { data_[role] = value; }

int Node::Data(int role) { return data_[role]; }

void Node::Paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget) {
  if (in_test_) {
    painter->fillRect(BoundingRect(), Qt::green);
  }

  if (visible_) {
    Draw(painter);

    if (children_.size() == 0) return;

    painter->translate(bounding_rect_.x(), bounding_rect_.y());
    for (auto it = begin(children_); it != end(children_); it++) {
      (*it)->Paint(painter, option, widget);
    }
    painter->translate(-bounding_rect_.x(), -bounding_rect_.y());
  }
}

void Node::TestMode() { in_test_ = true; }

void Node::SetToolTip(QString text) { tooltip_ = text; }

QString Node::ToolTip() { return tooltip_; }

bool Node::HasRunningActions() {
  return action_manager_->GetNumberOfRunningActionsInTarget(this) > 0;
}
