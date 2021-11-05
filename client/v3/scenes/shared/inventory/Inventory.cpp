// Copyright 2021 CatchChallenger
#include "Inventory.hpp"

#include <iostream>

#include "../../../FacilityLibClient.hpp"
#include "../../../cc/QtDatapackClientLoader.hpp"
#include "../../../general/base/CommonDatapack.hpp"
#include "../../../ui/LinkedDialog.hpp"
#include "InventoryItem.hpp"

using Scenes::Inventory;
using Scenes::InventoryItem;
using std::placeholders::_1;

Inventory::Inventory() : Node(nullptr) {
  lastItemSelected = -1;
  lastItemSize = 0;
  itemCount = 0;
  waitedObjectType = Inventory::ObjectType::ObjectType_All;
  inSelection = false;
  connexionManager = ConnectionManager::GetInstance();

  inventory = UI::GridView::Create(this);
  inventory->SetItemSpacing(5);
  inventory->TestMode();
  // if (!connect(inventory, &QListWidget::itemSelectionChanged, this,
  //&Inventory::on_inventory_itemSelectionChanged))

  descriptionBack = Sprite::Create(":/CC/images/interface/b1.png", this);
  inventory_description = UI::Label::Create(this);
  inventoryDestroy =
      UI::Button::Create(":/CC/images/interface/delete.png", this);
  inventoryUse = UI::Button::Create(":/CC/images/interface/validate.png", this);

  inventoryUse->SetOnClick(std::bind(&Inventory::inventoryUse_slot, this));
  inventoryDestroy->SetOnClick(
      std::bind(&Inventory::inventoryDestroy_slot, this));
}

Inventory::~Inventory() {
  delete inventory;
  inventory = nullptr;
  delete descriptionBack;
  descriptionBack = nullptr;
}

Inventory *Inventory::Create() { return new (std::nothrow) Inventory(); }

void Inventory::Draw(QPainter *painter) {
  (void)painter;
  auto font = inventory_description->GetFont();
  int space = 30;
  if (BoundingRect().width() < 800 || BoundingRect().height() < 600) {
    inventoryDestroy->SetSize(83 / 2, 94 / 2);
    inventoryUse->SetSize(83 / 2, 94 / 2);
    updateInventory(24);
  } else {
    inventoryDestroy->SetSize(83, 94);
    inventoryUse->SetSize(83, 94);
    font.setPixelSize(18);
    if (BoundingRect().width() > 1400 && BoundingRect().height() > 800) {
      if (itemCount < 7)
        updateInventory(96);
      else
        updateInventory(64);
    } else {
      if (itemCount < 7)
        updateInventory(48);
      else
        updateInventory(24);
    }
  }

  auto bounds = BoundingRect();
  inventory->SetPos(0, 0);
  inventory->SetSize(bounds.width(), bounds.height());

  int yTemp = bounds.height() - inventoryDestroy->Height() - 20;
  int xTemp = bounds.x();
  inventoryDestroy->SetPos(xTemp, yTemp);
  xTemp += inventoryDestroy->Width() + space / 2;
  descriptionBack->SetPos(xTemp, yTemp);
  inventory_description->SetPos(xTemp + 32 + space / 3, yTemp + 32);
  inventory_description->SetWidth(descriptionBack->Width() - 64);
  int xTempUse = bounds.x() + bounds.width() - space -
                 inventoryUse->Width();
  inventoryUse->SetPos(xTempUse, yTemp);
  descriptionBack->SetSize(xTempUse - xTemp - space / 2,
                           inventoryDestroy->Height());

  inventory_description->SetFont(font);
}

void Inventory::SetVar(const Inventory::ObjectType &waitedObjectType,
                       const bool inSelection, const int lastItemSelected) {
  this->inSelection = inSelection;
  this->waitedObjectType = waitedObjectType;
  this->lastItemSelected = lastItemSelected;

  const CatchChallenger::Player_private_and_public_informations
      &playerInformations =
          connexionManager->client->get_player_informations_ro();
  itemCount = playerInformations.items.size();
}

