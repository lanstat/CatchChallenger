// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_SCENES_MAP_CCMAP_HPP_
#define CLIENT_V3_SCENES_MAP_CCMAP_HPP_

#include <vector>

#include "../../core/Scene.hpp"
#include "../../entities/render/MapController.hpp"

namespace Scenes {
class CCMap : public Scene {
 public:
  static CCMap* Create();
  MapController mapController;

  void PaintChildItems(std::vector<Node *> childItems, qreal parentX,
                       qreal parentY, QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr);
  void Paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *) override;

 private:
  QPointF lastClickedPos;
  bool clicked;

  qreal scale;
  qreal x;
  qreal y;
  CCMap();

  /*
  void pathFindingNotFound();
  void repelEffectIsOver() const;
  void teleportConditionNotRespected(const std::string &text) const;
  void send_player_direction(const CatchChallenger::Direction &the_direction);
  void stopped_in_front_of(CatchChallenger::Map_client *map, const uint8_t &x,
                           const uint8_t &y);
  void actionOn(CatchChallenger::Map_client *map, const uint8_t &x,
                const uint8_t &y);
  void actionOnNothing();
  void blockedOn(const MapVisualiserPlayer::BlockedOn &blockOnVar);
  void wildFightCollision(CatchChallenger::Map_client *map, const uint8_t &x,
                          const uint8_t &y);
  void botFightCollision(CatchChallenger::Map_client *map, const uint8_t &x,
                         const uint8_t &y, const uint16_t &fightId);
  void error(const std::string &error);
  void errorWithTheCurrentMap();
  void currentMapLoaded();
  */
};
}  // namespace Scenes
#endif  // CLIENT_V3_SCENES_MAP_CCMAP_HPP_
