// Copyright 2021 CatchChallenger
#include "OverMap.hpp"

#include <QDesktopServices>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <iostream>
#include <utility>

#include "../../Language.hpp"
#include "../../cc/QtDatapackClientLoader.hpp"
#include "../../entities/ChatParsing.hpp"

using Scenes::OverMap;

using std::placeholders::_1;

OverMap::OverMap() {
  connexionManager = ConnectionManager::GetInstance();
  monstersDragged = nullptr;
  wasDragged = false;
  monsterDetails = nullptr;

  playerBackground = Sprite::Create(":/CC/images/interface/playerui.png", this);
  playerBackgroundBig = true;
  playerBackground->SetVisible(false);
  playerImage = Sprite::Create(playerBackground);
  name = UI::Label::Create(playerBackground);
  cash = UI::Label::Create(playerBackground);

  playersCountBack =
      Sprite::Create(":/CC/images/interface/multicount.png", this);
  playersCount = UI::Label::Create(this);

  chatBack = Sprite::Create(":/CC/images/interface/chatBackground.png", this);
  chatText = UI::Label::Create(this);
  chatInput = UI::Input::Create(this);
  chatType = UI::Combo::Create(":/CC/images/interface/button.png", this);
  chatType->SetZValue(5);
  chat = UI::Button::Create(":/CC/images/interface/chat.png", this);
  chatOver = UI::Label::Create(QColor(255, 255, 255), QColor(80, 71, 38), this);
  chat->SetCheckable(true);

  bag = UI::Button::Create(":/CC/images/interface/bag.png", this);
  bagOver = UI::Label::Create(QColor(255, 255, 255), QColor(80, 71, 38), this);
  buy = UI::Button::Create(":/CC/images/interface/buy.png", this);
  {
    QLinearGradient gradient1(0, 0, 0, 100);
    gradient1.setColorAt(0, QColor(0, 0, 0));
    gradient1.setColorAt(1, QColor(0, 0, 0));
    QLinearGradient gradient2(0, 0, 0, 100);
    gradient2.setColorAt(0, QColor(255, 255, 255));
    gradient2.setColorAt(0.5, QColor(178, 242, 241));
    buyOver = UI::Label::Create(gradient1, gradient2, this);
  }

  gainBack = Sprite::Create(":/CC/images/interface/chatBackground.png", this);
  gain = UI::Label::Create(gainBack);

  IG_dialog_textBack =
      Sprite::Create(":/CC/images/interface/message.png", this);
  IG_dialog_name = UI::Label::Create(IG_dialog_textBack);
  IG_dialog_text = UI::Label::Create(IG_dialog_textBack);
  IG_dialog_quit = UI::Button::Create(":/CC/images/interface/closedialog.png",
                                      IG_dialog_textBack);

  tipBack = Sprite::Create(":/CC/images/interface/chatBackground.png", this);
  tip = UI::Label::Create(tipBack);

  persistant_tipBack =
      Sprite::Create(":/CC/images/interface/chatBackground.png", this);
  persistant_tip = UI::Label::Create(persistant_tipBack);

  labelSlow = Sprite::Create(":/CC/images/multi/slow.png", this);

  if (false) {
    {
      gainString = "gain";
      gain->SetText(gainString);
    }
    {
      IG_dialog_nameString = "Name";
      IG_dialog_name->SetText(IG_dialog_nameString);
      IG_dialog_textString = "Text";
      IG_dialog_text->SetText(IG_dialog_textString);
    }
    {
      tipString = "tip";
      tip->SetText(tipString);
    }
    {
      persistant_tipString = "persistant_tip";
      persistant_tip->SetText(persistant_tipString);
    }
  }

  /*    if(!connect(back,&CustomButton::clicked,this,&OverMap::backMulti))
          abort();
      if(!connect(server_select,&CustomButton::clicked,this,&OverMap::server_select_clicked))
          abort();
      if(!connect(serverList,&QTreeWidget::itemSelectionChanged,this,&OverMap::itemSelectionChanged))
          abort();*/
  newLanguage();

  /*
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
  if (!connect(buy, &CustomButton::clicked, this, &OverMap::buyClicked))
    abort();
  */

  stopFlood.setSingleShot(false);
  stopFlood.start(1500);
}

OverMap::~OverMap() {}

void OverMap::resetAll() {
  for (auto it = begin(monsters); it != end(monsters); it++) {
    delete (*it);
  }
  monsters.clear();
}

void OverMap::setVar(CCMap *ccmap) {
  (void)ccmap;
  lastMessageSend.clear();
  connect(connexionManager->client,
          &CatchChallenger::Api_protocol_Qt::Qtnumber_of_player, this,
          &OverMap::updatePlayerNumber, Qt::UniqueConnection);
  if (!connexionManager->client->getCaracterSelected()) {
    connect(connexionManager->client,
            &CatchChallenger::Api_protocol_Qt::Qthave_current_player_info, this,
            &OverMap::have_current_player_info, Qt::UniqueConnection);
  } else {
    const auto &info = connexionManager->client->get_player_informations_ro();
    have_current_player_info(info);
  }
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
  connect(connexionManager->client,
          &CatchChallenger::Api_protocol_Qt::Qtnew_chat_text, this,
          &OverMap::new_chat_text, Qt::UniqueConnection);
  connect(connexionManager->client,
          &CatchChallenger::Api_protocol_Qt::Qtnew_system_text, this,
          &OverMap::new_system_text, Qt::UniqueConnection);
  updateChat();
}

