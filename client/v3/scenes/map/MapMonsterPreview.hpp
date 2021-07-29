// Copyright 2021 CatchChallenger
#ifndef MapMonsterPreview_H
#define MapMonsterPreview_H

#include <QObject>

#include "../../../../general/base/GeneralStructures.hpp"
#include "../../core/Sprite.hpp"

namespace Scenes {
class MapMonsterPreview : public QObject, public Sprite {
  Q_OBJECT
 public:
  MapMonsterPreview(const CatchChallenger::PlayerMonster &m,
                    Node *parent = nullptr);
  ~MapMonsterPreview();
  void regenCache();
  void setInDrag(bool drag);
  bool isPressed();
  const CatchChallenger::PlayerMonster &getMonster() const;

 private:
  void setPressed(const bool &pressed);

 private:
  CatchChallenger::PlayerMonster monster;
  bool pressed;
  bool m_drag;
 signals:
  void clicked();
};
}  // namespace Scenes
#endif  // MapMonsterPreview_H
