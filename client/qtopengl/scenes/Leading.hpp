// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_SCENES_LEADING_HPP_
#define CLIENT_QTOPENGL_SCENES_LEADING_HPP_

#include "../core/StackedScene.hpp"
#include "ParallaxForest.hpp"
#include "leading/Loading.hpp"
#include "leading/Menu.hpp"

namespace Scenes {
class Leading : public StackedScene {
 public:
  static Leading* Create();
  ~Leading();

 protected:
  void OnScreenSD() override;
  void OnScreenHD() override;
  void OnScreenHDR() override;
  void OnScreenResize() override;

 private:
  Leading();

  ParallaxForest* background_;
  Menu* menu_;
  Loading* loader_;

  void OnLoadingFinish();
};
}  // namespace Scenes
#endif  // CLIENT_QTOPENGL_SCENES_LEADING_HPP_