void OverMap::have_current_player_info(
    const CatchChallenger::Player_private_and_public_informations
        &informations) {
  playerBackground->SetVisible(true);
  QPixmap pPixm = QString::fromStdString(
      QtDatapackClientLoader::datapackLoader->getFrontSkinPath(
          informations.public_informations.skinId));
  if (playerBackgroundBig == true)
    pPixm = pPixm.scaledToHeight(pPixm.height() * 2, Qt::FastTransformation);
  playerImage->SetPixmap(pPixm);
  name->SetText(informations.public_informations.pseudo);
  std::cout << "info.public_informations.pseudo: "
            << informations.public_informations.pseudo << std::endl;
  cash->SetText(QString::number(informations.cash));
  for (auto it = begin(monsters); it != end(monsters); it++) {
    delete *it;
  }
  monsters.clear();
  for (CatchChallenger::PlayerMonster m : informations.playerMonster) {
    auto *t = MapMonsterPreview::Create(m, this);
    t->SetOnClick(std::bind(&OverMap::openMonster, this, _1));
    monsters.push_back(t);
  }
  std::pair<uint16_t, uint16_t> t =
      connexionManager->client->getLast_number_of_player();
  playersCount->SetText(QString::number(t.first));
}

void OverMap::openMonster(Node *item) {
  MapMonsterPreview *m = static_cast<MapMonsterPreview *>(item);
  if (monsterDetails == nullptr) {
    monsterDetails = MonsterDetails::Create();
  }
  monsterDetails->SetMonster(m->GetMonster());
  AddChild(monsterDetails);
}

void OverMap::updatePlayerNumber(const uint16_t &number,
                                 const uint16_t &max_players) {
  Q_UNUSED(max_players);
  playersCount->SetText(QString::number(number));
}

void OverMap::newLanguage() {
  // name->setHtml(tr(""));
  chatOver->SetText(tr("Chat"));
  bagOver->SetText(tr("Bag"));
  buyOver->SetText(tr("Buy"));
  /*
  chatType->setItemText(0, tr("All"));
  chatType->setItemText(1, tr("Local"));
  if (chatType->count() > 2) chatType->setItemText(2, tr("Clan"));
  */
}

