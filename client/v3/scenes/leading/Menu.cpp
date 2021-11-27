// Copyright 2021 CatchChallenger
#include "Menu.hpp"

#include <QDesktopServices>
#include <QWidget>
#include <iostream>
#include <string>

#include "../../../general/base/CommonSettingsCommon.hpp"
#include "../../../general/base/Version.hpp"
#include "../../Language.hpp"
#include "../../Settings.hpp"
#include "../../Ultimate.hpp"
#include "../../cc/QtDatapackClientLoader.hpp"
#include "../../core/AssetsLoader.hpp"
#include "../../core/SceneManager.hpp"
#include "../../core/Sprite.hpp"
#include "../../core/StackedScene.hpp"
#include "../../entities/FeedNews.hpp"
#include "../../entities/InternetUpdater.hpp"
#include "../../ui/Button.hpp"
#include "../../ui/Label.hpp"
#include "../../ui/ThemedButton.hpp"
#include "../ParallaxForest.hpp"
#include "Loading.hpp"
#include "Multi.hpp"

#ifndef CATCHCHALLENGER_NOAUDIO
  #include "../../core/AudioPlayer.hpp"
#endif

using Scenes::Menu;
using std::placeholders::_1;
using std::placeholders::_2;

Menu::Menu() {
  is_loaded_ = false;
  have_update_ = false;
  current_news_type_ = 0;

  solo_ = nullptr;

  auto loader = AssetsLoader::GetInstance();
  if (!loader->IsLoaded()) {
    loader->SetOnDataParsed(std::bind(&Menu::OnAssetsDataParsed, this));
    Loading::GetInstance()->SetNotifier(loader);
    AddChild(Loading::GetInstance());
  } else {
    is_loaded_ = true;
    Initialize();
  }
}

Menu::~Menu() {
  delete update_;
  if (solo_scene_) delete solo_scene_;
  if (multi_scene_) delete multi_scene_;
}

Menu *Menu::Create() { return new (std::nothrow) Menu(); }

void Menu::Initialize() {
  solo_ = UI::Button::Create(":/CC/images/interface/button.png", this);
  multi_ = UI::Button::Create(":/CC/images/interface/button.png", this);
  options_ = UI::Button::Create(":/CC/images/interface/options.png", this);
  debug_ = UI::Button::Create(":/CC/images/interface/debug.png", this);
  facebook_ = UI::Button::Create(":/CC/images/interface/facebook.png", this);
  facebook_->SetOutlineColor(QColor(0, 79, 154));
  facebook_->SetPixelSize(28);
  website_ = UI::Button::Create(":/CC/images/interface/bluetoolbox.png", this);
  website_->SetText("w");
  website_->SetOutlineColor(QColor(0, 79, 154));
  website_->SetPixelSize(28);
  warning_can_be_reset_ = true;

  have_fresh_feed_ = false;
  news_ = Sprite::Create(":/CC/images/interface/message.png", this);
  news_list_ = UI::ListView::Create(news_);

  // work
  UpdateNews();
  news_wait_ = Sprite::Create(news_);
  news_wait_->SetPixmap(":/CC/images/multi/busy.png");

  warning_ = UI::Label::Create(this);
  warning_->SetVisible(false);
  warning_string_ = QString(
      "<div style=\"background-color: rgb(255, 180, 180);border: 1px solid "
      "rgb(255, 221, 50);border-radius:5px;color: rgb(0, 0, "
      "0);\">&nbsp;%1&nbsp;</div>");
  news_update_ =
      UI::Button::Create(":/CC/images/interface/greenbutton.png", news_);
  news_update_->SetOutlineColor(QColor(44, 117, 0));
#ifndef __EMSCRIPTEN__
  InternetUpdater::internetUpdater = new InternetUpdater();
  if (!QObject::connect(InternetUpdater::internetUpdater,
                        &InternetUpdater::newUpdate,
                        std::bind(&Menu::NewUpdate, this, _1)))
    abort();
#endif
  FeedNews::feedNews = new FeedNews();
  if (!QObject::connect(FeedNews::feedNews, &FeedNews::feedEntryList,
                        std::bind(&Menu::FeedEntryList, this, _1, _2)))
    std::cerr << "connect(RssNews::rssNews,&RssNews::rssEntryList,this,&"
                 "MenuWindow::rssEntryList) failed"
              << std::endl;
  FeedNews::feedNews->checkCache();
#ifndef CATCHCHALLENGER_SOLO
  solo_->SetVisible(false);
#endif

  facebook_->SetOnClick(std::bind(&Menu::OpenFacebook, this));
  website_->SetOnClick(std::bind(&Menu::OpenWebsite, this));
  news_update_->SetOnClick(std::bind(&Menu::OpenUpdate, this));
  solo_->SetOnClick(std::bind(&Menu::GoToSolo, this));
  multi_->SetOnClick(std::bind(&Menu::GoToMulti, this));
  options_->SetOnClick(std::bind(&Menu::GoToOptions, this));
  debug_->SetOnClick(std::bind(&Menu::GoToDebug, this));

  solo_scene_ = nullptr;
  multi_scene_ = nullptr;
  option_scene_ = nullptr;

#ifndef CATCHCHALLENGER_NOAUDIO
  if (!Settings::settings->contains("audioVolume"))
    Settings::settings->setValue("audioVolume", 80);
  AudioPlayer::GetInstance()->SetVolume(
      Settings::settings->value("audioVolume").toUInt());
  const std::string &terr =
      AudioPlayer::GetInstance()->StartAmbiance(":/CC/music/loading.opus");
#endif

  ChangeLanguage();
}

