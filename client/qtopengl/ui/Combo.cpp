// Copyright 2021 CatchChallenger
#include "Combo.hpp"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <iostream>

#include "../core/AssetsLoader.hpp"
#include "../core/EventManager.hpp"

using UI::Combo;

Combo::Combo(QString pix, Node *parent) : Node(parent) {
  background_ = pix;

  label_ = Label::Create(this);
  label_->SetAlignment(Qt::AlignCenter);
}

Combo *Combo::Create(QString pix, Node *parent) {
  Combo *instance = new (std::nothrow) Combo(pix, parent);
  return instance;
}

Combo::~Combo() { delete label_; }

void Combo::Draw(QPainter *painter) {
  if (bounding_rect_.isEmpty()) return;
  auto pixmap = AssetsLoader::GetInstance()->GetImage(background_);
  auto tmp = pixmap->copy(0, 0, 223, 92);
  tmp = tmp.scaled(bounding_rect_.width(), bounding_rect_.height(),
                   Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  painter->drawPixmap(0, 0, tmp);
}

void Combo::MousePressEvent(const QPointF &p, bool &press_validated) {
  if (press_validated) return;
  if (!IsVisible()) return;
  if (!IsEnabled()) return;

  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  if (t.contains(p)) {
    press_validated = true;
    // SetPressed(true);
  }
}

void Combo::MouseReleaseEvent(const QPointF &p, bool &prev_validated) {
  if (prev_validated) {
    // SetPressed(false);
    return;
  }
  if (!IsEnabled()) return;
  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  // SetPressed(false);
  if (!prev_validated && IsVisible()) {
    if (t.contains(p)) {
      OnClick();
    }
  }
}

void Combo::MouseMoveEvent(const QPointF &point) { (void)point; }

void Combo::KeyPressEvent(QKeyEvent *event, bool &event_trigger) {
  (void)event;
  (void)event_trigger;
}

void Combo::KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) {
  (void)event;
  (void)event_trigger;
}

void Combo::SetCurrentIndex(uint8_t index) {
  current_index_ = index;
  label_->SetText(items_.at(index));
}

uint8_t Combo::CurrentIndex() { return current_index_; }

void Combo::AddItem(const QString &item) {
  items_.push_back(item);
  if (items_.size() == 1) {
    current_index_ = 0;
    label_->SetText(item);
  }
}

void Combo::OnClick() {
  current_index_++;
  if (current_index_ >= items_.size()) current_index_ = 0;

  label_->SetText(items_.at(current_index_));

  if (on_select_change_) {
    on_select_change_(current_index_);
  }
}

void Combo::SetOnSelectChange(std::function<void(uint8_t)> callback) {
  on_select_change_ = callback;
}

void Combo::SetSize(int width, int height) {
  if (width == bounding_rect_.width() && height == bounding_rect_.height())
    return;
  Node::SetSize(width, height);

  label_->SetSize(width, height);
}

void Combo::RegisterEvents() {
  EventManager::GetInstance()->AddMouseListener(this);
}

void Combo::UnRegisterEvents() {
  EventManager::GetInstance()->RemoveListener(this);
}
