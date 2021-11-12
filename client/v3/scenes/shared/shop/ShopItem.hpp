// Copyright 2021 <CatchChallenger>
#ifndef CLIENT_V3_SCENES_SHARED_SHOP_SHOPITEM_HPP_
#define CLIENT_V3_SCENES_SHARED_SHOP_SHOPITEM_HPP_

#include "../../../core/Node.hpp"
#include "../../../core/Sprite.hpp"

namespace Scenes {
class ShopItem : public Node {
 public:
  ~ShopItem();
  static ShopItem *Create(uint16_t object_id, uint32_t quantity,
                          uint32_t price);
  void SetSelected(bool selected);
  std::string Name();

  void Draw(QPainter *painter) override;
  void OnResize() override;
  void MousePressEvent(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEvent(const QPointF &point, bool &prev_validated) override;

 private:
  uint16_t object_id_;
  uint32_t quantity_;
  uint32_t price_;
  Sprite *icon_select_;
  bool is_selected_;

  std::string name_;

  ShopItem(uint16_t object_id, uint32_t quantity, uint32_t price);
};
}  // namespace Scenes
#endif  // CLIENT_V3_SCENES_SHARED_SHOP_SHOPITEM_HPP_
