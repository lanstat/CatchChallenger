// Copyright 2021 CatchChallenger
#ifndef MONSTERDETAILS_H
#define MONSTERDETAILS_H

#include "../../../general/base/GeneralStructures.hpp"
#include "../../core/Scene.hpp"
#include "../../core/Sprite.hpp"
#include "../../ui/Button.hpp"
#include "../../ui/Label.hpp"
#include "../../ui/ListView.hpp"
#include "../../ui/Progressbar.hpp"

namespace Scenes {
class MonsterDetails : public Scene {
 public:
  MonsterDetails();
  void setVar(const CatchChallenger::PlayerMonster &monster);
  void newLanguage();
  void Draw(QPainter *painter) override;

 private:
  UI::Button *quit;
  UI::Label *details;

  Sprite *monsterDetailsImage;

  UI::ListView *monsterDetailsSkills;

  Sprite *monsterDetailsCatched;
  UI::Label *monsterDetailsName;
  UI::Label *monsterDetailsLevel;
  UI::Label *hp_text;
  UI::Progressbar *hp_bar;
  UI::Label *xp_text;
  UI::Progressbar *xp_bar;
};
}  // namespace Scenes
#endif  // MONSTERDETAILS_H
