// Copyright 2021 CatchChallenger
#include "StackedScene.hpp"

#include <iostream>

#include "Node.hpp"
#include "Scene.hpp"

StackedScene::StackedScene(Node *parent) : Scene(parent) {
  background_ = nullptr;
  foreground_.clear();
}

StackedScene::~StackedScene() {}

void StackedScene::SetBackground(Scene *scene) {
  RemoveChild(background_);
  background_ = scene;
  AddChild(background_);
}

void StackedScene::PushForeground(Scene *scene) {
  if (!foreground_.empty()) RemoveChild(foreground_.back());
  foreground_.push_back(scene);
  AddChild(scene);
}

Scene *StackedScene::PopForeground() {
  if (foreground_.empty()) return nullptr;
  auto tmp = foreground_.back();

  RemoveChild(tmp);
  foreground_.pop_back();

  if (!foreground_.empty()) AddChild(foreground_.back());

  return tmp;
}
