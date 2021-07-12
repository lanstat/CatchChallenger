// Copyright 2021 CatchChallenger
#include "Sprite.hpp"

#include <QPainter>
#include <iostream>

#include "AssetsLoader.hpp"
#include "Node.hpp"
#include "Stateful.hpp"

Sprite::Sprite(Node *parent) : Node(parent) {}

Sprite::~Sprite() {}

Sprite *Sprite::Create(Node *parent) {
  Sprite *instance = new (std::nothrow) Sprite(parent);
  return instance;
}

Sprite *Sprite::Create(QString pix, Node *parent) {
  Sprite *instance = new (std::nothrow) Sprite(parent);
  instance->SetPixmap(pix);
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

void Sprite::Draw(QPainter *painter) { painter->drawPixmap(0, 0, cache_); }

void Sprite::SetPixmap(const QString &file_name) {
  SetPixmap(*AssetsLoader::GetInstance()->GetImage(file_name));
}

void Sprite::SetPixmap(const QPixmap pixmap) { SetPixmap(pixmap, true); }

void Sprite::SetPixmap(const QPixmap pixmap, bool preserve_size) {
  if (preserve_size) {
    cache_ = pixmap;
    Node::SetSize(cache_.width(), cache_.height());
  } else {
    cache_ = pixmap.scaledToWidth(bounding_rect_.width());
  }
  ReDraw();
}

void Sprite::SetSize(int width, int height) {
  cache_ = cache_.scaled(width, height);
  Node::SetSize(width, height);
}

void Sprite::MousePressEvent(const QPointF &point, bool &press_validated) {
  if (press_validated) return;
  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  if (t.contains(point)) {
    press_validated = true;
  }
}

void Sprite::MouseReleaseEvent(const QPointF &point, bool &prev_validated) {
  if (prev_validated) return;

  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  if (!prev_validated && IsVisible()) {
    if (t.contains(point)) {
      if (on_click_) {
        on_click_(this);
      }
    }
  }
}

void Sprite::MouseMoveEvent(const QPointF &point) { (void)point; }

void Sprite::KeyPressEvent(QKeyEvent *event, bool &event_trigger) {
  (void)event;
  (void)event_trigger;
}

void Sprite::KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) {
  (void)event;
  (void)event_trigger;
}

void Sprite::Strech(unsigned int border_size, int width, int height) {
  if (width == Width() && height == Height()) return;

  int tw = border_size;
  if (width < 50)
    tw = border_size / 4;
  else if (width > 250)
    tw = border_size;
  else
    tw = (width - 50) * border_size * 3 / 4 / 200 + border_size / 4;
  int th = border_size;
  if (height < 50)
    th = border_size / 4;
  else if (height > 250)
    th = border_size;
  else
    th = (height - 50) * border_size * 3 / 4 / 200 + border_size / 4;
  int min = tw;
  if (min > th) min = th;

  QPixmap background = cache_;
  QPixmap tr, tm, tl, mr, mm, ml, br, bm, bl;
  tl = background.copy(0, 0, border_size, border_size);
  tm = background.copy(border_size, 0,
                       background.width() - border_size - border_size,
                       border_size);
  tr = background.copy(background.width() - border_size, 0, border_size,
                       border_size);
  ml = background.copy(0, border_size, border_size,
                       background.height() - border_size - border_size);
  mm = background.copy(border_size, border_size,
                       background.width() - border_size - border_size,
                       background.height() - border_size - border_size);
  mr = background.copy(background.width() - border_size, border_size,
                       border_size,
                       background.height() - border_size - border_size);
  bl = background.copy(0, background.height() - border_size, border_size,
                       border_size);
  bm = background.copy(border_size, background.height() - border_size,
                       background.width() - border_size - border_size,
                       border_size);
  br = background.copy(background.width() - border_size,
                       background.height() - border_size, border_size,
                       border_size);

  if (min != tl.height()) {
    tl = tl.scaledToHeight(min, Qt::SmoothTransformation);
    tm = tm.scaledToHeight(min, Qt::SmoothTransformation);
    tr = tr.scaledToHeight(min, Qt::SmoothTransformation);
    ml = ml.scaledToWidth(min, Qt::SmoothTransformation);
    // mm=mm.scaledToHeight(minsize,Qt::SmoothTransformation);
    mr = mr.scaledToWidth(min, Qt::SmoothTransformation);
    bl = bl.scaledToHeight(min, Qt::SmoothTransformation);
    bm = bm.scaledToHeight(min, Qt::SmoothTransformation);
    br = br.scaledToHeight(min, Qt::SmoothTransformation);
  }

  auto tmp = QPixmap(width, height);
  tmp.fill(Qt::transparent);

  auto painter = new QPainter(&tmp);
  painter->drawPixmap(0, 0, min, min, tl);
  painter->drawPixmap(min, 0, width - min * 2, min, tm);
  painter->drawPixmap(width - min, 0, min, min, tr);

  painter->drawPixmap(0, min, min, height - min * 2, ml);
  painter->drawPixmap(min, min, width - min * 2, height - min * 2, mm);
  painter->drawPixmap(width - min, min, min, height - min * 2, mr);

  painter->drawPixmap(0, height - min, min, min, bl);
  painter->drawPixmap(min, height - min, width - min * 2, min, bm);
  painter->drawPixmap(width - min, height - min, min, min, br);
  painter->end();
  delete painter;
  SetPixmap(tmp);
}
