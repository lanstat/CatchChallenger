// Copyright 2021 CatchChallenger
#include "Menu.hpp"

#include <QDesktopServices>
#include <QWidget>
#include <iostream>

#include "../../../qt/FeedNews.hpp"
#include "../../../qt/GameLoader.hpp"
#include "../../../qt/InternetUpdater.hpp"
#include "../../../qt/Settings.hpp"
#include "../../../qt/Ultimate.hpp"
#include "../../Language.hpp"
#include "../../core/Sprite.hpp"
#include "../../ui/Button.hpp"
#include "../../ui/Label.hpp"
#include "../ParallaxForest.hpp"

#ifndef CATCHCHALLENGER_NOAUDIO
  #include "../../AudioGL.hpp"
#endif

Menu::Menu() {
  have_update_ = false;
  current_news_type_ = 0;

  AddChild(ParallaxForest::Create());

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
  website_->UpdateTextPercent(75);
  warning_can_be_reset_ = true;

  have_fresh_feed_ = false;
  news_ = Sprite::Create(":/CC/images/interface/message.png", this);
  news_text_ = UI::Label::Create(news_);
  /*
  newsTxt->setOpenExternalLinks(true);
  newsText->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  newsText->setTextInteractionFlags(Qt::TextBrowserInteraction);
  */
  // work
  UpdateNews();
  news_wait_ = Sprite::Create(news_);
  if (GameLoader::gameLoader == nullptr)
    GameLoader::gameLoader = new GameLoader();
  news_wait_->SetPixmap(
      *GameLoader::gameLoader->getImage(":/CC/images/multi/busy.png"));

  warning_ = UI::Label::Create(this);
  warning_->SetVisible(false);
  warning_string_ = QString(
      "<div style=\"background-color: rgb(255, 180, 180);border: 1px solid "
      "rgb(255, 221, 50);border-radius:5px;color: rgb(0, 0, "
      "0);\">&nbsp;%1&nbsp;</div>");
  news_update_ =
      UI::Button::Create(":/CC/images/interface/greenbutton.png", news_);
  news_update_->SetOutlineColor(QColor(44, 117, 0));

  /*
#ifndef __EMSCRIPTEN__
  InternetUpdater::internetUpdater = new InternetUpdater();
  if (!connect(InternetUpdater::internetUpdater, &InternetUpdater::newUpdate,
               this, &Menu::newUpdate))
    abort();
#endif
  FeedNews::feedNews = new FeedNews();
  if (!connect(FeedNews::feedNews, &FeedNews::feedEntryList, this,
               &Menu::FeedEntryList))
    std::cerr << "connect(RssNews::rssNews,&RssNews::rssEntryList,this,&"
                 "MenuWindow::rssEntryList) failed"
              << std::endl;
  FeedNews::feedNews->checkCache();
  */
#ifndef CATCHCHALLENGER_SOLO
  solo_->hide();
#endif

  /*if (!connect(facebook, &CustomButton::clicked, this, &Menu::openFacebook))
    abort();
  if (!connect(website, &CustomButton::clicked, this, &Menu::openWebsite))
    abort();
  if (!connect(newsUpdate, &CustomButton::clicked, this, &Menu::openUpdate))
    abort();
  if (!connect(solo, &CustomButton::clicked, this, &Menu::goToSolo)) abort();
  if (!connect(multi, &CustomButton::clicked, this, &Menu::goToMulti)) abort();
  if (!connect(options, &CustomButton::clicked, this, &Menu::goToOptions))
    abort();
  if (!connect(debug, &CustomButton::clicked, this, &Menu::goToDebug)) abort();
  if (!connect(&Language::language, &Language::newLanguage, this,
               &Menu::newLanguage, Qt::QueuedConnection))
    abort();*/

#ifndef CATCHCHALLENGER_NOAUDIO
  if (!Settings::settings->contains("audioVolume"))
    Settings::settings->setValue("audioVolume", 80);
  AudioGL::GetInstance()->setVolume(
      Settings::settings->value("audioVolume").toUInt());
  const std::string &terr =
      AudioGL::GetInstance()->startAmbiance(":/CC/music/loading.opus");
  if (!terr.empty()) setError(terr);
/*else
    setError("Sound ok:
"+QAudioDeviceInfo::defaultOutputDevice().deviceName().toStdString()); #else
    setError("Sound disabled");*/
#endif
  ChangeLanguage();

  // news 46,
}

Menu::~Menu() { delete update_; }

Menu *Menu::Create() { return new (std::nothrow) Menu(); }

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

void Menu::OnScreenResize() {
}

void Menu::OnScreenSD() {}

void Menu::OnScreenHD() {
  int buttonMargin = 10;
  solo_->SetSize(148, 61);
  solo_->SetPixelSize(23);
  multi_->SetSize(148, 61);
  multi_->SetPixelSize(23);
  options_->SetSize(41, 46);
  options_->SetPixelSize(23);
  debug_->SetSize(41, 46);
  debug_->SetPixelSize(23);
  facebook_->SetSize(41, 46);
  facebook_->SetPixelSize(18);
  website_->SetSize(41, 46);
  website_->SetPixelSize(18);
}

void Menu::OnScreenHDR() {}

