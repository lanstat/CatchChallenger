// Copyright 2021 CatchChallenger
#include "Row.hpp"

#include <iostream>

#include "../core/Node.hpp"

using UI::Row;

Row::Row(Node *parent) : Node(parent) {
  node_type_ = __func__;
  is_processing_resize_ = false;
  spacing_ = 0;
  mode_ = Mode::kDefault;
}

Row::~Row() {
  for (auto item : children_) {
    delete item;
  }
  children_.clear();
}

Row *Row::Create(Node *parent) {
  Row *instance = new (std::nothrow) Row(parent);
  return instance;
}

void Row::ReCalculateSize() {
  if (children_.size() == 0) return;
  int x = 0;
  int height = 0;
  int width = 0;
  for (auto item : children_) {
    if (mode_ == Mode::kDisplayNoneVisible) {
      if (!item->IsVisible()) continue;
    }
    item->SetX(x);
    item->SetY(0);
    width = item->Width() + x;
    if (item->Height() > height) {
      height = item->Height();
    }
    x = x + item->Width() + spacing_;
  }

  SetSize(width, height);
}

void Row::SetItemSpacing(int value) {
  spacing_ = value;
  ReCalculateSize();
}

void Row::OnChildResize(Node *node) {
  if (is_processing_resize_) return;
  is_processing_resize_ = true;
  ReCalculateSize();
  is_processing_resize_ = false;
}

void Row::AddChild(Node *node) {
  Node::AddChild(node);
  ReCalculateSize();
}

void Row::RemoveChild(Node *node) {
  Node::RemoveChild(node);
  ReCalculateSize();
}

void Row::Draw(QPainter *painter) {}

void Row::SetMode(Mode mode) {
  if (mode_ == mode) return;
  mode_ = mode;
  ReCalculateSize();
}

void Row::InsertChild(Node *node, int pos) {
  auto iter = children_.begin();
  int index = 0;
  while (index < children_.size()) {
    index++;
    ++iter;
  }
  children_.insert(iter, node);
  ReCalculateSize();
}
