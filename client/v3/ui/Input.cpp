// Copyright 2021 CatchChallenger
#include "Input.hpp"

#include <QPainter>
#include <iostream>
#ifdef Q_OS_ANDROID
#include <QInputMethod>
#endif

#include "../core/EventManager.hpp"
#include "../core/Node.hpp"

using UI::Input;

Input::Input(Node *parent) : Node(parent) {
  on_text_change_ = nullptr;
  label_ = Label::Create();
  background_ = Sprite::Create(":/CC/images/interface/input.png");
  inner_padding_ = QPointF(14, 11);
  focus_ = false;
  pressed_ = false;
}

Input::~Input() {
  UnRegisterEvents();
  on_text_change_ = nullptr;
  delete label_;
  delete background_;
}

Input *Input::Create(Node *parent) {
  Input *instance = new (std::nothrow) Input(parent);
  return instance;
}

void Input::MousePressEvent(const QPointF &point, bool &press_validated) {
  focus_ = false;
  if (press_validated) return;
  if (pressed_) return;
  if (!IsVisible()) return;
  if (!IsEnabled()) return;

  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  if (t.contains(point)) {
    press_validated = true;
    pressed_ = true;
  }
}

void Input::MouseReleaseEvent(const QPointF &point, bool &prev_validated) {
  if (prev_validated) {
    pressed_ = false;
    focus_ = false;
    return;
  }
  if (!pressed_) return;
  if (!IsEnabled()) return;
  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  pressed_ = false;
  if (!prev_validated && IsVisible()) {
    if (t.contains(point)) {
      prev_validated = true;
      focus_ = true;

#ifdef Q_OS_ANDROID
      
#endif
    }
  }
}

void Input::MouseMoveEvent(const QPointF &point) { (void)point; }

void Input::KeyPressEvent(QKeyEvent *event, bool &event_trigger) {
  if (!focus_) return;
  bool should_notify = false;

  if (event->key() > 31 && event->key() < 127) {
    text_ = text_ + event->text();
    should_notify = true;
  } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
    text_.clear();
    should_notify = true;
  } else if (event->key() == Qt::Key_Delete ||
             event->key() == Qt::Key_Backspace) {
    if (text_.isEmpty()) {
      return;
    }
    text_.remove(text_.size() - 1, 1);
    should_notify = true;
  }
  if (should_notify) {
    label_->SetText(text_);
    ReDraw();
    if (on_text_change_) {
      on_text_change_(text_.toStdString());
    }
  }
}

void Input::KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) {
  (void)event;
  (void)event_trigger;
}

void Input::SetFocus(bool focus) { focus_ = focus; }

bool Input::HasFocus() { return focus_; }

void Input::Draw(QPainter *painter) {
  background_->Paint(painter, nullptr, nullptr);
  label_->SetPos(inner_padding_.x(), Height() / 2 - label_->Height() / 2);
  label_->Paint(painter, nullptr, nullptr);
}

void Input::SetSize(qreal width, qreal height) {
  if (width == Width() && height == Height()) return;
  Node::SetSize(width, height);
  inner_padding_ = background_->Strech(14, 11, 10, Width(), Height());
}

QString Input::Value() const { return text_; }

void Input::SetValue(const QString &value) {
  text_ = value;
  label_->SetText(text_);
  ReDraw();
}

void Input::Clear() { SetValue(""); }

void Input::SetPlaceholder(const QString &value) { placeholder_ = value; }

void Input::SetFont(const QFont &font) {
  label_->SetFont(font);
  ReDraw();
}

void Input::RegisterEvents() {
  EventManager::GetInstance()->AddMouseListener(this);
  EventManager::GetInstance()->AddKeyboardListener(this);
}

void Input::UnRegisterEvents() {
  EventManager::GetInstance()->RemoveListener(this);
}

void Input::SetMode(InputMode mode) { mode_ = mode; }

void Input::SetMaxLength(int size) {}

void Input::SetOnTextChange(std::function<void(std::string)> callback) {
  on_text_change_ = callback;
}
