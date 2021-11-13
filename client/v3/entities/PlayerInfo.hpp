// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_ENTITIES_PLAYERINFO_HPP_
#define CLIENT_V3_ENTITIES_PLAYERINFO_HPP_

#include <QObject>
#include <functional>
#include <string>

#include "../../../general/base/GeneralStructures.hpp"
#include "../base/ConnectionManager.hpp"

class PlayerInfo {
 public:
  static PlayerInfo* GetInstance();

  std::string ClanName;
  bool HaveClanInformation;

  ~PlayerInfo();
  CatchChallenger::Player_private_and_public_informations& GetInformation();
  const CatchChallenger::Player_private_and_public_informations&
  GetInformationRO();
  void Notify();

 private:
  static PlayerInfo* instance_;
  ConnectionManager* connection_manager_;

  PlayerInfo();

 //signals:
  //void OnUpdateInfo(PlayerInfo* info);
};
#endif  // CLIENT_V3_ENTITIES_PLAYERINFO_HPP_
