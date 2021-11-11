// Copyright 2021 CatchChallenger
#include "LinkedDialog.hpp"

#include <iostream>

using UI::LinkedDialog;

LinkedDialog::LinkedDialog() : Dialog() {
  current_item_ = nullptr;
  on_next_ = nullptr;
  on_back_ = nullptr;
  on_use_item_ = nullptr;

  quit_ = Button::Create(":/CC/images/interface/cancel.png", this);
  next_ = Button::Create(":/CC/images/interface/next.png", this);
  back_ = Button::Create(":/CC/images/interface/back.png", this);

  AddActionButton(back_);
  AddActionButton(next_);
  AddActionButton(quit_);
}

LinkedDialog::~LinkedDialog() {
  delete quit_;
  quit_ = nullptr;
  delete next_;
  next_ = nullptr;
  delete back_;
  back_ = nullptr;
  for (auto item : items_) {
    delete item;
  }
  items_.clear();
  current_item_ = nullptr;
}

LinkedDialog* LinkedDialog::Create() {
  return new (std::nothrow) LinkedDialog();
}

void LinkedDialog::OnScreenResize() {
  Dialog::OnScreenResize();

  auto boundary = ContentBoundary();
  for (auto item : items_) {
    item->SetPos(boundary.x(), boundary.y());
    item->SetSize(boundary.width(), boundary.height());
  }
}

void LinkedDialog::AddItem(Node* item) {
  if (current_item_ == nullptr) {
    current_item_ = item;
  }

  auto boundary = ContentBoundary();
  item->SetPos(boundary.x(), boundary.y());
  item->SetSize(boundary.width(), boundary.height());
  items_.push_back(item);
}

void LinkedDialog::RemoveItem(Node* item) {}

void LinkedDialog::SetOnNext(std::function<void(int)> callback) {
  on_next_ = callback;
}

void LinkedDialog::SetOnBack(std::function<void(int)> callback) {
  on_back_ = callback;
}

void LinkedDialog::SetOnUseItem(std::function<void(uint16_t)> callback) {
  on_use_item_ = callback;
}

void LinkedDialog::OnActionClick(Node* node) {
  if (node == quit_) {
    Close();
    Parent()->RemoveChild(this);
    return;
  } else if (node == next_) {
    current_index_++;
    if (current_index_ >= items_.size()) {
      current_index_ = 0;
    }
    if (on_next_) on_next_(current_index_);
  } else if (node == back_) {
    current_index_--;
    if (current_index_ < 0) {
      current_index_ = items_.size() - 1;
    }
    if (on_back_) on_back_(current_index_);
  }
  if (current_item_ != nullptr) {
    current_item_->UnRegisterEvents();
    RemoveChild(current_item_);
  }
  current_item_ = items_.at(current_index_);
  AddChild(current_item_);
  current_item_->RegisterEvents();
}

void LinkedDialog::OnEnter() {
  UI::Dialog::OnEnter();
  if (current_item_ != nullptr) {
    current_item_->RegisterEvents();
  }
}

void LinkedDialog::OnExit() {
  if (current_item_ != nullptr) {
    current_item_->UnRegisterEvents();
  }
  UI::Dialog::OnExit();
}

void LinkedDialog::SetCurrentItem(int index) {
  if (current_item_ != nullptr) {
    current_item_->UnRegisterEvents();
    RemoveChild(current_item_);
  }
  current_index_ = index;
  current_item_ = items_.at(current_index_);
  // Only register if linkeddialog is showed
  if (Parent() != nullptr) {
    current_item_->RegisterEvents();
  }
  AddChild(current_item_);
}

Node* LinkedDialog::CurrentItem() {
  return current_item_;
}

Node* LinkedDialog::Item(int index) {
  return items_.at(index);
}
