// Copyright 2021 CatchChallenger
#include "Test.hpp"

#include <iostream>

using Scenes::Test;
using std::placeholders::_1;

Test::Test(): Scene(nullptr) {
  // sprite_ = Sprite::Create(":/CC/images/interface/teacher.png", this);
  label_ = UI::Label::Create(this);
}

Test::~Test() {
  delete sprite_;
}

Test* Test::Create() {
  return new (std::nothrow) Test();
}

void Test::OnScreenResize() {
  uint8_t widget_border = 46;
  //sprite_->Strech(widget_border, 70, 70);
  //sprite_->SetPos(100, 100);
  label_->SetPos(200, 100);
  label_->SetWidth(60);
  label_->SetText(QString::fromStdString("bene gesserit"));
}

void Test::OnEnter() {
}

void Test::OnExit() {
}