void Inventory::updateInventory(uint8_t targetSize) {
  if (lastItemSize == targetSize) return;
  lastItemSize = targetSize;
  const CatchChallenger::Player_private_and_public_informations
      &playerInformations =
          connexionManager->client->get_player_informations_ro();
  inventory_description->SetEnabled(false);
  inventoryUse->SetEnabled(false);
  inventoryDestroy->SetEnabled(false);
  auto parent = static_cast<UI::LinkedDialog *>(Parent());
  // TODO(lanstat): hide buttons from selection
  // back->SetVisible(!inSelection);
  // next->SetVisible(!inSelection);
  inventory->Clear();
  inventory->SetItemSize(targetSize, targetSize);
  itemCount = playerInformations.items.size();
  auto i = playerInformations.items.begin();
  while (i != playerInformations.items.cend()) {
    const uint16_t id = i->first;
    bool show = !inSelection;
    if (inSelection) {
      switch (waitedObjectType) {
        case ObjectType_Seed:
          // reputation requierement control is into load_plant_inventory() NOT:
          // on_listPlantList_itemSelectionChanged()
          if (QtDatapackClientLoader::datapackLoader->itemToPlants.find(id) !=
              QtDatapackClientLoader::datapackLoader->itemToPlants.cend())
            show = true;
          break;
        case ObjectType_UseInFight:
          if (connexionManager->client->isInFightWithWild() &&
              CatchChallenger::CommonDatapack::commonDatapack.items.trap.find(
                  id) != CatchChallenger::CommonDatapack::commonDatapack.items
                             .trap.cend())
            show = true;
          else if (CatchChallenger::CommonDatapack::commonDatapack.items
                       .monsterItemEffect.find(id) !=
                   CatchChallenger::CommonDatapack::commonDatapack.items
                       .monsterItemEffect.cend())
            show = true;
          else
            show = false;
          break;
        default:
          qDebug() << "waitedObjectType is unknow into load_inventory()";
          break;
      }
    }
    if (show) {
      const DatapackClientLoader::ItemExtra &itemExtra =
          QtDatapackClientLoader::datapackLoader->itemsExtra.at(id);
      auto item = InventoryItem::Create();
      if (QtDatapackClientLoader::datapackLoader->itemsExtra.find(id) !=
          QtDatapackClientLoader::datapackLoader->itemsExtra.cend()) {
        QPixmap p =
            QtDatapackClientLoader::datapackLoader->getImageExtra(id).image;
        p = p.scaledToHeight(targetSize);
        item->SetPixmap(p);
        if (i->second > 1) item->SetText(QString::number(i->second) + " ");
        item->SetText(item->Text() + QString::fromStdString(itemExtra.name));
        item->SetToolTip(QString::fromStdString(itemExtra.name));
      } else {
        item->SetPixmap(
            QtDatapackClientLoader::datapackLoader->defaultInventoryImage());
        if (i->second > 1)
          item->SetText(QStringLiteral("id: %1 (x%2)").arg(id).arg(i->second));
        else
          item->SetText(QStringLiteral("id: %1").arg(id));
      }
      item->SetData(99, id);
      item->SetOnClick(std::bind(&Inventory::OnItemClick, this, _1));
      inventory->AddItem(item);
      if (id == lastItemSelected) {
        item->SetSelected(true);
        on_inventory_itemSelectionChanged();
      }
    }
    ++i;
  }
}

void Inventory::inventoryUse_slot() {
  if (lastItemSelected != 1) return;
}

void Inventory::inventoryDestroy_slot() {
  if (lastItemSelected != 1) return;
}

