// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_ENTITIES_PLAYERINFO_HPP_
#define CLIENT_V3_ENTITIES_PLAYERINFO_HPP_

#include <string>
#include <functional>

class PlayerInfo {
 public:
  static PlayerInfo* GetInstance();
  std::string ClanName;
  bool HaveClanInformation;
  std::string PlayerName;
  int Money;

  void Notify();
  void AddObserver(std::function<void()> callback);

 private:
  static PlayerInfo* instance_;

  PlayerInfo();
};
#endif  // CLIENT_V3_ENTITIES_PLAYERINFO_HPP_
