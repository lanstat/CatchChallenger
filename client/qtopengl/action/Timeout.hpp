// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_ACTION_TIMEOUT_HPP_
#define CLIENT_QTOPENGL_ACTION_TIMEOUT_HPP_

#include "Action.hpp"

#include <cstdint>

class Timeout : public Action {
 public:
  ~Timeout();

  static Timeout* Create(int milliseconds);

  void Step() override;
  void Stop() override;
  void Start() override;
  bool IsDone() override;

 private:
  bool done_;

  uint64_t ellapsed_;
  uint64_t timeout_;

  Timeout();
  void CalculateDelta();
  void OnFinish();
};
#endif  // CLIENT_QTOPENGL_ACTION_TIMEOUT_HPP_
