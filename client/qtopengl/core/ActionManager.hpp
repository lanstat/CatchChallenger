// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_CORE_ACTIONMANAGER_HPP_
#define CLIENT_QTOPENGL_CORE_ACTIONMANAGER_HPP_

#include <vector>
#include <cstdint>

class Action;
class HashElement;
class Node;

class ActionManager {
 public:
  ~ActionManager();

  static ActionManager *GetInstance();
  void AddAction(Action *action, Node *target, bool paused);
  void RemoveAllActions();
  void RemoveAction(Action *action);
  void RemoveAllActionsFromTarget(Node *target);
  void RemoveActionAtIndex(unsigned int index, HashElement *element);
  Action *GetActionByTag(int tag, Node *target);
  std::vector<Action *> *GetAllActionsByTarget(Node *target);
  void Update();

 private:
  ActionManager();
  void DeleteHashElement(HashElement *element);

 protected:
  static ActionManager *instance_;
  struct HashElement *targets_;
  struct HashElement *current_target_;
  bool current_target_salvaged_;
  uint64_t last_execution_;
};
#endif  // CLIENT_QTOPENGL_CORE_ACTIONMANAGER_HPP_
