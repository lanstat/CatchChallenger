// Copyright 2021 CatchChallenger
#include "Backdrop.hpp"

#include <iostream>
#include <QPainter>

using UI::Backdrop;

Backdrop::Backdrop(QColor color, Node *parent) : Node(parent) {
  node_type_ = __func__;
  color_ = color;
}

Backdrop::~Backdrop() {}

Backdrop *Backdrop::Create(QColor color, Node *parent) {
  Backdrop *instance = new (std::nothrow) Backdrop(color, parent);
  return instance;
}

void Backdrop::Draw(QPainter *painter) {
  painter->fillRect(0, 0, bounding_rect_.width(), bounding_rect_.height(),
                    color_);
}
