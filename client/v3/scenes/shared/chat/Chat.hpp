// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_SCENES_SHARED_CHAT_CHAT_HPP_
#define CLIENT_V3_SCENES_SHARED_CHAT_CHAT_HPP_

#include <QObject>
#include <QTimer>

#include "../../../../../general/base/GeneralStructures.hpp"
#include "../../../base/ConnectionManager.hpp"
#include "../../../core/Node.hpp"
#include "../../../core/Sprite.hpp"
#include "../../../ui/Button.hpp"
#include "../../../ui/Combo.hpp"
#include "../../../ui/Input.hpp"
#include "../../../ui/Label.hpp"

namespace Scenes {
class Chat : public QObject, public Node {
 public:
  static Chat *Create();
  void OnEnter() override;
  void OnExit() override;
  void OnScreenResize() override;

 public slots:
  void updateTheTurtle();
  void detectSlowDown();
  void lastReplyTime(const uint32_t &time);

 private:
  Sprite *chatBack;
  UI::Label *chatText;
  UI::Input *chatInput;
  UI::Combo *chatType;
  UI::Button *chat;
  UI::Label *chatOver;
  Sprite *labelSlow;
  int lastReplyTimeValue;
  QTime lastReplyTimeSince;

  QTimer stopFlood;
  int numberForFlood;
  std::string lastMessageSend;
  ConnectionManager *connexionManager;

  Chat();
  void setVar();
  void updateChat();
  void comboBox_chat_type_currentIndexChanged(int index);
  void new_chat_text(CatchChallenger::Chat_type chat_type, std::string text,
                     std::string pseudo,
                     CatchChallenger::Player_type player_type);
  void lineEdit_chat_text_returnPressed();
  void removeNumberForFlood();
  void new_system_text(const CatchChallenger::Chat_type &chat_type,
                       const std::string &text);
}
}  // namespace Scenes

#endif  // CLIENT_V3_SCENES_SHARED_CHAT_CHAT_HPP_
