// Copyright 2021 CatchChallenger
#include "Chat.hpp"

using Scenes::Chat;

Chat::Chat() {
  chatBack = Sprite::Create(":/CC/images/interface/chatBackground.png", this);
  chatText = UI::Label::Create(this);
  chatInput = UI::Input::Create(this);
  chatType = UI::Combo::Create(":/CC/images/interface/button.png", this);
  chatType->SetZValue(5);
  chat = UI::Button::Create(":/CC/images/interface/chat.png", this);
  chatOver = UI::Label::Create(QColor(255, 255, 255), QColor(80, 71, 38), this);
  chat->SetCheckable(true);
  labelSlow = Sprite::Create(":/CC/images/multi/slow.png", this);


  lastReplyTimeValue = 0;

  if (!connect(&checkQueryTime, &QTimer::timeout, this,
               &OverMapLogic::detectSlowDown))
    abort();

  if (!connect(&stopFlood, &QTimer::timeout, this,
               &OverMap::removeNumberForFlood, Qt::QueuedConnection))
    abort();
  if (!connect(
          chatType,
          static_cast<void (ComboBox::*)(int)>(&ComboBox::currentIndexChanged),
          this, &OverMap::comboBox_chat_type_currentIndexChanged))
    abort();
  if (!connect(chatInput, &LineEdit::returnPressed, this,
               &OverMap::lineEdit_chat_text_returnPressed))
    abort();
  if (!connect(IG_dialog_quit, &CustomButton::clicked, this,
               &OverMap::IG_dialog_close))
    abort();

  stopFlood.setSingleShot(false);
  stopFlood.start(1500);
}

void Chat::newLanguage() {
  chatOver->SetText(tr("Chat"));
  /*
  chatType->setItemText(0, tr("All"));
  chatType->setItemText(1, tr("Local"));
  if (chatType->count() > 2) chatType->setItemText(2, tr("Clan"));
  */
}

void Chat::resetAll() {
  lastReplyTimeValue = 0;
}

void Chat::setVar() {
  lastMessageSend.clear();

  chatType->Clear();
  chatType->AddItem(tr("All"));
  // chatType->setItemData(0, 0, 99);
  chatType->AddItem(tr("Local"));
  // chatType->setItemData(1, 1, 99);
  /*if(haveClan)
  {
      chatType->addItem(tr("Clan"));
      chatType->setItemData(2,2,99);
  }*/
  numberForFlood = 0;
  if (!connect(connexionManager->client,
               &CatchChallenger::Api_client_real::QtlastReplyTime, this,
               &OverMapLogic::lastReplyTime, Qt::UniqueConnection))
    abort();
  connect(connexionManager->client,
          &CatchChallenger::Api_protocol_Qt::Qtnew_chat_text, this,
          &OverMap::new_chat_text, Qt::UniqueConnection);
  connect(connexionManager->client,
          &CatchChallenger::Api_protocol_Qt::Qtnew_system_text, this,
          &OverMap::new_system_text, Qt::UniqueConnection);
  updateChat();
}

