// Copyright 2021 CatchChallenger
#include "EventManager.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

#include "Node.hpp"
#include "uthash.h"

typedef struct HashElement {
  Node *target;
  bool paused;
  bool must_remove;
  UT_hash_handle hh;
} HashElementType;

EventManager *EventManager::instance_ = nullptr;

EventManager::EventManager()
    : mouse_listeners_(nullptr), keyboard_listeners_(nullptr) {}

EventManager::~EventManager() {}

EventManager *EventManager::GetInstance() {
  if (instance_ == nullptr) instance_ = new EventManager();
  return instance_;
}

void EventManager::AddMouseListener(Node *listener) {
  mouse_listeners_ = AddListener(mouse_listeners_, listener, false);
}

void EventManager::AddKeyboardListener(Node *listener) {
  keyboard_listeners_ = AddListener(keyboard_listeners_, listener, false);
}

HashElementType *EventManager::AddListener(HashElementType *item,
                                           Node *listener, bool paused) {
  HashElementType *element = nullptr;
  HASH_FIND_PTR(item, &listener, element);
  if (!element) {
    element = (HashElementType *)calloc(sizeof(*element), 1);
    element->paused = paused;
    element->target = listener;
    HASH_ADD_PTR(item, target, element);
  }
  return item;
}

HashElementType *EventManager::DeleteHashElement(HashElementType *item,
                                                 HashElementType *element) {
  HASH_DEL(item, element);
  free(element);
  return item;
}

HashElementType *EventManager::RemoveListener(HashElementType *item,
                                              Node *listener) {
  if (listener == nullptr) {
    return item;
  }

  HashElementType *element = nullptr;
  HASH_FIND_PTR(item, &listener, element);
  if (element) {
    if (current_target_ == element) {
      current_target_salvaged_ = true;
    } else {
      item = DeleteHashElement(item, element);
    }
  }
  return item;
}

void EventManager::RemoveAllListeners() {
  for (HashElementType *elt = mouse_listeners_; elt != nullptr;) {
    mouse_listeners_ = DeleteHashElement(mouse_listeners_, elt);
    elt = (HashElementType *)(elt->hh.next);
  }
  for (HashElementType *elt = keyboard_listeners_; elt != nullptr;) {
    keyboard_listeners_ = DeleteHashElement(keyboard_listeners_, elt);
    elt = (HashElementType *)(elt->hh.next);
  }
}

void EventManager::RemoveListener(Node *listener) {
  mouse_listeners_ = RemoveListener(mouse_listeners_, listener);
  keyboard_listeners_ = RemoveListener(keyboard_listeners_, listener);
}

void EventManager::MousePressEvent(const QPointF &point,
                                   bool &press_validated) {
  for (HashElementType *elt = mouse_listeners_; elt != nullptr;) {
    current_target_ = elt;
    current_target_salvaged_ = false;

    if (!current_target_->paused) {
      current_target_->target->MousePressEvent(point, press_validated);
    }

    elt = (HashElementType *)(elt->hh.next);
    if (current_target_salvaged_) {
      mouse_listeners_ = DeleteHashElement(mouse_listeners_, current_target_);
    }
  }

  current_target_ = nullptr;
}

void EventManager::MouseReleaseEvent(const QPointF &point,
                                     bool &prev_validated) {
  for (HashElementType *elt = mouse_listeners_; elt != nullptr;) {
    current_target_ = elt;
    current_target_salvaged_ = false;

    if (!current_target_->paused) {
      current_target_->target->MouseReleaseEvent(point, prev_validated);
    }

    elt = (HashElementType *)(elt->hh.next);
    if (current_target_salvaged_) {
      mouse_listeners_ = DeleteHashElement(mouse_listeners_, current_target_);
    }
  }

  current_target_ = nullptr;
}

void EventManager::MouseMoveEvent(const QPointF &point) {
  for (HashElementType *elt = mouse_listeners_; elt != nullptr;) {
    current_target_ = elt;
    current_target_salvaged_ = false;

    if (!current_target_->paused) {
      current_target_->target->MouseMoveEvent(point);
    }

    elt = (HashElementType *)(elt->hh.next);
    if (current_target_salvaged_) {
      mouse_listeners_ = DeleteHashElement(mouse_listeners_, current_target_);
    }
  }

  current_target_ = nullptr;
}

void EventManager::KeyPressEvent(QKeyEvent *event, bool &event_trigger) {
  for (HashElementType *elt = keyboard_listeners_; elt != nullptr;) {
    current_target_ = elt;
    current_target_salvaged_ = false;

    if (!current_target_->paused) {
      current_target_->target->KeyPressEvent(event, event_trigger);
    }

    elt = (HashElementType *)(elt->hh.next);
    if (current_target_salvaged_) {
      keyboard_listeners_ =
          DeleteHashElement(keyboard_listeners_, current_target_);
    }
  }

  current_target_ = nullptr;
}

void EventManager::KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) {
  for (HashElementType *elt = keyboard_listeners_; elt != nullptr;) {
    current_target_ = elt;
    current_target_salvaged_ = false;

    if (!current_target_->paused) {
      current_target_->target->KeyReleaseEvent(event, event_trigger);
    }

    elt = (HashElementType *)(elt->hh.next);
    if (current_target_salvaged_) {
      keyboard_listeners_ =
          DeleteHashElement(keyboard_listeners_, current_target_);
    }
  }

  current_target_ = nullptr;
}

void EventManager::Lock() {
  for (HashElementType *elt = mouse_listeners_; elt != nullptr;) {
    elt->paused = true;

    elt = (HashElementType *)(elt->hh.next);
  }
  for (HashElementType *elt = keyboard_listeners_; elt != nullptr;) {
    elt->paused = true;

    elt = (HashElementType *)(elt->hh.next);
  }
}

void EventManager::UnLock() {
  for (HashElementType *elt = mouse_listeners_; elt != nullptr;) {
    elt->paused = false;

    elt = (HashElementType *)(elt->hh.next);
  }
  for (HashElementType *elt = keyboard_listeners_; elt != nullptr;) {
    elt->paused = false;

    elt = (HashElementType *)(elt->hh.next);
  }
}
