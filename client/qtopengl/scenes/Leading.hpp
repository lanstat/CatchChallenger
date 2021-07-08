// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_SCENES_LEADING_HPP_
#define CLIENT_QTOPENGL_SCENES_LEADING_HPP_

#include "../core/Scene.hpp"
#include "ParallaxForest.hpp"

class Leading : public Scene {
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
  Scene* foreground_;

  void OnLoadingFinish();
};

#endif  // CLIENT_QTOPENGL_SCENES_LEADING_HPP_