void Chat::lineEdit_chat_text_returnPressed() {
  auto &playerInformations =
      connexionManager->client->get_player_informations();
  QString text = chatInput->Value();
  text.remove("\n");
  text.remove("\r");
  text.remove("\t");
  if (text.isEmpty()) return;
  if (text.contains(QRegularExpression("^ +$"))) {
    chatInput->Clear();
    connexionManager->client->add_system_text(CatchChallenger::Chat_type_system,
                                              "Space text not allowed");
    return;
  }
  if (text.size() > 256) {
    chatInput->Clear();
    connexionManager->client->add_system_text(CatchChallenger::Chat_type_system,
                                              "Message too long");
    return;
  }
  if (!text.startsWith('/')) {
    if (text.toStdString() == lastMessageSend) {
      chatInput->Clear();
      connexionManager->client->add_system_text(
          CatchChallenger::Chat_type_system, "Send message like as previous");
      return;
    }
    if (numberForFlood > 2) {
      chatInput->Clear();
      connexionManager->client->add_system_text(
          CatchChallenger::Chat_type_system, "Stop flood");
      return;
    }
  }
  numberForFlood++;
  lastMessageSend = text.toStdString();
  chatInput->SetValue(QString());
  if (!text.startsWith("/pm ")) {
    CatchChallenger::Chat_type chat_type;
    switch (chatType->ItemData(chatType->CurrentIndex(), 99)) {
      default:
      case 0:
        chat_type = CatchChallenger::Chat_type_all;
        break;
      case 1:
        chat_type = CatchChallenger::Chat_type_local;
        break;
      case 2:
        chat_type = CatchChallenger::Chat_type_clan;
        break;
    }
    connexionManager->client->sendChatText(chat_type, text.toStdString());
    if (!text.startsWith('/'))
      connexionManager->client->add_chat_text(
          chat_type, text.toStdString(),
          connexionManager->client->player_informations.public_informations
              .pseudo,
          connexionManager->client->player_informations.public_informations
              .type);
  } else if (text == "/clan_leave") {
    actionClan.push_back(ActionClan_Leave);
    connexionManager->client->leaveClan();
  } else if (text == "/clan_dissolve") {
    actionClan.push_back(ActionClan_Dissolve);
    connexionManager->client->dissolveClan();
  } else if (!text.startsWith("/clan_invite ")) {
    text.remove(0, std::string("/clan_invite ").size());
    if (!text.isEmpty()) {
      actionClan.push_back(ActionClan_Invite);
      connexionManager->client->inviteClan(text.toStdString());
    }
  } else if (!text.startsWith("/clan_eject ")) {
    text.remove(0, std::string("/clan_eject ").size());
    if (!text.isEmpty()) {
      actionClan.push_back(ActionClan_Eject);
      connexionManager->client->ejectClan(text.toStdString());
    }
  } else if (text == "/clan_informations") {
    if (!haveClanInformations)
      connexionManager->client->add_system_text(
          CatchChallenger::Chat_type::Chat_type_system, "No clan information");
    else if (clanName.empty() || playerInformations.clan == 0)
      connexionManager->client->add_system_text(
          CatchChallenger::Chat_type::Chat_type_system, "No clan");
    else
      connexionManager->client->add_system_text(
          CatchChallenger::Chat_type::Chat_type_system, "Name: " + clanName);
  } else if (text.contains(QRegularExpression("^/pm [^ ]+ .+$"))) {
    QString pseudo = text;
    pseudo.replace(QRegularExpression("^/pm ([^ ]+) .+$"), "\\1");
    text.replace(QRegularExpression("^/pm [^ ]+ (.+)$"), "\\1");
    connexionManager->client->sendPM(text.toStdString(), pseudo.toStdString());
    connexionManager->client->add_chat_text(
        CatchChallenger::Chat_type_pm, text.toStdString(),
        tr("To: ").toStdString() + pseudo.toStdString(),
        CatchChallenger::Player_type_normal);
  }
  updateChat();
}

void Chat::comboBox_chat_type_currentIndexChanged(int index) {
  Q_UNUSED(index)
  updateChat();
}

void Chat::new_system_text(const CatchChallenger::Chat_type &chat_type,
                              const std::string &text) {
  updateChat();
}

void Chat::new_chat_text(CatchChallenger::Chat_type chat_type,
                            std::string text, std::string pseudo,
                            CatchChallenger::Player_type player_type) {
  updateChat();
}

void Chat::updateChat() {
  const std::vector<CatchChallenger::Api_protocol::ChatEntry> &chat_list =
      connexionManager->client->getChatContent();
  QString nameHtml;
  unsigned int index = 0;
  while (index < chat_list.size()) {
    const CatchChallenger::Api_protocol::ChatEntry &entry = chat_list.at(index);
    bool addPlayerInfo = true;
    if (entry.chat_type == CatchChallenger::Chat_type_system ||
        entry.chat_type == CatchChallenger::Chat_type_system_important)
      addPlayerInfo = false;
    if (!addPlayerInfo)
      nameHtml +=
          QString::fromStdString(CatchChallenger::ChatParsing::new_chat_message(
              std::string(), CatchChallenger::Player_type_normal,
              entry.chat_type, entry.text));
    else
      nameHtml +=
          QString::fromStdString(CatchChallenger::ChatParsing::new_chat_message(
              entry.player_pseudo, entry.player_type, entry.chat_type,
              entry.text));
    index++;
  }
  chatText->SetText(nameHtml);
}

void Chat::removeNumberForFlood() {
  if (numberForFlood <= 0) return;
  numberForFlood--;
}

/*
void OverMap::keyPressEvent(QKeyEvent *event, bool &eventTriggerGeneral) {
  if (chat->isChecked() && chatInput->hasFocus())
    chatInput->keyPressEvent(event);
  else
    eventTriggerGeneral = false;
}

void OverMap::keyReleaseEvent(QKeyEvent *event, bool &eventTriggerGeneral) {
  if (chat->isChecked() && chatInput->hasFocus())
    chatInput->keyReleaseEvent(event);
  else
    eventTriggerGeneral = false;
}
*/

