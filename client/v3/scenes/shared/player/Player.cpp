// Copyright 2021 CatchChallenger
#include "Player.hpp"

#include <QPainter>
#include <iostream>

#include "../../../ui/LinkedDialog.hpp"

using Scenes::Player;

Player::Player() {
  name_label = UI::Label::Create(this);
  name_value = UI::Label::Create(this);
  cash_label = UI::Label::Create(this);
  cash_value = UI::Label::Create(this);
  type_label = UI::Label::Create(this);
  type_value = UI::Label::Create(this);

  connexionManager = ConnectionManager::GetInstance();

  newLanguage();
}

Player::~Player() {
}

Player *Player::Create() { return new (std::nothrow) Player(); }

void Player::OnResize() {
  int idealW = BoundingRect().width();
  unsigned int space = 30;
  if (BoundingRect().width() < 800 || BoundingRect().height() < 600) {
    space /= 2;
  }

  int tempY = space + 30;
  name_label->SetPos(space, tempY);
  name_value->SetPos(idealW / 2, tempY);
  tempY += name_label->Height() + space;
  cash_label->SetPos(space, tempY);
  cash_value->SetPos(idealW / 2, tempY);
  tempY += cash_label->Height() + space;
  type_label->SetPos(space, tempY);
  type_value->SetPos(idealW / 2, tempY);
}

void Player::newLanguage() {
  name_label->SetText(QObject::tr("Name: "));
  cash_label->SetText(QObject::tr("Cash: "));
  type_label->SetText(QObject::tr("Type: "));
}

void Player::RegisterEvents() {
  static_cast<UI::LinkedDialog *>(Parent())->SetTitle(QObject::tr("PLAYER"));
  const CatchChallenger::Player_private_and_public_informations
      &playerInformations =
          connexionManager->client->get_player_informations_ro();

  name_value->SetText(playerInformations.public_informations.pseudo);
  cash_value->SetText(QString::number(playerInformations.cash));
  switch (playerInformations.public_informations.type) {
    default:
    case CatchChallenger::Player_type_normal:
      type_value->SetText(QObject::tr("Normal player"));
      break;
    case CatchChallenger::Player_type_premium:
      type_value->SetText(QObject::tr("Premium player"));
      break;
    case CatchChallenger::Player_type_dev:
      type_value->SetText(QObject::tr("Developer player"));
      break;
    case CatchChallenger::Player_type_gm:
      type_value->SetText(QObject::tr("Admin"));
      break;
  }
}

void Player::UnRegisterEvents() { }
