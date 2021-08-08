// Copyright 2021 <CatchChallenger>
#ifndef CLIENT_V3_SCENES_SHARED_MONSTER_MONSTERITEM_HPP_
#define CLIENT_V3_SCENES_SHARED_MONSTER_MONSTERITEM_HPP_

#include "../../../../../general/base/GeneralStructures.hpp"
#include "../../../base/ConnectionManager.hpp"
#include "../../../core/Node.hpp"
#include "../../../core/Sprite.hpp"
#include "../../../ui/Button.hpp"
#include "../../../ui/Label.hpp"

namespace Scenes {
class MonsterItem : public Node {
 public:
  ~MonsterItem();
  static MonsterItem *Create(CatchChallenger::PlayerMonster monster);

  void Draw(QPainter *painter) override;
  void RegisterEvents() override;
  void UnRegisterEvents() override;

  void OnResize() override;

 private:
  Sprite *icon_;
  UI::Button *select_;
  CatchChallenger::PlayerMonster monster_;
  ConnectionManager *connection_manager_;
  CatchChallenger::Api_protocol_Qt *client_;
  QString name_;

  MonsterItem(CatchChallenger::PlayerMonster monster);
  void LoadMonster();
};
}  // namespace Scenes
#endif  // CLIENT_V3_SCENES_SHARED_MONSTER_MONSTERITEM_HPP_
