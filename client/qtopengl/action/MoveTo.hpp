// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_ACTION_MOVETO_HPP_
#define CLIENT_QTOPENGL_ACTION_MOVETO_HPP_

#include "Action.hpp"

class MoveTo : public Action {
 public:
  ~MoveTo();

  static MoveTo* Create(int milliseconds, int x, int y);

  virtual void Step();
  virtual void Stop();
  virtual void Start();
  virtual bool IsDone();

 private:
  bool done_;

  int delta_x_;
  int delta_y_;
  int end_x_;
  int end_y_;
  int milliseconds_;

  MoveTo();
  void CalculateDelta();
  void OnFinish();
};
#endif  // CLIENT_QTOPENGL_ACTION_MOVETO_HPP_
