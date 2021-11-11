// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_SCENES_SHARED_SHOP_SHOP_HPP_
#define CLIENT_V3_SCENES_SHARED_SHOP_SHOP_HPP_

#include "../../../../../general/base/GeneralStructures.hpp"
#include "../../../../../general/base/GeneralStructuresXml.hpp"
#include "../../../base/ConnectionManager.hpp"
#include "../../../core/Node.hpp"
#include "../../../core/Sprite.hpp"
#include "../../../ui/Dialog.hpp"
#include "../../../ui/Label.hpp"
#include "../../../ui/ListView.hpp"
#include "../../../ui/ThemedButton.hpp"

namespace Scenes {
class Shop : public UI::Dialog {
 public:
  static Shop *Create();
  ~Shop();

  void Draw(QPainter *painter) override;
  void OnEnter() override;
  void OnExit() override;
  void OnActionClick(Node *node) override;
  void OnScreenResize() override;

  void SetSeller(const CatchChallenger::Bot &bot, uint16_t shop_id);
  void SetOnItemClick(std::function<void(std::string)> callback);
  void Close();

 private:
  Shop();
  void OnItemClick(Node *node);
  void HaveShopList(const std::vector<CatchChallenger::ItemToSellOrBuy> &items);

  Sprite *portrait_;
  UI::Button *quit_;
  UI::Button *buy_;
  UI::Label *seller_name_;
  UI::ListView *content_;
  std::function<void(std::string)> on_item_click_;
  ConnectionManager *connexionManager;
  uint16_t shop_id_;
  Node *selected_item_;
};
}  // namespace Scenes
#endif  // CLIENT_V3_SCENES_SHARED_SHOP_SHOP_HPP_
