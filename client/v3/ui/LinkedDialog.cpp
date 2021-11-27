// Copyright 2021 CatchChallenger
#include "LinkedDialog.hpp"

#include <iostream>

using std::placeholders::_1;
using UI::Dialog;
using UI::LinkedDialog;

LinkedDialog::LinkedDialog(bool show_navigation) : Dialog(true) {
  current_item_ = nullptr;
  on_next_ = nullptr;
  on_back_ = nullptr;
  on_use_item_ = nullptr;

  show_navigation_ = show_navigation;

  next_ = Button::Create(":/CC/images/interface/next.png");
  next_->SetOnClick(std::bind(&LinkedDialog::OnActionClick, this, _1));
  back_ = Button::Create(":/CC/images/interface/back.png");
  back_->SetOnClick(std::bind(&LinkedDialog::OnActionClick, this, _1));

  if (show_navigation_) {
    AddActionButton(back_);
    AddActionButton(next_);
  }
}

LinkedDialog::~LinkedDialog() {
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

LinkedDialog* LinkedDialog::Create(bool show_navigation) {
  return new (std::nothrow) LinkedDialog(show_navigation);
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
  if (node == next_) {
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

Node* LinkedDialog::CurrentItem() { return current_item_; }

Node* LinkedDialog::Item(int index) { return items_.at(index); }

void LinkedDialog::ShowNavigation(bool show) {
  if (show == show_navigation_) return;
  show_navigation_ = show;
  if (show_navigation_) {
    AddActionButton(next_);
    InsertActionButton(back_, 0);
  } else {
    RemoveActionButton(back_);
    RemoveActionButton(next_);
  }
}
