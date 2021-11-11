// Copyright 2021 <CatchChallenger>
#include "MonsterItem.hpp"

#include <iostream>

#include "../../../cc/QtDatapackClientLoader.hpp"

using CatchChallenger::PlayerMonster;
using Scenes::MonsterItem;

MonsterItem::MonsterItem(CatchChallenger::PlayerMonster monster) {
  monster_ = monster;
  icon_ = Sprite::Create(this);
  select_ = UI::Button::Create(":/CC/images/interface/greenbutton.png", this);
  connection_manager_ = ConnectionManager::GetInstance();
  client_ = connection_manager_->client;

  icon_->SetSize(60, 60);
  select_->SetSize(120, 40);
  select_->SetText("Seleccionar");
  select_->SetPixelSize(12);
  SetSize(200, 80);
  LoadMonster();
}

MonsterItem::~MonsterItem() {
  delete icon_;
  delete select_;
}

MonsterItem* MonsterItem::Create(CatchChallenger::PlayerMonster monster) {
  return new (std::nothrow) MonsterItem(monster);
}

void MonsterItem::Draw(QPainter* painter) {
  auto background = Sprite::Create(":/CC/images/interface/b1-trimmed.png");
  background->Strech(20, 18, 10, Width(), Height());
  auto text = UI::Label::Create();
  text->SetWidth(bounding_rect_.width());
  text->SetAlignment(Qt::AlignCenter);
  text->SetText(name_);
  text->SetPos(0, Height() / 2 - text->Height() / 2);

  background->Draw(painter);
  text->Paint(painter, nullptr, nullptr);

  delete background;
  delete text;
}

void MonsterItem::LoadMonster() {
  const uint16_t id = monster_.monster;
  const DatapackClientLoader::MonsterExtra monster_extra =
      QtDatapackClientLoader::datapackLoader->monsterExtra.at(id);
  if (QtDatapackClientLoader::datapackLoader->monsterExtra.find(id) !=
      QtDatapackClientLoader::datapackLoader->monsterExtra.cend()) {
    QPixmap p =
        QtDatapackClientLoader::datapackLoader->getMonsterExtra(id).thumb;
    p = p.scaledToHeight(Height() - 20);
    icon_->SetPixmap(p);
    name_ = QString::fromStdString(monster_extra.name);
  } else {
    icon_->SetPixmap(
        QtDatapackClientLoader::datapackLoader->defaultInventoryImage());
    name_ = QStringLiteral("id: %1").arg(id);
  }
}

void MonsterItem::RegisterEvents() { 
  select_->RegisterEvents(); 
}

void MonsterItem::UnRegisterEvents() { select_->UnRegisterEvents(); }

void MonsterItem::OnResize() {
  icon_->SetPos(10, Height() / 2 - icon_->Height() / 2);
  select_->SetPos(Width() - 10 - select_->Width(),
                  Height() / 2 - select_->Height() / 2);
}
