// Copyright 2021 CatchChallenger
#include "Button.hpp"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <iostream>

#include "../../qt/GameLoader.hpp"

using UI::Button;

Button::Button(QString pix, Node *parent) : Node(parent) {
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
  cache_ = nullptr;
  checkable_ = false;
  checked_ = false;

  bounding_rect_ = QRectF(0.0, 0.0, 223.0, 92.0);
}

Button *Button::Create(QString pix, Node *parent) {
  Button *instance = new (std::nothrow) Button(pix, parent);
  return instance;
}

void Button::SetSize(int w, int h) {
  if (bounding_rect_.width() == w && bounding_rect_.height() == h) return;
  Node::SetSize(w, h);
  if (cache_ != nullptr) delete cache_;
  cache_ = nullptr;
  UpdateTextPath();
}

void Button::SetPos(int x, int y) {
  if (bounding_rect_.x() == x && bounding_rect_.y() == y) return;
  Node::SetPos(x, y);
}

Button::~Button() {
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

void Button::Draw(QPainter *painter) {
  if (!IsVisible()) return;
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
  QPixmap scaled_background;
  QPixmap temp(*GameLoader::gameLoader->getImage(background_));
  if (temp.isNull()) abort();
  if (!IsEnabled())
    scaled_background =
        temp.copy(0, temp.height() * 2 / 3, temp.width(), temp.height() / 3);
  else if (checked_)
    scaled_background =
        temp.copy(0, temp.height() / 3, temp.width(), temp.height() / 3);
  else
    scaled_background = temp.copy(0, 0, temp.width(), temp.height() / 3);
  scaled_background =
      scaled_background.scaled(bounding_rect_.width(), bounding_rect_.height(),
                               Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  paint.drawPixmap(0, 0, bounding_rect_.width(), bounding_rect_.height(),
                   scaled_background);
  UpdateTextPath();

  if (text_path_ != nullptr && paint.isActive()) {
    paint.setRenderHint(QPainter::Antialiasing);
    qreal pen_width = font_->pixelSize() * 0.057;
    if (IsEnabled())
      paint.setPen(QPen(outline_color_, pen_width, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    else
      paint.setPen(QPen(QColor(125, 125, 125), pen_width, Qt::SolidLine,
                        Qt::RoundCap, Qt::RoundJoin));
    paint.setBrush(Qt::white);
    paint.drawPath(*text_path_);
  }
  *cache_ = QPixmap::fromImage(image);

  {
    painter->drawPixmap(bounding_rect_.x(), bounding_rect_.y(),
                        bounding_rect_.width(), bounding_rect_.height(),
                        *cache_);
    return;
  }
}

void Button::SetText(const QString &text) {
  if (text_ == text) return;
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
  text_ = text;
  UpdateTextPath();
}

bool Button::SetPixelSize(uint8_t size) {
  int tempsize = size * 1.5;
  if (font_->pixelSize() == tempsize) return true;
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
  font_->setPixelSize(tempsize);
  return true;
}

void Button::UpdateTextPath() {
  const QString &text = this->text_;
  if (text_path_ != nullptr) delete text_path_;
  QPainterPath temp_path;
  temp_path.addText(0, 0, *font_, text_);
  QRectF rect = temp_path.boundingRect();
  text_path_ = new QPainterPath();
  const int h = bounding_rect_.height();
  int new_height = 0;
  if (checked_)
    new_height = (h * (percent_ + 20) / 100);
  else
    new_height = (h * percent_ / 100);
  const int p = font_->pixelSize();
  const int temp_height = new_height / 2 + p / 2;
  text_path_->addText(bounding_rect_.width() / 2 - rect.width() / 2,
                      temp_height, *font_, text);
}

bool Button::UpdateTextPercent(uint8_t percent) {
  if (percent_ == percent) return true;
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
  if (percent > 100) return false;
  percent_ = percent;
  UpdateTextPath();
  // update();
  return true;
}

void Button::SetFont(const QFont &font) {
  *font_ = font;
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
}

QFont Button::GetFont() const { return *font_; }

void Button::SetOutlineColor(const QColor &color) { outline_color_ = color; }

void Button::SetPressed(const bool &pressed) {
  if (checkable_) {
    if (pressed_ == false && pressed == true && checked_ == false)
      checked_ = true;
    else if (pressed_ == false && pressed == true && checked_ == true)
      checked_ = false;
  } else {
    checked_ = pressed;
  }
  pressed_ = pressed;
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
}

void Button::MousePressEventXY(const QPointF &p, bool &press_validated) {
  if (press_validated) return;
  if (pressed_) return;
  if (!IsVisible()) return;
  if (!IsEnabled()) return;

  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  if (t.contains(p)) {
    press_validated = true;
    SetPressed(true);
  }
}

void Button::MouseReleaseEventXY(const QPointF &p, bool &prev_validated) {
  if (prev_validated) {
    SetPressed(false);
    return;
  }
  if (!pressed_) return;
  if (!IsEnabled()) return;
  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  SetPressed(false);
  if (!prev_validated && IsVisible()) {
    if (t.contains(p)) {
      if (on_click_) {
        on_click_(this);
      }
    }
  }
}

void Button::MouseMoveEventXY(const QPointF &point) { (void)point; }

void Button::SetEnabled(bool enabled) {
  if (enabled == IsEnabled()) return;
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
  Node::SetEnabled(enabled);
}

void Button::SetCheckable(bool checkable) { checkable_ = checkable; }

bool Button::IsChecked() const { return checked_; }
