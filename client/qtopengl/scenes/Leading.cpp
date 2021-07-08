// Copyright 2021 CatchChallenger
#include "Leading.hpp"

#include <iostream>

#include "../core/AssetsLoader.hpp"
#include "ParallaxForest.hpp"
#include "leading/Loading.hpp"
#include "leading/Menu.hpp"

Leading::Leading() {
  background_ = ParallaxForest::Create();
  AddChild(background_);

  foreground_ = Loading::Create();

  static_cast<Loading*>(foreground_)
      ->SetOnFinished(std::bind(&Leading::OnLoadingFinish, this));
  AddChild(foreground_);
}

Leading::~Leading() {
  delete background_;
  delete foreground_;
}

Leading* Leading::Create() {
  Leading* instance = new (std::nothrow) Leading();
  return instance;
}

void Leading::OnLoadingFinish() {
  RemoveChild(foreground_);
  foreground_ = Menu::Create();
  AddChild(foreground_);
}

void Leading::OnScreenSD() {}

void Leading::OnScreenHD() {}

void Leading::OnScreenHDR() {}

void Leading::OnScreenResize() {}
