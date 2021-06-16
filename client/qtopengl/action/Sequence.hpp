// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_ACTION_SEQUENCE_HPP_
#define CLIENT_QTOPENGL_ACTION_SEQUENCE_HPP_

#include <vector>

#include "Action.hpp"

using std::vector;

class Sequence : public Action {
 public:
  ~Sequence();

  static Sequence* Create(const vector<Action*>& actions);

  void Step() override;
  void Stop() override;
  void Start() override;
  bool IsDone() override;

 private:
  bool done_;
  vector<Action*> actions_;
  unsigned int current_;

  Sequence();
  void OnFinish();
};
#endif  // CLIENT_QTOPENGL_ACTION_SEQUENCE_HPP_ 
