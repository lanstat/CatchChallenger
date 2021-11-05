// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_SCENES_SHARED_INVENTORY_INVENTORY_HPP_
#define CLIENT_V3_SCENES_SHARED_INVENTORY_INVENTORY_HPP_

#include "../../../base/ConnectionManager.hpp"
#include "../../../core/Node.hpp"
#include "../../../core/Sprite.hpp"
#include "../../../ui/Button.hpp"
#include "../../../ui/GridView.hpp"
#include "../../../ui/Label.hpp"

namespace Scenes {
class Inventory : public Node {
 public:
  static Inventory *Create();
  ~Inventory();

  enum ObjectType {
    ObjectType_All,
    ObjectType_Seed,
    ObjectType_Sell,
    ObjectType_SellToMarket,
    ObjectType_Trade,
    ObjectType_MonsterToTrade,
    ObjectType_MonsterToTradeToMarket,
    ObjectType_MonsterToLearn,
    ObjectType_MonsterToFight,
    ObjectType_MonsterToFightKO,
    ObjectType_ItemOnMonsterOutOfFight,
    ObjectType_ItemOnMonster,
    ObjectType_ItemEvolutionOnMonster,
    ObjectType_ItemLearnOnMonster,
    ObjectType_UseInFight
  };

  void SetVar(const ObjectType &waitedObjectType, const bool inSelection,
              const int lastItemSelected = -1);

  void on_inventory_itemSelectionChanged();
  void inventoryUse_slot();
  void inventoryDestroy_slot();
  void updateInventory(uint8_t targetSize);
  void OnEnter() override;
  void OnExit() override;
  void Draw(QPainter *painter) override;

 private:
  UI::GridView *inventory;

  Sprite *descriptionBack;
  UI::Label *inventory_description;
  UI::Button *inventoryDestroy;
  UI::Button *inventoryUse;

  ConnectionManager *connexionManager;
  Inventory::ObjectType waitedObjectType;
  bool inSelection;
  uint8_t lastItemSize;
  uint8_t itemCount;
  int lastItemSelected;

  Inventory();

  void useItem(uint16_t item);
  void deleteItem(uint16_t item);
  void OnItemClick(Node *node);
};
}  // namespace Scenes
#endif  // CLIENT_V3_SCENES_SHARED_INVENTORY_INVENTORY_HPP_
