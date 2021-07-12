// Copyright 2021 CatchChallenger
#include "MultiItem.hpp"

#include "../../ConnexionInfo.hpp"
#include "../../ui/Button.hpp"

using Scenes::MultiItem;

MultiItem::MultiItem(const ConnexionInfo &connexionInfo)
    : Button(":/CC/images/interface/button.png"),
      m_connexionInfo(connexionInfo) {
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
  /*
  if (m_connexionInfo.connexionCounter > 0) {
    const QPixmap &p =
        *GameLoader::gameLoader->getImage(":/CC/images/interface/top.png");
    paint.drawPixmap(5, image.width() / 2 - p.width() / 2, p.width(),
                     p.height(), p);
  }*/
  SetText(connexionInfoLabel);
}
