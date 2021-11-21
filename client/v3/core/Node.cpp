// Copyright 2021 CatchChallenger
#include "Node.hpp"

#include <QPainter>
#include <QVariant>
#include <algorithm>
#include <iostream>

#include "../action/Action.hpp"
#include "ActionManager.hpp"

Node::Node(Node *parent) {
  class_name_ = __func__;
  on_click_ = nullptr;
  on_resize_ = nullptr;
  bounding_rect_ = QRectF(0.0, 0.0, 0.0, 0.0);
  visible_ = true;
  enabled_ = true;
  action_manager_ = ActionManager::GetInstance();
  in_test_ = false;
  parent_ = nullptr;
  node_cache_ = nullptr;
  should_cache_ = true;
  z_ = 0;
  scale_ = 1;

  if (parent) {
    parent->AddChild(this);
  }
}

Node::~Node() {
  if (node_cache_ != nullptr) {
    delete node_cache_;
  }
  node_cache_ = nullptr;

  if (parent_) {
    parent_->RemoveChild(this);
  }
}

void Node::SetParent(Node *parent) { parent_ = parent; }

Node *Node::Parent() { return parent_; }

Node *Node::Root() {
  if (parent_ == nullptr) return nullptr;
  Node *parent = parent_;
  while (parent->Parent() != nullptr) {
    parent = parent->Parent();
  }
  return parent;
}

void Node::AddChild(Node *child) {
  if (child->Parent() && child->Parent() != this) {
    child->Parent()->RemoveChild(child);
  }
  child->SetParent(this);
  child->OnEnter();
  ReArrangeChild(child);
}

void Node::RemoveChild(Node *child) {
  if (child == nullptr) return;
  child->OnExit();
  child->SetParent(nullptr);
  node_to_delete_.push_back(child);

  CleanNodeToDelete();
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

void Node::SetPos(qreal x, qreal y) { bounding_rect_.moveTo(x, y); }

void Node::SetX(qreal x) { bounding_rect_.moveLeft(x); }

void Node::SetY(qreal y) { bounding_rect_.moveTop(y); }

qreal Node::X() { return bounding_rect_.x(); }

qreal Node::Y() { return bounding_rect_.y(); }

void Node::SetSize(qreal w, qreal h) {
  if (scale_ > 0) {
    w = w * scale_;
    h = h * scale_;
  }
  bounding_rect_.setWidth(w);
  bounding_rect_.setHeight(h);
  OnResize();

  if (parent_ != nullptr) {
    parent_->OnChildResize(this);
  }

  if (on_resize_) {
    on_resize_(this);
  }
  ReDraw();
}

void Node::SetHeight(qreal h) { Node::SetSize(Width(), h); }

void Node::SetWidth(qreal w) { Node::SetSize(w, Height()); }

qreal Node::Height() { return bounding_rect_.height(); }

qreal Node::Width() { return bounding_rect_.width(); }

void Node::SetVisible(bool visible) { visible_ = visible; }

bool Node::IsVisible() { return visible_; }

void Node::SetEnabled(bool enabled) { enabled_ = enabled; }

bool Node::IsEnabled() const { return enabled_; }

void Node::SetOnClick(const std::function<void(Node *)> &callback) {
  on_click_ = callback;
}

void Node::SetData(int role, int value) { data_[role] = value; }

int Node::Data(int role) { return data_[role]; }

void Node::SetData(int role, std::string value) { data_str_[role] = value; }

std::string Node::DataStr(int role) { return data_str_[role]; }

void Node::Paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget) {
  if (in_test_) {
    painter->fillRect(BoundingRect(), Qt::green);
  }

  if (visible_) {
    if (!node_cache_) {
      if (bounding_rect_.isEmpty()) return;
      node_cache_ = new QPixmap(Width(), Height());
      node_cache_->fill(Qt::transparent);
      QPainter painter_c(node_cache_);
      if (scale_ != 1) {
        painter_c.scale(scale_, scale_);
      }
      Draw(&painter_c);
      painter_c.end();
    }
    painter->drawPixmap(X(), Y(), *node_cache_);

    if (!should_cache_) ReDraw();

    if (children_.size() == 0) return;

    painter->translate(bounding_rect_.x(), bounding_rect_.y());
    for (auto it = begin(children_); it != end(children_); it++) {
      (*it)->Paint(painter, option, widget);
    }
    painter->translate(-bounding_rect_.x(), -bounding_rect_.y());
  }

  CleanNodeToDelete();
}

void Node::CleanNodeToDelete() {
  if (node_to_delete_.empty()) return;

  int iter = 0;
  while (iter < node_to_delete_.size()) {
    children_.erase(std::remove(children_.begin(), children_.end(),
                                node_to_delete_.at(iter)),
                    children_.end());
    iter++;
  }

  node_to_delete_.clear();
}

void Node::ShouldCache(bool cache) { should_cache_ = cache; }

void Node::TestMode() { in_test_ = true; }

void Node::SetToolTip(QString text) { tooltip_ = text; }

QString Node::ToolTip() { return tooltip_; }

bool Node::HasRunningActions() {
  return action_manager_->GetNumberOfRunningActionsInTarget(this) > 0;
}

void Node::ReDraw() {
  if (node_cache_) {
    delete node_cache_;
  }
  node_cache_ = nullptr;
}

void Node::OnEnter() {
  if (!children_.empty()) {
    for (auto it = begin(children_); it != end(children_); it++) {
      (*it)->OnEnter();
    }
  }
}

void Node::OnExit() {
  if (!children_.empty()) {
    for (auto it = begin(children_); it != end(children_); it++) {
      (*it)->OnExit();
    }
  }
}

void Node::RegisterEvents() {}

void Node::UnRegisterEvents() {}

void Node::SetZValue(int index) {
  z_ = index;
  if (Parent()) {
    Parent()->ReArrangeChild(this);
  }
}

int Node::ZValue() { return z_; }

void Node::ReArrangeChild(Node *node) {
  children_.erase(std::remove(children_.begin(), children_.end(), node),
                  children_.end());
  size_t index = 0;
  for (auto child : children_) {
    if (node->ZValue() < child->ZValue()) break;
    index++;
  }
  children_.insert(children_.begin() + index, node);
}

void Node::MousePressEvent(const QPointF &, bool &) {}

void Node::MouseReleaseEvent(const QPointF &, bool &) {}

void Node::MouseMoveEvent(const QPointF &) {}

void Node::KeyPressEvent(QKeyEvent *, bool &) {}

void Node::KeyReleaseEvent(QKeyEvent *, bool &) {}

std::vector<Node *> Node::Children() { return children_; }

void Node::SetScale(qreal value) {
  scale_ = value;
  const auto rect = BoundingRect();
  SetSize(rect.width(), rect.height());
}

qreal Node::Scale() { return scale_; }

QString Node::NodeType() { return node_type_; }

void Node::Draw(QPainter *painter) {
  std::cout << "LAN_[" << __FILE__ << ":" << __LINE__ << "] "
            << node_type_.toStdString() << "~nooo" << std::endl;
}

void Node::OnResize() {}

std::string Node::ClassName() const { return class_name_; }

void Node::OnChildResize(Node *child) {}

void Node::SetOnResize(const std::function<void(Node *)> &callback) {
  on_resize_ = callback;
}

void Node::RemoveAllChildrens(bool release) {
  if (!children_.empty()) {
    std::vector<Node *> tmp = children_;
    children_.clear();
    if (release) {
      for (auto it = begin(tmp); it != end(tmp); it++) {
        delete (*it);
      }
    }
  }
}
