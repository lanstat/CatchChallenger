// Copyright 2021 CatchChallenger
#include "Plant.hpp"

#include <math.h>

#include <QImage>
#include <QPainter>
#include <iostream>

#include "../../../../general/base/CommonDatapack.hpp"
#include "../../../FacilityLibClient.hpp"
#include "../../../Language.hpp"
#include "../../../cc/QtDatapackClientLoader.hpp"
#include "../../../entities/tiled/tiled_tile.hpp"

using Scenes::Plant;

Plant::Plant() : UI::Dialog() {
  SetDialogSize(900, 600);
  lastItemSelected = -1;
  lastItemSize = 0;
  inSelection = false;

  quit = UI::Button::Create(":/CC/images/interface/cancel.png", this);
  next = UI::Button::Create(":/CC/images/interface/next.png", this);
  back = UI::Button::Create(":/CC/images/interface/back.png", this);

  plantList = UI::ListView::Create(this);
  // plantList->setColumnCount(7);
  // plantList->setHeaderHidden(true);
  // plantList->setIconSize(QSize(64, 64));
  // plantList->setColumnWidth(0, 230);
  // plantList->setColumnWidth(1, 0);
  // plantList->setColumnWidth(2, 0);
  // plantList->setColumnWidth(3, 0);
  // plantList->setColumnWidth(4, 0);
  // plantList->setColumnWidth(5, 270);
  // plantList->setColumnWidth(6, 150);
  // plantList->setText(0, QCoreApplication::translate("interfaceCopy",
  // "Source", nullptr));
  // if (!connect(plantList, &QTreeWidget::itemSelectionChanged, this,
  //&Plant::on_inventory_itemSelectionChanged))
  // abort();

  descriptionBack = Sprite::Create(":/CC/images/interface/b1.png", this);
  inventory_description = UI::Label::Create(this);
  inventoryUse = UI::Button::Create(":/CC/images/interface/validate.png", this);

  inventoryUse->SetOnClick(std::bind(&Plant::inventoryUse_slot, this));
  connexionManager = ConnectionManager::GetInstance();

  AddActionButton(back);
  AddActionButton(next);
  AddActionButton(quit);

  NewLanguage();
}
// 26,
Plant::~Plant() {}

void Plant::OnScreenResize() {
  UI::Dialog::OnScreenResize();
  unsigned int space = 30;

  plantListProxy->setPos(x + wdialog->currentBorderSize() + space,
                         y + label.pixmap().height() / 2 + space);
  plantList->setFixedSize(idealW - space * 2 - wdialog->currentBorderSize() * 2,
                          idealH - label.pixmap().height() / 2 - space - space -
                              inventoryUse->height() - space);

  int yTemp =
      y + label.pixmap().height() / 2 + space + plantList->height() + space / 2;
  int xTemp = x + wdialog->currentBorderSize() + space;
  xTemp += inventoryUse->width() + space / 2;
  descriptionBack->setPos(xTemp, yTemp);
  inventory_description->setPos(
      xTemp + descriptionBack->currentBorderSize() + space / 3,
      yTemp + descriptionBack->currentBorderSize() + space / 3);
  inventory_description->setTextWidth(descriptionBack->width() -
                                      descriptionBack->currentBorderSize() * 2 -
                                      space / 2);
  int xTempUse =
      x + idealW - wdialog->currentBorderSize() - space - inventoryUse->width();
  inventoryUse->setPos(xTempUse, yTemp);
  descriptionBack->setSize(xTempUse - xTemp - space / 2,
                           inventoryUse->height());
}

void Plant::NewLanguage() { SetTitle(tr("Plant")); }

void Plant::SetVar(const bool inSelection, const int lastItemSelected) {
  this->inSelection = inSelection;
  this->lastItemSelected = lastItemSelected;
  updatePlant();
}

