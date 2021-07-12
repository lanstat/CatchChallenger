// Copyright 2021 CatchChallenger
#include "Leading.hpp"

#include <iostream>

#include "../core/AssetsLoader.hpp"
#include "ParallaxForest.hpp"
#include "leading/Loading.hpp"
#include "leading/Menu.hpp"

using Scenes::Leading;

Leading::Leading() {
  background_ = ParallaxForest::Create();
  SetBackground(background_);

  loader_ = Loading::Create();

  loader_->SetOnFinished(std::bind(&Leading::OnLoadingFinish, this));
  PushForeground(loader_);
}

Leading::~Leading() {
  delete background_;
  delete loader_;
  delete menu_;
}

Leading* Leading::Create() {
  Leading* instance = new (std::nothrow) Leading();
  return instance;
}

void Leading::OnLoadingFinish() {
  PopForeground();
  menu_ = Menu::Create();
  PushForeground(menu_);
}

void Leading::OnScreenSD() {}

void Leading::OnScreenHD() {}

void Leading::OnScreenHDR() {}

void Leading::OnScreenResize() {}