void Chat::OnScreenResize() {
  unsigned int space = 10;


  // TODO(lanstat): verificar como obtener este valor
  // int physicalDpiX = w->physicalDpiX();
  int physicalDpiX = 300;

  unsigned int xLeft = space;
  if (bounding_rect_.width() < 800 || bounding_rect_.height() < 600) {
    chat->SetSize(84 / 2, 93 / 2);
    chatOver->SetVisible(false);
  } else {
    chat->SetSize(84, 93);
    chatOver->SetVisible(physicalDpiX < 200);
  }
  if (bounding_rect_.width() < 800 || bounding_rect_.height() < 600) {
    chatBack->SetSize(200, 200);
    chatInput->SetWidth(140);
    chatType->SetWidth(60);
  } else {
    chatBack->SetSize(300, 400);
    chatInput->SetWidth(200);
    chatType->SetWidth(100);
  }
  unsigned int chatX = xLeft;
  unsigned int chatY = bounding_rect_.height() - space - chat->Height();
  chat->SetPos(chatX, chatY);
  xLeft += chat->Width() + space;
  if (chatOver->IsVisible()) {
    chatOver->SetPixelSize(18);
    chatOver->SetPos(chatX + chat->Width() / 2 - chatOver->Width() / 2,
                     bounding_rect_.height() - space - chatOver->Height());
  }
  chatBack->SetVisible(chat->IsChecked());
  chatText->SetVisible(chat->IsChecked());
  chatInput->SetVisible(chat->IsChecked());
  chatType->SetVisible(chat->IsChecked());
  if (chat->IsChecked()) {
    unsigned int y = chatY - 3 - chatInput->Height();
    chatInput->SetPos(space, y);
    chatType->SetPos(space + chatInput->Width() + 2, y);

    y -= 3 + chatBack->Height();
    chatBack->SetPos(space, y);
    chatText->SetPos(space + space, y + space);
  }
  if (labelSlow->IsVisible()) {
    labelSlow->SetPos(xLeft, chatY);
    xLeft += labelSlow->Pixmap().width() + space;
  }

  // compose from right to left
  unsigned int xRight = bounding_rect_.width() - space;
  if (buy->IsVisible()) {
    if (bounding_rect_.width() < 800 || bounding_rect_.height() < 600) {
      buy->SetSize(84 / 2, 93 / 2);
      buyOver->SetVisible(false);
    } else {
      buy->SetSize(84, 93);
      buyOver->SetVisible(physicalDpiX < 200);
    }
    unsigned int buyX = xRight - buy->Width();
    buy->SetPos(buyX, bounding_rect_.height() - space - buy->Height());
    xRight -= buy->Width() + space;
    if (buyOver->IsVisible()) {
      buyOver->SetPixelSize(18);
      buyOver->SetPos(buyX + buy->Width() / 2 - buyOver->Width() / 2,
                      bounding_rect_.height() - space - buyOver->Height());
    }
  }
  {
    if (bounding_rect_.width() < 800 || bounding_rect_.height() < 600) {
      bag->SetSize(84 / 2, 93 / 2);
      bagOver->SetVisible(false);
    } else {
      bag->SetSize(84, 93);
      bagOver->SetVisible(physicalDpiX < 200);
    }
    unsigned int bagX = xRight - bag->Width();
    bag->SetPos(bagX, bounding_rect_.height() - space - bag->Height());
    xRight -= bag->Width() + space;
    if (bagOver->IsVisible()) {
      bagOver->SetPixelSize(18);
      bagOver->SetPos(bagX + bag->Width() / 2 - bagOver->Width() / 2,
                      bounding_rect_.height() - space - bagOver->Height());
    }
  }
  Q_UNUSED(xRight);

  gainBack->SetVisible(!gainString.isEmpty());
  gain->SetVisible(gainBack->IsVisible());
  if (gainBack->IsVisible()) {
    const QRectF &f = gain->BoundingRect();
    gainBack->SetSize(f.width() + space * 2, f.height() + space * 2);
    gainBack->SetPos(bounding_rect_.width() / 2 - gainBack->Width() / 2, space);
    gain->SetPos(space, space);
  }

  IG_dialog_textBack->SetVisible(!IG_dialog_textString.isEmpty());
  IG_dialog_name->SetVisible(IG_dialog_textBack->IsVisible());
  IG_dialog_text->SetVisible(IG_dialog_textBack->IsVisible());
  IG_dialog_quit->SetVisible(IG_dialog_textBack->IsVisible());
  if (IG_dialog_textBack->IsVisible()) {
    const QRectF &b1 = IG_dialog_name->BoundingRect();
    const QRectF &b2 = IG_dialog_text->BoundingRect();
    int width = b2.width();
    int height = b1.height() + 10 + b2.height();
    if (bounding_rect_.width() < 800 || bounding_rect_.height() < 600) {
      if (width < 350) width = 350;
      if (height < 200) height = 200;
      IG_dialog_quit->SetSize(84 / 3, 93 / 3);
    } else {
      if (width < 600) width = 600;
      if (height < 400) height = 400;
      IG_dialog_quit->SetSize(84 / 2, 93 / 2);
    }
    IG_dialog_textBack->SetSize(width, height);

    int x = bounding_rect_.width() / 2 - IG_dialog_textBack->Width() / 2;
    int y = bounding_rect_.height() / 2 - IG_dialog_textBack->Height() / 2;
    IG_dialog_textBack->SetPos(x, y);
    if (IG_dialog_nameString.isEmpty()) {
      IG_dialog_text->SetPos(8, 8);
    } else {
      IG_dialog_name->SetPos(8, 8);
      IG_dialog_text->SetPos(8, 8 + space + IG_dialog_name->Height());
    }
    IG_dialog_quit->SetPos(
        IG_dialog_textBack->Width() - IG_dialog_quit->Width(), 0);
  }

  int yMiddle = bounding_rect_.height() - space;

  persistant_tipBack->SetVisible(!persistant_tipString.isEmpty());
  persistant_tip->SetVisible(persistant_tipBack->IsVisible());
  if (persistant_tip->IsVisible()) {
    const QRectF &f = persistant_tip->BoundingRect();
    persistant_tipBack->SetSize(f.width() + space * 2, f.height() + space * 2);
    persistant_tip->SetPos(space, space);
    persistant_tipBack->SetPos(
        bounding_rect_.width() / 2 - persistant_tipBack->Width() / 2,
        yMiddle - persistant_tipBack->Height());
    yMiddle -= (f.height() + space * 2 + space);
  }

  tipBack->SetVisible(!tipString.isEmpty());
  tip->SetVisible(tipBack->IsVisible());
  if (tipBack->IsVisible()) {
    const QRectF &f = tip->BoundingRect();
    tipBack->SetSize(f.width() + space * 2, f.height() + space * 2);

    tip->SetPos(space, space);
    tipBack->SetPos(bounding_rect_.width() / 2 - tipBack->Width() / 2,
                    yMiddle - tipBack->Height());
    yMiddle -= (f.height() + space * 2 + space);
  }
}

