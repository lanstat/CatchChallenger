// Copyright 2021 CatchChallenger
#include "Slider.hpp"

using UI::Slider;

Slider::Slider(Node *parent): Node(parent) {
  on_value_change_ = nullptr;
}

Slider::~Slider() {

}

Slider* Slider::Create(Node *parent) {
  return new (std::nothrow) Slider(parent);
}

void Slider::SetValue(qreal value) {
  value_ = value;
}

void Slider::SetMaximum(qreal value) {
  maximum_ = value;
}

void Slider::MousePressEvent(const QPointF &point, bool &press_validated) {}

void Slider::MouseReleaseEvent(const QPointF &point, bool &prev_validated) {}

void Slider::MouseMoveEvent(const QPointF &point) {}

void Slider::Draw(QPainter *painter) {}

void Slider::RegisterEvents() {}

void Slider::UnRegisterEvents() {}

void Slider::SetOnValueChange(std::function<void (qreal)> callback) {
  on_value_change_ = callback;
}

qreal Slider::Value() {
  return value_;
}
