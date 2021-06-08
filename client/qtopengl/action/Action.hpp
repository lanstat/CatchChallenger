// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_ACTION_ACTION_HPP_
#define CLIENT_QTOPENGL_ACTION_ACTION_HPP_

class Node;

class Action {
 public:
  ~Action();
  void SetTarget(Node *node);
  Node *Target();
  void SetTag(int tag);
  int Tag();
  virtual void Step() {}
  virtual void Stop() {}
  virtual void Start(bool animate = true, bool trigger_done = true) = 0;
  virtual bool IsDone() = 0;

 protected:
  Node *node_;
  bool trigger_done_;

  Action();

 private:
  int tag_;
};
#endif  // CLIENT_QTOPENGL_ACTION_ACTION_HPP_
