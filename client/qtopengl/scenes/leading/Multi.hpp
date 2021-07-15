// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_SCENES_LEADING_MULTI_HPP_
#define CLIENT_QTOPENGL_SCENES_LEADING_MULTI_HPP_

#include <QHash>
#include <QNetworkAccessManager>
#include <QSet>
#include <QString>
#include <vector>

#include "../../ConnexionInfo.hpp"
#include "../../core/Scene.hpp"
#include "../../core/Sprite.hpp"
#include "../../ui/Button.hpp"
#include "../../ui/Input.hpp"
#include "../../ui/Label.hpp"
#include "../../ui/ListView.hpp"
#include "AddOrEditServer.hpp"

class ListEntryEnvolued;
class Login;
class MultiItem;
class QNetworkReply;

class SelectedServer {
 public:
  QString unique_code;
  bool isCustom;
};

namespace Scenes {
class Multi : public Scene {
 public:
  static Multi *Create();
  ~Multi();
  void displayServerList();
  void server_add_clicked();
  void server_add_finished();
  void server_edit_clicked();
  void server_edit_finished();
  void server_select_clicked();
  void server_select_finished();
  void server_remove_clicked();
  void saveConnexionInfoList();
  std::vector<ConnexionInfo> loadXmlConnexionInfoList();
  std::vector<ConnexionInfo> loadXmlConnexionInfoList(
      const QByteArray &xmlContent);
  std::vector<ConnexionInfo> loadXmlConnexionInfoList(const QString &file);
  std::vector<ConnexionInfo> loadConfigConnexionInfoList();
  void httpFinished();
  void downloadFile();
  void newLanguage();
  void on_server_refresh_clicked();
  void OnEnter() override;
  void OnExit() override;

 protected:
  void OnScreenSD() override;
  void OnScreenHD() override;
  void OnScreenHDR() override;
  void OnScreenResize() override;

  void OnSelectServer(Node* item);

 private:
  Multi();
  std::vector<ConnexionInfo> temp_customConnexionInfoList,
      temp_xmlConnexionInfoList, mergedConnexionInfoList;
  QList<MultiItem *> serverConnexion;
  SelectedServer selectedServer;  // no selected if unique_code empty
  AddOrEditServer *addServer;
  Login *login;

  QNetworkAccessManager qnam;
  QNetworkReply *reply;

  UI::Button *server_add;
  UI::Button *server_remove;
  UI::Button *server_edit;
  UI::Button *server_select;
  UI::Button *server_refresh;
  UI::Button *back;
  UI::Label *warning;

  Sprite *wdialog;
  UI::Label *serverEmpty;
  UI::ListView *scrollZone;

  UI::Input *tmmp_;

  void GoBack();
  void connectToServer(ConnexionInfo connexionInfo, QString login,
                       QString pass);
};
}  // namespace Scenes
#endif  // CLIENT_QTOPENGL_SCENES_LEADING_MULTI_HPP_
