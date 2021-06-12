// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_CORE_ACTIONMANAGER_HPP_
#define CLIENT_QTOPENGL_CORE_ACTIONMANAGER_HPP_

#include <vector>

class Action;
class HashElement;
class Node;

class ActionManager {
 public:
  ~ActionManager();

  static ActionManager *GetInstance();
  void AddAction(Action *action, Node *target, bool paused);
  void RemoveAllActions();
  void RemoveAllActionFromTarget(Node *target);
  Action *GetActionByTag(int tag, Node *target);
  std::vector<Action *> *GetAllActionsByTarget(Node *target);
  void Update();

 private:
  ActionManager();

 protected:
  static ActionManager *instance_;
  struct HashElement *targets_;
  struct HashElement *current_target_;
  bool current_target_salvaged_;
};
#endif  // CLIENT_QTOPENGL_CORE_ACTIONMANAGER_HPP_
