// Copyright 2021 CatchChallenger
#include "OverMap.hpp"

#include <QDesktopServices>
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

  player_portrait_ = PlayerPortrait::Create(this);
  player_portrait_->SetPos(0, 0);

  playersCountBack =
      Sprite::Create(":/CC/images/interface/multicount.png", this);
  playersCount = UI::Label::Create(this);

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

  toast_ = Toast::Create(this);
  toast_->SetVisible(false);

  npc_talk_ = NpcTalk::Create();
  chat_ = Chat::Create(this);

  tip_ = Tip::Create(this);
  tip_->SetVisible(false);

  persistant_tipBack =
      Sprite::Create(":/CC/images/interface/chatBackground.png", this);
  persistant_tip = UI::Label::Create(persistant_tipBack);

  if (false) {
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

  buy->SetOnClick(std::bind(&OverMap::buyClicked, this));
}

OverMap::~OverMap() {
  delete tip_;
  tip_ = nullptr;
  delete player_portrait_;
  player_portrait_ = nullptr;
  delete chat_;
  chat_ = nullptr;
}

void OverMap::resetAll() {
  for (auto it = begin(monsters); it != end(monsters); it++) {
    delete (*it);
  }
  monsters.clear();
}

void OverMap::setVar(CCMap *ccmap) {
  (void)ccmap;
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
}

void OverMap::have_current_player_info(
    const CatchChallenger::Player_private_and_public_informations
        &informations) {
  player_portrait_->SetVisible(true);
  player_portrait_->SetInformation(informations);

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
  bagOver->SetText(tr("Bag"));
  buyOver->SetText(tr("Buy"));
}

void OverMap::OnScreenResize() {
  unsigned int space = 10;

  player_portrait_->SetPos(space, space);

  int tempx = player_portrait_->X() + player_portrait_->Width();
  unsigned int monsterIndex = 0;
  while (monsterIndex < monsters.size()) {
    MapMonsterPreview *m = monsters.at(monsterIndex);
    m->SetPos(tempx, player_portrait_->Y());
    tempx += space + 56;
    monsterIndex++;
  }

  playersCountBack->SetPos(
      bounding_rect_.width() - space - playersCountBack->Width(), space);
  playersCount->SetPos(bounding_rect_.width() - space - 80, space + 15);

  // TODO(lanstat): verificar como obtener este valor
  // int physicalDpiX = w->physicalDpiX();
  int physicalDpiX = 300;

  unsigned int xLeft = space;

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
  tip_->SetY(150);
  toast_->SetY(150);

  chat_->SetPos(10, Height() - 210);
  chat_->SetSize(400, 200);
}

void OverMap::OnEnter() {
  for (auto item : monsters) {
    item->RegisterEvents();
  }
  buy->RegisterEvents();
  tip_->RegisterEvents();
  toast_->RegisterEvents();
  chat_->RegisterEvents();
}

void OverMap::OnExit() {
  for (auto item : monsters) {
    item->UnRegisterEvents();
  }
  buy->UnRegisterEvents();
  tip_->UnRegisterEvents();
  toast_->UnRegisterEvents();
  chat_->UnRegisterEvents();
  std::cout<< "LAN_[" << __FILE__ << ":" << __LINE__ << "] "<< "asdasd" << std::endl;
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

void OverMap::buyClicked() {
  QDesktopServices::openUrl(tr("https://shop.first-world.info/en/") +
                            "#CatchChallenger");
}
