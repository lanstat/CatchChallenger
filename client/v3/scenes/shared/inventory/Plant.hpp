// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_SCENES_SHARED_INVENTORY_PLANT_HPP_
#define CLIENT_V3_SCENES_SHARED_INVENTORY_PLANT_HPP_

#include "../../../base/ConnectionManager.hpp"
#include "../../../ui/Button.hpp"
#include "../../../core/Node.hpp"
#include "../../../core/Sprite.hpp"
#include "../../../ui/ListView.hpp"
#include "../../../ui/Label.hpp"

namespace Scenes {
class Plant : public Node {
 public:
  static Plant *Create();
  ~Plant();

  void OnResize() override;
  void SetVar(const bool inSelection, const int lastItemSelected = -1);
  void SetOnUseItem(std::function<void(uint16_t)> callback);
  void RegisterEvents();
  void UnRegisterEvents();

 private:
  UI::ListView *plantList;

  Sprite *descriptionBack;
  UI::Label *inventory_description;
  UI::Button *inventoryUse;
  UI::Button *select_;

  ConnectionManager *connexionManager;
  bool inSelection;
  uint8_t lastItemSize;
  int lastItemSelected;

  Plant();
  void NewLanguage();
  void updatePlant();
  void inventoryUse_slot();
  void on_inventory_itemSelectionChanged(Node *item);
};
}  // namespace Scenes
#endif  // CLIENT_V3_SCENES_SHARED_INVENTORY_PLANT_HPP_
