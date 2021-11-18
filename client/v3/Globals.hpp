// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_GLOBALS_HPP_
#define CLIENT_V3_GLOBALS_HPP_

#include <QThread>

#include "entities/solo/InternalServer.hpp"
#include "scenes/battle/Battle.hpp"
#include "scenes/leading/Leading.hpp"
#include "scenes/map/Map.hpp"
#include "ui/MessageDialog.hpp"

class Globals {
 public:
  static CatchChallenger::InternalServer* InternalServer;
#ifndef NOTHREADS
  static QThread* ThreadSolo;
#endif
  static Scenes::Leading* GetLeadingScene();
  static Scenes::Map* GetMapScene();
  static Scenes::Battle* GetBattleScene();
  static UI::MessageDialog* GetAlertDialog();

 private:
  static Scenes::Leading* leading_;
  static Scenes::Map* map_;
  static Scenes::Battle* battle_;
  static UI::MessageDialog *alert_;
};

#endif  // CLIENT_V3_GLOBALS_HPP_
