// Copyright 2021 <CatchChallenger>
#include "ShopItem.hpp"

#include <QPainter>
#include <iostream>

#include "../../../../../general/base/CommonDatapack.hpp"
#include "../../../cc/QtDatapackClientLoader.hpp"
#include "../../../ui/Label.hpp"
#include "../../../ui/ThemedButton.hpp"

using Scenes::ShopItem;

ShopItem::ShopItem(uint16_t object_id, uint32_t quantity, uint32_t price) {
  object_id_ = object_id;
  quantity_ = quantity;
  price_ = price;
  is_selected_ = false;
  icon_select_ = Sprite::Create(":/CC/images/interface/star.png", this);
  icon_select_->SetVisible(is_selected_);
  SetSize(60, 60);
}

ShopItem::~ShopItem() {}

ShopItem *ShopItem::Create(uint16_t object_id, uint32_t quantity,
                           uint32_t price) {
  return new (std::nothrow) ShopItem(object_id, quantity, price);
}

void ShopItem::Draw(QPainter *painter) {
  auto item = QtDatapackClientLoader::datapackLoader->itemsExtra.at(object_id_);
  QPixmap p =
      QtDatapackClientLoader::datapackLoader->getImageExtra(object_id_).image;
  p = p.scaledToHeight(32);

  name_ = item.name;

  auto pix = Sprite::Create(":/CC/images/interface/b1-trimmed.png");
  pix->Strech(18, 16, 10, Width(), Height());
  pix->SetPos(0, 0);
  pix->Paint(painter, nullptr, nullptr);

  pix->SetPixmap(p);
  pix->SetPos(10, Height() / 2 - pix->Height() / 2);
  pix->Paint(painter, nullptr, nullptr);

  auto text = UI::Label::Create(Qt::white, QColor(30, 25, 17));
  text->SetText(item.name);
  text->SetPixelSize(12);
  text->SetPos(pix->Width() + pix->X() + 10, 5);
  text->Paint(painter, nullptr, nullptr);

  text->SetPos(pix->Width() + pix->X() + 10, text->Height());
  text->SetText(item.description);
  text->SetPixelSize(8);
  text->Paint(painter, nullptr, nullptr);

  auto button = UI::GreenButton::Create();
  button->SetSize(60, 25);
  button->SetPixelSize(8);
  button->SetText(QStringLiteral("%1 $").arg(price_));
  button->SetPos(Width() - button->Width() - 10,
                 Height() / 2 - button->Height() / 2);
  button->Paint(painter, nullptr, nullptr);

  delete pix;
  delete text;
  delete button;
}

void ShopItem::OnResize() {
  icon_select_->SetSize(40, 40);
  icon_select_->SetPos(10, Height() / 2 - icon_select_->Height() / 2);
  ReDraw();
}

void ShopItem::SetSelected(bool selected) {
  is_selected_ = selected;
  icon_select_->SetVisible(is_selected_);
}

void ShopItem::MousePressEvent(const QPointF &point, bool &press_validated) {
  if (press_validated) {
    SetSelected(false);
    return;
  }
  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  if (t.contains(point)) {
    press_validated = true;
  }
}

void ShopItem::MouseReleaseEvent(const QPointF &point, bool &prev_validated) {
  if (prev_validated) {
    is_selected_ = false;
    return;
  }

  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  if (!prev_validated && IsVisible()) {
    if (t.contains(point)) {
      SetSelected(true);
      if (on_click_) {
        on_click_(this);
      }
    }
  }
}

std::string ShopItem::Name() {
  return name_;
}