void Menu::setError(const std::string &error) {
  /*
  if (warning_->toHtml().isEmpty() || !warning->isVisible() ||
      warningCanBeReset) {
    warningCanBeReset = false;
    std::cerr << "ScreenTransition::errorString(" << error << ")" << std::endl;
    warning->setVisible(true);
    warning->setHtml(warningString.arg(QString::fromStdString(error)));
  }
  if (error.empty()) {
    warningCanBeReset = true;
    warning->setVisible(false);
  }*/
}

#ifndef __EMSCRIPTEN__
void Menu::NewUpdate(const std::string &version) {
  Q_UNUSED(version);
  have_update_ = true;
  /*
      ui->update->setText(QString::fromStdString(InternetUpdater::getText(version)));
      ui->update->setVisible(true);*/
}
#endif

void Menu::FeedEntryList(const std::vector<FeedNews::FeedEntry> &entryList,
                         std::string error) {
  entry_list_ = entryList;
  if (entry_list_.empty()) {
    if (!error.empty()) setError(error);
    return;
  }
  if (current_news_type_ != 0) UpdateNews();

  news_wait_->SetVisible(false);
  have_fresh_feed_ = true;
}

void Menu::UpdateNews() {
  switch (current_news_type_) {
    case 0:
      if (news_->IsVisible()) news_->SetVisible(false);
      break;
    case 1:
      if (entry_list_.empty()) {
        news_->SetVisible(false);
      } else {
        news_list_->Clear();
        if (entry_list_.size() > 1) {
          auto item = UI::YellowButton::Create();
          item->SetHeight(30);
          item->SetPixelSize(10);
          item->SetText(entry_list_.at(0).title);
          item->SetData(99, entry_list_.at(0).link.toStdString());
          item->SetOnClick(std::bind(&Menu::OnClickLink, this, _1));
          news_list_->AddItem(item);
        }
      }
      break;
    default:
    case 2:
      if (entry_list_.empty()) {
        news_->SetVisible(false);
      } else {
        news_list_->Clear();
        auto text = UI::Label::Create();
        text->SetPixelSize(12);
        text->SetText(tr("Latest news:"));
        news_list_->AddItem(text);
        if (entry_list_.size() == 1) {
          auto item = UI::YellowButton::Create();
          item->SetHeight(30);
          item->SetPixelSize(10);
          item->SetText(entry_list_.at(0).title);
          item->SetData(99, entry_list_.at(0).link.toStdString());
          item->SetOnClick(std::bind(&Menu::OnClickLink, this, _1));
          news_list_->AddItem(item);
        } else {
          unsigned int index = 0;
          while (index < entry_list_.size() && index < 3) {
            auto item = UI::YellowButton::Create();
            item->SetHeight(30);
            item->SetPixelSize(10);
            item->SetText(entry_list_.at(index).title);
            item->SetData(99, entry_list_.at(index).link.toStdString());
            item->SetOnClick(std::bind(&Menu::OnClickLink, this, _1));
            news_list_->AddItem(item);
            index++;
          }
        }
      }
      break;
  }
}

void Menu::OpenWebsite() {
  if (!QDesktopServices::openUrl(
          QUrl("https://catchchallenger.first-world.info/")))
    std::cerr << "Menu::openWebsite() failed" << std::endl;
}

void Menu::OpenFacebook() {
  if (!QDesktopServices::openUrl(
          QUrl("https://www.facebook.com/CatchChallenger/")))
    std::cerr << "Menu::openFacebook() failed" << std::endl;
}

void Menu::OpenUpdate() {
  if (!QDesktopServices::openUrl(
          QUrl("https://catchchallenger.first-world.info/download.html")))
    std::cerr << "Menu::openFacebook() failed" << std::endl;
}

void Menu::GoToSolo() {
  if (!solo_scene_) solo_scene_ = Solo::Create();
  static_cast<StackedScene *>(Parent())->PushForeground(solo_scene_);
}

void Menu::GoToMulti() {
  if (!multi_scene_) multi_scene_ = Multi::Create();
  static_cast<StackedScene *>(Parent())->PushForeground(multi_scene_);
}

void Menu::GoToOptions() {
  if (!option_scene_) option_scene_ = Options::Create();
  AddChild(option_scene_);
}

void Menu::GoToDebug() {}

void Menu::ChangeLanguage() {
  if (!is_loaded_) return;
  solo_->SetText(tr("Solo"));
  multi_->SetText(tr("Multi"));
  news_update_->SetText(tr("Update!"));
  UpdateNews();
}

void Menu::OnEnter() {
  if (!is_loaded_) return;
  solo_->RegisterEvents();
  multi_->RegisterEvents();
  options_->RegisterEvents();
  facebook_->RegisterEvents();
  website_->RegisterEvents();
  news_list_->RegisterEvents();
}

void Menu::OnExit() {
  if (!is_loaded_) return;
  solo_->UnRegisterEvents();
  multi_->UnRegisterEvents();
  options_->UnRegisterEvents();
  facebook_->UnRegisterEvents();
  website_->UnRegisterEvents();
  news_list_->UnRegisterEvents();
}

void Menu::OnAssetsDataParsed() {
  is_loaded_ = true;
  RemoveChild(Loading::GetInstance());
  SceneManager::GetInstance()->setWindowTitle(
      tr("CatchChallenger %1")
          .arg(QString::fromStdString(CatchChallenger::Version::str)));
  QtDatapackClientLoader::datapackLoader = new QtDatapackClientLoader();

  Initialize();

  // Reload events
  OnScreenResize();
  OnEnter();
}

void Menu::OnClickLink(Node *node) {
  if (!QDesktopServices::openUrl(
        QUrl(QString::fromStdString(node->DataStr(99)))))
    std::cerr << "Menu::OnClickLink() failed" << std::endl;
}
