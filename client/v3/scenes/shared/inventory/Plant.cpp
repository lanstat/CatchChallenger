// Copyright 2021 CatchChallenger
#include "Plant.hpp"

#include <QImage>
#include <iostream>

#include "../../../Language.hpp"
#include "../../../cc/QtDatapackClientLoader.hpp"
#include "../../../ui/LinkedDialog.hpp"
#include "PlantItem.hpp"

using Scenes::Plant;

Plant::Plant() : Node() {
  lastItemSize = 0;
  inSelection = false;

  plantList = UI::ListView::Create(this);
  plantList->SetItemSpacing(5);

  connexionManager = ConnectionManager::GetInstance();

  select_ = UI::Button::Create(":/CC/images/interface/validate.png");
  select_->SetOnClick(std::bind(&Plant::inventoryUse_slot, this));

  NewLanguage();
}

Plant::~Plant() {}

Plant *Plant::Create() { return new (std::nothrow) Plant(); }

void Plant::OnResize() {
  unsigned int space = 20;

  plantList->SetPos(0, 0);
  plantList->SetSize(Width() / 3, Height());
}

void Plant::NewLanguage() {}

void Plant::SetVar(const bool inSelection, const int lastItemSelected) {
  this->inSelection = inSelection;
  this->lastItemSelected = lastItemSelected;
  updatePlant();
}

void Plant::updatePlant() {
  const CatchChallenger::Player_private_and_public_informations
      &playerInformations =
          connexionManager->client->get_player_informations_ro();
  plantList->Clear();
  auto i = playerInformations.items.begin();
  while (i != playerInformations.items.cend()) {
    const uint16_t id = i->first;
    if (QtDatapackClientLoader::datapackLoader->itemToPlants.find(id) !=
        QtDatapackClientLoader::datapackLoader->itemToPlants.cend()) {
      auto item = PlantItem::Create(id, i->second);
      item->SetData(99, id);
      if (id == lastItemSelected) item->SetSelected(true);
      plantList->AddItem(item);
    }
    ++i;
  }
}

void Plant::inventoryUse_slot() {
  // QList<QTreeWidgetItem *> items = plantList->selectedItems();
  // if (items.size() != 1) return;
  // emit useItem(items.first()->data(0, 99).toInt());
}

void Plant::on_inventory_itemSelectionChanged(Node *item) {
  lastItemSelected = item->Data(99);
  if (QtDatapackClientLoader::datapackLoader->itemsExtra.find(item->Data(99)) ==
      QtDatapackClientLoader::datapackLoader->itemsExtra.cend()) {
    return;
  }
  const QtDatapackClientLoader::ItemExtra &content =
      QtDatapackClientLoader::datapackLoader->itemsExtra.at(item->Data(99));
  // std::cout << "description: " << content.description << std::endl;

}

void Plant::RegisterEvents() {
  auto linked = static_cast<UI::LinkedDialog *>(Parent());
  linked->SetTitle(QObject::tr("PLANT"));

  linked->AddActionButton(select_);
  plantList->RegisterEvents();
}

void Plant::UnRegisterEvents() {
  plantList->UnRegisterEvents();
  static_cast<UI::LinkedDialog *>(Parent())->RemoveActionButton(select_);
}
