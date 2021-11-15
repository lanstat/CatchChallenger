// Copyright 2021 CatchChallenger
#include "MonsterThumb.hpp"

#include <iostream>

#include "../../base/ConnectionManager.hpp"
#include "../../core/EventManager.hpp"

using Scenes::MonsterThumb;
using std::placeholders::_1;

MonsterThumb::MonsterThumb(Node *parent) : Node(parent) {
  monster_details_ = nullptr;
}

MonsterThumb::~MonsterThumb() {
  if (monster_details_) {
    delete monster_details_;
  }
  monster_details_ = nullptr;
}

MonsterThumb *MonsterThumb::Create(Node *parent) {
  return new (std::nothrow) MonsterThumb(parent);
}

void MonsterThumb::LoadMonsters(
    const CatchChallenger::Player_private_and_public_informations
        &informations) {
  Reset();
  int tempx = 0;
  int space = 10;
  qreal height = 0;
  int index = 0;
  for (CatchChallenger::PlayerMonster m : informations.playerMonster) {
    auto *t = MapMonsterPreview::Create(m, index, this);
    t->SetOnClick(std::bind(&MonsterThumb::OpenMonster, this, _1));
    t->SetOnDrop(std::bind(&MonsterThumb::OnDropMonster, this, _1));
    t->SetPos(tempx, 0);
    tempx += space + t->Width();
    height = t->Height();
    monsters_.push_back(t);
    index++;
  }
  if (tempx > 0) {
    tempx -= space;
  }
  SetSize(tempx, height);
}

void MonsterThumb::Draw(QPainter *painter) {}

void MonsterThumb::OnResize() {}

void MonsterThumb::OpenMonster(Node *item) {
  MapMonsterPreview *m = static_cast<MapMonsterPreview *>(item);
  if (monster_details_ == nullptr) {
    monster_details_ = MonsterDetails::Create();
  }
  monster_details_->SetMonster(m->GetMonster());
  Parent()->AddChild(monster_details_);
}

void MonsterThumb::RegisterEvents() {
  for (auto m : monsters_) {
    m->RegisterEvents();
  }
}

void MonsterThumb::UnRegisterEvents() {
  for (auto m : monsters_) {
    m->UnRegisterEvents();
  }
}

void MonsterThumb::OnDropMonster(Node *item) {
  auto connection = ConnectionManager::GetInstance();
  MapMonsterPreview *current = static_cast<MapMonsterPreview *>(item);
  int position = 0;
  std::sort(monsters_.begin(), monsters_.end(),
            [](MapMonsterPreview *a, MapMonsterPreview *b) {
              return a->X() < b->X();
            });
  for (auto it = begin(monsters_); it != end(monsters_); it++) {
    if (current->GetMonster().monster == (*it)->GetMonster().monster) {
      break;
    }
    position++;
  }
  if (position != current->Index()) {
    if (position < current->Index()) {
      connection->client->monsterMoveDown(current->Index() - position);
    } else {
      connection->client->monsterMoveUp(position - current->Index());
    }
  }
  LoadMonsters(connection->client->get_player_informations_ro());
}

void MonsterThumb::Reset() {
  for (auto it = begin(monsters_); it != end(monsters_); it++) {
    RemoveChild((*it));
    delete *it;
  }
  monsters_.clear();
}
