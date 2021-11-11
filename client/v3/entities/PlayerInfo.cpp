// Copyright 2021 CatchChallenger
#include "PlayerInfo.hpp"

PlayerInfo *PlayerInfo::instance_ = nullptr;

PlayerInfo::PlayerInfo() {
  HaveClanInformation = false;
}

PlayerInfo* PlayerInfo::GetInstance() {
  if (instance_ == nullptr) {
    instance_ = new PlayerInfo();
  }
  return instance_;
}

void PlayerInfo::Notify() {

}

void PlayerInfo::AddObserver(std::function<void()> callback) {

}
