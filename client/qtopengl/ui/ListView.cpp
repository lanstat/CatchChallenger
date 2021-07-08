// Copyright 2021 CatchChallenger
#include "ListView.hpp"

#include <QPainter>
#include <iostream>

#include "../core/Node.hpp"

using UI::ListView;

ListView::ListView(Node *parent) : Node(parent) {
  content_rect_ = QRectF(0.0, 0.0, 0.0, 0.0);
  slider_rect_ = QRectF(0.0, 0.0, 0.0, 0.0);
  show_slider_ = false;
  is_slider_pressed_ = false;
  is_horizontal_ = false;
}

ListView::~ListView() {
  for (auto item : items_) {
    delete item;
  }
}

ListView *ListView::Create(Node *parent) {
  ListView *instance = new (std::nothrow) ListView(parent);
  return instance;
}

void ListView::Draw(QPainter *painter) {
  if (items_.size() == 0) return;

  if (visible_) {
    int x1 = bounding_rect_.x();
    int y1 = bounding_rect_.y();
    int x2 = bounding_rect_.x() + bounding_rect_.width();
    int y2 = bounding_rect_.y() + bounding_rect_.height();

    content_.fill(Qt::transparent);
    QPainter painter_c(&content_);
    if (painter_c.isActive()) {
      for (auto item : items_) {
        item->Paint(&painter_c, nullptr, nullptr);
      }
      painter_c.end();
    }

    painter->drawPixmap(x1, y1, content_, content_rect_.x(), content_rect_.y(),
                        BoundingRect().width(), BoundingRect().height());

    if (show_slider_) {
      if (is_horizontal_) {
        painter->drawPixmap(x2 - 22, y1, slider_rail_);
      } else {
        painter->drawPixmap(x1, y2 - 22, slider_rail_);
      }
      painter->drawPixmap(slider_rect_.x(), slider_rect_.y(), slider_);
    }
  }
}

void ListView::AddItem(Node *node) {
  node->SetParent(nullptr);
  items_.push_back(node);
  ReCalculateSize();
}

Node *ListView::GetItem(uint8_t index) { return items_.at(index); }

void ListView::RemoveItem(uint8_t index) {
  items_.erase(items_.begin() + index);
}

Node *ListView::GetSelectedItem() { return selected_item_; }

void ListView::SetSize(int width, int height) {
  Node::SetSize(width, height);
  ReCalculateSize();
}

void ListView::ReCalculateSize() {
  if (!is_horizontal_) {
    content_rect_ = QRectF(0.0, 0.0, Width() - 22, 0.0);

    if (items_.size() == 0) return;
    int x = 0;
    int y = 0;
    for (auto item : items_) {
      item->SetX(x);
      item->SetY(y);
      content_rect_.setHeight(item->Height() + y);
      y = y + item->Height();
    }

    show_slider_ = content_rect_.height() > Height();

    if (!show_slider_) {
      content_rect_.setWidth(Width());
    }

    content_ = QPixmap(content_rect_.width(), content_rect_.height());
    content_scale_ = content_rect_.height() / BoundingRect().height();

    if (show_slider_) {
      int slider_height = (BoundingRect().height() / content_rect_.height()) *
                          BoundingRect().height();

      auto tmp = QPixmap(":/CC/images/interface/sliderBarV.png");
      auto top = tmp.copy(0, 0, 22, 5);
      auto bottom = tmp.copy(0, 38, 22, 5);
      auto middle = tmp.copy(0, 5, 22, 5);
      middle = middle.scaled(22, Height() - 10);

      slider_rail_ = QPixmap(22, Height());
      slider_rail_.fill(Qt::transparent);

      auto painter = new QPainter(&slider_rail_);
      painter->drawPixmap(0, 0, top);
      painter->drawPixmap(0, 5, middle);
      painter->drawPixmap(0, Height() - 5, bottom);
      painter->end();
      delete painter;

      slider_rect_ = QRectF(BoundingRect().x() + BoundingRect().width() - 22,
                            BoundingRect().y(), 22, slider_height);

      slider_ = QPixmap(22, slider_height);
      slider_.fill(Qt::transparent);

      painter = new QPainter(&slider_);

      tmp = QPixmap(":/CC/images/interface/sliderV.png");
      top = tmp.copy(0, 0, 22, 18);
      bottom = tmp.copy(0, 19, 22, 18);
      middle = tmp.copy(0, 18, 22, 1);

      if (slider_height <= 36) {
        painter->drawPixmap(0, 0, top.scaled(22, slider_height / 2));
        painter->drawPixmap(0, slider_height / 2,
                            bottom.scaled(22, slider_height / 2));
      } else {
        painter->drawPixmap(0, 0, top);
        painter->drawPixmap(0, 18, middle.scaled(22, slider_height - 36));
        painter->drawPixmap(0, slider_height - 18, bottom);
      }
      painter->end();
      delete painter;
    }
  } else {
    content_rect_ = QRectF(0.0, 0.0, 0.0, Height() - 22);

    if (items_.size() == 0) return;
    int x = 0;
    int y = 0;
    for (auto item : items_) {
      item->SetX(x);
      item->SetY(y);
      content_rect_.setWidth(item->Width() + x);
      x = x + item->Width();
    }

    show_slider_ = content_rect_.width() > Width();

    if (!show_slider_) {
      content_rect_.setWidth(Width());
    }

    content_ = QPixmap(content_rect_.width(), content_rect_.height());
    content_scale_ = content_rect_.width() / BoundingRect().width();

    if (show_slider_) {
      int slider_width = (BoundingRect().width() / content_rect_.width()) *
                         BoundingRect().width();

      auto tmp = QPixmap(":/CC/images/interface/sliderBarV.png");
      auto top = tmp.copy(0, 0, 22, 5);
      auto bottom = tmp.copy(0, 38, 22, 5);
      auto middle = tmp.copy(0, 5, 22, 5);
      middle = middle.scaled(22, Height() - 10);

      slider_rail_ = QPixmap(22, Height());
      slider_rail_.fill(Qt::transparent);

      auto painter = new QPainter(&slider_rail_);
      painter->drawPixmap(0, 0, top);
      painter->drawPixmap(0, 5, middle);
      painter->drawPixmap(0, Height() - 5, bottom);
      painter->rotate(90);
      painter->end();
      delete painter;

      slider_rect_ = QRectF(BoundingRect().x(),
                            BoundingRect().y() + BoundingRect().height() - 22,
                            slider_width, 22);

      slider_ = QPixmap(slider_width, 22);
      slider_.fill(Qt::transparent);

      painter = new QPainter(&slider_);

      tmp = QPixmap(":/CC/images/interface/sliderV.png");
      top = tmp.copy(0, 0, 22, 18);
      bottom = tmp.copy(0, 19, 22, 18);
      middle = tmp.copy(0, 18, 22, 1);

      if (slider_width <= 36) {
        painter->drawPixmap(0, 0, top.scaled(22, slider_width / 2));
        painter->drawPixmap(0, slider_width / 2,
                            bottom.scaled(22, slider_width / 2));
      } else {
        painter->drawPixmap(0, 0, top);
        painter->drawPixmap(0, 18, middle.scaled(22, slider_width - 36));
        painter->drawPixmap(0, slider_width - 18, bottom);
      }
      painter->rotate(90);
      painter->end();
      delete painter;
    }
  }
}