void OverMap::OnScreenResize() {
  unsigned int space = 10;
  if (playerBackground->IsVisible()) {
    if (playerBackgroundBig == true &&
        (bounding_rect_.width() < 800 || bounding_rect_.height() < 600)) {
      playerBackgroundBig = false;
      playerBackground->SetPixmap(":/CC/images/interface/playeruiL.png");

      const auto &informations =
          connexionManager->client->get_player_informations_ro();
      QPixmap pPixm = QString::fromStdString(
          QtDatapackClientLoader::datapackLoader->getFrontSkinPath(
              informations.public_informations.skinId));
      playerImage->SetPixmap(pPixm);

    } else if (playerBackgroundBig == false && bounding_rect_.width() > 800 &&
               bounding_rect_.height() > 600) {
      playerBackgroundBig = true;
      playerBackground->SetPixmap(":/CC/images/interface/playerui.png");

      const auto &informations =
          connexionManager->client->get_player_informations_ro();
      QPixmap pPixm = QString::fromStdString(
          QtDatapackClientLoader::datapackLoader->getFrontSkinPath(
              informations.public_informations.skinId));
      pPixm = pPixm.scaledToHeight(pPixm.height() * 2, Qt::FastTransformation);
      playerImage->SetPixmap(pPixm);
    }
    if (playerBackgroundBig == true)
      playerImage->SetPos(0, -10);
    else
      playerImage->SetPos(0, -5);
    playerBackground->SetPos(space, space);
    if (playerBackgroundBig) {
      name->SetPos(playerBackground->X() + 160, playerBackground->Y() + 10);
      cash->SetPos(playerBackground->X() + 220, playerBackground->Y() + 57);
      name->SetVisible(true);
      cash->SetVisible(true);
    } else {
      name->SetVisible(false);
      cash->SetVisible(false);
    }
    int tempx = playerBackground->X() + playerBackground->Width() + space;
    if (playerBackgroundBig != true)
      tempx = playerBackground->X() + playerBackground->Width();
    unsigned int monsterIndex = 0;
    while (monsterIndex < monsters.size()) {
      MapMonsterPreview *m = monsters.at(monsterIndex);
      if (playerBackgroundBig == true)
        m->SetPos(tempx, playerBackground->Y());
      else
        m->SetPos(tempx, 0);
      tempx += space + 56;
      monsterIndex++;
    }
  }

  playersCountBack->SetPos(
      bounding_rect_.width() - space - playersCountBack->Width(), space);
  playersCount->SetPos(bounding_rect_.width() - space - 80, space + 15);

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

void OverMap::OnEnter() {
  for (auto item : monsters) {
    item->RegisterEvents();
  }
}

void OverMap::OnExit() {
  for (auto item : monsters) {
    item->UnRegisterEvents();
  }
}

/*
void OverMap::mousePressEventXY(const QPointF &p, bool &pressValidated,
                                bool &callParentClass) {
  chat->mousePressEventXY(p, pressValidated);
  buy->mousePressEventXY(p, pressValidated);
  bag->mousePressEventXY(p, pressValidated);
  QRectF f(IG_dialog_textBack->x(), IG_dialog_textBack->y(),
           IG_dialog_textBack->width(), IG_dialog_textBack->height());
  if (IG_dialog_textBack->isVisible() && f.contains(p)) {
    IG_dialog_quit->mousePressEventXY(p, pressValidated);
    pressValidated = true;
  }
  if (chat->isChecked()) {
    const QRectF &b = QRectF(chatBack->x(), chatBack->y(), chatBack->width(),
                             chatBack->height() + 5 + chatInput->height());
    const QRectF &t = mapRectToScene(b);
    if (t.contains(p)) {
      pressValidated = true;
      callParentClass = true;
    }
    const QRectF &b2 = QRectF(chatInput->x(), chatInput->y(),
                              chatInput->width(), chatInput->height());
    const QRectF &t2 = mapRectToScene(b2);
    if (!t2.contains(p)) chatInput->clearFocus();
  }

  playerBackground->mousePressEventXY(p, pressValidated);
  for (int i = 0; i < monsters.size(); ++i) {
    bool previousState = pressValidated;
    monsters.at(i)->mousePressEventXY(p, pressValidated);
    if (previousState == false && pressValidated == true) {
      wasDragged = false;
      m_startPress = p;
      monstersDragged = monsters.at(i);
    }
  }
}

void OverMap::mouseReleaseEventXY(const QPointF &p, bool &pressValidated,
                                  bool &callParentClass) {
  if (monstersDragged != nullptr) monstersDragged->setInDrag(false);
  monstersDragged = nullptr;
  chat->mouseReleaseEventXY(p, pressValidated);
  buy->mouseReleaseEventXY(p, pressValidated);
  bag->mouseReleaseEventXY(p, pressValidated);
  QRectF f(IG_dialog_textBack->x(), IG_dialog_textBack->y(),
           IG_dialog_textBack->width(), IG_dialog_textBack->height());
  if (IG_dialog_textBack->isVisible() && f.contains(p)) {
    IG_dialog_quit->mouseReleaseEventXY(p, pressValidated);
    pressValidated = true;
  }
  if (chat->isChecked()) {
    const QRectF &b = QRectF(
        chatBack->x(), chatBack->y(), chatBack->width(),
        chatBack->height() + 5 + chatInput->height() + 5 + chat->height());
    const QRectF &t = mapRectToScene(b);
    if (t.contains(p)) {
      pressValidated = true;
      callParentClass = true;
    }
    const QRectF &b2 = QRectF(chatInput->x(), chatInput->y(),
                              chatInput->width(), chatInput->height());
    const QRectF &t2 = mapRectToScene(b2);
    if (!t2.contains(p)) chatInput->clearFocus();
  }

  playerBackground->mouseReleaseEventXY(p, pressValidated);
  if (wasDragged) pressValidated = true;
  for (int i = 0; i < monsters.size(); ++i)
    monsters.at(i)->mouseReleaseEventXY(p, pressValidated);
  wasDragged = false;
  m_startPress = QPointF(0.0, 0.0);
}

void OverMap::mouseMoveEventXY(
    const QPointF &p, bool &pressValidated ,
    bool &callParentClass) {
  qreal xDiff = p.x() - m_startPress.x();
  qreal yDiff = p.y() - m_startPress.y();
  if (xDiff < -10 || xDiff > 10 || yDiff < -10 || yDiff > 10) {
    wasDragged = true;
    if (monstersDragged != nullptr) {
      bool fakeBool = true;
      monstersDragged->mouseReleaseEventXY(p, fakeBool);
      monstersDragged->setInDrag(true);
    }
  }
}
*/

void OverMap::IG_dialog_close() { IG_dialog_textString.clear(); }

void OverMap::lineEdit_chat_text_returnPressed() {
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

void OverMap::comboBox_chat_type_currentIndexChanged(int index) {
  Q_UNUSED(index)
  updateChat();
}

void OverMap::new_system_text(const CatchChallenger::Chat_type &chat_type,
                              const std::string &text) {
  updateChat();
}

void OverMap::new_chat_text(CatchChallenger::Chat_type chat_type,
                            std::string text, std::string pseudo,
                            CatchChallenger::Player_type player_type) {
  updateChat();
}

void OverMap::updateChat() {
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

void OverMap::removeNumberForFlood() {
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

void OverMap::buyClicked() {
  QDesktopServices::openUrl(tr("https://shop.first-world.info/en/") +
                            "#CatchChallenger");
}
