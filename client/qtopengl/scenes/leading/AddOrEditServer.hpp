// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_SCENES_LEADING_ADDOREDITSERVER_HPP_
#define CLIENT_QTOPENGL_SCENES_LEADING_ADDOREDITSERVER_HPP_

#include <QComboBox>
#include <QObject>

#include "../../core/Scene.hpp"
#include "../../core/Sprite.hpp"
#include "../../ui/Button.hpp"
#include "../../ui/Input.hpp"
#include "../../ui/Label.hpp"
#include "../../ui/Combo.hpp"

namespace Scenes {
class AddOrEditServer : public Scene {
 public:
  ~AddOrEditServer();
  static AddOrEditServer* Create();

  int type() const;
  void on_ok_clicked();
  QString server() const;
  uint16_t port() const;
  QString proxyServer() const;
  uint16_t proxyPort() const;
  QString name() const;
  void setType(const int &type);
  void setEdit(const bool &edit);
  void setServer(const QString &server);
  void setPort(const uint16_t &port);
  void setName(const QString &name);
  void setProxyServer(const QString &proxyServer);
  void setProxyPort(const uint16_t &proxyPort);
  bool isOk() const;
  void on_type_currentIndexChanged(int index);
  void on_cancel_clicked();

 protected:
  void OnScreenSD() override;
  void OnScreenHD() override;
  void OnScreenHDR() override;
  void OnScreenResize() override;
  void Draw(QPainter *painter) override;

 private:
  AddOrEditServer();

  bool edit;
  Sprite *wdialog;
  UI::Button *quit;
  UI::Label *title;
  Sprite *label;
  UI::Button *validate;
  UI::Combo *m_type;

  UI::Label *serverText;
  UI::Input *serverInput;
  UI::Input *portInput;

  UI::Label *nameText;
  UI::Input *nameInput;

  UI::Label *proxyText;
  UI::Input *proxyInput;
  UI::Input *proxyPortInput;

  UI::Label *warning;

  int x, y;
  bool ok;

  QString serverPrevious;
  QString portPrevious;
  QString namePrevious;
  QString proxyPrevious;
  QString proxyPortPrevious;

  void removeAbove();
  void newLanguage();
};
}  // namespace Scenes
#endif  // CLIENT_QTOPENGL_SCENES_LEADING_ADDOREDITSERVER_HPP_