void ListView::MousePressEvent(const QPointF &point, bool &press_validated) {
  if (press_validated) return;

  const QRectF &b = slider_rect_;
  const QRectF &t = MapRectToScene(b);
  if (show_slider_ && t.contains(point)) {
    press_validated = true;
    is_slider_pressed_ = true;
  } else {
    // Verify if point is in listview, if true calculate the relative position
    // of the point in the content
    const QRectF &b2 = BoundingRect();
    const QRectF &t2 = MapRectToScene(b2);
    if (t2.contains(point)) {
      QPointF auxiliar = QPointF(content_rect_.x() + point.x() - t2.x(),
                                 content_rect_.y() + point.y() - t2.y());
      for (auto item : items_) {
        item->MousePressEvent(auxiliar, press_validated);
      }
    }
  }
}

void ListView::MouseReleaseEvent(const QPointF &point, bool &prev_validated) {
  is_slider_pressed_ = false;
  // Verify if point is in listview, if true calculate the relative position
  // of the point in the content
  const QRectF &b2 = BoundingRect();
  const QRectF &t2 = MapRectToScene(b2);
  if (t2.contains(point)) {
    QPointF auxiliar = QPointF(content_rect_.x() + point.x() - t2.x(),
                               content_rect_.y() + point.y() - t2.y());
    for (auto item : items_) {
      item->MouseReleaseEvent(auxiliar, prev_validated);
    }
  }
}

void ListView::MouseMoveEvent(const QPointF &point) {
  // TODO(lanstat): Verificar comportamiento cuando se use horizontal
  if (is_slider_pressed_) {
    const QRectF &b = BoundingRect();
    const QRectF &t = MapRectToScene(b);
    int delta = point.y() - (t.y() + BoundingRect().y());
    if (delta < BoundingRect().y()) {
      delta = BoundingRect().y();
    } else if (delta + slider_rect_.height() >
               BoundingRect().y() + BoundingRect().height()) {
      delta =
          BoundingRect().y() + BoundingRect().height() - slider_rect_.height();
    }
    content_rect_.setY((delta - BoundingRect().y()) * content_scale_);
    slider_rect_.moveTop(delta);
  }
}

void ListView::KeyPressEvent(QKeyEvent *event, bool &event_trigger) {
  (void)event;
  (void)event_trigger;
}

void ListView::KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) {
  (void)event;
  (void)event_trigger;
}

void ListView::Clear() {
  for (std::vector<Node *>::iterator it = items_.begin(); it != items_.end();
       ++it) {
    delete *it;
  }
  items_.clear();
}

void ListView::SetHorizontal(bool horizontal) {
  if (is_horizontal_ == horizontal) return;
  is_horizontal_ = horizontal;
  ReCalculateSize();
}

uint8_t ListView::Count() { return items_.size(); }
