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

  descriptionBack = Sprite::Create(this);
  inventory_description = UI::Label::Create(this);
  inventoryUse = UI::Button::Create(":/CC/images/interface/validate.png", this);

  inventoryUse->SetOnClick(std::bind(&Plant::inventoryUse_slot, this));
  connexionManager = ConnectionManager::GetInstance();

  NewLanguage();
}

Plant::~Plant() {}

Plant *Plant::Create() { return new (std::nothrow) Plant(); }

void Plant::OnResize() {
  unsigned int space = 20;

  plantList->SetPos(0, 0);

  inventoryUse->SetSize(83 / 2, 94 / 2);
  int yTemp = Height() - inventoryUse->Height();
  plantList->SetSize(Width(), yTemp);
  descriptionBack->SetPos(0, yTemp);
  descriptionBack->SetPixmap(":/CC/images/interface/b1-trimmed.png");
  descriptionBack->Strech(18, 16, 10, Width() - inventoryUse->Width() - space,
                          inventoryUse->Height());
  inventory_description->SetPos(0, yTemp);
  inventory_description->SetWidth(descriptionBack->Width());
  inventoryUse->SetPos(Width() - inventoryUse->Width(), yTemp);
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
  inventoryUse->SetEnabled(false);
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
    inventory_description->SetVisible(false);
    inventoryUse->SetEnabled(false);
    inventory_description->SetText(QObject::tr("Unknown description"));
    return;
  }
  const QtDatapackClientLoader::ItemExtra &content =
      QtDatapackClientLoader::datapackLoader->itemsExtra.at(item->Data(99));
  inventory_description->SetText(QString::fromStdString(content.description));
  // std::cout << "description: " << content.description << std::endl;

  inventory_description->SetVisible(
      !inSelection &&
      /* is a plant */
      QtDatapackClientLoader::datapackLoader->itemToPlants.find(item->Data(
          99)) != QtDatapackClientLoader::datapackLoader->itemToPlants.cend());
  inventoryUse->SetEnabled(true);
}

void Plant::RegisterEvents() {
  static_cast<UI::LinkedDialog *>(Parent())->SetTitle(QObject::tr("PLANT"));
}

void Plant::UnRegisterEvents() {}