void Inventory::on_inventory_itemSelectionChanged() {
  if (QtDatapackClientLoader::datapackLoader->itemsExtra.find(
          lastItemSelected) ==
      QtDatapackClientLoader::datapackLoader->itemsExtra.cend()) {
    inventory_description->SetVisible(false);
    inventoryUse->SetEnabled(false);
    inventoryDestroy->SetEnabled(!inSelection);
    inventory_description->SetText(QObject::tr("Unknown description"));
    return;
  }
  const QtDatapackClientLoader::ItemExtra &content =
      QtDatapackClientLoader::datapackLoader->itemsExtra.at(lastItemSelected);
  inventoryDestroy->SetEnabled(!inSelection);
  inventory_description->SetText(QString::fromStdString(content.description));

  bool isRecipe = false;
  {
    /* is a recipe */
    isRecipe = CatchChallenger::CommonDatapack::commonDatapack
                   .itemToCraftingRecipes.find(lastItemSelected) !=
               CatchChallenger::CommonDatapack::commonDatapack
                   .itemToCraftingRecipes.cend();
    if (isRecipe) {
      const uint16_t &recipeId =
          CatchChallenger::CommonDatapack::commonDatapack.itemToCraftingRecipes
              .at(lastItemSelected);
      const CatchChallenger::CraftingRecipe &recipe =
          CatchChallenger::CommonDatapack::commonDatapack.craftingRecipes.at(
              recipeId);
      if (!connexionManager->client->haveReputationRequirements(
              recipe.requirements.reputation)) {
        std::string string;
        unsigned int index = 0;
        while (index < recipe.requirements.reputation.size()) {
          string +=
              CatchChallenger::FacilityLibClient::reputationRequirementsToText(
                  recipe.requirements.reputation.at(index));
          index++;
        }
        inventory_description->SetText(
            inventory_description->Text() + "\n" +
            QObject::tr(
                "<span style=\"color:#D50000\">Don't meet the requirements: "
                "%1</span>")
                .arg(QString::fromStdString(string)));
        isRecipe = false;
      }
    }
  }
  inventoryUse->SetEnabled(
      inSelection || isRecipe ||
      /* is a repel */
      CatchChallenger::CommonDatapack::commonDatapack.items.repel.find(
          lastItemSelected) !=
          CatchChallenger::CommonDatapack::commonDatapack.items.repel.cend() ||
      /* is a item with monster effect */
      CatchChallenger::CommonDatapack::commonDatapack.items.monsterItemEffect
              .find(lastItemSelected) !=
          CatchChallenger::CommonDatapack::commonDatapack.items
              .monsterItemEffect.cend() ||
      /* is a item with monster effect out of fight */
      (CatchChallenger::CommonDatapack::commonDatapack.items
               .monsterItemEffectOutOfFight.find(lastItemSelected) !=
           CatchChallenger::CommonDatapack::commonDatapack.items
               .monsterItemEffectOutOfFight.cend() &&
       !connexionManager->client->isInFight()) ||
      /* is a evolution item */
      CatchChallenger::CommonDatapack::commonDatapack.items.evolutionItem.find(
          lastItemSelected) != CatchChallenger::CommonDatapack::commonDatapack
                                   .items.evolutionItem.cend() ||
      /* is a evolution item */
      (CatchChallenger::CommonDatapack::commonDatapack.items.itemToLearn.find(
           lastItemSelected) != CatchChallenger::CommonDatapack::commonDatapack
                                    .items.itemToLearn.cend() &&
       !connexionManager->client->isInFight()));
}

void Inventory::OnItemClick(Node *node) {
  lastItemSelected = node->Data(99);
  on_inventory_itemSelectionChanged();
  auto items = inventory->Items();
  for (auto item : items) {
    if (node == item) continue;
    static_cast<InventoryItem *>(item)->SetSelected(false);
  }
}

void Inventory::OnEnter() {
  static_cast<UI::LinkedDialog *>(Parent())->SetTitle(QObject::tr(
      "Bag", "You can translate by inventory if the word is shorter"));
  inventory->RegisterEvents();
}

void Inventory::OnExit() { inventory->UnRegisterEvents(); }
