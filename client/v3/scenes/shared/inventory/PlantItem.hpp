// Copyright 2021 <CatchChallenger>
#ifndef CLIENT_V3_SCENES_SHARED_INVENTORY_PLANTITEM_HPP_
#define CLIENT_V3_SCENES_SHARED_INVENTORY_PLANTITEM_HPP_

#include "../../../core/Node.hpp"

namespace Scenes {
class PlantItem : public Node {
 public:
  ~PlantItem();
  static PlantItem *Create(uint16_t plant_id, uint32_t quantity);
  uint16_t PlantId();
  void SetSelected(bool selected);

  void Draw(QPainter *painter) override;
  void OnResize() override;

 private:
  uint16_t plant_id_;
  uint32_t quantity_;
  QPixmap item_cache_;

  PlantItem(uint16_t plant_id, uint32_t quantity);
  void DrawItem();
  QPixmap SetCanvas(const QPixmap &image, unsigned int size);
  QPixmap SetCanvas(const QImage &image, unsigned int size);
};
}  // namespace Scenes
#endif  // CLIENT_V3_SCENES_SHARED_INVENTORY_PLANTITEM_HPP_
