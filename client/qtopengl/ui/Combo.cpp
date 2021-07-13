// Copyright 2021 CatchChallenger
#include "Combo.hpp"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <iostream>

#include "../core/AssetsLoader.hpp"

using UI::Combo;

Combo::Combo(QString pix, Node *parent) : Node(parent) {
  text_path_ = nullptr;

  font_ = new QFont();
  font_->setFamily("Comic Sans MS");
  font_->setPixelSize(35);
  font_->setStyleHint(QFont::Monospace);
  font_->setBold(true);
  font_->setStyleStrategy(QFont::ForceOutline);

  pressed_ = false;
  background_ = pix;
  outline_color_ = QColor(217, 145, 0);
  percent_ = 75;
  checkable_ = false;
  checked_ = false;

  bounding_rect_ = QRectF(0.0, 0.0, 223.0, 92.0);
}

Combo *Combo::Create(QString pix, Node *parent) {
  Combo *instance = new (std::nothrow) Combo(pix, parent);
  return instance;
}

Combo::~Combo() {
  if (text_path_ != nullptr) {
    delete text_path_;
    text_path_ = nullptr;
  }
  if (font_ != nullptr) {
    delete font_;
    font_ = nullptr;
  }
}

void Combo::Draw(QPainter *painter) {
  if (bounding_rect_.isEmpty()) return;
}

void Combo::SetText(const QString &text) {
  if (text_ == text) return;
  text_ = text;
  ReDraw();
}

void Combo::SetOutlineColor(const QColor &color) { outline_color_ = color; }

void Combo::MousePressEvent(const QPointF &p, bool &press_validated) {
  if (press_validated) return;
  if (pressed_) return;
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
  if (!pressed_) return;
  if (!IsEnabled()) return;
  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  // SetPressed(false);
  if (!prev_validated && IsVisible()) {
    if (t.contains(p)) {
      if (on_click_) {
        on_click_(this);
      }
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
}

uint8_t Combo::CurrentIndex() {
  return current_index_;
}

void Combo::AddItem(const QString &item) {

}
