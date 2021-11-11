// Copyright 2021 <CatchChallenger>
#include "MonsterBag.hpp"

#include <iostream>

#include "../../../Language.hpp"
#include "../../../base/ConnectionManager.hpp"
#include "../../../cc/QtDatapackClientLoader.hpp"
#include "../../../core/SceneManager.hpp"
#include "../../../ui/MessageDialog.hpp"
#include "../../../ui/LinkedDialog.hpp"
#include "MonsterItem.hpp"

using CatchChallenger::PlayerMonster;
using Scenes::MonsterBag;

MonsterBag::MonsterBag() {
  monster_list_ = UI::ListView::Create(this);
  monster_list_->SetItemSpacing(10);

  connection_manager_ = ConnectionManager::GetInstance();
  client_ = connection_manager_->client;
}

MonsterBag::~MonsterBag() {
  delete monster_list_;
  monster_list_ = nullptr;
}

MonsterBag *MonsterBag::Create() { return new (std::nothrow) MonsterBag(); }

void MonsterBag::Draw(QPainter *painter) {
  auto b2 = Sprite::Create(":/CC/images/interface/b2.png");
  b2->Strech(24, 22, 20, Width(), Height());
  b2->SetPos(0, 0);
  b2->Paint(painter, nullptr, nullptr);
}

void MonsterBag::OnResize() {
  monster_list_->SetPos(20, 20);
  monster_list_->SetSize(Width() - 40, Height() - 40);
  ReDraw();
}

void MonsterBag::LoadMonster() {
  const std::vector<PlayerMonster> &monsters = client_->getPlayerMonster();

  monster_list_->Clear();
  auto i = monsters.begin();
  while (i != monsters.cend()) {
    auto item = MonsterItem::Create(*i);
    monster_list_->AddItem(item);
    ++i;
  }
}

void MonsterBag::OnSelectWidget() {}

void MonsterBag::OnAcceptClicked() {
  if (19 == client_->getCurrentMonster()->monster) {
    ShowMessageDialog(QObject::tr("Warning"),
                      QObject::tr("The monster selected is already on battle"));
    return;
  }
}

void MonsterBag::ClearSelection() {
}

void MonsterBag::ShowMessageDialog(const QString &title,
                                   const QString &message) {
  ClearSelection();
  if (message_dialog_ == nullptr) {
    message_dialog_ = UI::MessageDialog::Create();
  }

  message_dialog_->SetTitle(title);
  message_dialog_->SetMessage(message);
  AddChild(message_dialog_);
}

void MonsterBag::RegisterEvents() {
  static_cast<UI::LinkedDialog *>(Parent())->SetTitle(QObject::tr("MONSTERS"));
  LoadMonster();
  monster_list_->RegisterEvents();
}

void MonsterBag::UnRegisterEvents() {
  monster_list_->UnRegisterEvents();
}
