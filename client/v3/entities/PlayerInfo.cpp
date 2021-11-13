// Copyright 2021 CatchChallenger
#include "PlayerInfo.hpp"

PlayerInfo* PlayerInfo::instance_ = nullptr;

PlayerInfo::PlayerInfo() { 
  HaveClanInformation = false; 
  connection_manager_ = ConnectionManager::GetInstance();
}

PlayerInfo::~PlayerInfo() {}

PlayerInfo* PlayerInfo::GetInstance() {
  if (instance_ == nullptr) {
    instance_ = new PlayerInfo();
  }
  return instance_;
}

CatchChallenger::Player_private_and_public_informations
&PlayerInfo::GetInformation() {
  return connection_manager_->client->get_player_informations();
}

const CatchChallenger::Player_private_and_public_informations&
PlayerInfo::GetInformationRO() {
  return connection_manager_->client->get_player_informations_ro();
}

void PlayerInfo::Notify() {
  //emit OnUpdateInfo(this);
}