QPixmap Plant::setCanvas(const QPixmap &image, unsigned int size) {
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

QPixmap Plant::setCanvas(const QImage &image, unsigned int size) {
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

void Plant::updatePlant() {
  const CatchChallenger::Player_private_and_public_informations
      &playerInformations =
          connexionManager->client->get_player_informations_ro();
#ifdef DEBUG_BASEWINDOWS
  qDebug() << "BaseWindow::load_inventory()";
#endif
  inventory_description->setEnabled(false);
  inventoryUse->setEnabled(false);
  back->setVisible(!inSelection);
  next->setVisible(!inSelection);
  plantList->clear();
  // plantList->setIconSize(QSize(targetSize,targetSize));
  auto i = playerInformations.items.begin();
  while (i != playerInformations.items.cend()) {
    const uint16_t id = i->first;
    if (QtDatapackClientLoader::datapackLoader->itemToPlants.find(id) !=
        QtDatapackClientLoader::datapackLoader->itemToPlants.cend()) {
      const DatapackClientLoader::ItemExtra &itemExtra =
          QtDatapackClientLoader::datapackLoader->itemsExtra.at(id);
      const QtDatapackClientLoader::QtPlantExtra &contentExtra =
          QtDatapackClientLoader::datapackLoader->QtplantExtra[id];
      const CatchChallenger::Plant &plant =
          CatchChallenger::CommonDatapack::commonDatapack.plants[id];
      QTreeWidgetItem *item = new QTreeWidgetItem();
      if (QtDatapackClientLoader::datapackLoader->itemsExtra.find(id) !=
              QtDatapackClientLoader::datapackLoader->itemsExtra.cend() &&
          contentExtra.tileset != nullptr) {
        QPixmap p =
            QtDatapackClientLoader::datapackLoader->getImageExtra(id).image;
        // Plant::setCanvas(p,64)
        p = p.scaledToWidth(p.width() * 2);
        item->setIcon(0, p);

        item->setText(0, QString::fromStdString(itemExtra.name) + "\n" +
                             tr("Quantity: ") + QString::number(i->second) +
                             " ");

        if (contentExtra.tileset != nullptr) {
          item->setIcon(1, Plant::setCanvas(
                               contentExtra.tileset->tileAt(0)->image(), 32));
          item->setIcon(2, Plant::setCanvas(
                               contentExtra.tileset->tileAt(1)->image(), 32));
          item->setIcon(3, Plant::setCanvas(
                               contentExtra.tileset->tileAt(2)->image(), 32));
          item->setIcon(4, Plant::setCanvas(
                               contentExtra.tileset->tileAt(3)->image(), 32));
          item->setIcon(5, Plant::setCanvas(
                               contentExtra.tileset->tileAt(4)->image(), 32));
        }

        item->setText(5, QString::fromStdString(
                             CatchChallenger::FacilityLibClient::timeToString(
                                 plant.fruits_seconds)));
        item->setText(6, tr("Quantity: %1")
                             .arg((double)plant.fix_quantity +
                                  ((double)plant.random_quantity) /
                                      RANDOM_FLOAT_PART_DIVIDER));
      } else {
        item->setIcon(
            0, QtDatapackClientLoader::datapackLoader->defaultInventoryImage());

        if (i->second > 1)
          item->setText(0,
                        QStringLiteral("id: %1 (x%2)").arg(id).arg(i->second));
        else
          item->setText(0, QStringLiteral("id: %1").arg(id));

        if (contentExtra.tileset != nullptr) {
          item->setIcon(1, Plant::setCanvas(
                               contentExtra.tileset->tileAt(0)->image(), 32));
          item->setIcon(2, Plant::setCanvas(
                               contentExtra.tileset->tileAt(1)->image(), 32));
          item->setIcon(3, Plant::setCanvas(
                               contentExtra.tileset->tileAt(2)->image(), 32));
          item->setIcon(4, Plant::setCanvas(
                               contentExtra.tileset->tileAt(3)->image(), 32));
          item->setIcon(5, Plant::setCanvas(
                               contentExtra.tileset->tileAt(4)->image(), 32));
        }

        item->setText(5, QString::fromStdString(
                             CatchChallenger::FacilityLibClient::timeToString(
                                 plant.fruits_seconds)));
        item->setText(6, tr("Quantity: %1")
                             .arg((double)plant.fix_quantity +
                                  ((double)plant.random_quantity) /
                                      RANDOM_FLOAT_PART_DIVIDER));
      }
      item->setData(0, 99, id);
      plantList->addTopLevelItem(item);
      if (id == lastItemSelected) item->setSelected(true);
    }
    ++i;
  }
}

void Plant::inventoryUse_slot() {
  QList<QTreeWidgetItem *> items = plantList->selectedItems();
  if (items.size() != 1) return;
  emit useItem(items.first()->data(0, 99).toInt());
}

void Plant::on_inventory_itemSelectionChanged(Node *item) {
  lastItemSelected = item->Data(99);
  if (QtDatapackClientLoader::datapackLoader->itemsExtra.find(item->Data(99)) ==
      QtDatapackClientLoader::datapackLoader->itemsExtra.cend()) {
    inventory_description->SetVisible(false);
    inventoryUse->SetEnabled(false);
    inventory_description->SetText(tr("Unknown description"));
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