void OverMapLogic::updateTheTurtle() {
  if (!multiplayer) {
    if (!labelSlow->IsVisible()) return;
    labelSlow->SetVisible(false);
    return;
  }
  if (lastReplyTimeValue > 700 && lastReplyTimeSince.elapsed() < 15 * 1000) {
    // qDebug() << "The last query was slow:" << lastReplyTimeValue << ">" <<
    // 700 << " && " << lastReplyTimeSince.elapsed() << "<" << 15*1000;
    if (labelSlow->IsVisible()) return;
    labelSlow->SetToolTip(
        tr("The last query was slow (%1ms)").arg(lastReplyTimeValue));
    labelSlow->SetVisible(true);
    return;
  }
  if (worseQueryTime > 700) {
    if (labelSlow->IsVisible()) return;
    labelSlow->SetToolTip(
        tr("Remain query in suspend (%1ms ago)").arg(worseQueryTime));
    labelSlow->SetVisible(true);
    return;
  }
  if (!labelSlow->IsVisible()) return;
  labelSlow->SetVisible(false);
}

void OverMapLogic::detectSlowDown() {
  if (connexionManager->client == nullptr) return;
  uint32_t queryCount = 0;
  worseQueryTime = 0;
  std::vector<std::string> middleQueryList;
  const std::map<uint8_t, uint64_t> &values =
      connexionManager->client->getQuerySendTimeList();
  queryCount += values.size();
  for (const auto n : values) {
    middleQueryList.push_back(std::to_string(n.first));
    std::time_t result = std::time(nullptr);
    if ((uint64_t)result >= n.second) {
      const uint32_t &time = result - n.second;
      if (time > worseQueryTime) worseQueryTime = time;
    }
  }
  /*if(queryCount>0)
      ui->labelQueryList->setText(
                  tr("Running query: %1 (%3 and %4), query with worse time:
  %2ms") .arg(queryCount) .arg(worseQueryTime)
                  .arg(QString::fromStdString(stringimplode(connexionManager->client->getQueryRunningList(),';')))
                  .arg(QString::fromStdString(stringimplode(middleQueryList,";")))
                  );
  else
      ui->labelQueryList->setText(tr("No query running"));*/
  updateTheTurtle();
}

void OverMapLogic::lastReplyTime(const uint32_t &time) {
  // ui->labelLastReplyTime->setText(tr("Last reply time: %1ms").arg(time));
  if (lastReplyTimeValue <= 700 || lastReplyTimeSince.elapsed() > 15 * 1000) {
    lastReplyTimeValue = time;
    lastReplyTimeSince.restart();
  } else {
    if (time > 700) {
      lastReplyTimeValue = time;
      lastReplyTimeSince.restart();
    }
  }
  updateTheTurtle();
}
