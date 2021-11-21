// Copyright 2021 <CatchChallenger>
#include "MonsterBag.hpp"

#include <iostream>

#include "../../../Language.hpp"
#include "../../../base/ConnectionManager.hpp"
#include "../../../cc/QtDatapackClientLoader.hpp"
#include "../../../core/SceneManager.hpp"
#include "../../../ui/LinkedDialog.hpp"
#include "../../../ui/MessageDialog.hpp"
#include "MonsterItem.hpp"

using CatchChallenger::PlayerMonster;
using Scenes::MonsterBag;
using std::placeholders::_1;

MonsterBag::MonsterBag() {
  on_select_monster_ = nullptr;
  monster_list_ = UI::ListView::Create(this);
  monster_list_->SetItemSpacing(10);
  show_select_ = false;

  select_ = UI::Button::Create(":/CC/images/interface/validate.png");
  select_->SetOnClick(std::bind(&MonsterBag::OnAcceptClicked, this));

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
  int index = 0;
  while (i != monsters.cend()) {
    auto item = MonsterItem::Create(*i);
    item->SetData(99, (*i).monster);
    item->SetData(98, index);
    item->SetOnClick(std::bind(&MonsterBag::OnSelectItem, this, _1));
    monster_list_->AddItem(item);
    ++i;
    index++;
  }
}

void MonsterBag::OnSelectItem(Node *node) {
  selected_item_ = node;
  auto items = monster_list_->Items();
  for (auto item : items) {
    if (node == item) {
      continue;
    }
    static_cast<MonsterItem *>(item)->SetSelected(false);
  }
}

void MonsterBag::OnAcceptClicked() {
  if (selected_item_->Data(99) == client_->getCurrentMonster()->monster) {
    ShowMessageDialog(QObject::tr("Warning"),
                      QObject::tr("The monster selected is already on battle"));
    return;
  }
  on_select_monster_(selected_item_->Data(98));
}

void MonsterBag::ClearSelection() {}

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
  auto linked = static_cast<UI::LinkedDialog *>(Parent());
  linked->SetTitle(QObject::tr("MONSTERS"));
  LoadMonster();
  monster_list_->RegisterEvents();
  if (show_select_) {
    linked->AddActionButton(select_);
  }
}

void MonsterBag::UnRegisterEvents() {
  monster_list_->UnRegisterEvents();
  if (Parent() == nullptr) return;
  static_cast<UI::LinkedDialog *>(Parent())->RemoveActionButton(select_);
}

void MonsterBag::ShowSelectAction(bool show) {
  if (show_select_ == show) return;

  show_select_ = show;
  if (Parent() == nullptr) return;
  if (show_select_) {
    static_cast<UI::LinkedDialog *>(Parent())->AddActionButton(select_);
  } else {
    static_cast<UI::LinkedDialog *>(Parent())->RemoveActionButton(select_);
  }
}

void MonsterBag::SetOnSelectMonster(std::function<void(uint8_t)> callback) {
  on_select_monster_ = callback;
}
