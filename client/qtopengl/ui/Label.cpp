// Copyright 2021 CatchChallenger
#include "Label.hpp"

#include <QPainter>
#include <iostream>

#include "../core/Node.hpp"

using UI::Label;

Label::Label(QLinearGradient text_color, QLinearGradient border_color,
             Node *parent)
    : Node(parent) {
  font_ = new QFont();
  font_->setFamily("Comic Sans MS");
  font_->setPixelSize(25);
  font_->setStyleHint(QFont::Monospace);
  font_->setStyleStrategy(QFont::ForceOutline);

  pen_gradient_ = border_color;
  brush_gradient_ = text_color;

  cache_ = nullptr;
  text_path_ = nullptr;
}

Label::~Label() { delete font_; }

Label *Label::Create(Node *parent) {
  Label *instance = Create(QColor(25, 11, 1), QColor(255, 255, 255), parent);
  return instance;
}

Label *Label::Create(QColor text, QColor border, Node *parent) {
  QLinearGradient pen(0, 0, 0, 100);
  pen.setColorAt(0, text);
  pen.setColorAt(1, text);
  QLinearGradient brush(0, 0, 0, 100);
  brush.setColorAt(0, border);
  brush.setColorAt(1, border);

  Label *instance = new (std::nothrow) Label(pen, brush, parent);
  return instance;
}

Label *Label::Create(QColor text, QLinearGradient border, Node *parent) {
  QLinearGradient pen(0, 0, 0, 100);
  pen.setColorAt(0, text);
  pen.setColorAt(1, text);

  Label *instance = new (std::nothrow) Label(pen, border, parent);
  return instance;
}

Label *Label::Create(QLinearGradient pen, QLinearGradient border,
                     Node *parent) {
  Label *instance = new (std::nothrow) Label(pen, border, parent);
  return instance;
}

void Label::Draw(QPainter *painter) {
  const unsigned int h = bounding_rect_.height();
  const unsigned int w = bounding_rect_.width();

  if (cache_ != nullptr && !cache_->isNull() && cache_->width() == w &&
      cache_->height() == h) {
    painter->drawPixmap(bounding_rect_.x(), bounding_rect_.y() + h * 0.1, w, h,
                        *cache_);
    return;
  }
  if (bounding_rect_.isEmpty()) return;
  if (cache_ != nullptr) delete cache_;
  cache_ = new QPixmap();

  QImage image(w, h, QImage::Format_ARGB32);
  image.fill(Qt::transparent);
  QPainter paint;
  if (image.isNull()) abort();
  paint.begin(&image);

  if (text_path_ != nullptr && paint.isActive()) {
    paint.setRenderHint(QPainter::Antialiasing);
    qreal pen_width = 2.0;
    if (font_->pixelSize() <= 12)
      pen_width = 2;
    else if (font_->pixelSize() <= 18)
      pen_width = 3;
    else if (font_->pixelSize() <= 24)
      pen_width = 4;
    else
      pen_width = 5;

    pen_gradient_.setStart(0, 0);
    pen_gradient_.setFinalStop(0, h);
    QPen pen(pen_gradient_, pen_width, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
    QBrush brush(brush_gradient_);
    paint.setPen(pen);
    paint.drawPath(*text_path_);
    paint.setBrush(brush);
    paint.setPen(
        QPen(Qt::transparent, 0, Qt::NoPen, Qt::RoundCap, Qt::RoundJoin));
    paint.drawPath(*text_path_);
  }

  *cache_ = QPixmap::fromImage(image);

  painter->drawPixmap(bounding_rect_.x(), bounding_rect_.y() + h * 0.1, w, h,
                      *cache_);
}

void Label::SetText(const QString &text) {
  if (text_ == text) return;
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
  text_ = text;
  UpdateTextPath();
}

void Label::SetPixelSize(uint8_t size) {
  int tempsize = size * 1.5;
  if (font_->pixelSize() == tempsize) return;
  if (cache_ != nullptr) {
    delete cache_;
    cache_ = nullptr;
  }
  font_->setPixelSize(tempsize);
}

void Label::UpdateTextPath() {
  const QString &text = text_;
  if (text_path_ != nullptr) delete text_path_;
  QPainterPath temp_path;
  temp_path.addText(0, 20, *font_, text_);
  QRectF rect = temp_path.boundingRect();
  text_path_ = new QPainterPath();
  SetSize(rect.width() + 15, rect.height() + 15);
  int new_height = bounding_rect_.height();
  const int p = font_->pixelSize();
  const int temp_height = new_height / 2 + p / 2;
  const Qt::Alignment a = Qt::AlignCenter;  // alignment();
  if (a.testFlag(Qt::AlignLeft))
    text_path_->addText(0, temp_height - 1, *font_, text);
  else if (a.testFlag(Qt::AlignRight))
    text_path_->addText(bounding_rect_.width() - rect.width(), temp_height - 1,
                        *font_, text);
  else
    text_path_->addText(bounding_rect_.width() / 2 - rect.width() / 2,
                        temp_height - 1, *font_, text);
}

void Label::SetFont(const QFont &font) { *this->font_ = font; }

QFont Label::GetFont() const { return *font_; }

void Label::MousePressEventXY(const QPointF &point, bool &press_validated) {}

void Label::MouseReleaseEventXY(const QPointF &point, bool &prev_validated) {}

void Label::MouseMoveEventXY(const QPointF &point) { (void)point; }
