// Copyright 2021 <CatchChallenger>
#include "MonsterBag.hpp"

#include <iostream>

#include "../../../Language.hpp"
#include "../../../base/ConnectionManager.hpp"
#include "../../../cc/QtDatapackClientLoader.hpp"
#include "../../../core/SceneManager.hpp"
#include "../../../ui/MessageDialog.hpp"
#include "MonsterItem.hpp"

using CatchChallenger::PlayerMonster;
using Scenes::MonsterBag;

MonsterBag::MonsterBag() {
  quit_ = UI::Button::Create(":/CC/images/interface/cancel.png", this);

  monster_list_ = UI::ListView::Create(this);
  monster_list_->SetItemSpacing(10);

  newLanguage();

  connection_manager_ = ConnectionManager::GetInstance();
  client_ = connection_manager_->client;

  AddActionButton(quit_);
}

MonsterBag::~MonsterBag() {
  delete quit_;
}

MonsterBag *MonsterBag::Create() { return new (std::nothrow) MonsterBag(); }

void MonsterBag::OnScreenResize() {
  UI::Dialog::OnScreenResize();
  if (bounding_rect_.width() < 600 || bounding_rect_.height() < 480) {
    quit_->SetSize(83 / 2, 94 / 2);
  } else {
    quit_->SetSize(83, 94);
  }

  auto inner = ContentBoundary();
  monster_list_->SetPos(inner.x(), inner.y());
  monster_list_->SetSize(inner.width(), inner.height());
}

void MonsterBag::newLanguage() { SetTitle(tr("Monsters")); }

void MonsterBag::OnCloseClicked() {
  ClearSelection();
  Parent()->RemoveChild(this);
}

void MonsterBag::LoadMonster() {
  const std::vector<PlayerMonster> &monsters = client_->getPlayerMonster();

#ifdef DEBUG_BASEWINDOWS
  qDebug() << "MonsterBag::LoadMonster()";
#endif
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
    ShowMessageDialog(tr("Warning"),
                      tr("The monster selected is already on battle"));
    return;
  }
  OnCloseClicked();
}

void MonsterBag::ClearSelection() {
  // monster_list_->selectionModel()->clearSelection();
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

void MonsterBag::OnEnter() {
  UI::Dialog::OnEnter();
  LoadMonster();
  monster_list_->RegisterEvents();
}

void MonsterBag::OnExit() {
  monster_list_->UnRegisterEvents();
  UI::Dialog::OnExit();
}

void MonsterBag::OnActionClick(Node * node) {}
