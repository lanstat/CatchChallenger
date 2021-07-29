// Copyright 2021 CatchChallenger
#ifndef OverMap_H
#define OverMap_H

#include <QObject>
#include <QTimer>
#include <string>
#include <vector>

#include "../../../../general/base/GeneralStructures.hpp"
#include "../../base/ConnectionManager.hpp"
#include "../../core/Scene.hpp"
#include "../../core/Sprite.hpp"
#include "../../ui/Button.hpp"
#include "../../ui/Combo.hpp"
#include "../../ui/Input.hpp"
#include "../../ui/Label.hpp"
#include "CCMap.hpp"
#include "MapMonsterPreview.hpp"
#include "MonsterDetails.hpp"

namespace Scenes {
class OverMap : public QObject, public Scene {
  Q_OBJECT

 public:
  enum ActionClan {
    ActionClan_Create,
    ActionClan_Leave,
    ActionClan_Dissolve,
    ActionClan_Invite,
    ActionClan_Eject
  };

  OverMap();
  ~OverMap();
  virtual void resetAll();
  virtual void setVar(CCMap *ccmap);
  void newLanguage();

  /*
  void mousePressEventXY(const QPointF &p, bool &pressValidated,
                         bool &callParentClass) override;
  void mouseReleaseEventXY(const QPointF &p, bool &pressValidated,
                           bool &callParentClass) override;
  void mouseMoveEventXY(const QPointF &p,
                        bool &pressValidated,
                        bool &callParentClass);
  void keyPressEvent(QKeyEvent *event, bool &eventTriggerGeneral) override;
  void keyReleaseEvent(QKeyEvent *event, bool &eventTriggerGeneral) override;
  */
  void lineEdit_chat_text_returnPressed();
  void buyClicked();
  void IG_dialog_close();
  void comboBox_chat_type_currentIndexChanged(int index);
  void new_system_text(const CatchChallenger::Chat_type &chat_type,
                       const std::string &text);
  void new_chat_text(CatchChallenger::Chat_type chat_type, std::string text,
                     std::string pseudo,
                     CatchChallenger::Player_type player_type);
  void removeNumberForFlood();

  void openMonster();
  void updatePlayerNumber(const uint16_t &number, const uint16_t &max_players);
  void have_current_player_info(
      const CatchChallenger::Player_private_and_public_informations
          &informations);

 protected:
  QTimer stopFlood;
  int numberForFlood;
  std::string lastMessageSend;
  ConnectionManager *connexionManager;
  MonsterDetails *monsterDetails;

  Sprite *playerBackground;
  bool playerBackgroundBig;
  Sprite *playerImage;
  UI::Label *name;
  UI::Label *cash;
  std::vector<MapMonsterPreview *> monsters;
  QPointF m_startPress;
  bool wasDragged;
  MapMonsterPreview *monstersDragged;

  Sprite *playersCountBack;
  UI::Label *playersCount;

  Sprite *chatBack;
  UI::Label *chatText;
  UI::Input *chatInput;
  UI::Combo *chatType;
  UI::Button *chat;
  UI::Label *chatOver;

  UI::Button *bag;
  UI::Label *bagOver;
  UI::Button *buy;
  UI::Label *buyOver;

  Sprite *gainBack;
  UI::Label *gain;
  QString gainString;

  Sprite *IG_dialog_textBack;
  UI::Label *IG_dialog_name;
  UI::Label *IG_dialog_text;
  UI::Button *IG_dialog_quit;
  QString IG_dialog_nameString;
  QString IG_dialog_textString;
  Sprite *labelSlow;

  Sprite *tipBack;
  UI::Label *tip;
  QString tipString;

  Sprite *persistant_tipBack;
  UI::Label *persistant_tip;
  QString persistant_tipString;

  std::vector<ActionClan> actionClan;
  std::string clanName;
  bool haveClanInformations;

  void OnScreenResize() override;

 private:
  void updateChat();
};
}  // namespace Scenes
#endif  // MULTI_H
