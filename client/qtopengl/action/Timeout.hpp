// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_ACTION_TIMEOUT_HPP_
#define CLIENT_QTOPENGL_ACTION_TIMEOUT_HPP_

#include "Action.hpp"

class Timeout : public Action {
 public:
  ~Timeout();

  static Timeout* Create(int milliseconds);

  void Step(unsigned int ellapsed) override;
  void Stop() override;
  void Start() override;
  bool IsDone() override;

 private:
  bool done_;

  unsigned int ellapsed_;
  unsigned int timeout_;

  Timeout();
  void OnFinish();
};
#endif  // CLIENT_QTOPENGL_ACTION_TIMEOUT_HPP_
