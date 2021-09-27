// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_SCENES_TEST_TEST_HPP_
#define CLIENT_V3_SCENES_TEST_TEST_HPP_

#include "../../core/Scene.hpp"
#include "../../core/Sprite.hpp"
#include "../../ui/Label.hpp"

namespace Scenes {
class Test : public Scene {
 public:
  static Test *Create();
  ~Test();
  void OnEnter() override;
  void OnExit() override;

 protected:
  void OnScreenResize() override;

 private:
  Test();
  Sprite *sprite_;
  UI::Label *label_;
};
}  // namespace Scenes
#endif  // CLIENT_V3_SCENES_TEST_TEST_HPP_
