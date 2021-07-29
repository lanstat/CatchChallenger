// Copyright 2021 CatchChallenger
#include "TileLayerItem.hpp"

#include <iostream>

TileLayerItem::TileLayerItem(Tiled::TileLayer *tileLayer,
                             Tiled::MapRenderer *renderer, Node *parent)
    : Node(parent), mTileLayer(tileLayer), mRenderer(renderer) {
  // setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
  bounding_rect_ = mRenderer->boundingRect(mTileLayer->bounds());
  std::cout<< "LAN_[" << __FILE__ << ":" << __LINE__ << "] "<< bounding_rect_.width() << std::endl;
}

void TileLayerItem::Draw(QPainter *painter) {
  // TODO(lanstat): verify how works option
  // mRenderer->drawTileLayer(painter, mTileLayer, option->rect);
  std::cout<< "LAN_[" << __FILE__ << ":" << __LINE__ << "] "<< "1" << std::endl;
  mRenderer->drawTileLayer(painter, mTileLayer, BoundingRect());
}
