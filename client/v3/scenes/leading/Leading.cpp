// Copyright 2021 CatchChallenger
#include "Leading.hpp"

#include <iostream>
#include <string>

using Scenes::Leading;

Leading::Leading() {
  background_ = ParallaxForest::Create();
  SetBackground(background_);

  menu_ = Menu::Create();
  PushForeground(menu_);
}

Leading::~Leading() {
  delete background_;
  delete menu_;
}

Leading* Leading::Create() {
  Leading* instance = new (std::nothrow) Leading();
  return instance;
}
