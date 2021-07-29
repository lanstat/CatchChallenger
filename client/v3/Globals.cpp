// Copyright 2021 CatchChallenger
#include "Globals.hpp"

CatchChallenger::InternalServer *Globals::InternalServer = nullptr;

QThread *Globals::ThreadSolo = nullptr;

Scenes::Leading *Globals::leading_instance_ = nullptr;

Scenes::Map *Globals::map_instance_ = nullptr;

Scenes::Leading *Globals::GetLeadingScene() {
  if (!leading_instance_) {
    leading_instance_ = Scenes::Leading::Create();
  }
  return leading_instance_;
}

Scenes::Map *Globals::GetMapScene() {
  if (!map_instance_) {
    map_instance_ = Scenes::Map::Create();
  }
  return map_instance_;
}
