// Copyright 2021 CatchChallenger
#include "Utils.hpp"

#include <iostream>
#include <typeinfo>

Node *Utils::GetCurrentScene(Node *node) {
  Node *parent = node;
  while (parent != nullptr) {
    if (parent->ClassName() == "Scene") {
      break;
    }
    parent = parent->Parent();
  }
  return parent;
}
