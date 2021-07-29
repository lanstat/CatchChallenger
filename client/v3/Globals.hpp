// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_GLOBALS_HPP_
#define CLIENT_QTOPENGL_GLOBALS_HPP_

#include <QThread>

#include "../qt/solo/InternalServer.hpp"
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

 private:
  static Scenes::Leading* leading_instance_;
  static Scenes::Map* map_instance_;
};

#endif  // CLIENT_QTOPENGL_GLOBALS_HPP_
