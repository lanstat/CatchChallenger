// Copyright 2021 CatchChallenger
#include "Progressbar.hpp"

#include <QPainter>
#include <cmath>
#include <iostream>

#include "../action/Tick.hpp"
#include "../core/Node.hpp"

using UI::Progressbar;

Progressbar::Progressbar(Node *parent)
    : Node(parent), on_increment_done_(nullptr) {
  text_path_ = nullptr;

  font_ = new QFont();
  font_->setFamily("Comic Sans MS");
  font_->setPixelSize(16);
  font_->setStyleHint(QFont::Monospace);
  font_->setBold(true);
  font_->setStyleStrategy(QFont::ForceOutline);
  bounding_rect_ = QRectF(0.0, 0.0, 200.0, 82.0);
  cache_ = nullptr;

  value_ = 0;
  internal_value_ = 0;
  min_ = 0;
  max_ = 0;

  tick_ = Tick::Create(50, std::bind(&Progressbar::OnTick, this));
}

Progressbar::~Progressbar() {
  if (text_path_ != nullptr) {
    delete text_path_;
    text_path_ = nullptr;
  }
  if (font_ != nullptr) {
    delete font_;
    font_ = nullptr;
  }
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
}

Progressbar *Progressbar::Create(Node *parent) {
  Progressbar *instance = new (std::nothrow) Progressbar(parent);
  return instance;
}

void Progressbar::Draw(QPainter *painter) {
  if (cache_ != nullptr) {
    painter->drawPixmap(bounding_rect_.x(), bounding_rect_.y(),
                        bounding_rect_.width(), bounding_rect_.height(),
                        *cache_);
    return;
  }
  if (bounding_rect_.isEmpty()) return;
  if (cache_ != nullptr) delete cache_;
  cache_ = new QPixmap();
  QImage image(bounding_rect_.width(), bounding_rect_.height(),
               QImage::Format_ARGB32);
  image.fill(Qt::transparent);
  QPainter paint;
  if (image.isNull()) abort();
  paint.begin(&image);

  if (background_left_.isNull() ||
      background_left_.height() != bounding_rect_.height()) {
    QPixmap background(":/CC/images/interface/Pbarbackground.png");
    if (background.isNull()) abort();
    QPixmap bar(":/CC/images/interface/Pbarforeground.png");
    if (bar.isNull()) abort();
    if (bounding_rect_.height() == background.height()) {
      background_left_ = background.copy(0, 0, 41, 82);
      background_middle_ = background.copy(41, 0, 824, 82);
      background_right_ = background.copy(865, 0, 41, 82);
      bar_left_ = bar.copy(0, 0, 26, 82);
      bar_middle_ = bar.copy(26, 0, 620, 82);
      bar_right_ = bar.copy(646, 0, 26, 82);
    } else {
      background_left_ = background.copy(0, 0, 41, 82)
                             .scaledToHeight(bounding_rect_.height(),
                                             Qt::SmoothTransformation);
      background_middle_ = background.copy(41, 0, 824, 82)
                               .scaledToHeight(bounding_rect_.height(),
                                               Qt::SmoothTransformation);
      background_right_ = background.copy(865, 0, 41, 82)
                              .scaledToHeight(bounding_rect_.height(),
                                              Qt::SmoothTransformation);
      bar_left_ = bar.copy(0, 0, 26, 82)
                      .scaledToHeight(bounding_rect_.height(),
                                      Qt::SmoothTransformation);
      bar_middle_ = bar.copy(26, 0, 620, 82)
                        .scaledToHeight(bounding_rect_.height(),
                                        Qt::SmoothTransformation);
      bar_right_ = bar.copy(646, 0, 26, 82)
                       .scaledToHeight(bounding_rect_.height(),
                                       Qt::SmoothTransformation);
    }
  }
  paint.drawPixmap(0, 0, background_left_.width(), background_left_.height(),
                   background_left_);
  paint.drawPixmap(background_left_.width(), 0,
                   bounding_rect_.width() - background_left_.width() -
                       background_right_.width(),
                   background_left_.height(), background_middle_);
  paint.drawPixmap(bounding_rect_.width() - background_right_.width(), 0,
                   background_right_.width(), background_right_.height(),
                   background_right_);

  int startX = 18 * bounding_rect_.height() / 82;
  int size = bounding_rect_.width() - startX - startX;
  if (Maximum() == 0) return;
  float inpixel = internal_value_ * size / Maximum();
  if (inpixel < (bar_left_.width() + bar_right_.width())) {
    if (inpixel > 0) {
      const int tempW = inpixel / 2;
      if (tempW > 0) {
        QPixmap bar_left_C = bar_left_.copy(0, 0, tempW, bar_left_.height());
        paint.drawPixmap(startX, 0, bar_left_C.width(), bar_left_C.height(),
                         bar_left_C);
        const float pixelremaining = inpixel - bar_left_C.width();
        if (pixelremaining > 0) {
          QPixmap bar_right_C =
              bar_right_.copy(bar_right_.width() - pixelremaining, 0,
                              pixelremaining, bar_right_.height());
          paint.drawPixmap(startX + bar_left_C.width(), 0, bar_right_C.width(),
                           bar_right_C.height(), bar_right_C);
        }
      }
    }
  } else {
    paint.drawPixmap(startX, 0, bar_left_.width(), bar_left_.height(),
                     bar_left_);
    const unsigned int pixelremaining =
        inpixel - (bar_left_.width() + bar_right_.width());
    if (pixelremaining > 0)
      paint.drawPixmap(startX + bar_left_.width(), 0, pixelremaining,
                       bar_middle_.height(), bar_middle_);
    paint.drawPixmap(startX + bar_left_.width() + pixelremaining, 0,
                     bar_right_.width(), bar_right_.height(), bar_right_);
  }

  if (false) {
    const unsigned int font_height = bounding_rect_.height() / 4;
    if (font_height >= 11) {
      QString text = "%p";
      text.replace("%p", QString::number(internal_value_ * 100 / Maximum()));

      if (oldText != text) {
        font_->setPixelSize(font_height);
        if (text_path_ != nullptr) delete text_path_;
        QPainterPath tempPath;
        tempPath.addText(0, 0, *font_, text);
        QRectF rect = tempPath.boundingRect();
        text_path_ = new QPainterPath();
        const int h = bounding_rect_.height();
        const int newHeight = h * 95 / 100;
        const int p = font_->pointSize();
        const int tempHeight = newHeight / 2 + p / 2;
        text_path_->addText(bounding_rect_.width() / 2 - rect.width() / 2,
                            tempHeight, *font_, text);

        oldText = text;
      }

      if (paint.isActive()) {
        paint.setRenderHint(QPainter::Antialiasing);
        int penWidth = 1;
        if (font_height > 16) penWidth = 2;
        paint.setPen(QPen(QColor(14, 102, 0), penWidth, Qt::SolidLine,
                          Qt::RoundCap, Qt::RoundJoin));
        paint.setBrush(Qt::white);
        paint.drawPath(*text_path_);
      }
    }
  }

  *cache_ = QPixmap::fromImage(image);

  {
    painter->drawPixmap(bounding_rect_.x(), bounding_rect_.y(),
                        bounding_rect_.width(), bounding_rect_.height(),
                        *cache_);
    return;
  }
}