/*
void Menu::paint(QPainter *, const QStyleOptionGraphicsItem *,
                 QWidget *widget) {
  int buttonMargin = 10;
  if (widget->width() < 600 || widget->height() < 600) {
    solo->setSize(148, 61);
    solo->setPixelSize(23);
    multi->setSize(148, 61);
    multi->setPixelSize(23);
    options->setSize(41, 46);
    options->setPixelSize(23);
    debug->setSize(41, 46);
    debug->setPixelSize(23);
    facebook->setSize(41, 46);
    facebook->setPixelSize(18);
    website->setSize(41, 46);
    website->setPixelSize(18);
  } else {
    buttonMargin = 30;
    solo->setSize(223, 92);
    solo->setPixelSize(35);
    multi->setSize(223, 92);
    multi->setPixelSize(35);
    options->setSize(62, 70);
    options->setPixelSize(35);
    debug->setSize(62, 70);
    debug->setPixelSize(35);
    facebook->setSize(62, 70);
    facebook->setPixelSize(28);
    website->setSize(62, 70);
    website->setPixelSize(28);
  }
  int verticalMargin =
      widget->height() / 2 + multi->height() / 2 + buttonMargin;
  if (solo->isVisible()) {
    solo->setPos(widget->width() / 2 - solo->width() / 2,
                 widget->height() / 2 - multi->height() / 2 - solo->height() -
                     buttonMargin);
    multi->setPos(widget->width() / 2 - multi->width() / 2,
                  widget->height() / 2 - multi->height() / 2);
  } else {
    multi->setPos(widget->width() / 2 - multi->width() / 2,
                  widget->height() / 2 - multi->height());
    verticalMargin = widget->height() / 2 + buttonMargin;
  }
  debug->setPos(10, 10);
  const int horizontalMargin = (multi->width() - options->width() -
                                facebook->width() - website->width()) /
                               2;
  options->setPos(widget->width() / 2 - facebook->width() / 2 -
                      horizontalMargin - options->width(),
                  verticalMargin);
  facebook->setPos(widget->width() / 2 - facebook->width() / 2, verticalMargin);
  website->setPos(
      widget->width() / 2 + facebook->width() / 2 + horizontalMargin,
      verticalMargin);
  warning->setPos(widget->width() / 2 - warning->boundingRect().width() / 2, 5);

  if (widget->height() < 280) {
    newsUpdate->setVisible(false);
    if (currentNewsType != 0) {
      currentNewsType = 0;
      updateNews();
    }
  } else {
    news->setPos(widget->width() / 2 - news->width() / 2,
                 widget->height() - news->height() - 5);
    newsText->setPos(news->currentBorderSize() + 2,
                     news->currentBorderSize() + 2);
    newsWait->setPos(news->width() - newsWait->pixmap().width() -
                         news->currentBorderSize() - 2,
                     news->height() / 2 - newsWait->pixmap().height() / 2);
    news->setVisible(true);

    if (widget->width() < 600 || widget->height() < 480) {
      int w = widget->width() - 9 * 2;
      if (w > 300) w = 300;
      news->setSize(w, 40);
      newsWait->setVisible(widget->width() > 450 && !haveFreshFeed);
      newsUpdate->setVisible(false);

      if (currentNewsType != 1) {
        currentNewsType = 1;
        updateNews();
      }
    } else {
      news->setSize(600 - 9 * 2, 120);
      newsWait->setVisible(!haveFreshFeed);
      newsUpdate->setSize(136, 57);
      newsUpdate->setPixelSize(18);
      newsUpdate->setPos(news->width() - 136 - news->currentBorderSize() - 2,
                         news->height() / 2 - 57 / 2);
      newsUpdate->setVisible(haveUpdate);

      if (currentNewsType != 2) {
        currentNewsType = 2;
        updateNews();
      }
    }
  }
}
*/

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
        if (entry_list_.size() > 1)
          news_text_->SetText(QStringLiteral("<a href=\"%1\">%2</a>")
                                  .arg(entry_list_.at(0).link)
                                  .arg(entry_list_.at(0).title));
        news_text_->SetVisible(true);
      }
      break;
    default:
    case 2:
      if (entry_list_.empty()) {
        news_->SetVisible(false);
      } else {
        if (entry_list_.size() == 1) {
          news_text_->SetText(tr("Latest news:") + " " +
                              QStringLiteral("<a href=\"%1\">%2</a>")
                                  .arg(entry_list_.at(0).link)
                                  .arg(entry_list_.at(0).title));
        } else {
          QStringList entryHtmlList;
          unsigned int index = 0;
          while (index < entry_list_.size() && index < 3) {
            entryHtmlList << QStringLiteral(" - <a href=\"%1\">%2</a>")
                                 .arg(entry_list_.at(index).link)
                                 .arg(entry_list_.at(index).title);
            index++;
          }
          news_text_->SetText(tr("Latest news:") + QStringLiteral("<br />") +
                              entryHtmlList.join("<br />"));
        }
        news_text_->SetVisible(true);
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

void Menu::ChangeLanguage() {
  solo_->SetText(tr("Solo"));
  multi_->SetText(tr("Multi"));
  news_update_->SetText(tr("Update!"));
  UpdateNews();
}
