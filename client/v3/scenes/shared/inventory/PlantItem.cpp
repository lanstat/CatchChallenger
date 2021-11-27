// Copyright 2021 <CatchChallenger>
#include "PlantItem.hpp"

#include <math.h>

#include <QPainter>
#include <iostream>

#include "../../../../../general/base/CommonDatapack.hpp"
#include "../../../FacilityLibClient.hpp"
#include "../../../cc/QtDatapackClientLoader.hpp"
#include "../../../core/Sprite.hpp"
#include "../../../libraries/tiled/tiled_tile.hpp"
#include "../../../ui/Label.hpp"

using Scenes::PlantItem;

PlantItem::PlantItem(uint16_t plant_id, uint32_t quantity): UI::SelectableItem() {
  plant_id_ = plant_id;
  quantity_ = quantity;
  SetSize(200, 60);
}

PlantItem::~PlantItem() {
}

PlantItem *PlantItem::Create(uint16_t plant_id, uint32_t quantity) {
  return new (std::nothrow) PlantItem(plant_id, quantity);
}

void PlantItem::DrawContent(QPainter *painter) {
  auto icon = Sprite::Create();
  auto text = UI::Label::Create();

  const DatapackClientLoader::ItemExtra &itemExtra =
      QtDatapackClientLoader::datapackLoader->itemsExtra.at(plant_id_);
  const QtDatapackClientLoader::QtPlantExtra &contentExtra =
      QtDatapackClientLoader::datapackLoader->QtplantExtra[plant_id_];
  const CatchChallenger::Plant &plant =
      CatchChallenger::CommonDatapack::commonDatapack.plants[plant_id_];
  if (QtDatapackClientLoader::datapackLoader->itemsExtra.find(plant_id_) !=
          QtDatapackClientLoader::datapackLoader->itemsExtra.cend()) {
    QPixmap p =
        QtDatapackClientLoader::datapackLoader->getImageExtra(plant_id_).image;
    p = p.scaledToWidth(p.width() * 2);
    icon->SetPixmap(p);
    icon->SetPos(10, Height() / 2 - icon->Height() / 2);
    icon->Paint(painter, nullptr, nullptr);

    text->SetPixelSize(12);
    text->SetText(QString::fromStdString(itemExtra.name) + "\n" +
                  QObject::tr("Quantity: ") + QString::number(quantity_) + " ");
    text->SetPos(Width() / 2 - text->Width() / 2, 5);
    text->Paint(painter, nullptr, nullptr);

    //if (contentExtra.tileset != nullptr) {
      //icon->SetPixmap(SetCanvas(contentExtra.tileset->tileAt(0)->image(), 32));
      //icon->SetPos(40, 10);
      //icon->Paint(painter, nullptr, nullptr);

      //icon->SetPixmap(SetCanvas(contentExtra.tileset->tileAt(1)->image(), 32));
      //icon->SetPos(80, 10);
      //icon->Paint(painter, nullptr, nullptr);

      //icon->SetPixmap(SetCanvas(contentExtra.tileset->tileAt(2)->image(), 32));
      //icon->SetPos(120, 10);
      //icon->Paint(painter, nullptr, nullptr);

      //icon->SetPixmap(SetCanvas(contentExtra.tileset->tileAt(3)->image(), 32));
      //icon->SetPos(160, 10);
      //icon->Paint(painter, nullptr, nullptr);

      //icon->SetPixmap(SetCanvas(contentExtra.tileset->tileAt(4)->image(), 32));
      //icon->SetPos(200, 10);
      //icon->Paint(painter, nullptr, nullptr);

      ////icon->SetPixmap(SetCanvas(contentExtra.tileset->tileAt(5)->image(), 32));
      ////icon->SetPos(240, 10);
      ////icon->Paint(painter, nullptr, nullptr);
    //}

    //text->SetText(
        //QString::fromStdString(CatchChallenger::FacilityLibClient::timeToString(
            //plant.fruits_seconds)));
    //text->SetPos(280, 10);
    //text->Paint(painter, nullptr, nullptr);

    //text->SetText(
        //QObject::tr("Quantity: %1")
            //.arg((double)plant.fix_quantity +
                 //((double)plant.random_quantity) / RANDOM_FLOAT_PART_DIVIDER));
    //text->SetPos(320, 10);
    //text->Paint(painter, nullptr, nullptr);
  } else {
    icon->SetPixmap(
        QtDatapackClientLoader::datapackLoader->defaultInventoryImage());
    icon->SetPos(10, 10);
    icon->Paint(painter, nullptr, nullptr);

    text->SetPixelSize(12);
    text->SetPos(20, 10);
    if (quantity_ > 1)
      text->SetText(
          QStringLiteral("id: %1 (x%2)").arg(plant_id_).arg(quantity_));
    else
      text->SetText(QStringLiteral("id: %1").arg(plant_id_));
    text->Paint(painter, nullptr, nullptr);

    if (contentExtra.tileset != nullptr) {
      icon->SetPixmap(SetCanvas(contentExtra.tileset->tileAt(0)->image(), 32));
      icon->SetPos(40, 10);
      icon->Paint(painter, nullptr, nullptr);

      icon->SetPixmap(SetCanvas(contentExtra.tileset->tileAt(1)->image(), 32));
      icon->SetPos(80, 10);
      icon->Paint(painter, nullptr, nullptr);

      icon->SetPixmap(SetCanvas(contentExtra.tileset->tileAt(2)->image(), 32));
      icon->SetPos(120, 10);
      icon->Paint(painter, nullptr, nullptr);

      icon->SetPixmap(SetCanvas(contentExtra.tileset->tileAt(3)->image(), 32));
      icon->SetPos(160, 10);
      icon->Paint(painter, nullptr, nullptr);

      icon->SetPixmap(SetCanvas(contentExtra.tileset->tileAt(4)->image(), 32));
      icon->SetPos(200, 10);
      icon->Paint(painter, nullptr, nullptr);

      icon->SetPixmap(SetCanvas(contentExtra.tileset->tileAt(5)->image(), 32));
      icon->SetPos(240, 10);
      icon->Paint(painter, nullptr, nullptr);
    }
    text->SetText(
        QString::fromStdString(CatchChallenger::FacilityLibClient::timeToString(
            plant.fruits_seconds)));
    text->SetPos(280, 10);
    text->Paint(painter, nullptr, nullptr);

    text->SetText(
        QObject::tr("Quantity: %1")
            .arg((double)plant.fix_quantity +
                 ((double)plant.random_quantity) / RANDOM_FLOAT_PART_DIVIDER));
    text->SetPos(320, 10);
    text->Paint(painter, nullptr, nullptr);
  }

  delete icon;
  delete text;
}

