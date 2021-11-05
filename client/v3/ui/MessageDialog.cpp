// Copyright 2021 <CatchChallenger>
#include "MessageDialog.hpp"

using UI::MessageDialog;

MessageDialog::MessageDialog() {
  quit_ = UI::Button::Create(":/CC/images/interface/cancel.png", this);

  message_ = UI::Label::Create(this);

  AddActionButton(quit_);

  newLanguage();
}

MessageDialog::~MessageDialog() {
  delete quit_;
  delete message_;
}

MessageDialog *MessageDialog::Create() {
  return new (std::nothrow) MessageDialog();
}

void MessageDialog::SetMessage(const QString &text) { message_->SetText(text); }

void MessageDialog::OnScreenResize() {
  if (bounding_rect_.width() < 600 || bounding_rect_.height() < 480) {
    quit_->SetSize(83 / 2, 94 / 2);
  } else {
    quit_->SetSize(83, 94);
  }
  message_->SetPos(x_, y_ + 60);
}

void MessageDialog::newLanguage() { SetTitle(tr("Alert")); }

void MessageDialog::OnActionClick(Node *node) {
  if (node == quit_) {
    Parent()->RemoveChild(this);
    Close();
  }
}
