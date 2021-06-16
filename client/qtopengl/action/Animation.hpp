// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_ACTION_ANIMATION_HPP_
#define CLIENT_QTOPENGL_ACTION_ANIMATION_HPP_

#include <vector>
#include <cstdint>

#include "Action.hpp"

class QPixmap;

class Animation : public Action {
 public:
  ~Animation();

  static Animation* Create(std::vector<QPixmap> frames, int delay,
                           unsigned int loop = 1);

  void SetFrames(std::vector<QPixmap> frames);

  void Step() override;
  void Stop() override;
  void Start() override;
  bool IsDone() override;

 private:
  bool done_;

  std::vector<QPixmap> frames_;
  uint8_t index_;
  uint64_t ellapsed_;
  int delay_;
  unsigned int loop_;

  Animation();
  void OnFinish();
};
#endif  // CLIENT_QTOPENGL_ACTION_ANIMATION_HPP_