void PlantItem::OnResize() { ReDraw(); }

QPixmap PlantItem::SetCanvas(const QPixmap &image, unsigned int size) {
  int wratio = floor((float)size / (float)image.width());
  int hratio = floor((float)size / (float)image.height());
  if (wratio < 1) wratio = 1;
  if (hratio < 1) hratio = 1;
  int minratio = wratio;
  if (minratio > hratio) minratio = hratio;
  QPixmap i = image.scaledToWidth(image.width() * minratio);
  QImage temp(size, size, QImage::Format_ARGB32_Premultiplied);
  temp.fill(Qt::transparent);
  QPainter paint;
  if (i.isNull()) abort();
  paint.begin(&temp);
  paint.drawPixmap((size - i.width()) / 2, (size - i.height()) / 2, i.width(),
                   i.height(), i);
  return QPixmap::fromImage(temp);
}

QPixmap PlantItem::SetCanvas(const QImage &image, unsigned int size) {
  int wratio = floor((float)size / (float)image.width());
  int hratio = floor((float)size / (float)image.height());
  if (wratio < 1) wratio = 1;
  if (hratio < 1) hratio = 1;
  int minratio = wratio;
  if (minratio > hratio) minratio = hratio;
  QImage i = image.scaledToWidth(image.width() * minratio);
  QImage temp(size, size, QImage::Format_ARGB32_Premultiplied);
  temp.fill(Qt::transparent);
  QPainter paint;
  if (i.isNull()) abort();
  paint.begin(&temp);
  paint.drawImage((size - i.width()) / 2, (size - i.height()) / 2, i, 0, 0,
                  i.width(), i.height());
  return QPixmap::fromImage(temp);
}

uint16_t PlantItem::PlantId() { return plant_id_; }
