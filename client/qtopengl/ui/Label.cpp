// Copyright 2021 CatchChallenger
#include "Label.hpp"

#include <QPainter>

using UI::Label;

Label::Label(QGraphicsItem *parent) : QGraphicsItem(parent) {
  font_ = new QFont();
  font_->setFamily("Comic Sans MS");
  font_->setPixelSize(25);
  font_->setStyleHint(QFont::Monospace);
  font_->setStyleStrategy(QFont::ForceOutline);

  color_ = new QColor(33, 33, 33);
}

Label::~Label() {
  delete font_;
  delete color_;
}

void Label::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *) {
  painter->setPen(*color_);
  painter->setFont(*font_);
  painter->drawText(100, 100, text_);
}

QRectF Label::boundingRect() const { return bounding_rect_; }

void Label::SetText(const QString &text) {
  if (this->text_ == text) return;
  this->text_ = text;
}

void Label::SetPixelSize(uint8_t size) {
  if (font_->pixelSize() == size) return;
  font_->setPixelSize(size);
}

void Label::SetFont(const QFont &font) { *this->font_ = font; }

QFont Label::GetFont() const { return *font_; }

void Label::SetColor(const QColor &color) { *color_ = color; }

QColor Label::GetColor() const { return *color_; }