void Progressbar::SetMaximum(const int &value) {
  if (this->max_ == value) return;
  this->max_ = value;
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
}

void Progressbar::SetMinimum(const int &value) {
  if (min_ == value) return;
  min_ = value;
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
}

void Progressbar::SetValue(const int &value) {
  if (value_ == value) return;
  if (value > Maximum()) {
    value_ = Maximum();
  } else {
    value_ = value;
  }
  internal_value_ = value;
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
}

void Progressbar::IncrementValue(const int &delta, const bool &animate) {
  int tmp = delta;
  if (value_ + tmp > Maximum()) {
    tmp = Maximum() - value_;
  }
  value_ = value_ + tmp;
  if (animate) {
    increment_times_ = 0;
    delta_value_ = tmp / 10.0f;

    RunAction(tick_);
  }
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
}

bool Progressbar::OnTick() {
  bool should_continue;
  if (increment_times_ < 9) {
    internal_value_ += delta_value_;
    increment_times_++;
    should_continue = true;
  } else {
    internal_value_ = value_;
    should_continue = false;
    if (on_increment_done_) {
      on_increment_done_();
    }
  }
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
  return should_continue;
}

int Progressbar::Maximum() { return this->max_; }

int Progressbar::Minimum() { return this->min_; }

int Progressbar::Value() { return this->value_; }

void Progressbar::SetSize(int width, int height) {
  if (bounding_rect_.width() == width && bounding_rect_.height() == height)
    return;
  Node::SetSize(width, height);

  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
}

void Progressbar::MousePressEventXY(const QPointF &point,
                                    bool &press_validated) {
  (void)point;
  (void)press_validated;
}

void Progressbar::MouseReleaseEventXY(const QPointF &point,
                                      bool &prev_validated) {
  (void)point;
  (void)prev_validated;
}

void Progressbar::MouseMoveEventXY(const QPointF &point) { (void)point; }

void Progressbar::SetOnIncrementDone(const std::function<void()> &callback) {
  on_increment_done_ = callback;
}
