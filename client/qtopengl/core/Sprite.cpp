// Copyright 2021 CatchChallenger
#include "Sprite.hpp"

#include <QPainter>

#include "Node.hpp"
#include "Stateful.hpp"

Sprite::Sprite(QGraphicsItem *parent) : Node(parent) {}

Sprite::~Sprite() {}

Sprite *Sprite::Create(QGraphicsItem *parent) {
  Sprite *instance = new (std::nothrow) Sprite(parent);
  return instance;
}

void *Sprite::StoreInternal() {
  State *data = new State();
  data->x = X();
  data->y = Y();
  return data;
}

void Sprite::UseInternal(void *data) {
  auto object = reinterpret_cast<State *>(data);
  SetPos(object->x, object->y);
}

void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) {
  if (visible_) {
    painter->drawPixmap(X(), Y(), cache_);
  }
}

void Sprite::SetPixmap(const QString &file_name) {}

void Sprite::SetPixmap(const QPixmap pixmap) {
  cache_ = pixmap;
  Node::SetSize(cache_.width(), cache_.height());
}

void Sprite::SetSize(int width, int height) {
  cache_ = cache_.scaled(width, height);
  Node::SetSize(width, height);
}

void Sprite::MousePressEventXY(const QPointF &point, bool &press_validated) {
  if (press_validated) return;
  const QRectF &b = boundingRect();
  const QRectF &t = mapRectToScene(b);
  if (t.contains(point)) {
    press_validated = true;
  }
}

void Sprite::MouseReleaseEventXY(const QPointF &point, bool &prev_validated) {
  if (prev_validated) return;

  const QRectF &b = boundingRect();
  const QRectF &t = mapRectToScene(b);
  if (!prev_validated && IsVisible()) {
    if (t.contains(point)) {
      if (on_click_) {
        on_click_(this);
      }
    }
  }
}

void Sprite::MouseMoveEventXY(const QPointF &point) {}
