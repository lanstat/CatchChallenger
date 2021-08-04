// Copyright 2021 CatchChallenger
#include "Player.hpp"

#include <QPainter>
#include <iostream>

using Scenes::Player;

Player::Player() {
  quit = UI::Button::Create(":/CC/images/interface/cancel.png", this);
  next = UI::Button::Create(":/CC/images/interface/next.png", this);
  back = UI::Button::Create(":/CC/images/interface/back.png", this);

  name_label = UI::Label::Create(background_);
  name_value = UI::Label::Create(background_);
  cash_label = UI::Label::Create(background_);
  cash_value = UI::Label::Create(background_);
  type_label = UI::Label::Create(background_);
  type_value = UI::Label::Create(background_);

  /*
  if (!connect(&Language::language, &Language::newLanguage, this,
               &Player::newLanguage, Qt::QueuedConnection))
    abort();
  if (!connect(next, &CustomButton::clicked, this, &Player::sendNext,
               Qt::QueuedConnection))
    abort();
  if (!connect(back, &CustomButton::clicked, this, &Player::sendBack,
               Qt::QueuedConnection))
    abort();
  */
  connexionManager = ConnectionManager::GetInstance();

  newLanguage();

  AddActionButton(back);
  AddActionButton(next);
  AddActionButton(quit);
}

Player::~Player() {
  delete quit;
  delete next;
  delete back;
}

Player *Player::Create() { return new (std::nothrow) Player(); }

void Player::OnActionClick(Node *node) {
  if (node == quit) {
    Parent()->RemoveChild(this);
    Close();
  }
}

void Player::OnScreenResize() {
  UI::Dialog::OnScreenResize();
  int idealW = background_->Width();
  unsigned int space = 30;
  if (BoundingRect().width() < 800 || BoundingRect().height() < 600) {
    space /= 2;
  }

  int tempY = space + y_;
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
  SetTitle(tr("PLAYER"));
  name_label->SetText(tr("Name: "));
  cash_label->SetText(tr("Cash: "));
  type_label->SetText(tr("Type: "));
}

void Player::OnEnter() {
  UI::Dialog::OnEnter();
  const CatchChallenger::Player_private_and_public_informations
      &playerInformations =
          connexionManager->client->get_player_informations_ro();

  name_value->SetText(playerInformations.public_informations.pseudo);
  cash_value->SetText(QString::number(playerInformations.cash));
  switch (playerInformations.public_informations.type) {
    default:
    case CatchChallenger::Player_type_normal:
      type_value->SetText(tr("Normal player"));
      break;
    case CatchChallenger::Player_type_premium:
      type_value->SetText(tr("Premium player"));
      break;
    case CatchChallenger::Player_type_dev:
      type_value->SetText(tr("Developer player"));
      break;
    case CatchChallenger::Player_type_gm:
      type_value->SetText(tr("Admin"));
      break;
  }
}

void Player::OnExit() { UI::Dialog::OnExit(); }
