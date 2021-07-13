// Copyright 2021 CatchChallenger
#include "AddOrEditServer.hpp"

#include <QDesktopServices>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QPainter>
#include <QTextDocument>

#include "../../../qt/Settings.hpp"
#include "../../../qt/Ultimate.hpp"
#include "../../core/AssetsLoader.hpp"

using Scenes::AddOrEditServer;

AddOrEditServer::AddOrEditServer() {
  wdialog = Sprite::Create(":/CC/images/interface/message.png", this);
  label = Sprite::Create(this);
  ok = false;
  edit = false;

  x = -1;
  y = -1;
  label->SetPixmap(":/CC/images/interface/label.png");
  quit = UI::Button::Create(":/CC/images/interface/cancel.png", this);
  validate = UI::Button::Create(":/CC/images/interface/validate.png", this);

  QLinearGradient gradient1(0, 0, 0, 100);
  gradient1.setColorAt(0.25, QColor(230, 153, 0));
  gradient1.setColorAt(0.75, QColor(255, 255, 255));
  QLinearGradient gradient2(0, 0, 0, 100);
  gradient2.setColorAt(0, QColor(64, 28, 2));
  gradient2.setColorAt(1, QColor(64, 28, 2));
  title = UI::Label::Create(gradient2, gradient1, this);

  QPixmap p = *AssetsLoader::GetInstance()->GetImage(
      ":/CC/images/interface/inputText.png");
  p = p.scaled(p.width(), 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  m_type = UI::Combo::Create(":/CC/images/interface/button.png", this);
  m_type->AddItem(QString("Tcp"));
  m_type->AddItem(QString("WS"));
  serverText = UI::Label::Create(this);
  serverText->SetVisible(false);
  serverInput = UI::Input::Create(this);
  // serverInput->setPlaceholderText("ws://www.server.com:9999/");
  portInput = UI::Input::Create(this);
  /*
  portInput->setMinimum(1);
  portInput->setMaximum(65535);
  */
  portInput->SetValue(QString::number(42489));

  nameText = UI::Label::Create(this);
  nameInput = UI::Input::Create(this);

  proxyText = UI::Label::Create(this);
  proxyInput = UI::Input::Create(this);
  proxyPortInput = UI::Input::Create(this);
  /*
  proxyPortInput->setMinimum(1);
  proxyPortInput->setMaximum(65535);
  */

  warning = UI::Label::Create(this);
  warning->SetVisible(false);

  /*
  if (!connect(
          m_type,
          static_cast<void (ComboBox::*)(int)>(&ComboBox::currentIndexChanged),
          this, &AddOrEditServer::on_type_currentIndexChanged,
          Qt::QueuedConnection))
    abort();
    */
  on_type_currentIndexChanged(0);

  quit->SetOnClick(std::bind(&AddOrEditServer::on_cancel_clicked, this));
  validate->SetOnClick(std::bind(&AddOrEditServer::on_ok_clicked, this));

  newLanguage();

#if defined(NOTCPSOCKET) || defined(NOWEBSOCKET)
  m_type->SetVisible(false);
  #if defined(NOTCPSOCKET)
  proxyPortInput->SetVisible(false);
  // serverInput->setPlaceholderText("ws://www.server.com:9999/");
  #endif
#endif
  RegisterMouseListener(quit);
  RegisterMouseListener(validate);
  RegisterMouseListener(nameInput);
  RegisterKeyboardListerner(nameInput);
}

AddOrEditServer::~AddOrEditServer() {
  delete wdialog;
  delete quit;
  delete validate;
  delete title;
  delete warning;
  delete m_type;
  delete nameText;
}

AddOrEditServer *AddOrEditServer::Create() {
  return new (std::nothrow) AddOrEditServer();
}

void AddOrEditServer::OnScreenResize() {
  bool forcedCenter = true;
  int idealW = 900;
  int idealH = 400;
  if (bounding_rect_.width() < 600 || bounding_rect_.height() < 480) {
    idealW /= 2;
    idealH /= 2;
  }
  if (idealW > bounding_rect_.width()) idealW = bounding_rect_.width();
  int top = 36;
  int bottom = 94 / 2;
  if (bounding_rect_.width() < 600 || bounding_rect_.height() < 480) {
    top /= 2;
    bottom /= 2;
  }
  if ((idealH + top + bottom) > bounding_rect_.height())
    idealH = bounding_rect_.height() - (top + bottom);

  if (x < 0 || y < 0 || forcedCenter) {
    x = bounding_rect_.width() / 2 - idealW / 2;
    y = bounding_rect_.height() / 2 - idealH / 2;
  } else {
    if ((x + idealW) > bounding_rect_.width())
      x = bounding_rect_.width() - idealW;
    if ((y + idealH + bottom) > bounding_rect_.height())
      y = bounding_rect_.height() - (idealH + bottom);
    if (y < top) y = top;
  }

  wdialog->SetPos(x, y);
  wdialog->SetSize(idealW, idealH);

  auto font = serverText->GetFont();
  if (bounding_rect_.width() < 600 || bounding_rect_.height() < 480) {
    // label->SetScale(0.5);
    quit->SetSize(83 / 2, 94 / 2);
    validate->SetSize(83 / 2, 94 / 2);
    label->SetPos(x + (idealW - label->Width() / 2) / 2, y - 36 / 2);
    title->SetPixelSize(30 / 2);
    font.setPixelSize(30 / 2);
  } else {
    // label->SetScale(1);
    quit->SetSize(83, 94);
    validate->SetSize(83, 94);
    label->SetPos(x + (idealW - label->Width()) / 2, y - 36);
    title->SetPixelSize(30);
    font.setPixelSize(30);
  }
  serverText->SetFont(font);
  nameText->SetFont(font);
  proxyText->SetFont(font);

  unsigned int nameBackgroundNewHeight = 50;
  unsigned int space = 30;
  if (bounding_rect_.width() < 600 || bounding_rect_.height() < 480) {
    font.setPixelSize(30 * 0.75 / 2);
    serverInput->SetFont(font);
    portInput->SetFont(font);
    nameInput->SetFont(font);
    proxyInput->SetFont(font);
    proxyPortInput->SetFont(font);
    space = 10;
    nameBackgroundNewHeight = 50 / 2;
  } else {
    font.setPixelSize(30 * 0.75);
    serverInput->SetFont(font);
    portInput->SetFont(font);
    nameInput->SetFont(font);
    proxyInput->SetFont(font);
    proxyPortInput->SetFont(font);
  }

  quit->SetPos(x + (idealW / 2 - quit->Width() - space / 2),
               y + idealH - quit->Height() / 2);
  validate->SetPos(x + (idealW / 2 + space / 2),
                   y + idealH - quit->Height() / 2);
  const QRectF trect = title->BoundingRect();
  title->SetPos(x + (idealW - title->BoundingRect().width()) / 2,
                y - trect.height() / 2);

  const QRectF &serverTextRect = serverText->BoundingRect();
  const QRectF &nameTextRect = nameText->BoundingRect();
  const QRectF &proxyTextRect = proxyText->BoundingRect();

  int border_size = 46;

  const unsigned int &serverBackgroundNewWidth =
      idealW - nameTextRect.width() - border_size * 4;
  const unsigned int &nameBackgroundNewWidth =
      idealW - nameTextRect.width() - border_size * 4;
  const unsigned int &proxyBackgroundNewWidth =
      idealW - nameTextRect.width() - border_size * 4;
  if ((unsigned int)nameInput->Width() != nameBackgroundNewWidth ||
      (unsigned int)nameInput->Height() != nameBackgroundNewHeight) {
    serverInput->SetHeight(nameBackgroundNewHeight);
    portInput->SetHeight(nameBackgroundNewHeight);
    if (portInput->IsVisible()) {
      serverInput->SetWidth(serverBackgroundNewWidth * 3 / 4);
      portInput->SetWidth(serverBackgroundNewWidth * 1 / 4);
    } else
      serverInput->SetWidth(serverBackgroundNewWidth);
    nameInput->SetSize(nameBackgroundNewWidth, nameBackgroundNewHeight);
    proxyInput->SetSize(proxyBackgroundNewWidth * 3 / 4,
                        nameBackgroundNewHeight);
    proxyPortInput->SetSize(proxyBackgroundNewWidth * 1 / 4,
                            nameBackgroundNewHeight);
  }
  {
    m_type->SetPos(x + border_size * 2, y + top * 1.5);
    serverText->SetPos(x + border_size * 2, y + top * 1.5);
    const unsigned int serverBackgroundW =
        serverText->X() + serverTextRect.width();
    serverInput->SetPos(
        serverBackgroundW,
        serverText->Y() +
            (serverTextRect.height() - serverInput->BoundingRect().height()) /
                2);
    if (portInput->IsVisible())
      portInput->SetPos(
          serverBackgroundW + serverInput->Width(),
          serverText->Y() +
              (serverTextRect.height() - serverInput->BoundingRect().height()) /
                  2);
  }
  {
    nameText->SetPos(x + border_size * 2,
                     serverText->Y() + serverTextRect.height() + space);
    const unsigned int nameBackgroundW = nameText->X() + nameTextRect.width();
    nameInput->SetPos(
        nameBackgroundW,
        nameText->Y() +
            (nameTextRect.height() - nameInput->BoundingRect().height()) / 2);
  }
  {
    proxyText->SetPos(x + border_size * 2,
                      nameText->Y() + nameTextRect.height() + space);
    const unsigned int proxyBackgroundW =
        proxyText->X() + proxyTextRect.width();
    proxyInput->SetPos(
        proxyBackgroundW,
        proxyText->Y() +
            (proxyTextRect.height() - proxyInput->BoundingRect().height()) / 2);
    proxyPortInput->SetPos(
        proxyBackgroundW + proxyInput->Width(),
        proxyText->Y() +
            (proxyTextRect.height() - proxyInput->BoundingRect().height()) / 2);
  }
}

void AddOrEditServer::newLanguage() {
  proxyText->SetText(tr("Proxy: "));
  nameText->SetText(tr("Name: "));
  serverText->SetText(tr("Server: "));
  if (edit)
    title->SetText(tr("EDIT"));
  else
    title->SetText(tr("ADD"));
}

int AddOrEditServer::type() const {
#if !defined(NOTCPSOCKET) && !defined(NOWEBSOCKET)
  return m_type->CurrentIndex();
#else
  #if defined(NOTCPSOCKET)
  return 1;
  #else
    #if defined(NOWEBSOCKET)
  return 0;
    #else
      #error add server but no tcp or web socket defined
  return -1;
    #endif
  #endif
#endif
}

void AddOrEditServer::setType(const int &type) {
#if !defined(NOTCPSOCKET) && !defined(NOWEBSOCKET)
  m_type->SetCurrentIndex(type);
#else
  #if defined(NOTCPSOCKET)
  m_type->SetCurrentIndex(1);
  #else
    #if defined(NOWEBSOCKET)
  m_type->SetCurrentIndex(0);
    #endif
  #endif
#endif
}

void AddOrEditServer::setEdit(const bool &edit) {
  this->edit = edit;
  title->SetText(tr("EDIT"));
}

void AddOrEditServer::on_ok_clicked() {
  if (nameText->Text() == QStringLiteral("internal")) {
    warning->SetText(tr("The name can't be \"internal\""));
    warning->SetVisible(true);
    return;
  }
  if (type() == 0) {
    if (!server().contains(QRegularExpression("^[a-zA-Z0-9\\.:\\-_]+$"))) {
      warning->SetText(tr("The host seam don't be a valid hostname or ip"));
      warning->SetVisible(true);
      return;
    }
  } else if (type() == 1) {
    if (!server().startsWith("ws://") && !server().startsWith("wss://")) {
      warning->SetText(
          tr("The web socket url seam wrong, not start with ws:// or wss://"));
      warning->SetVisible(true);
      return;
    }
  }
  ok = true;
  // emit removeAbove();
  // close();
}

QString AddOrEditServer::server() const { return serverInput->Value(); }

uint16_t AddOrEditServer::port() const { return portInput->Value().toUInt(); }

QString AddOrEditServer::proxyServer() const { return proxyInput->Value(); }

uint16_t AddOrEditServer::proxyPort() const {
  return proxyPortInput->Value().toUInt();
}

QString AddOrEditServer::name() const { return nameInput->Value(); }

void AddOrEditServer::setServer(const QString &server) {
  serverInput->SetValue(server);
}

void AddOrEditServer::setPort(const uint16_t &port) {
  portInput->SetValue(QString::number(port));
}

void AddOrEditServer::setName(const QString &name) {
  nameInput->SetValue(name);
}

void AddOrEditServer::setProxyServer(const QString &proxyServer) {
  proxyInput->SetValue(proxyServer);
}

void AddOrEditServer::setProxyPort(const uint16_t &proxyPort) {
  proxyPortInput->SetValue(QString::number(proxyPort));
}

bool AddOrEditServer::isOk() const { return ok; }

void AddOrEditServer::on_type_currentIndexChanged(int index) {
  switch (index) {
    case 0:
      portInput->SetVisible(true);
      serverInput->SetPlaceholder("www.server.com");
      break;
    case 1:
      portInput->SetVisible(false);
      serverInput->SetPlaceholder("ws://www.server.com:9999/");
      break;
    default:
      return;
  }
}

void AddOrEditServer::on_cancel_clicked() {
  ok = false;
  // emit removeAbove();
}

void AddOrEditServer::Draw(QPainter *painter) {
  painter->fillRect(0, 0, bounding_rect_.width(), bounding_rect_.height(),
                    QColor(0, 0, 0, 120));
}

void AddOrEditServer::OnScreenSD() {}

void AddOrEditServer::OnScreenHD() {}

void AddOrEditServer::OnScreenHDR() {}
