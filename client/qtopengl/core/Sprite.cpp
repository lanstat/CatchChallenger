// Copyright 2021 CatchChallenger
#include "Sprite.hpp"

#include <QGraphicsWidget>

#include "Node.hpp"
#include "Stateful.hpp"

Sprite::Sprite(QGraphicsItem *parent) : Node(parent) {
  x_ = 0;
  y_ = 0;
}

Sprite::~Sprite() {}

Sprite *Sprite::Create(QGraphicsItem *parent) {
  Sprite *instance = new (std::nothrow) Sprite(parent);
  return instance;
}

void *Sprite::StoreInternal() {
  State *data = new State();
  data->x = x_;
  data->y = y_;
  return data;
}

void Sprite::UseInternal(void *data) {
  auto object = reinterpret_cast<State *>(data);
  x_ = object->x;
  y_ = object->y;
}
