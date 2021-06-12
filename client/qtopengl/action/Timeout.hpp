// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_ACTION_TIMEOUT_HPP_
#define CLIENT_QTOPENGL_ACTION_TIMEOUT_HPP_

#include "Action.hpp"

#include <cstdint>

class Timeout : public Action {
 public:
  ~Timeout();

  static Timeout* Create(int milliseconds);

  virtual void Step();
  virtual void Stop();
  virtual void Start();
  virtual bool IsDone();

 private:
  bool done_;

  uint64_t ellapsed_;
  int timeout_;

  Timeout();
  void CalculateDelta();
  void OnFinish();
};
#endif  // CLIENT_QTOPENGL_ACTION_TIMEOUT_HPP_
