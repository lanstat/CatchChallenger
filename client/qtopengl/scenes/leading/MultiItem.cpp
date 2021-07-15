// Copyright 2021 CatchChallenger
#include "MultiItem.hpp"

#include <QPainter>
#include <iostream>

#include "../../ConnexionInfo.hpp"
#include "../../core/AssetsLoader.hpp"
#include "../../core/Sprite.hpp"
#include "../../ui/Button.hpp"
#include "../../ui/Label.hpp"

using Scenes::MultiItem;

MultiItem::MultiItem(const ConnexionInfo &connexionInfo)
    : Node(), m_connexionInfo(connexionInfo) {
  cache_ = nullptr;

  SetSize(200, 60);
  FetchName();
}

MultiItem::~MultiItem() {}

MultiItem *MultiItem::Create(const ConnexionInfo &connexionInfo) {
  return new (std::nothrow) MultiItem(connexionInfo);
}

void MultiItem::FetchName() {
  QString connexionInfoLabel;
  if (!m_connexionInfo.name.isEmpty()) {
    connexionInfoLabel = m_connexionInfo.name;
  } else {
#ifdef NOTCPSOCKET
    connexionInfoLabel = m_connexionInfo.ws;
#else
  #ifdef NOWEBSOCKET
    connexionInfoLabel = m_connexionInfo.host;
  #else
    if (!m_connexionInfo.host.isEmpty())
      connexionInfoLabel = m_connexionInfo.host;
    else
      connexionInfoLabel = m_connexionInfo.ws;
  #endif
#endif
    if (connexionInfoLabel.size() > 32)
      connexionInfoLabel =
          connexionInfoLabel.left(15) + "..." + connexionInfoLabel.right(15);
  }
  name_ = connexionInfoLabel;
  if (cache_) {
    delete cache_;
    cache_ = nullptr;
  }
}

void MultiItem::Draw(QPainter *painter) {
  if (Width() == 0 || Height() == 0) return;

  auto background = Sprite::Create(":/CC/images/interface/b1-trimmed.png");
  background->Strech(20, 18, 20, Width(), Height());
  auto text = UI::Label::Create(QColor(255, 255, 255), QColor(30, 25, 17));
  text->SetWidth(bounding_rect_.width());
  text->SetAlignment(Qt::AlignCenter);
  text->SetY(17);
  text->SetText(name_);

  background->Draw(painter);

  if (m_connexionInfo.connexionCounter > 0) {
    const QPixmap &p = *AssetsLoader::GetInstance()->GetImage(
        ":/CC/images/interface/star.png");
    const int size = Height() - 20;
    painter->drawPixmap(10, 10, size, size, p);
  }
  text->Paint(painter, nullptr, nullptr);

  delete background;
  delete text;
}

void MultiItem::MousePressEvent(const QPointF &point, bool &press_validated) {
  if (press_validated) return;
  if (!IsVisible()) return;
  if (!IsEnabled()) return;

  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  if (t.contains(point)) {
    press_validated = true;
    ReDraw();
  }
}

void MultiItem::MouseReleaseEvent(const QPointF &point, bool &prev_validated) {
  if (prev_validated) {
    ReDraw();
    return;
  }
  if (!IsEnabled()) return;
  const QRectF &b = BoundingRect();
  const QRectF &t = MapRectToScene(b);
  if (!prev_validated && IsVisible()) {
    if (t.contains(point)) {
      if (on_click_) {
        on_click_(this);
      }
    }
  }
}

void MultiItem::MouseMoveEvent(const QPointF &point) { (void)point; }

void MultiItem::KeyPressEvent(QKeyEvent *event, bool &event_trigger) {
  (void)event;
  (void)event_trigger;
}

void MultiItem::KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) {
  (void)event;
  (void)event_trigger;
}

ConnexionInfo MultiItem::Connection() const { return m_connexionInfo; }
