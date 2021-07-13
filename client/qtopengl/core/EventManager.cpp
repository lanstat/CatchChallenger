// Copyright 2021 CatchChallenger
#include "EventManager.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

#include "Node.hpp"
#include "uthash.h"

EventManager *EventManager::instance_ = nullptr;

EventManager::EventManager() {}

EventManager::~EventManager() {}

EventManager *EventManager::GetInstance() {
  if (instance_ == nullptr) instance_ = new EventManager();
  return instance_;
}

void EventManager::AddMouseListener(Node *listener) {
  AddListener(&mouse_listeners_, listener);
}

void EventManager::AddKeyboardListener(Node *listener) {
  AddListener(&keyboard_listeners_, listener);
}

void EventManager::AddListener(std::vector<Node *> *item, Node *listener) {
  item->push_back(listener);
}

void EventManager::RemoveAllListeners() {
  mouse_listeners_.clear();
  keyboard_listeners_.clear();
}

void EventManager::RemoveListener(Node *listener) {
  RemoveListener(&mouse_listeners_, listener);
  RemoveListener(&keyboard_listeners_, listener);
}

void EventManager::RemoveListener(std::vector<Node *> *item, Node *listener) {
  auto length = item->size();
  uint index = 0;
  while (index < length) {
    if (item->at(index) == listener) {
      item->erase(item->cbegin() + index);
      break;
    }
  }
}

void EventManager::MousePressEvent(const QPointF &point,
                                   bool &press_validated) {
  auto length = mouse_listeners_.size();
  uint index = 0;
  while (index < length) {
    Node *node = mouse_listeners_.at(index);
    node->MousePressEvent(point, press_validated);
    index++;
  }
}

void EventManager::MouseReleaseEvent(const QPointF &point,
                                     bool &prev_validated) {
  auto length = mouse_listeners_.size();
  uint index = 0;
  while (index < length) {
    Node *node = mouse_listeners_.at(index);
    node->MouseReleaseEvent(point, prev_validated);
    index++;
  }
}

void EventManager::MouseMoveEvent(const QPointF &point) {
  auto length = mouse_listeners_.size();
  uint index = 0;
  while (index < length) {
    mouse_listeners_.at(index)->MouseMoveEvent(point);
    index++;
  }
}

void EventManager::KeyPressEvent(QKeyEvent *event, bool &event_trigger) {
  auto length = keyboard_listeners_.size();
  uint index = 0;
  while (index < length) {
    keyboard_listeners_.at(index)->KeyPressEvent(event, event_trigger);
    index++;
  }
}

void EventManager::KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) {
  auto length = keyboard_listeners_.size();
  uint index = 0;
  while (index < length) {
    keyboard_listeners_.at(index)->KeyReleaseEvent(event, event_trigger);
    index++;
  }
}

void EventManager::Lock() {}

void EventManager::UnLock() {}
