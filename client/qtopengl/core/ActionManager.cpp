// Copyright 2021 CatchChallenger
#include "ActionManager.hpp"

#include <iostream>
#include <vector>

#include "../action/Action.hpp"
#include "Node.hpp"
#include "uthash.h"

typedef struct HashElement {
  Node *target;
  std::vector<Action *> *actions;
  Action *current_action;
  bool paused;
  bool current_action_salvaged;
  UT_hash_handle hh;
} HashElementType;

ActionManager *ActionManager::instance_ = nullptr;

ActionManager::ActionManager() : targets_(nullptr), current_target_(nullptr) {}

ActionManager::~ActionManager() {}

ActionManager *ActionManager::GetInstance() {
  if (instance_ == nullptr) instance_ = new ActionManager();
  return instance_;
}

void ActionManager::AddAction(Action *action, Node *target, bool paused) {
  if (action == nullptr || target == nullptr) return;

  HashElementType *element = nullptr;
  HASH_FIND_PTR(targets_, &target, element);
  if (!element) {
    element = (HashElementType *)calloc(sizeof(*element), 1);
    element->paused = paused;
    element->target = target;
    element->actions = new std::vector<Action *>();
    HASH_ADD_PTR(targets_, target, element);
  }

  element->actions->push_back(action);
  action->SetTarget(target);
}

Action *ActionManager::GetActionByTag(int tag, Node *target) {
  HashElementType *element = nullptr;
  HASH_FIND_PTR(targets_, &target, element);

  if (element) {
    if (element->actions != nullptr) {
      auto length = element->actions->size();
      uint index = 0;
      while (index < length) {
        Action *action = element->actions->at(index);
        if (action->Tag() == tag) {
          return action;
        }
        index++;
      }
    }
  }

  return nullptr;
}

std::vector<Action *> *ActionManager::GetAllActionsByTarget(Node *target) {
  HashElementType *element = nullptr;
  HASH_FIND_PTR(targets_, &target, element);

  if (element) {
    if (element->actions != nullptr) {
      return element->actions;
    }
  }

  return nullptr;
}

void ActionManager::RemoveAllActions() {}

void ActionManager::RemoveAllActionFromTarget(Node *target) { (void)target; }

void ActionManager::Update() {
  for (HashElementType *elt = targets_; elt != nullptr;) {
    current_target_ = elt;
    current_target_salvaged_ = false;

    if (!current_target_->paused) {
      auto length = elt->actions->size();
      uint index = 0;
      while (index < length) {
        Action *action = elt->actions->at(index);
        current_target_->current_action = action;
        if (current_target_->current_action == nullptr) {
          continue;
        }
        current_target_->current_action_salvaged = false;
        current_target_->current_action->Step();

        /*
        if (current_target_->current_action_salvaged) {
          //          current_target_->current_action->release();
        } else if (current_target_->current_action->IsDone()) {
          current_target_->current_action->Stop();

          Action *action = current_target_->current_action;
          current_target_->current_action = nullptr;
          // removeAction(action);
        }*/

        current_target_->current_action = nullptr;
        index++;
      }
    }

    elt = (HashElementType *)(elt->hh.next);
  }

  current_target_ = nullptr;
}
