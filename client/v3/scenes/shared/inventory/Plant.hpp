// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_SCENES_SHARED_INVENTORY_PLANT_HPP_
#define CLIENT_V3_SCENES_SHARED_INVENTORY_PLANT_HPP_

#include "../../../base/ConnectionManager.hpp"
#include "../../../ui/Button.hpp"
#include "../../../core/Sprite.hpp"
#include "../../../ui/Dialog.hpp"
#include "../../../ui/ListView.hpp"
#include "../../../ui/Label.hpp"

namespace Scenes {
class Plant : public UI::Dialog {
 public:
  static Plant *Create();
  ~Plant();

  void OnScreenResize() override;
  void SetVar(const bool inSelection, const int lastItemSelected = -1);

  void SetOnNext(std::function<void()> callback);
  void SetOnBack(std::function<void()> callback);
  void SetOnUseItem(std::function<void(uint16_t)> callback);

 private:
  UI::Button *quit;
  UI::Button *next;
  UI::Button *back;

  UI::ListView *plantList;

  Sprite *descriptionBack;
  UI::Label *inventory_description;
  UI::Button *inventoryUse;

  ConnectionManager *connexionManager;
  bool inSelection;
  uint8_t lastItemSize;
  int lastItemSelected;

  void NewLanguage();
  void updatePlant();
  void inventoryUse_slot();
  void on_inventory_itemSelectionChanged(Node *item);
  static QPixmap setCanvas(const QPixmap &image, unsigned int size);
  static QPixmap setCanvas(const QImage &image, unsigned int size);
};
}  // namespace Scenes
#endif  // CLIENT_V3_SCENES_SHARED_INVENTORY_PLANT_HPP_
