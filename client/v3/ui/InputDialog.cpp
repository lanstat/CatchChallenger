// Copyright 2021 <CatchChallenger>
#include "InputDialog.hpp"

#include <iostream>

#include "../core/SceneManager.hpp"

using UI::InputDialog;

InputDialog::InputDialog() {
  SetDialogSize(500, 300);

  message_ = UI::Label::Create(this);
  message_->SetAlignment(Qt::AlignCenter);

  input_ = UI::Input::Create(this);

  accept_ = UI::Button::Create(":/CC/images/interface/validate.png", this);
  accept_->SetOnClick(std::bind(&InputDialog::OnAcceptClick, this));
  AddActionButton(accept_);

  newLanguage();
}

InputDialog::~InputDialog() {
  delete message_;
  message_ = nullptr;
}

InputDialog *InputDialog::Create() { return new (std::nothrow) InputDialog(); }

void InputDialog::Draw(QPainter *painter) { UI::Dialog::Draw(painter); }

void InputDialog::OnScreenResize() {
  UI::Dialog::OnScreenResize();
  auto content = ContentBoundary();
  message_->SetPos(content.x(), content.y());
  message_->SetWidth(content.width());

  input_->SetPos(content.x(), content.y() + message_->Height());
  input_->SetSize(content.width(), 45);
}

void InputDialog::newLanguage() {}

void InputDialog::OnAcceptClick() {
  callback_(input_->Value());
  Close();
}

void InputDialog::ShowInputNumber(const QString &message,
                                  std::function<void(QString)> callback,
                                  const QString &default_value) {
  ShowInputNumber(QString(), message, callback, default_value);
}

void InputDialog::ShowInputNumber(const QString &title, const QString &message,
                                  std::function<void(QString)> callback,
                                  const QString &default_value) {
  if (!title.isEmpty()) {
    SetTitle(title);
  }

  input_->SetValue(default_value);
  callback_ = callback;
  message_->SetText(message);
  SceneManager::GetInstance()->ShowOverlay(this);
}
