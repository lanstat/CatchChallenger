// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_GLOBALS_HPP_
#define CLIENT_QTOPENGL_GLOBALS_HPP_

#include <QThread>

#include "../qt/solo/InternalServer.hpp"
#include "scenes/battle/Battle.hpp"
#include "scenes/leading/Leading.hpp"
#include "scenes/map/Map.hpp"

class Globals {
 public:
  static CatchChallenger::InternalServer* InternalServer;
#ifndef NOTHREADS
  static QThread* ThreadSolo;
#endif
  static Scenes::Leading* GetLeadingScene();
  static Scenes::Map* GetMapScene();
  static Scenes::Battle* GetBattleScene();

 private:
  static Scenes::Leading* leading_;
  static Scenes::Map* map_;
  static Scenes::Battle* battle_;
};

#endif  // CLIENT_QTOPENGL_GLOBALS_HPP_
