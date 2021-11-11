// Copyright 2021 CatchChallenger
#include "Shop.hpp"

#include <iostream>

#include "../../../../../general/base/CommonDatapackServerSpec.hpp"
#include "../../../cc/QtDatapackClientLoader.hpp"
#include "../../../core/AssetsLoader.hpp"
#include "../../../core/Logger.hpp"
#include "../../../entities/Utils.hpp"
#include "../../../ui/Row.hpp"
#include "../../test/TestItem.hpp"

using Scenes::Shop;
using std::placeholders::_1;

Shop::Shop() {
  on_item_click_ = nullptr;
  selected_item_ = nullptr;
  SetDialogSize(600, 500);
  portrait_ = Sprite::Create(this);
  seller_name_ = UI::Label::Create(this);
  quit_ = UI::Button::Create(":/CC/images/interface/cancel.png", this);
  buy_ = UI::Button::Create(":/CC/images/interface/buy.png", this);
  content_ = UI::ListView::Create(this);
  connexionManager = ConnectionManager::GetInstance();
  if (!QObject::connect(connexionManager->client,
                        &CatchChallenger::Api_client_real::QthaveShopList,
                        std::bind(&Shop::HaveShopList, this, _1)))
    abort();
  AddActionButton(buy_);
  AddActionButton(quit_);
}

Shop::~Shop() {
  delete portrait_;
  portrait_ = nullptr;
  delete quit_;
  quit_ = nullptr;
  delete content_;
  content_ = nullptr;
}

Shop *Shop::Create() { return new (std::nothrow) Shop(); }

void Shop::Draw(QPainter *painter) {
  UI::Dialog::Draw(painter);
  auto inner = ContentPlainBoundary();
  auto b2 = Sprite::Create(":/CC/images/interface/b2.png");
  b2->Strech(24, 22, 20, 160, 160);
  b2->SetPos(inner.x(), inner.y());
  b2->Paint(painter, nullptr, nullptr);

  int x = b2->X() + b2->Width() + 20;

  b2->Strech(24, 22, 20, inner.width() + inner.x() - x, inner.height());
  b2->SetPos(x, inner.y());
  b2->Paint(painter, nullptr, nullptr);

  delete b2;
}

void Shop::OnEnter() {
  UI::Dialog::OnEnter();
  content_->RegisterEvents();
}

void Shop::OnExit() {
  content_->UnRegisterEvents();
  UI::Dialog::OnExit();
}

void Shop::SetSeller(const CatchChallenger::Bot &bot, uint16_t shop_id) {
  shop_id_ = shop_id;
  if (bot.properties.find("skin") != bot.properties.cend()) {
    QPixmap skin(QString::fromStdString(
        QtDatapackClientLoader::datapackLoader->getFrontSkinPath(
            std::stoi(bot.properties.at("skin")))));
    if (!skin.isNull()) {
      portrait_->SetPixmap(skin.scaled(160, 160));
      portrait_->SetVisible(true);
    } else {
      portrait_->SetVisible(false);
    }
  } else {
    portrait_->SetVisible(false);
  }
  content_->Clear();
#ifndef CATCHCHALLENGER_CLIENT_INSTANT_SHOP
  seller_name_->SetText(tr("Waiting the shop content"));
  connexionManager->client->getShopList(shop_id);
#else
  {
    // std::vector<ItemToSellOrBuy> items;
    const auto &shop =
        CommonDatapackServerSpec::commonDatapackServerSpec.shops.at(shopId);
    std::cout << "LAN_[" << __FILE__ << ":" << __LINE__ << "] "
              << (int)shop.size() << std::endl;
    // unsigned int index = 0;
    // while (index < shop.items.size()) {
    // if (shop.prices.at(index) > 0) {
    // ItemToSellOrBuy newItem;
    // newItem.object = shop.items.at(index);
    // newItem.price = shop.prices.at(index);
    // newItem.quantity = 0;
    // items << newItem;
    //}
    // index++;
    //}
    // haveShopList(items);
  }
#endif
}

void Shop::OnActionClick(Node *node) {
  if (node == quit_) {
    Close();
  } else if (node == buy_) {
    if (selected_item_ != nullptr) {
      connexionManager->client->buyObject(
          selected_item_->Data(90), selected_item_->Data(91),
          selected_item_->Data(92), selected_item_->Data(93));
    }
  }
}

void Shop::OnScreenResize() {
  UI::Dialog::OnScreenResize();

  auto content = ContentPlainBoundary();

  content_->SetPos(content.x() + 190, content.y() + 20);
  content_->SetSize(content.width() - 190, content.height());
  portrait_->SetPos(content.x(), content.y());
  seller_name_->SetPos(content.x(), content.y() + 180);
}

void Shop::Close() { Parent()->RemoveChild(this); }

void Shop::OnItemClick(Node *node) {
  selected_item_ = node;
  auto items = content_->Items();
  for (auto item : items) {
    if (node == item) continue;
    static_cast<TestItem *>(item)->SetSelected(false);
  }
}

void Shop::HaveShopList(
    const std::vector<CatchChallenger::ItemToSellOrBuy> &items) {
  seller_name_->SetVisible(false);

  for (auto it = begin(items); it != end(items); it++) {
    auto node = Scenes::TestItem::Create();

    auto item =
        QtDatapackClientLoader::datapackLoader->itemsExtra.at((*it).object);
    QPixmap p =
        QtDatapackClientLoader::datapackLoader->getImageExtra((*it).object)
            .image;
    p = p.scaledToHeight(32);
    node->SetPixmap(p);
    node->SetText(QString::fromStdString(item.name));
    node->SetOnClick(std::bind(&Shop::OnItemClick, this, _1));
    node->SetData(90, shop_id_);
    node->SetData(91, (*it).object);
    node->SetData(92, 1);
    node->SetData(93, (*it).price);
    content_->AddItem(node);
  }
}
