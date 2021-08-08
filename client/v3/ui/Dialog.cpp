// Copyright 2021 CatchChallenger
#include "Dialog.hpp"

#include <QPainter>
#include <iostream>

#include "../core/EventManager.hpp"

using UI::Dialog;
using std::placeholders::_1;

Dialog::Dialog() {
  on_close_ = nullptr;

  background_ = Sprite::Create(":/CC/images/interface/message.png");
  title_background_ = Sprite::Create();

  x_ = -1;
  y_ = -1;
  title_background_->SetPixmap(":/CC/images/interface/label.png");

  QLinearGradient gradient1(0, 0, 0, 100);
  gradient1.setColorAt(0.25, QColor(230, 153, 0));
  gradient1.setColorAt(0.75, QColor(255, 255, 255));
  QLinearGradient gradient2(0, 0, 0, 100);
  gradient2.setColorAt(0, QColor(64, 28, 2));
  gradient2.setColorAt(1, QColor(64, 28, 2));
  title_ = UI::Label::Create(gradient2, gradient1);
}

Dialog::~Dialog() {
  delete background_;
  delete title_;
  delete title_background_;
  actions_.clear();
}

void Dialog::OnScreenResize() {
  bool forcedCenter = true;
  int idealW = 900;
  int idealH = 400;
  if (bounding_rect_.width() < 600 || bounding_rect_.height() < 480) {
    idealW /= 2;
    idealH /= 2;
  }
  if (idealW > bounding_rect_.width()) idealW = bounding_rect_.width();
  int top = 36;
  int bottom = 94 / 2;
  if (bounding_rect_.width() < 600 || bounding_rect_.height() < 480) {
    top /= 2;
    bottom /= 2;
  }
  if ((idealH + top + bottom) > bounding_rect_.height())
    idealH = bounding_rect_.height() - (top + bottom);

  if (x_ < 0 || y_ < 0 || forcedCenter) {
    x_ = bounding_rect_.width() / 2 - idealW / 2;
    y_ = bounding_rect_.height() / 2 - idealH / 2;
  } else {
    if ((x_ + idealW) > bounding_rect_.width())
      x_ = bounding_rect_.width() - idealW;
    if ((y_ + idealH + bottom) > bounding_rect_.height())
      y_ = bounding_rect_.height() - (idealH + bottom);
    if (y_ < top) y_ = top;
  }

  background_->SetPos(x_, y_);
  background_->Strech(42, 42, 30, idealW, idealH);

  unsigned int space = 30;
  if (bounding_rect_.width() < 600 || bounding_rect_.height() < 480) {
    title_background_->SetPos(
        x_ + (idealW - title_background_->Width() / 2) / 2, y_ - 36 / 2);
    title_->SetPixelSize(30 / 2);
    space = 10;
  } else {
    title_background_->SetPos(x_ + (idealW - title_background_->Width()) / 2,
                              y_ - 36);
    title_->SetPixelSize(30);
  }

  const QRectF trect = title_->BoundingRect();
  title_->SetPos(x_ + (idealW - title_->BoundingRect().width()) / 2,
                 y_ - trect.height() / 2);

  content_boundary_ = QRectF(x_ + 30, y_ + 60, idealW - 60, idealH - 100);
}

void Dialog::Draw(QPainter *painter) {
  painter->fillRect(0, 0, bounding_rect_.width(), bounding_rect_.height(),
                    QColor(0, 0, 0, 120));
  background_->Paint(painter, nullptr, nullptr);
  title_background_->Paint(painter, nullptr, nullptr);
  title_->Paint(painter, nullptr, nullptr);

  RecalculateActionButtons();
}

void Dialog::OnScreenSD() {}

void Dialog::OnScreenHD() {}

void Dialog::OnScreenHDR() {}

void Dialog::SetOnClose(std::function<void()> callback) {
  on_close_ = callback;
}

void Dialog::OnEnter() {
  EventManager::GetInstance()->Lock();

  for (auto it = begin(actions_); it != end(actions_); it++) {
    (*it)->RegisterEvents();
  }
}

void Dialog::OnExit() {
  for (auto it = begin(actions_); it != end(actions_); it++) {
    (*it)->UnRegisterEvents();
  }

  EventManager::GetInstance()->UnLock();
}

void Dialog::AddActionButton(Button *action) {
  action->SetParent(this);
  action->SetOnClick(std::bind(&Dialog::OnActionButton, this, _1));
  actions_.push_back(action);
  ReDraw();
}

void Dialog::RemoveActionButton(Button *action) {
  int size = actions_.size();
  int index = 0;
  while (index < size) {
    if (actions_.at(index) == action) {
      actions_.erase(actions_.cbegin() + index);
      break;
    }
    index++;
  }
  ReDraw();
}

void Dialog::RecalculateActionButtons() {
  if (actions_.empty()) return;

  unsigned int space = 30;
  unsigned int width = 83;
  unsigned int height = 94;
  if (bounding_rect_.width() < 600 || bounding_rect_.height() < 480) {
    width /= 2;
    height /= 2;
    space = 10;
  }

  unsigned int max_width = 0;
  for (auto it = begin(actions_); it != end(actions_); it++) {
    max_width += width + space;
    (*it)->SetSize(width, height);
  }
  max_width -= space;

  unsigned int x = bounding_rect_.width() / 2 - max_width / 2;
  unsigned int y = (background_->Y() + background_->Height()) - height / 2;
  for (auto it = begin(actions_); it != end(actions_); it++) {
    (*it)->SetPos(x, y);
    x += width + space;
  }
}

void Dialog::SetTitle(const QString &text) {
  title_->SetText(text.toUpper());
  ReDraw();
}

QString Dialog::Title() const { return title_->Text(); }

void Dialog::OnActionButton(Node *node) { OnActionClick(node); }

void Dialog::Close() {
  if (on_close_) {
    on_close_();
  }
}

QRectF Dialog::ContentBoundary() const {
  return content_boundary_;
}
