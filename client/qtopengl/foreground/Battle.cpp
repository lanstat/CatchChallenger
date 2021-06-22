// Copyright 2021 <CatchChallenger>
#include "Battle.hpp"

#include <QGraphicsProxyWidget>
#include <QImageReader>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPainter>
#include <iostream>
#include <string>
#include <vector>

#include "../../../general/base/CommonDatapack.hpp"
#include "../../../general/base/CommonSettingsCommon.hpp"
#include "../CCprogressbar.hpp"
#include "../CustomButton.hpp"
#include "../ImagesStrechMiddle.hpp"
#include "../above/MessageDialog.hpp"
#include "../above/inventory/Inventory.hpp"
#include "../above/monster/MonsterBag.hpp"
#include "../action/Animation.hpp"
#include "../action/Blink.hpp"
#include "../action/MoveTo.hpp"
#include "../action/Timeout.hpp"
#include "../cc/ClientFightEngine.hpp"
#include "../cc/QtDatapackClientLoader.hpp"
#include "../core/ActionManager.hpp"
#include "../core/Sprite.hpp"
#include "../ui/Button.hpp"
#include "../ui/Label.hpp"
#include "../ui/ListView.hpp"
#include "../ui/Progressbar.hpp"

using namespace CatchChallenger;
using CatchChallenger::PublicPlayerMonster;
using std::placeholders::_1;

// Width/height base obtain from background resource
const int BASE_WIDTH = 200;
const int BASE_HEIGHT = 110;

Battle *Battle::battle = nullptr;

Battle::Battle() {
  screen_width_ = 1440;
  screen_height_ = 840;

  action_manager_ = ActionManager::GetInstance();

  labelFightBackground = new QGraphicsPixmapItem(this);
  labelFightBackgroundPix =
      QPixmap(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->getDatapackPath()) +
              "/map/fight/grass/background.png");
  labelFightForeground = new QGraphicsPixmapItem(this);
  labelFightForegroundPix =
      QPixmap(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->getDatapackPath()) +
              "/map/fight/grass/foreground.png");

  frameFightBottom =
      new ImagesStrechMiddle(28, ":/CC/images/interface/b1.png", this);
  labelFightBottomName = new QGraphicsTextItem(frameFightBottom);
  labelFightBottomLevel = new QGraphicsTextItem(frameFightBottom);
  player_hp_bar_ = UI::Progressbar::Create(frameFightBottom);
  progressBarFightBottomExp = new CCprogressbar(frameFightBottom);

  frameFightTop =
      new ImagesStrechMiddle(28, ":/CC/images/interface/b1.png", this);
  labelFightTopName = new QGraphicsTextItem(frameFightTop);
  enemy_hp_bar_ = UI::Progressbar::Create(frameFightTop);

  labelFightMonsterAttackBottom = new QGraphicsPixmapItem(this);
  labelFightMonsterAttackTop = new QGraphicsPixmapItem(this);
  labelFightPlateformBottom = new QGraphicsPixmapItem(this);
  labelFightPlateformTop = new QGraphicsPixmapItem(this);
  labelFightTrap = new QGraphicsPixmapItem(this);

  player_ = Sprite::Create(this);
  enemy_ = Sprite::Create(this);

  stackedWidgetFightBottomBar =
      new ImagesStrechMiddle(46, ":/CC/images/interface/message.png", this);
  status_label_ = UI::Label::Create(stackedWidgetFightBottomBar);
  pushButtonFightEnterNext = new CustomButton(
      ":/CC/images/interface/greenbutton.png", stackedWidgetFightBottomBar);

  pushButtonFightAttack = new CustomButton(
      ":/CC/images/interface/greenbutton.png", stackedWidgetFightBottomBar);
  pushButtonFightBag = new CustomButton(":/CC/images/interface/button.png",
                                        stackedWidgetFightBottomBar);
  pushButtonFightMonster = new CustomButton(":/CC/images/interface/button.png",
                                            stackedWidgetFightBottomBar);
  toolButtonFightQuit = new CustomButton(":/CC/images/interface/button.png",
                                         stackedWidgetFightBottomBar);

  pushButtonFightAttackConfirmed = new CustomButton(
      ":/CC/images/interface/greenbutton.png", stackedWidgetFightBottomBar);
  pushButtonFightReturn = new CustomButton(":/CC/images/interface/button.png",
                                           stackedWidgetFightBottomBar);
  labelFightAttackDetails = new QGraphicsTextItem(stackedWidgetFightBottomBar);
  player_skills_ = UI::ListView::Create(stackedWidgetFightBottomBar);
  player_skills_->SetPos(20, 20);
  player_skills_->SetSize(500, 100);

  m_stackedWidgetFightBottomBarIndex = 0;

  if (!connect(pushButtonFightEnterNext, &CustomButton::clicked, this,
               &Battle::on_pushButtonFightEnterNext_clicked))
    abort();
  if (!connect(toolButtonFightQuit, &CustomButton::clicked, this,
               &Battle::on_toolButtonFightQuit_clicked))
    abort();
  if (!connect(pushButtonFightAttack, &CustomButton::clicked, this,
               &Battle::on_pushButtonFightAttack_clicked))
    abort();
  if (!connect(pushButtonFightMonster, &CustomButton::clicked, this,
               &Battle::on_pushButtonFightMonster_clicked))
    abort();
  if (!connect(pushButtonFightReturn, &CustomButton::clicked, this,
               &Battle::on_pushButtonFightReturn_clicked))
    abort();
  if (!connect(pushButtonFightBag, &CustomButton::clicked, this,
               &Battle::OnBagButtonClick))
    abort();

  displayExpTimer.setSingleShot(true);
  displayExpTimer.setInterval(20);
  moveFightMonsterBothTimer.setSingleShot(true);
  moveFightMonsterBothTimer.setInterval(20);
  botFightTimer.setSingleShot(true);
  botFightTimer.setInterval(1000);
  displayTrapTimer.setSingleShot(true);
  displayTrapTimer.setInterval(50);

  if (!connect(&displayExpTimer, &QTimer::timeout, this,
               &Battle::displayExperienceGain))
    abort();
  if (!connect(&moveFightMonsterBothTimer, &QTimer::timeout, this,
               &Battle::moveFightMonsterBoth))
    abort();
  if (!connect(&botFightTimer, &QTimer::timeout, this,
               &Battle::botFightFullDiffered))
    abort();
  if (!connect(&displayTrapTimer, &QTimer::timeout, this, &Battle::displayTrap))
    abort();

  newLanguage();

  escape = false;
  escapeSuccess = false;

  message_dialog_ = nullptr;
  inventory_ = nullptr;
  monsters_bag_ = nullptr;
  stackedWidgetFightBottomBarsetCurrentWidget(0);
}

Battle::~Battle() {
  delete message_dialog_;
  delete inventory_;
  delete monsters_bag_;
}

void Battle::setVar(ConnexionManager *connexionManager) {
  this->connexionManager = connexionManager;
  client_ = connexionManager->client;
  fightEngine = static_cast<CommonFightEngine *>(client_);
}

QPixmap Battle::GetBackSkin(const uint32_t &skinId) {
  /*const QPixmap skin(QString::fromStdString(getBackSkinPath(skinId)));
  if(!skin.isNull())
      return skin;*/
  return QPixmap(":/CC/images/player_default/back2.png");
}

void Battle::stackedWidgetFightBottomBarsetCurrentWidget(int index) {
  m_stackedWidgetFightBottomBarIndex = index;
  switch (m_stackedWidgetFightBottomBarIndex) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    default:
      abort();
      break;
  }

  status_label_->SetVisible(m_stackedWidgetFightBottomBarIndex == 0);
  pushButtonFightEnterNext->setVisible(m_stackedWidgetFightBottomBarIndex == 0);

  pushButtonFightAttack->setVisible(m_stackedWidgetFightBottomBarIndex == 1);
  pushButtonFightBag->setVisible(m_stackedWidgetFightBottomBarIndex == 1);
  pushButtonFightMonster->setVisible(m_stackedWidgetFightBottomBarIndex == 1);
  toolButtonFightQuit->setVisible(m_stackedWidgetFightBottomBarIndex == 1 &&
                                  battleType == BattleType_Wild);

  pushButtonFightAttackConfirmed->setVisible(
      m_stackedWidgetFightBottomBarIndex == 2);
  pushButtonFightReturn->setVisible(m_stackedWidgetFightBottomBarIndex == 2);
  labelFightAttackDetails->setVisible(m_stackedWidgetFightBottomBarIndex == 2);
  player_skills_->SetVisible(m_stackedWidgetFightBottomBarIndex == 2);
}

void Battle::newLanguage() {
  // labelFightBottomLevel->setPlainText(tr("Level 100"));
  // labelFightTopLevel->setText(tr("Level 100"));
  status_label_->SetText("");
  labelFightAttackDetails->setPlainText("");

  pushButtonFightEnterNext->setText(tr("Next"));

  pushButtonFightAttack->setText(tr("Attack"));
  pushButtonFightBag->setText(tr("Bag"));
  pushButtonFightMonster->setText(tr("Monster"));
  toolButtonFightQuit->setText(tr("Escape"));

  pushButtonFightAttackConfirmed->setText(tr("Attack"));
  pushButtonFightReturn->setText(tr("Return"));
}

QRectF Battle::boundingRect() const { return QRectF(); }

void Battle::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                   QWidget *w) {
  action_manager_->Update();

  painter->setBackground(QBrush(QColor(0, 0, 0)));
  painter->setPen(Qt::NoPen);
  painter->drawRect(0, 0, 10, 10);
  unsigned int space = 10;
  //    unsigned int fontSize=20;
  // unsigned int multiItemH=100;
  /*if(w->height()>300)
      fontSize=w->height()/6;*/
  if (w->width() < 900 || w->height() < 600) {
    pushButtonFightEnterNext->setSize(148, 61);
    pushButtonFightEnterNext->setPixelSize(23);

    pushButtonFightAttack->setSize(148, 61);
    pushButtonFightAttack->setPixelSize(23);
    pushButtonFightBag->setSize(148, 61);
    pushButtonFightBag->setPixelSize(23);
    pushButtonFightMonster->setSize(148, 61);
    pushButtonFightMonster->setPixelSize(23);
    toolButtonFightQuit->setSize(148, 61);
    toolButtonFightQuit->setPixelSize(23);

    pushButtonFightAttackConfirmed->setSize(148, 61);
    pushButtonFightAttackConfirmed->setPixelSize(23);
    pushButtonFightReturn->setSize(148, 61);
    pushButtonFightReturn->setPixelSize(23);
    // multiItemH=75;
  } else {
    space = 30;
    pushButtonFightEnterNext->setSize(100, 50);
    pushButtonFightEnterNext->setPixelSize(20);

    pushButtonFightAttack->setSize(200, 70);
    pushButtonFightAttack->setPixelSize(20);
    pushButtonFightBag->setSize(200, 70);
    pushButtonFightBag->setPixelSize(20);
    pushButtonFightMonster->setSize(200, 70);
    pushButtonFightMonster->setPixelSize(20);
    toolButtonFightQuit->setSize(200, 70);
    toolButtonFightQuit->setPixelSize(20);

    pushButtonFightAttackConfirmed->setSize(223, 92);
    pushButtonFightAttackConfirmed->setPixelSize(35);
    pushButtonFightReturn->setSize(223, 92);
    pushButtonFightReturn->setPixelSize(35);
  }

  unsigned int minRatio = w->width() / BASE_WIDTH;
  unsigned int tempRatio = w->height() / BASE_HEIGHT;
  if (minRatio > tempRatio) minRatio = tempRatio;
  unsigned int finalWidth = BASE_WIDTH * minRatio;
  unsigned int finalHeight = BASE_HEIGHT * minRatio;
  unsigned int finalX = (w->width() - finalWidth) / 2;
  unsigned int finalY = (w->height() - finalHeight) / 2;
  if (labelFightBackground->pixmap().width() !=
      labelFightBackgroundPix.width() * (int)minRatio) {
    QPixmap t = labelFightBackgroundPix.scaled(
        labelFightBackgroundPix.width() * minRatio,
        labelFightBackgroundPix.height() * minRatio);
    labelFightBackground->setPixmap(t);
    t = labelFightForegroundPix.scaled(
        labelFightForegroundPix.width() * minRatio,
        labelFightForegroundPix.height() * minRatio);
    labelFightForeground->setPixmap(t);
  }
  labelFightBackground->setPos(finalX, finalY);
  labelFightForeground->setPos(finalX, finalY);

  frameFightBottom->setPos(w->width() * 0.63, w->height() * 0.6);
  frameFightBottom->setSize(400, 170);
  labelFightBottomName->setPos(20, 20);
  progressBarFightBottomExp->setPos(20, 110);
  progressBarFightBottomExp->setSize(frameFightBottom->width() - 40, 44);
  player_hp_bar_->SetPos(20, 60);
  player_hp_bar_->SetSize(frameFightBottom->width() - 40, 44);

  frameFightTop->setPos(100, 100);
  frameFightTop->setSize(400, 170);
  labelFightTopName->setPos(20, 20);
  labelFightBottomLevel->setPos(300, 20);
  enemy_hp_bar_->SetPos(20, 60);
  enemy_hp_bar_->SetSize(frameFightTop->width() - 40, 44);

  labelFightMonsterAttackBottom->setPos(finalX + 60 * minRatio,
                                        finalY + 280 * minRatio);
  // labelFightMonsterAttackBottom->setSize(160*minRatio,160*minRatio);
  labelFightMonsterAttackTop->setPos(finalX + 510 * minRatio,
                                     finalY + 90 * minRatio);
  // labelFightMonsterAttackTop->setSize(160*minRatio,160*minRatio);
  labelFightPlateformBottom->setPos(finalX + 30 * minRatio,
                                    finalY + 350 * minRatio);
  // labelFightPlateformBottom->setSize(230*minRatio,90*minRatio);
  labelFightPlateformTop->setPos(finalX + 460 * minRatio,
                                 finalY + 170 * minRatio);
  // labelFightPlateformTop->setSize(260*minRatio,90*minRatio);
  labelFightTrap->setPos(finalX + 280 * minRatio, finalY + 230 * minRatio);
  // labelFightTrap->setSize(160*minRatio,160*minRatio);

  stackedWidgetFightBottomBar->setSize(w->width(), 160);
  stackedWidgetFightBottomBar->setPos(0, w->height() - 160);

  uint frameWidth = stackedWidgetFightBottomBar->width();
  uint frameHeight = stackedWidgetFightBottomBar->height();
  switch (m_stackedWidgetFightBottomBarIndex) {
    case 0:
      status_label_->SetPos(space, frameHeight / 2 - 24 / 2);
      pushButtonFightEnterNext->setPos(
          frameWidth - space - pushButtonFightEnterNext->width(),
          frameHeight / 2 - pushButtonFightEnterNext->height() / 2);
      break;
    case 1:
      pushButtonFightAttack->setPos(
          frameWidth / 2 - space / 2 - pushButtonFightAttack->width(), 10);
      pushButtonFightBag->setPos(
          pushButtonFightAttack->x() + pushButtonFightAttack->width() + 5,
          pushButtonFightAttack->y());
      pushButtonFightMonster->setPos(
          pushButtonFightAttack->x(),
          pushButtonFightAttack->y() + pushButtonFightAttack->height() + 5);
      toolButtonFightQuit->setPos(
          pushButtonFightBag->x(),
          pushButtonFightAttack->y() + pushButtonFightAttack->height() + 5);
      break;
    case 2:
      pushButtonFightReturn->setPos(stackedWidgetFightBottomBar->width() -
                                        space - pushButtonFightReturn->width(),
                                    stackedWidgetFightBottomBar->height() -
                                        space -
                                        pushButtonFightReturn->height());
      pushButtonFightAttackConfirmed->setPos(
          pushButtonFightReturn->x() - pushButtonFightAttackConfirmed->width() -
              space,
          pushButtonFightReturn->y());
      labelFightAttackDetails->setPos(
          stackedWidgetFightBottomBar->width() / 2 + space / 2, space);
      break;
  }
}

void Battle::mousePressEventXY(const QPointF &p, bool &pressValidated,
                               bool &callParentClass) {
  (void)callParentClass;
  pushButtonFightEnterNext->mousePressEventXY(p, pressValidated);

  pushButtonFightAttack->mousePressEventXY(p, pressValidated);
  pushButtonFightBag->mousePressEventXY(p, pressValidated);
  pushButtonFightMonster->mousePressEventXY(p, pressValidated);
  toolButtonFightQuit->mousePressEventXY(p, pressValidated);

  pushButtonFightAttackConfirmed->mousePressEventXY(p, pressValidated);
  pushButtonFightReturn->mousePressEventXY(p, pressValidated);

  player_->MousePressEventXY(p, pressValidated);
  player_skills_->MousePressEventXY(p, pressValidated);
}

void Battle::mouseReleaseEventXY(const QPointF &p, bool &pressValidated,
                                 bool &callParentClass) {
  (void)callParentClass;
  pushButtonFightEnterNext->mouseReleaseEventXY(p, pressValidated);

  pushButtonFightAttack->mouseReleaseEventXY(p, pressValidated);
  pushButtonFightBag->mouseReleaseEventXY(p, pressValidated);
  pushButtonFightMonster->mouseReleaseEventXY(p, pressValidated);
  toolButtonFightQuit->mouseReleaseEventXY(p, pressValidated);

  pushButtonFightAttackConfirmed->mouseReleaseEventXY(p, pressValidated);
  pushButtonFightReturn->mouseReleaseEventXY(p, pressValidated);

  player_->MouseReleaseEventXY(p, pressValidated);
  player_skills_->MouseReleaseEventXY(p, pressValidated);
}

void Battle::mouseMoveEventXY(const QPointF &p, bool &pressValidated,
                              bool &callParentClass) {
  (void)callParentClass;
  player_skills_->MouseMoveEventXY(p);
}

void Battle::OnBagButtonClick() { ShowBagDialog(); }

// need correct match, else tp to die can failed and do mistake
bool Battle::check_monsters() {
  /*    unsigned int index=0;
      unsigned int sub_size;
      unsigned int sub_index;
      while(index<client->player_informations.playerMonster.size())
      {
          const PlayerMonster
     &monster=client->player_informations.playerMonster.at(index);
          if(CatchChallenger::CommonDatapack::commonDatapack.monsters.find(monster.monster)==CatchChallenger::CommonDatapack::commonDatapack.monsters.cend())
          {
              error(QStringLiteral("the monster %1 is not into monster
     list").arg(monster.monster).toStdString()); return false;
          }
          const Monster
     &monsterDef=CatchChallenger::CommonDatapack::commonDatapack.monsters.at(monster.monster);
          if(monster.level>CATCHCHALLENGER_MONSTER_LEVEL_MAX)
          {
              error(QStringLiteral("the level %1 is greater than max
     level").arg(monster.level).toStdString()); return false;
          }
          Monster::Stat
     stat=ClientFightEngine::getStat(monsterDef,monster.level);
          if(monster.hp>stat.hp)
          {
              error(QStringLiteral("the hp %1 is greater than max hp for the
     level %2").arg(stat.hp).arg(monster.level).toStdString()); return false;
          }
          if(monster.remaining_xp>monsterDef.level_to_xp.at(monster.level-1))
          {
              error(QStringLiteral("the hp %1 is greater than max hp for the
     level %2").arg(monster.remaining_xp).arg(monster.level).toStdString());
              return false;
          }
          sub_size=static_cast<uint32_t>(monster.buffs.size());
          sub_index=0;
          while(sub_index<sub_size)
          {
              if(CatchChallenger::CommonDatapack::commonDatapack.monsterBuffs.find(monster.buffs.at(sub_index).buff)==CatchChallenger::CommonDatapack::commonDatapack.monsterBuffs.cend())
              {
                  error(QStringLiteral("the buff %1 is not into the buff
     list").arg(monster.buffs.at(sub_index).buff).toStdString()); return false;
              }
              if(monster.buffs.at(sub_index).level>CatchChallenger::CommonDatapack::commonDatapack.monsterBuffs.at(monster.buffs.at(sub_index).buff).level.size())
              {
                  error(QStringLiteral("the buff have not the level %1 is not
     into the buff list").arg(monster.buffs.at(sub_index).level).toStdString());
                  return false;
              }
              sub_index++;
          }
          sub_size=static_cast<uint32_t>(monster.skills.size());
          sub_index=0;
          while(sub_index<sub_size)
          {
              if(CatchChallenger::CommonDatapack::commonDatapack.monsterSkills.find(monster.skills.at(sub_index).skill)==CatchChallenger::CommonDatapack::commonDatapack.monsterSkills.cend())
              {
                  error(QStringLiteral("the skill %1 is not into the skill
     list").arg(monster.skills.at(sub_index).skill).toStdString()); return
     false;
              }
              if(monster.skills.at(sub_index).level>CatchChallenger::CommonDatapack::commonDatapack.monsterSkills.at(monster.skills.at(sub_index).skill).level.size())
              {
                  error(QStringLiteral("the skill have not the level %1 is not
     into the skill
     list").arg(monster.skills.at(sub_index).level).toStdString()); return
     false;
              }
              sub_index++;
          }
          index++;
      }
      //connexionManager->client->setPlayerMonster(client->player_informations.playerMonster);
      */
  return true;
}

void Battle::load_monsters() {
  /*
  const std::vector<PlayerMonster> &playerMonsters =
      client_->getPlayerMonster();
  monsterList.clear();
  monsterspos_items_graphical.clear();
  if (playerMonsters.empty()) return;
  const uint8_t &currentMonsterPosition =
      client_->getCurrentSelectedMonsterNumber();
  unsigned int index = 0;
  while (index < playerMonsters.size()) {
    const CatchChallenger::PlayerMonster &monster = playerMonsters.at(index);
    if (inSelection) {
      if (waitedObjectType == ObjectType_MonsterToFight &&
          index == currentMonsterPosition) {
        index++;
        continue;
      }
      switch (waitedObjectType) {
        case ObjectType_ItemEvolutionOnMonster:
          if (monster.hp == 0 || monster.egg_step > 0) {
            index++;
            continue;
          }
          if (CatchChallenger::CommonDatapack::commonDatapack.items
                  .evolutionItem.find(objectInUsing.back()) ==
              CatchChallenger::CommonDatapack::commonDatapack.items
                  .evolutionItem.cend()) {
            index++;
            continue;
          }
          if (CatchChallenger::CommonDatapack::commonDatapack.items
                  .evolutionItem.at(objectInUsing.back())
                  .find(monster.monster) ==
              CatchChallenger::CommonDatapack::commonDatapack.items
                  .evolutionItem.at(objectInUsing.back())
                  .cend()) {
            index++;
            continue;
          }
          break;
        case ObjectType_ItemLearnOnMonster:
          if (CatchChallenger::CommonDatapack::commonDatapack.items.itemToLearn
                  .find(objectInUsing.back()) ==
              CatchChallenger::CommonDatapack::commonDatapack.items.itemToLearn
                  .cend()) {
            index++;
            continue;
          }
          if (CatchChallenger::CommonDatapack::commonDatapack.items.itemToLearn
                  .at(objectInUsing.back())
                  .find(monster.monster) ==
              CatchChallenger::CommonDatapack::commonDatapack.items.itemToLearn
                  .at(objectInUsing.back())
                  .cend()) {
            index++;
            continue;
          }
          break;
        case ObjectType_MonsterToFight:
        case ObjectType_MonsterToFightKO:
        case ObjectType_ItemOnMonster:
          if (monster.hp == 0 || monster.egg_step > 0) {
            index++;
            continue;
          }
          break;
        default:
          break;
      }
    }
    if (CatchChallenger::CommonDatapack::commonDatapack.monsters.find(
            monster.monster) !=
        CatchChallenger::CommonDatapack::commonDatapack.monsters.cend()) {
      CatchChallenger::Monster::Stat stat =
          CatchChallenger::ClientFightEngine::getStat(
              CatchChallenger::CommonDatapack::commonDatapack.monsters.at(
                  monster.monster),
              monster.level);

      QListWidgetItem *item = new QListWidgetItem();
      item->setToolTip(QString::fromStdString(
          QtDatapackClientLoader::datapackLoader->monsterExtra
              .at(monster.monster)
              .description));
      if (!QtDatapackClientLoader::datapackLoader->QtmonsterExtra
               .at(monster.monster)
               .thumb.isNull())
        item->setIcon(QtDatapackClientLoader::datapackLoader->QtmonsterExtra
                          .at(monster.monster)
                          .thumb);
      else
        item->setIcon(QtDatapackClientLoader::datapackLoader->QtmonsterExtra
                          .at(monster.monster)
                          .front);

      if (waitedObjectType == ObjectType_MonsterToLearn && inSelection) {
        QHash<uint32_t, uint8_t> skillToDisplay;
        unsigned int sub_index = 0;
        while (sub_index <
               CatchChallenger::CommonDatapack::commonDatapack.monsters
                   .at(monster.monster)
                   .learn.size()) {
          Monster::AttackToLearn learn =
              CatchChallenger::CommonDatapack::commonDatapack.monsters
                  .at(monster.monster)
                  .learn.at(sub_index);
          if (learn.learnAtLevel <= monster.level) {
            unsigned int sub_index2 = 0;
            while (sub_index2 < monster.skills.size()) {
              const PlayerMonster::PlayerSkill &skill =
                  monster.skills.at(sub_index2);
              if (skill.skill == learn.learnSkill) break;
              sub_index2++;
            }
            if (
                // if skill not found
                (sub_index2 == monster.skills.size() &&
                 learn.learnSkillLevel == 1) ||
                // if skill already found and need level up
                (sub_index2 < monster.skills.size() &&
                 (monster.skills.at(sub_index2).level + 1) ==
                     learn.learnSkillLevel)) {
              if (skillToDisplay.contains(learn.learnSkill)) {
                if (skillToDisplay.value(learn.learnSkill) >
                    learn.learnSkillLevel)
                  skillToDisplay[learn.learnSkill] = learn.learnSkillLevel;
              } else
                skillToDisplay[learn.learnSkill] = learn.learnSkillLevel;
            }
          }
          sub_index++;
        }
        if (skillToDisplay.isEmpty())
          item->setPlainText(
              tr("%1, level: %2\nHP: %3/%4\n%5")
                  .arg(QString::fromStdString(
                      QtDatapackClientLoader::datapackLoader->monsterExtra
                          .at(monster.monster)
                          .name))
                  .arg(monster.level)
                  .arg(monster.hp)
                  .arg(stat.hp)
                  .arg(tr("No skill to learn")));
        else
          item->setPlainText(
              tr("%1, level: %2\nHP: %3/%4\n%5")
                  .arg(QString::fromStdString(
                      QtDatapackClientLoader::datapackLoader->monsterExtra
                          .at(monster.monster)
                          .name))
                  .arg(monster.level)
                  .arg(monster.hp)
                  .arg(stat.hp)
                  .arg(tr("%n skill(s) to learn", "", skillToDisplay.size())));
      } else {
        item->setPlainText(
            tr("%1, level: %2\nHP: %3/%4")
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->monsterExtra
                        .at(monster.monster)
                        .name))
                .arg(monster.level)
                .arg(monster.hp)
                .arg(stat.hp));
      }
      if (monster.hp == 0) {
        item->setBackgroundColor(QColor(255, 220, 220, 255));
      } else if (index == currentMonsterPosition) {
        if (!client_->isInFight())
          item->setBackgroundColor(QColor(200, 255, 255, 255));
      }
      monsterList.append(item);
      monsterspos_items_graphical[item] = static_cast<uint8_t>(index);
    } else {
      error(QStringLiteral("Unknown monster: %1")
                .arg(monster.monster)
                .toStdString());
      return;
    }
    index++;
  }
  if (inSelection) {
    monsterListMoveUp->setVisible(false);
    monsterListMoveDown->setVisible(false);
    toolButton_monster_list_quit->setVisible(waitedObjectType !=
                                             ObjectType_MonsterToFightKO);
  } else {
    monsterListMoveUp->setVisible(true);
    monsterListMoveDown->setVisible(true);
    toolButton_monster_list_quit->setVisible(true);
    on_monsterList_itemSelectionChanged();
  }
  selectMonster->setVisible(true);
  */
}

void Battle::botFight(Map_client *map, const uint8_t &x, const uint8_t &y,
                      const uint16_t &fightId) {
  if (CommonDatapackServerSpec::commonDatapackServerSpec.botFights.find(
          fightId) ==
      CommonDatapackServerSpec::commonDatapackServerSpec.botFights.cend()) {
    emit error("fight id not found at collision");
    return;
  }
  init_environement_display(map, x, y);
  botFightList.push_back(fightId);
  if (botFightList.size() > 1 || !battleInformationsList.empty()) return;
  botFightFull(fightId);
}

void Battle::botFightFull(const uint16_t &fightId) {
  this->fightId = fightId;
  botFightTimer.start();
}

void Battle::prepareFight() {
  escape = false;
  escapeSuccess = false;
}

void Battle::botFightFullDiffered() {
  prepareFight();
  frameFightTop->setVisible(false);
  ShowStatusMessage(
      QtDatapackClientLoader::datapackLoader->botFightsExtra.at(fightId).start,
      false, false);
  std::vector<PlayerMonster> botFightMonstersTransformed;
  const std::vector<BotFight::BotFightMonster> &monsters =
      CommonDatapackServerSpec::commonDatapackServerSpec.botFights.at(fightId)
          .monsters;
  unsigned int index = 0;
  while (index < monsters.size()) {
    const BotFight::BotFightMonster &botFightMonster = monsters.at(index);
    botFightMonstersTransformed.push_back(
        FacilityLib::botFightMonsterToPlayerMonster(
            monsters.at(index),
            ClientFightEngine::getStat(
                CommonDatapack::commonDatapack.monsters.at(botFightMonster.id),
                monsters.at(index).level)));
    index++;
  }
  client_->setBotMonster(botFightMonstersTransformed, fightId);
  player_->SetPixmap(playerBackImage.scaled(160, 160));
  init_current_monster_display(nullptr);
  pushButtonFightEnterNext->setVisible(false);
  battleType = BattleType_Bot;
  QPixmap botImage;
  if (actualBot.properties.find("skin") != actualBot.properties.cend()) {
    botImage = getFrontSkin(actualBot.properties.at("skin"));
  } else {
    botImage = QPixmap(QStringLiteral(":/CC/images/player_default/front.png"));
  }
  enemy_->SetPixmap(botImage.scaled(300, 300));
  qDebug() << QStringLiteral("The bot %1 is a front of you").arg(fightId);
  battleStep = BattleStep_Presentation;
  moveType = MoveType_Enter;
  resetPosition(true, true, true);
  battleStep = BattleStep_Presentation;
  moveFightMonsterBoth();
}

// Initialize the battle for wild encounter
bool Battle::WildFightInitialize(Map_client *map, const uint8_t &x,
                                 const uint8_t &y) {
  ConfigurePlayer();
  ConfigureEnemy();
  ConfigureCommons();

  this->map = map;
  if (!client_->isInFight()) {
    emit error("is in fight but without monster");
    return false;
  }
  frameFightBottom->setVisible(false);
  init_environement_display(map, x, y);
  init_other_monster_display();
  PublicPlayerMonster *otherMonster = client_->getOtherMonster();
  if (otherMonster == NULL) {
    emit error("NULL pointer for other monster at wildFightCollision()");
    return false;
  }
  battleStep = BattleStep_PresentationMonster;
  battleType = BattleType_Wild;
  resetPosition(true, false, true);
  qDebug() << QStringLiteral(
                  "You are in front of monster id: %1 level: %2 with hp: %3")
                  .arg(otherMonster->monster)
                  .arg(otherMonster->level)
                  .arg(otherMonster->hp);
  return true;
}

void Battle::init_other_monster_display() { updateOtherMonsterInformation(); }

void Battle::init_environement_display(Map_client *map, const uint8_t &x,
                                       const uint8_t &y) {
  const Player_private_and_public_informations &playerInformations =
      client_->get_player_informations_ro();
  Q_UNUSED(x);
  Q_UNUSED(y);
  // map not located
  if (map == NULL) {
    labelFightBackgroundPix =
        QPixmap(QStringLiteral(":/CC/images/interface/fight/background.png"));
    labelFightForegroundPix =
        QPixmap(QStringLiteral(":/CC/images/interface/fight/foreground.png"));
    labelFightPlateformTopPix = QPixmap(
        QStringLiteral(":/CC/images/interface/fight/plateform-front.png"));
    labelFightPlateformBottomPix = QPixmap(
        QStringLiteral(":/CC/images/interface/fight/plateform-background.png"));

    labelFightBackground->setPixmap(
        QPixmap(QStringLiteral(":/CC/images/interface/fight/background.png"))
            .scaled(800, 440));
    labelFightForeground->setPixmap(
        QPixmap(QStringLiteral(":/CC/images/interface/fight/foreground.png"))
            .scaled(800, 440));
    labelFightPlateformTop->setPixmap(
        QPixmap(
            QStringLiteral(":/CC/images/interface/fight/plateform-front.png"))
            .scaled(260, 90));
    labelFightPlateformBottom->setPixmap(
        QPixmap(QStringLiteral(
                    ":/CC/images/interface/fight/plateform-background.png"))
            .scaled(230, 90));
    return;
  }
  const MonstersCollisionValue &monstersCollisionValue =
      MoveOnTheMap::getZoneCollision(*map, x, y);
  unsigned int index = 0;
  while (index < monstersCollisionValue.walkOn.size()) {
    const MonstersCollision &monstersCollision =
        CommonDatapack::commonDatapack.monstersCollision.at(
            monstersCollisionValue.walkOn.at(index));
    const MonstersCollisionTemp &monstersCollisionTemp =
        CommonDatapack::commonDatapack.monstersCollisionTemp.at(
            monstersCollisionValue.walkOn.at(index));
    if (monstersCollision.item == 0 ||
        playerInformations.items.find(monstersCollision.item) !=
            playerInformations.items.cend()) {
      if (!monstersCollisionTemp.background.empty()) {
        const QString &baseSearch =
            QString::fromStdString(
                connexionManager->client->datapackPathBase()) +
            DATAPACK_BASE_PATH_MAPBASE +
            QString::fromStdString(monstersCollisionTemp.background);
        if (QFile(baseSearch + "/background.png").exists()) {
          labelFightBackground->setPixmap(
              QPixmap(baseSearch + QStringLiteral("/background.png"))
                  .scaled(800, 440));
        } else if (QFile(baseSearch + "/background.jpg").exists()) {
          const QList<QByteArray> &supportedImageFormats =
              QImageReader::supportedImageFormats();
          if (supportedImageFormats.contains("jpeg") ||
              supportedImageFormats.contains("jpg"))
            labelFightBackground->setPixmap(
                QPixmap(baseSearch + QStringLiteral("/background.jpg"))
                    .scaled(800, 440));
        } else
          labelFightBackground->setPixmap(
              QPixmap(
                  QStringLiteral(":/CC/images/interface/fight/background.png"))
                  .scaled(800, 440));

        if (QFile(baseSearch + "/foreground.png").exists())
          labelFightForeground->setPixmap(
              QPixmap(baseSearch + QStringLiteral("/foreground.png"))
                  .scaled(800, 440));
        else if (QFile(baseSearch + "/foreground.gif").exists())
          labelFightForeground->setPixmap(
              QPixmap(baseSearch + QStringLiteral("/foreground.gif"))
                  .scaled(800, 440));
        else
          labelFightForeground->setPixmap(
              QPixmap(
                  QStringLiteral(":/CC/images/interface/fight/foreground.png"))
                  .scaled(800, 440));

        if (QFile(baseSearch + "/plateform-front.png").exists())
          labelFightPlateformTop->setPixmap(
              QPixmap(baseSearch + QStringLiteral("/plateform-front.png"))
                  .scaled(260, 90));
        else if (QFile(baseSearch + "/plateform-front.gif").exists())
          labelFightPlateformTop->setPixmap(
              QPixmap(baseSearch + QStringLiteral("/plateform-front.gif"))
                  .scaled(260, 90));
        else
          labelFightPlateformTop->setPixmap(
              QPixmap(QStringLiteral(
                          ":/CC/images/interface/fight/plateform-front.png"))
                  .scaled(260, 90));

        if (QFile(baseSearch + "/plateform-background.png").exists())
          labelFightPlateformBottom->setPixmap(
              QPixmap(baseSearch + QStringLiteral("/plateform-background.png"))
                  .scaled(230, 90));
        else if (QFile(baseSearch + "/plateform-background.gif").exists())
          labelFightPlateformBottom->setPixmap(
              QPixmap(baseSearch + QStringLiteral("/plateform-background.gif"))
                  .scaled(230, 90));
        else
          labelFightPlateformBottom->setPixmap(
              QPixmap(
                  QStringLiteral(
                      ":/CC/images/interface/fight/plateform-background.png"))
                  .scaled(230, 90));
      } else {
        labelFightBackgroundPix = QPixmap(
            QStringLiteral(":/CC/images/interface/fight/background.png"));
        labelFightForegroundPix = QPixmap(
            QStringLiteral(":/CC/images/interface/fight/foreground.png"));
        labelFightPlateformTopPix = QPixmap(
            QStringLiteral(":/CC/images/interface/fight/plateform-front.png"));
        labelFightPlateformBottomPix = QPixmap(QStringLiteral(
            ":/CC/images/interface/fight/plateform-background.png"));

        labelFightBackground->setPixmap(
            QPixmap(
                QStringLiteral(":/CC/images/interface/fight/background.png"))
                .scaled(800, 440));
        labelFightForeground->setPixmap(
            QPixmap(
                QStringLiteral(":/CC/images/interface/fight/foreground.png"))
                .scaled(800, 440));
        labelFightPlateformTop->setPixmap(
            QPixmap(QStringLiteral(
                        ":/CC/images/interface/fight/plateform-front.png"))
                .scaled(260, 90));
        labelFightPlateformBottom->setPixmap(
            QPixmap(QStringLiteral(
                        ":/CC/images/interface/fight/plateform-background.png"))
                .scaled(230, 90));
      }
      return;
    }
    index++;
  }
  labelFightBackgroundPix =
      QPixmap(QStringLiteral(":/CC/images/interface/fight/background.png"));
  labelFightForegroundPix =
      QPixmap(QStringLiteral(":/CC/images/interface/fight/foreground.png"));
  labelFightPlateformTopPix = QPixmap(
      QStringLiteral(":/CC/images/interface/fight/plateform-front.png"));
  labelFightPlateformBottomPix = QPixmap(
      QStringLiteral(":/CC/images/interface/fight/plateform-background.png"));

  labelFightBackground->setPixmap(
      QPixmap(QStringLiteral(":/CC/images/interface/fight/background.png"))
          .scaled(800, 440));
  labelFightForeground->setPixmap(
      QPixmap(QStringLiteral(":/CC/images/interface/fight/foreground.png"))
          .scaled(800, 440));
  labelFightPlateformTop->setPixmap(
      QPixmap(QStringLiteral(":/CC/images/interface/fight/plateform-front.png"))
          .scaled(260, 90));
  labelFightPlateformBottom->setPixmap(
      QPixmap(QStringLiteral(
                  ":/CC/images/interface/fight/plateform-background.png"))
          .scaled(230, 90));
}

void Battle::init_current_monster_display(PlayerMonster *fightMonster) {
  if (fightMonster == nullptr) fightMonster = client_->getCurrentMonster();
  if (fightMonster != nullptr) {
    // current monster
    ShowStatusMessage(QString::fromStdString(""), true, false);
    frameFightBottom->setVisible(false);
    labelFightBottomName->setPlainText(QString::fromStdString(
        QtDatapackClientLoader::datapackLoader->monsterExtra
            .at(fightMonster->monster)
            .name));
    Monster::Stat fightStat = ClientFightEngine::getStat(
        CommonDatapack::commonDatapack.monsters.at(fightMonster->monster),
        fightMonster->level);
    player_hp_bar_->SetMaximum(fightStat.hp);
    player_hp_bar_->SetValue(fightMonster->hp);
    const Monster &monsterGenericInfo =
        CommonDatapack::commonDatapack.monsters.at(fightMonster->monster);
    int level_to_xp =
        monsterGenericInfo.level_to_xp.at(fightMonster->level - 1);
    progressBarFightBottomExp->setMaximum(level_to_xp);
    progressBarFightBottomExp->setValue(fightMonster->remaining_xp);
    // do the buff, todo the remake
    {
      player_buff_->Clear();
      unsigned int index = 0;
      while (index < fightMonster->buffs.size()) {
        const PlayerBuff &buffEffect = fightMonster->buffs.at(index);
        auto item = Sprite::Create();
        if (QtDatapackClientLoader::datapackLoader->monsterBuffsExtra.find(
                buffEffect.buff) ==
            QtDatapackClientLoader::datapackLoader->monsterBuffsExtra.cend()) {
          item->SetToolTip(tr("Unknown buff"));
          item->SetPixmap(QPixmap(":/CC/images/interface/buff.png"));
        } else {
          item->SetPixmap(
              QtDatapackClientLoader::datapackLoader->QtmonsterBuffsExtra
                  .at(buffEffect.buff)
                  .icon);
          if (buffEffect.level <= 1) {
            item->SetToolTip(QString::fromStdString(
                QtDatapackClientLoader::datapackLoader->monsterBuffsExtra
                    .at(buffEffect.buff)
                    .name));
          } else {
            item->SetToolTip(tr("%1 at level %2")
                                 .arg(QString::fromStdString(
                                     QtDatapackClientLoader::datapackLoader
                                         ->monsterBuffsExtra.at(buffEffect.buff)
                                         .name))
                                 .arg(buffEffect.level));
          }
          item->SetToolTip(
              item->ToolTip() + "\n" +
              QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterBuffsExtra
                      .at(buffEffect.buff)
                      .description));
        }
        item->SetData(99,
                      buffEffect.buff);  // to prevent duplicate buff, because
                                         // add can be to re-enable an already
                                         // enable buff (for larger period then)
        player_buff_->AddItem(item);
        index++;
      }
    }
  } else {
    emit error("Try fight without monster");
  }
  battleStep = BattleStep_PresentationMonster;
}

void Battle::on_pushButtonFightEnterNext_clicked() {
  pushButtonFightEnterNext->setVisible(false);
  switch (battleStep) {
    case BattleStep_Presentation:
      if (client_->isInFightWithWild()) {
        player_->RunAction(player_exit_);
      } else {
        moveType = MoveType_Leave;
        moveFightMonsterBoth();
      }
      break;
    case BattleStep_PresentationMonster:
    default:
      init_current_monster_display(nullptr);
      frameFightTop->show();
      frameFightBottom->show();
      player_->RunAction(player_enter_);
      PlayerMonster *monster = client_->getCurrentMonster();
      if (monster != NULL) {
        ShowStatusMessage(
            tr("Protect me %1!")
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->monsterExtra
                        .at(monster->monster)
                        .name)),
            true, false);
      } else {
        qDebug() << "on_pushButtonFightEnterNext_clicked(): NULL pointer for "
                    "current monster";
        ShowStatusMessage(tr("Protect me %1!").arg("(Unknow monster)"), true,
                          false);
      }
      break;
  }
}

/*void Battle::teleportTo(const uint32_t &mapId,const uint16_t &x,const uint16_t
&y,const Direction &direction)
{
    Q_UNUSED(mapId);
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(direction);
    if(connexionManager->client->currentMonsterIsKO() &&
!connexionManager->client->haveAnotherMonsterOnThePlayerToFight())//then is
dead, is teleported to the last rescue point
    {
        qDebug() << "tp on loose: " << fightTimerFinish;
        if(fightTimerFinish)
            loose();
        else
            fightTimerFinish=true;
    }
    else
        qDebug() << "normal tp";
}*/

void Battle::updateCurrentMonsterInformationXp() {
  PlayerMonster *monster = client_->getCurrentMonster();
  if (monster == NULL) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "NULL pointer at updateCurrentMonsterInformation()");
    return;
  }
  progressBarFightBottomExp->setValue(monster->remaining_xp);
  labelFightBottomLevel->setPlainText(tr("Level %1").arg(monster->level));
  const Monster &monsterInformations =
      CommonDatapack::commonDatapack.monsters.at(monster->monster);
  uint32_t maxXp = monsterInformations.level_to_xp.at(monster->level - 1);
  progressBarFightBottomExp->setMaximum(maxXp);
}

void Battle::updateCurrentMonsterInformation() {
  if (!client_->getAbleToFight()) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "Try update the monster when have not any ready monster");
    return;
  }
  PlayerMonster *monster = client_->getCurrentMonster();
  if (monster == NULL) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "NULL pointer at updateCurrentMonsterInformation()");
    return;
  }
#ifdef CATCHCHALLENGER_DEBUG_FIGHT
  qDebug() << "Now visible monster have hp : " << monster->hp;
#endif
  player_->SetPixmap(
      QtDatapackClientLoader::datapackLoader->getMonsterExtra(monster->monster)
          .back.scaled(400, 400));
  frameFightBottom->setVisible(true);
  frameFightBottom->show();
  currentMonsterLevel = monster->level;
  UpdateAttackSkills();
}

void Battle::moveFightMonsterBoth() {
  /*if (moveType == MoveType_Enter) {
    QPointF p = labelFightMonsterTop->pos();
    p.setX(p.rx() - 4);
    labelFightMonsterTop->setPos(p);
    p = labelFightMonsterBottom->pos();
    p.setX(p.rx() + 3);
    labelFightMonsterBottom->setPos(p);
    if (labelFightMonsterTop->pos().rx() > 910 ||
        labelFightMonsterBottom->pos().rx() < 60) {
      moveFightMonsterBothTimer.start();
    } else {
      if (battleStep == BattleStep_Presentation) {
        resetPosition(false, true, true);
        pushButtonFightEnterNext->show();
      } else if (battleStep == BattleStep_PresentationMonster) {
        doNextActionStep = DoNextActionStep_Start;
        doNextAction();
      } else {
        pushButtonFightEnterNext->show();
      }
    }
  }
  if (moveType == MoveType_Leave) {
    QPointF p = labelFightMonsterTop->pos();
    p.setX(p.rx() + 4);
    labelFightMonsterTop->setPos(p);
    p = labelFightMonsterBottom->pos();
    p.setX(p.rx() - 3);
    labelFightMonsterBottom->setPos(p);
    if (labelFightMonsterTop->pos().rx() < 800 ||
        labelFightMonsterBottom->pos().rx() <
            (-labelFightMonsterBottom->pixmap().width())) {
      moveFightMonsterBothTimer.start();
    } else {
      if (battleStep == BattleStep_Presentation) {
        init_current_monster_display(nullptr);
        pushButtonFightEnterNext->setVisible(false);
        init_other_monster_display();
        frameFightTop->setVisible(false);
        updateCurrentMonsterInformation();
        updateOtherMonsterInformation();
        resetPosition(true, true, true);
        QString text;
        PublicPlayerMonster *otherMonster = client_->getOtherMonster();
        if (otherMonster != NULL) {
          text += tr("The other player call %1!")
                      .arg(QString::fromStdString(
                          QtDatapackClientLoader::datapackLoader->monsterExtra
                              .at(otherMonster->monster)
                              .name));
        } else {
          qDebug() << "on_pushButtonFightEnterNext_clicked(): NULL pointer for "
                      "current monster";
          text += tr("The other player call %1!").arg("(Unknow monster)");
        }
        text += "<br />";
        PublicPlayerMonster *monster = client_->getCurrentMonster();
        if (monster != NULL) {
          text += tr("You call %1!")
                      .arg(QString::fromStdString(
                          QtDatapackClientLoader::datapackLoader->monsterExtra
                              .at(monster->monster)
                              .name));
        } else {
          qDebug() << "on_pushButtonFightEnterNext_clicked(): NULL pointer for "
                      "current monster";
          text += tr("You call %1!").arg("(Unknow monster)");
        }
        stackedWidgetFightBottomBarsetCurrentWidget(0);
        labelFightEnter->setHtml(text);
        battleStep = BattleStep_PresentationMonster;
        moveType = MoveType_Enter;
        moveFightMonsterBoth();
      } else if (battleStep == BattleStep_PresentationMonster) {
        doNextActionStep = DoNextActionStep_Start;
        doNextAction();
      } else
        pushButtonFightEnterNext->show();
    }
  }
  if (moveType == MoveType_Dead) {
    QPointF p = labelFightMonsterBottom->pos();
    p.setY(p.ry() + 4);
    labelFightMonsterBottom->setPos(p);
    p = labelFightMonsterTop->pos();
    p.setX(p.rx() + 4);
    labelFightMonsterTop->setPos(p);
    if (labelFightMonsterTop->pos().rx() < 800 ||
        labelFightMonsterBottom->pos().ry() < 440) {
      moveFightMonsterBothTimer.start();
    } else {
      if (battleStep == BattleStep_Presentation) {
        labelFightMonsterBottom->setPixmap(playerBackImage.scaled(160, 160));
        init_current_monster_display(nullptr);
        init_other_monster_display();
        updateCurrentMonsterInformation();
        updateOtherMonsterInformation();
        battleStep = BattleStep_PresentationMonster;
        moveType = MoveType_Enter;
        moveFightMonsterBoth();
        frameFightTop->show();
        frameFightBottom->show();
      } else {
        pushButtonFightEnterNext->show();
      }
    }
  }*/
}

void Battle::updateOtherMonsterInformation() {
  if (!client_->isInFight()) {
    newError(tr("Internal error").toStdString() + ", file:" +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "updateOtherMonsterInformation() but have not other monter");
    return;
  }
  enemy_->UseState(kReadyForBattle);
  PublicPlayerMonster *otherMonster = client_->getOtherMonster();
  if (otherMonster != NULL) {
    enemy_->SetPixmap(QtDatapackClientLoader::datapackLoader
                          ->getMonsterExtra(otherMonster->monster)
                          .front.scaled(400, 400));
    // other monster
    frameFightTop->setVisible(true);
    frameFightTop->show();
    labelFightTopName->setPlainText(QString::fromStdString(
        QtDatapackClientLoader::datapackLoader->monsterExtra
            .at(otherMonster->monster)
            .name));
    // labelFightTopLevel->setText(tr("Level %1").arg(otherMonster->level));
    Monster::Stat otherStat = ClientFightEngine::getStat(
        CommonDatapack::commonDatapack.monsters.at(otherMonster->monster),
        otherMonster->level);
    enemy_hp_bar_->SetMaximum(otherStat.hp);
    enemy_hp_bar_->SetValue(otherMonster->hp);
    // do the buff
    {
      enemy_buff_->Clear();
      unsigned int index = 0;
      while (index < otherMonster->buffs.size()) {
        const PlayerBuff &buffEffect = otherMonster->buffs.at(index);
        auto item = Sprite::Create();
        if (QtDatapackClientLoader::datapackLoader->monsterBuffsExtra.find(
                buffEffect.buff) ==
            QtDatapackClientLoader::datapackLoader->monsterBuffsExtra.cend()) {
          item->SetToolTip(tr("Unknown buff"));
          item->SetPixmap(QPixmap(":/CC/images/interface/buff.png"));
        } else {
          item->SetPixmap(
              QtDatapackClientLoader::datapackLoader->QtmonsterBuffsExtra
                  .at(buffEffect.buff)
                  .icon);
          if (buffEffect.level <= 1)
            item->SetToolTip(QString::fromStdString(
                QtDatapackClientLoader::datapackLoader->monsterBuffsExtra
                    .at(buffEffect.buff)
                    .name));
          else
            item->SetToolTip(tr("%1 at level %2")
                                 .arg(QString::fromStdString(
                                     QtDatapackClientLoader::datapackLoader
                                         ->monsterBuffsExtra.at(buffEffect.buff)
                                         .name))
                                 .arg(buffEffect.level));
          item->SetToolTip(
              item->ToolTip() + "\n" +
              QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterBuffsExtra
                      .at(buffEffect.buff)
                      .description));
        }
        item->SetData(99,
                      buffEffect.buff);  // to prevent duplicate buff, because
                                         // add can be to re-enable an already
                                         // enable buff (for larger period then)
        enemy_buff_->AddItem(item);
        index++;
      }
    }
  } else {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "Unable to load the other monster");
    return;
  }
}

void Battle::on_toolButtonFightQuit_clicked() {
  if (!client_->canDoFightAction()) {
    ShowMessageDialog(
        "Communication problem",
        tr("Sorry but the client wait more data from the server to do it"));
    return;
  }
  doNextActionStep = DoNextActionStep_Start;
  escape = true;
  qDebug()
      << "Battle::on_toolButtonFightQuit_clicked(): fight engine tryEscape()";
  if (fightEngine->tryEscape())
    escapeSuccess = true;
  else
    escapeSuccess = false;
  doNextAction();
}

void Battle::on_pushButtonFightAttack_clicked() {
  stackedWidgetFightBottomBarsetCurrentWidget(2);
}

void Battle::on_pushButtonFightMonster_clicked() { ShowMonsterDialog(); }

void Battle::on_pushButtonFightReturn_clicked() {
  stackedWidgetFightBottomBarsetCurrentWidget(1);
}

void Battle::checkEvolution() {
  PlayerMonster *monster = client_->evolutionByLevelUp();
  if (monster != NULL) {
    const Monster &monsterInformations =
        CommonDatapack::commonDatapack.monsters.at(monster->monster);
    const QtDatapackClientLoader::MonsterExtra &monsterInformationsExtra =
        QtDatapackClientLoader::datapackLoader->monsterExtra.at(
            monster->monster);
    unsigned int index = 0;
    while (index < monsterInformations.evolutions.size()) {
      const Monster::Evolution &evolution =
          monsterInformations.evolutions.at(index);
      if (evolution.type == Monster::EvolutionType_Level &&
          evolution.data.level == monster->level) {
        monsterEvolutionPostion = client_->getPlayerMonsterPosition(monster);
        const Monster &monsterInformationsEvolution =
            CommonDatapack::commonDatapack.monsters.at(evolution.evolveTo);
        const QtDatapackClientLoader::MonsterExtra
            &monsterInformationsEvolutionExtra =
                QtDatapackClientLoader::datapackLoader->monsterExtra.at(
                    evolution.evolveTo);
        /*
        // create animation widget
        if (animationWidget != NULL) delete animationWidget;
        if (qQuickViewContainer != NULL) delete qQuickViewContainer;
        animationWidget = new QQuickView();
        qQuickViewContainer = QWidget::createWindowContainer(animationWidget);
        qQuickViewContainer->setMinimumSize(QSize(width(), height()));
        qQuickViewContainer->setMaximumSize(QSize(width(), height()));
        qQuickViewContainer->setFocusPolicy(Qt::TabFocus);
        verticalLayoutPageAnimation->addWidget(qQuickViewContainer);
        // show the animation
        stackedWidget->setCurrentWidget(page_animation);
        previousAnimationWidget = page_map;
        if (baseMonsterEvolution != NULL) delete baseMonsterEvolution;
        if (targetMonsterEvolution != NULL) delete targetMonsterEvolution;
        baseMonsterEvolution = new QmlMonsterGeneralInformations(
            monsterInformations, monsterInformationsExtra);
        targetMonsterEvolution = new QmlMonsterGeneralInformations(
            monsterInformationsEvolution, monsterInformationsEvolutionExtra);
        if (evolutionControl != NULL) delete evolutionControl;
        evolutionControl = new EvolutionControl(
            monsterInformations, monsterInformationsExtra,
            monsterInformationsEvolution, monsterInformationsEvolutionExtra);
        if (!connect(evolutionControl, &EvolutionControl::cancel, this,
                     &Battle::evolutionCanceled, Qt::QueuedConnection))
          abort();
        animationWidget->rootContext()->setContextProperty("animationControl",
                                                           &animationControl);
        animationWidget->rootContext()->setContextProperty("evolutionControl",
                                                           evolutionControl);
        animationWidget->rootContext()->setContextProperty("canBeCanceled",
                                                           QVariant(true));
        animationWidget->rootContext()->setContextProperty("itemEvolution",
                                                           QString());
        animationWidget->rootContext()->setContextProperty(
            "baseMonsterEvolution", baseMonsterEvolution);
        animationWidget->rootContext()->setContextProperty(
            "targetMonsterEvolution", targetMonsterEvolution);
        const QString datapackQmlFile =
            QString::fromStdString(client->datapackPathBase()) +
            "qml/evolution-animation.qml";
        if (QFile(datapackQmlFile).exists())
          animationWidget->setSource(QUrl::fromLocalFile(datapackQmlFile));
        else
          animationWidget->setSource(
              QStringLiteral("qrc:/qml/evolution-animation.qml"));*/
        return;
      }
      index++;
    }
  }
  while (!tradeEvolutionMonsters.empty()) {
    const uint8_t monsterPosition = tradeEvolutionMonsters.at(0);
    PlayerMonster *const playerMonster =
        client_->monsterByPosition(monsterPosition);
    if (playerMonster == NULL) {
      tradeEvolutionMonsters.erase(tradeEvolutionMonsters.begin());
      continue;
    }
    const Monster &monsterInformations =
        CommonDatapack::commonDatapack.monsters.at(playerMonster->monster);
    const QtDatapackClientLoader::MonsterExtra &monsterInformationsExtra =
        QtDatapackClientLoader::datapackLoader->monsterExtra.at(
            playerMonster->monster);
    unsigned int index = 0;
    while (index < monsterInformations.evolutions.size()) {
      const Monster::Evolution &evolution =
          monsterInformations.evolutions.at(index);
      if (evolution.type == Monster::EvolutionType_Trade) {
        monsterEvolutionPostion = monsterPosition;
        const Monster &monsterInformationsEvolution =
            CommonDatapack::commonDatapack.monsters.at(evolution.evolveTo);
        const QtDatapackClientLoader::MonsterExtra
            &monsterInformationsEvolutionExtra =
                QtDatapackClientLoader::datapackLoader->monsterExtra.at(
                    evolution.evolveTo);
        /*
        // create animation widget
        if (animationWidget != NULL) delete animationWidget;
        if (qQuickViewContainer != NULL) delete qQuickViewContainer;
        animationWidget = new QQuickView();
        qQuickViewContainer = QWidget::createWindowContainer(animationWidget);
        qQuickViewContainer->setMinimumSize(QSize(800, 600));
        qQuickViewContainer->setMaximumSize(QSize(800, 600));
        qQuickViewContainer->setFocusPolicy(Qt::TabFocus);
        verticalLayoutPageAnimation->addWidget(qQuickViewContainer);
        // show the animation
        stackedWidget->setCurrentWidget(page_animation);
        previousAnimationWidget = page_map;
        if (baseMonsterEvolution != NULL) delete baseMonsterEvolution;
        if (targetMonsterEvolution != NULL) delete targetMonsterEvolution;
        baseMonsterEvolution = new QmlMonsterGeneralInformations(
            monsterInformations, monsterInformationsExtra);
        targetMonsterEvolution = new QmlMonsterGeneralInformations(
            monsterInformationsEvolution, monsterInformationsEvolutionExtra);
        if (evolutionControl != NULL) delete evolutionControl;
        evolutionControl = new EvolutionControl(
            monsterInformations, monsterInformationsExtra,
            monsterInformationsEvolution, monsterInformationsEvolutionExtra);
        if (!connect(evolutionControl, &EvolutionControl::cancel, this,
                     &Battle::evolutionCanceled, Qt::QueuedConnection))
          abort();
        animationWidget->rootContext()->setContextProperty("animationControl",
                                                           &animationControl);
        animationWidget->rootContext()->setContextProperty("evolutionControl",
                                                           evolutionControl);
        animationWidget->rootContext()->setContextProperty("canBeCanceled",
                                                           QVariant(true));
        animationWidget->rootContext()->setContextProperty("itemEvolution",
                                                           QString());
        animationWidget->rootContext()->setContextProperty(
            "baseMonsterEvolution", baseMonsterEvolution);
        animationWidget->rootContext()->setContextProperty(
            "targetMonsterEvolution", targetMonsterEvolution);
        const QString datapackQmlFile =
            QString::fromStdString(client->datapackPathBase()) +
            "qml/evolution-animation.qml";
        if (QFile(datapackQmlFile).exists())
          animationWidget->setSource(QUrl::fromLocalFile(datapackQmlFile));
        else
          animationWidget->setSource(
              QStringLiteral("qrc:/qml/evolution-animation.qml"));*/
        tradeEvolutionMonsters.erase(tradeEvolutionMonsters.begin());
        return;
      }
      index++;
    }
    tradeEvolutionMonsters.erase(tradeEvolutionMonsters.begin());
  }
}

void Battle::loose() {
  qDebug() << "loose()";
#ifdef CATCHCHALLENGER_EXTRA_CHECK
  PublicPlayerMonster *currentMonster = client_->getCurrentMonster();
  if (currentMonster != NULL) {
    if ((int)currentMonster->hp != player_hp_bar_->Value()) {
      emit error(
          QStringLiteral("Current monster damage don't match with the internal "
                         "value (loose && currentMonster): %1!=%2")
              .arg(currentMonster->hp)
              .arg(player_hp_bar_->Value())
              .toStdString());
      return;
    }
  }
  PublicPlayerMonster *otherMonster = client_->getOtherMonster();
  if (otherMonster != NULL) {
    if ((int)otherMonster->hp != enemy_hp_bar_->Value()) {
      emit error(
          QStringLiteral("Current monster damage don't match with the internal "
                         "value (loose && otherMonster): %1!=%2")
              .arg(otherMonster->hp)
              .arg(enemy_hp_bar_->Value())
              .toStdString());
      return;
    }
  }
#endif
  if (CommonDatapack::commonDatapack.monsters.empty()) return;
  client_->healAllMonsters();
  client_->fightFinished();
  // mapController->unblock();
  // stackedWidget->setCurrentWidget(page_map);
  fightTimerFinish = false;
  escape = false;
  doNextActionStep = DoNextActionStep_Start;
  load_monsters();
  switch (battleType) {
    case BattleType_Bot:
      if (botFightList.empty()) {
        emit error("battle info not found at collision");
        return;
      }
      botFightList.erase(botFightList.cbegin());
      fightId = 0;
      break;
    case BattleType_OtherPlayer:
      if (battleInformationsList.empty()) {
        emit error("battle info not found at collision");
        return;
      }
      battleInformationsList.erase(battleInformationsList.cbegin());
      break;
    default:
      break;
  }
  if (!battleInformationsList.empty()) {
    const BattleInformations &battleInformations =
        battleInformationsList.front();
    battleInformationsList.erase(battleInformationsList.cbegin());
    battleAcceptedByOtherFull(battleInformations);
  } else if (!botFightList.empty()) {
    uint16_t fightId = botFightList.front();
    botFightList.erase(botFightList.cbegin());
    // botFight(fightId);
  }
  checkEvolution();
}

void Battle::win() {
#ifdef CATCHCHALLENGER_EXTRA_CHECK
  PublicPlayerMonster *currentMonster = client_->getCurrentMonster();
  if (currentMonster != NULL) {
    if ((int)currentMonster->hp != player_hp_bar_->Value()) {
      emit error(
          QStringLiteral("Current monster damage don't match with the internal "
                         "value (win && currentMonster): %1!=%2")
              .arg(currentMonster->hp)
              .arg(player_hp_bar_->Value())
              .toStdString());
      return;
    }
  }
  PublicPlayerMonster *otherMonster = client_->getOtherMonster();
  if (otherMonster != NULL)
    if ((int)otherMonster->hp != enemy_hp_bar_->Value()) {
      emit error(QStringLiteral("Current monster damage don't match with the "
                                "internal value (win && otherMonster): %1!=%2")
                     .arg(otherMonster->hp)
                     .arg(enemy_hp_bar_->Value())
                     .toStdString());
      return;
    }
#endif
  client_->fightFinished();
#ifdef CATCHCHALLENGER_EXTRA_CHECK
  if (currentMonster != NULL)
    if ((int)currentMonster->hp != player_hp_bar_->Value()) {
      emit error(
          QStringLiteral("Current monster damage don't match with the internal "
                         "value (win end && currentMonster): %1!=%2")
              .arg(currentMonster->hp)
              .arg(player_hp_bar_->Value())
              .toStdString());
      return;
    }
#endif
  // mapController->unblock();
  /*if (zonecatch)
    stackedWidget->setCurrentWidget(page_zonecatch);
  else
    stackedWidget->setCurrentWidget(page_map);*/
  fightTimerFinish = false;
  escape = false;
  doNextActionStep = DoNextActionStep_Start;
  load_monsters();
  switch (battleType) {
    case BattleType_Bot:
      /*if (!zonecatch) {
        if (CommonDatapackServerSpec::commonDatapackServerSpec
                .botFights.find(fightId) ==
            CommonDatapackServerSpec::commonDatapackServerSpec
                .botFights.cend()) {
          emit error("fight id not found at collision");
          return;
        }
        addCash(
            CommonDatapackServerSpec::commonDatapackServerSpec
                .botFights.at(fightId)
                .cash;
        client_->addBeatenBotFight(fightId);
      }*/
      if (botFightList.empty()) {
        emit error("battle info not found at collision");
        return;
      }
      botFightList.erase(botFightList.cbegin());
      fightId = 0;
      break;
    case BattleType_OtherPlayer:
      if (battleInformationsList.empty()) {
        emit error("battle info not found at collision");
        return;
      }
      battleInformationsList.erase(battleInformationsList.cbegin());
      break;
    default:
      break;
  }
  /*if (!battleInformationsList.empty()) {
    const BattleInformations &battleInformations =
        battleInformationsList.front();
    battleInformationsList.erase(battleInformationsList.cbegin());
    battleAcceptedByOtherFull(battleInformations);
  } else if (!botFightList.empty()) {
    const uint16_t fightId = botFightList.front();
    botFightList.erase(botFightList.cbegin());
    botFight(fightId);
  }*/
  checkEvolution();
  emit Finished();
}

void Battle::displayExperienceGain() {
  PlayerMonster *currentMonster = client_->getCurrentMonster();
  if (currentMonster == NULL) {
    mLastGivenXP = 0;
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "displayAttack(): crash: unable to get the current monster");
    doNextAction();
    return;
  }
  if (progressBarFightBottomExp->value() ==
      progressBarFightBottomExp->maximum()) {
    progressBarFightBottomExp->setValue(0);
    // progressBarFightBottomExp->repaint();
  }
  // animation finished
  if (mLastGivenXP <= 0) {
#ifdef CATCHCHALLENGER_EXTRA_CHECK
    if (mLastGivenXP > 4294000000) {
      newError(tr("Internal error").toStdString() + ", file: " +
                   std::string(__FILE__) + ":" + std::to_string(__LINE__),
               QStringLiteral("part0: mLastGivenXP is negative").toStdString());
      doNextAction();
      return;
    }
    if (currentMonsterLevel > currentMonster->level) {
      newError(tr("Internal error").toStdString() + ", file: " +
                   std::string(__FILE__) + ":" + std::to_string(__LINE__),
               QStringLiteral(
                   "par0: displayed level greater than the real level: %1>%2")
                   .arg(currentMonsterLevel)
                   .arg(currentMonster->level)
                   .toStdString());
      mLastGivenXP = 0;
      doNextAction();
      return;
    }
#endif
    if (currentMonsterLevel == currentMonster->level &&
        (uint32_t)progressBarFightBottomExp->value() >
            currentMonster->remaining_xp) {
      /*message(
          QStringLiteral(
              "part0: displayed xp greater than the real xp: %1>%2, auto fix")
              .arg(progressBarFightBottomExp->value())
              .arg(currentMonster->remaining_xp)
              .toStdString());*/
      progressBarFightBottomExp->setValue(currentMonster->remaining_xp);
      mLastGivenXP = 0;
      doNextAction();
      return;
    }
    doNextAction();
    return;
  }

  // if start, display text
  if (displayAttackProgression == 0) {
    ShowStatusMessage(
        tr("You %1 gain %2 of experience ")
            .arg(QString::fromStdString(
                QtDatapackClientLoader::datapackLoader->monsterExtra
                    .at(currentMonster->monster)
                    .name))
            .arg(mLastGivenXP),
        false, false);
  }
#ifdef CATCHCHALLENGER_EXTRA_CHECK
  if (mLastGivenXP > 4294000000) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             QStringLiteral("part1: mLastGivenXP is negative").toStdString());
    doNextAction();
    return;
  }
  if (currentMonsterLevel > currentMonster->level) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             QStringLiteral(
                 "part1: displayed level greater than the real level: %1>%2")
                 .arg(currentMonsterLevel)
                 .arg(currentMonster->level)
                 .toStdString());
    mLastGivenXP = 0;
    doNextAction();
    return;
  }
#endif

  uint32_t xp_to_change;
  if (currentMonsterLevel < CATCHCHALLENGER_MONSTER_LEVEL_MAX) {
    xp_to_change = progressBarFightBottomExp->maximum() / 200;  // 0.5%
    xp_to_change = 0.5;
    if (xp_to_change == 0) xp_to_change = 1;
    if (xp_to_change > mLastGivenXP) xp_to_change = mLastGivenXP;
  } else {
    xp_to_change = 0;
    mLastGivenXP = 0;
    doNextAction();
    return;
  }

  uint32_t maxXp = progressBarFightBottomExp->maximum();
  if (((uint32_t)progressBarFightBottomExp->value() + xp_to_change) >=
      (uint32_t)maxXp) {
    xp_to_change = maxXp - progressBarFightBottomExp->value();
    if (xp_to_change > mLastGivenXP) xp_to_change = mLastGivenXP;
    if (mLastGivenXP > xp_to_change)
      mLastGivenXP -= xp_to_change;
    else
      mLastGivenXP = 0;

    const Monster::Stat &oldStat = client_->getStat(
        CommonDatapack::commonDatapack.monsters.at(currentMonster->monster),
        currentMonsterLevel);
    const Monster::Stat &newStat = connexionManager->client->getStat(
        CommonDatapack::commonDatapack.monsters.at(currentMonster->monster),
        currentMonsterLevel + 1);
    if (oldStat.hp < newStat.hp) {
      qDebug()
          << QStringLiteral(
                 "Now the old hp: %1/%2 increased of %3 for the old level %4")
                 .arg(player_hp_bar_->Value())
                 .arg(player_hp_bar_->Maximum())
                 .arg(newStat.hp - oldStat.hp)
                 .arg(currentMonsterLevel);
      player_hp_bar_->SetMaximum(player_hp_bar_->Maximum() +
                                 (newStat.hp - oldStat.hp));
      player_hp_bar_->SetValue(player_hp_bar_->Value() +
                               (newStat.hp - oldStat.hp));
    } else {
      qDebug() << QStringLiteral(
                      "The hp at level change is untouched: %1/%2 for the old "
                      "level %3")
                      .arg(player_hp_bar_->Value())
                      .arg(player_hp_bar_->Maximum())
                      .arg(currentMonsterLevel);
    }
    progressBarFightBottomExp->setMaximum(
        CommonDatapack::commonDatapack.monsters.at(currentMonster->monster)
            .level_to_xp.at(currentMonsterLevel - 1));
    progressBarFightBottomExp->setValue(progressBarFightBottomExp->maximum());
    // progressBarFightBottomExp->repaint();
#ifdef CATCHCHALLENGER_EXTRA_CHECK
    if (mLastGivenXP > 4294000000) {
      newError(tr("Internal error").toStdString() + ", file: " +
                   std::string(__FILE__) + ":" + std::to_string(__LINE__),
               QStringLiteral("part2: mLastGivenXP is negative").toStdString());
      doNextAction();
      return;
    }
    if (currentMonsterLevel > currentMonster->level) {
      newError(tr("Internal error").toStdString() + ", file: " +
                   std::string(__FILE__) + ":" + std::to_string(__LINE__),
               QStringLiteral(
                   "part2: displayed level greater than the real level: %1>%2")
                   .arg(currentMonsterLevel)
                   .arg(currentMonster->level)
                   .toStdString());
      mLastGivenXP = 0;
      doNextAction();
      return;
    }
#endif
    if (currentMonsterLevel >= CATCHCHALLENGER_MONSTER_LEVEL_MAX) {
      mLastGivenXP = 0;
      doNextAction();
      return;
    }
    currentMonsterLevel++;
    labelFightBottomLevel->setPlainText(
        tr("Level %1").arg(currentMonsterLevel));
    displayExpTimer.start();
    displayAttackProgression++;
    return;
  } else {
    if (currentMonsterLevel < CATCHCHALLENGER_MONSTER_LEVEL_MAX) {
      progressBarFightBottomExp->setValue(progressBarFightBottomExp->value() +
                                          xp_to_change);
      // progressBarFightBottomExp->repaint();
    } else {
      mLastGivenXP = 0;
      doNextAction();
      return;
    }
  }
  if (mLastGivenXP > xp_to_change)
    mLastGivenXP -= xp_to_change;
  else
    mLastGivenXP = 0;
#ifdef CATCHCHALLENGER_EXTRA_CHECK
  if (mLastGivenXP > 4294000000) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             QStringLiteral("part3: mLastGivenXP is negative").toStdString());
    doNextAction();
    return;
  }
  if (currentMonsterLevel > currentMonster->level) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             QStringLiteral(
                 "part3: displayed level greater than the real level: %1>%2")
                 .arg(currentMonsterLevel)
                 .arg(currentMonster->level)
                 .toStdString());
    mLastGivenXP = 0;
    doNextAction();
    return;
  }
#endif

  displayExpTimer.start();
  displayAttackProgression++;
}

void Battle::displayTrap() {
  PublicPlayerMonster *otherMonster = client_->getOtherMonster();
  PublicPlayerMonster *currentMonster = client_->getCurrentMonster();
  if (otherMonster == NULL) {
    error(
        "displayAttack(): crash: unable to get the other monster to use trap");
    doNextAction();
    return;
  }
  if (currentMonster == NULL) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "displayAttack(): crash: unable to get the current monster");
    doNextAction();
    return;
  }
  // if start, display text
  if (displayTrapProgression == 0) {
    // TODO(lanstat): modificar cuando se tenga un mecanismo para gif
    /* if (movie != NULL) {
      movie->stop();
      delete movie;
    }
    QString skillAnimation =
        QString::fromStdString(
            QtDatapackClientLoader::datapackLoader->getDatapackPath()) +
        DATAPACK_BASE_PATH_ITEM;
    QString fileAnimation =
        skillAnimation + QStringLiteral("%1.mng").arg(trapItemId);
    if (QFile(fileAnimation).exists()) {
      movie = new QMovie(fileAnimation, QByteArray(), labelFightTrap);
      movie->setScaledSize(
          QSize(labelFightTrap->width(), labelFightTrap->height()));
      if (movie->isValid()) {
        labelFightTrap->setMovie(movie);
        movie->start();
      } else
                qDebug() << QStringLiteral("movie loaded is not valid for:
%1").arg(fileAnimation);
    } else {
      fileAnimation = skillAnimation + QStringLiteral("%1.gif").arg(trapItemId);
      if (QFile(fileAnimation).exists()) {
        movie = new QMovie(fileAnimation, QByteArray(), labelFightTrap);
        movie->setScaledSize(
            QSize(labelFightTrap->width(), labelFightTrap->height()));
        if (movie->isValid()) {
          labelFightTrap->setMovie(movie);
          movie->start();
        } else
                    qDebug() << QStringLiteral("movie loaded is not valid for:
%1").arg(fileAnimation);
      }
    } */
    ShowStatusMessage(
        QStringLiteral("Try catch the wild %1")
            .arg(QString::fromStdString(
                QtDatapackClientLoader::datapackLoader->monsterExtra
                    .at(otherMonster->monster)
                    .name)),
        false, false);
    displayTrapProgression = 1;
    labelFightTrap->show();
  }
  uint32_t animationTime;
  if (displayTrapProgression == 1)
    animationTime = 1500;
  else
    animationTime = 2000;

  if (displayTrapProgression == 1 &&
      (uint32_t)updateTrapTime.elapsed() < animationTime) {
    /*labelFightTrap->setPos(labelFightMonsterBottom->pos().x() +
                               (labelFightMonsterTop->pos().x() -
                                labelFightMonsterBottom->pos().x()) *
                                   updateTrapTime.elapsed() / animationTime,
                           labelFightMonsterBottom->pos().y() -
                               (labelFightMonsterBottom->pos().y() -
                                labelFightMonsterTop->pos().y()) *
                                   updateTrapTime.elapsed() / animationTime);*/
  } else {
    labelFightTrap->setPos(enemy_->X(), enemy_->Y());
  }
  // if (!client_->playerMonster_catchInProgress.empty()) return;
  if ((uint32_t)updateTrapTime.elapsed() > animationTime) {
    updateTrapTime.restart();
    if (displayTrapProgression == 1) {
      displayTrapProgression = 2;
      /*if (movie != NULL) {
        movie->stop();
        delete movie;
      }
      QString skillAnimation =
          QString::fromStdString(
              QtDatapackClientLoader::datapackLoader->getDatapackPath()) +
          DATAPACK_BASE_PATH_ITEM;
      QString fileAnimation;
      if (trapSuccess)
        fileAnimation =
            skillAnimation + QStringLiteral("%1_success.mng").arg(trapItemId);
      else
        fileAnimation =
            skillAnimation + QStringLiteral("%1_failed.mng").arg(trapItemId);
      if (QFile(fileAnimation).exists()) {
        movie = new QMovie(fileAnimation, QByteArray(), labelFightTrap);
        movie->setScaledSize(
            QSize(labelFightTrap->width(), labelFightTrap->height()));
        if (movie->isValid()) {
          labelFightTrap->setMovie(movie);
          movie->start();
        } else
                    qDebug() << QStringLiteral("movie loaded is not valid for:
%1").arg(fileAnimation);
      } else {
        if (trapSuccess)
          fileAnimation =
              skillAnimation + QStringLiteral("%1_success.gif").arg(trapItemId);
        else
          fileAnimation =
              skillAnimation + QStringLiteral("%1_failed.gif").arg(trapItemId);
        if (QFile(fileAnimation).exists()) {
          movie = new QMovie(fileAnimation, QByteArray(), labelFightTrap);
          movie->setScaledSize(
              QSize(labelFightTrap->width(), labelFightTrap->height()));
          if (movie->isValid()) {
            labelFightTrap->setMovie(movie);
            movie->start();
          } else
                        qDebug() << QStringLiteral("movie loaded is not valid
for: %1").arg(fileAnimation);
        }
      }*/
    } else {
      if (trapSuccess) {
        client_->catchIsDone();  // why? do at the screen change to return on
                                 // the map, not here!
        ShowStatusMessage(
            tr("You have catched the wild %1")
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->monsterExtra
                        .at(otherMonster->monster)
                        .name)));
      } else {
        ShowStatusMessage(
            tr("You have failed the catch of the wild %1")
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->monsterExtra
                        .at(otherMonster->monster)
                        .name)));
      }
      labelFightTrap->hide();
      return;
    }
  }
  displayTrapTimer.start();
}

void Battle::resetPosition(bool outOfScreen, bool topMonster,
                           bool bottomMonster) {
  if (outOfScreen) {
    if (bottomMonster) {
      player_->UseState(kOutScreen);
    }
    if (topMonster) {
      enemy_->UseState(kOutScreen);
    }
  } else {
    if (bottomMonster) {
      player_->UseState(kReadyForBattle);
    }
    if (topMonster) {
      enemy_->UseState(kReadyForBattle);
    }
  }
}

/*
void Battle::tradeAddTradeMonster(const PlayerMonster &monster)
{
    tradeOtherMonsters.push_back(monster);
    QString genderString;
    switch(monster.gender)
    {
        case 0x01:
        genderString=tr("Male");
        break;
        case 0x02:
        genderString=tr("Female");
        break;
        default:
        genderString=tr("Unknown");
        break;
    }
    QListWidgetItem *item=new QListWidgetItem();
    if(QtDatapackClientLoader::datapackLoader->monsterExtra.find(monster.monster)!=
            QtDatapackClientLoader::datapackLoader->monsterExtra.cend())
    {
        item->setIcon(QtDatapackClientLoader::datapackLoader->QtmonsterExtra.at(monster.monster).front);
        item->setText(QString::fromStdString(QtDatapackClientLoader::datapackLoader->monsterExtra.at(monster.monster).name));
        item->setToolTip(QStringLiteral("Level: %1, Gender:
%2").arg(monster.level).arg(genderString));
    }
    else
    {
        item->setIcon(QIcon(":/CC/images/monsters/default/front.png"));
        item->setText(tr("Unknown"));
        item->setToolTip(QStringLiteral("Level: %1, Gender:
%2").arg(monster.level).arg(genderString));
    }
    tradeOtherMonsters->addItem(item);
}

//learn
bool Battle::showLearnSkillByPosition(const uint8_t &monsterPosition)
{
    QFont MissingQuantity;
    MissingQuantity.setItalic(true);
    learnAttackList->clear();
    attack_to_learn_graphical.clear();
    std::vector<PlayerMonster>
playerMonster=connexionManager->client->getPlayerMonster();
    //get the right monster
    QHash<uint16_t,uint8_t> skillToDisplay;
    unsigned int index=monsterPosition;
    PlayerMonster monster=playerMonster.at(index);
    learnMonster->setPixmap(QtDatapackClientLoader::datapackLoader->QtmonsterExtra.at(monster.monster).front.scaled(160,160));
    if(CommonSettingsServer::commonSettingsServer.useSP)
    {
        learnSP->setVisible(true);
        learnSP->setText(tr("SP: %1").arg(monster.sp));
    }
    else
        learnSP->setVisible(false);
    if(Ultimate::ultimate.isUltimate())
        learnInfo->setText(tr("<b>%1</b><br />Level %2").arg(
                               QString::fromStdString(QtDatapackClientLoader::datapackLoader->monsterExtra.at(monster.monster).name))
                           .arg(monster.level));
    unsigned int sub_index=0;
    while(sub_index<CommonDatapack::commonDatapack.monsters.at(monster.monster).learn.size())
    {
        Monster::AttackToLearn
learn=CommonDatapack::commonDatapack.monsters.at(monster.monster).learn.at(sub_index);
        if(learn.learnAtLevel<=monster.level)
        {
            unsigned int sub_index2=0;
            while(sub_index2<monster.skills.size())
            {
                const PlayerMonster::PlayerSkill
&skill=monster.skills.at(sub_index2); if(skill.skill==learn.learnSkill) break;
                sub_index2++;
            }
            if(
                    //if skill not found
                    (sub_index2==monster.skills.size() &&
learn.learnSkillLevel==1)
                    ||
                    //if skill already found and need level up
                    (sub_index2<monster.skills.size() &&
(monster.skills.at(sub_index2).level+1)==learn.learnSkillLevel)
            )
            {
                if(skillToDisplay.contains(learn.learnSkill))
                {
                    if(skillToDisplay.value(learn.learnSkill)>learn.learnSkillLevel)
                        skillToDisplay[learn.learnSkill]=learn.learnSkillLevel;
                }
                else
                    skillToDisplay[learn.learnSkill]=learn.learnSkillLevel;
            }
        }
        sub_index++;
    }
    QHashIterator<uint16_t,uint8_t> i(skillToDisplay);
    while (i.hasNext()) {
        i.next();
        QListWidgetItem *item=new QListWidgetItem();
        const uint32_t &level=i.value();
        if(CommonSettingsServer::commonSettingsServer.useSP)
        {
            if(level<=1)
                item->setText(tr("%1\nSP cost: %2")
                            .arg(QString::fromStdString(QtDatapackClientLoader::datapackLoader->monsterSkillsExtra.at(i.key()).name))
                            .arg(CommonDatapack::commonDatapack.monsterSkills.at(i.key()).level.at(i.value()-1).sp_to_learn)
                        );
            else
                item->setText(tr("%1 level %2\nSP cost: %3")
                            .arg(QString::fromStdString(QtDatapackClientLoader::datapackLoader->monsterSkillsExtra.at(i.key()).name))
                            .arg(level)
                            .arg(CommonDatapack::commonDatapack.monsterSkills.at(i.key()).level.at(i.value()-1).sp_to_learn)
                        );
        }
        else
        {
            if(level<=1)
                item->setText(QString::fromStdString(QtDatapackClientLoader::datapackLoader->monsterSkillsExtra.at(i.key()).name));
            else
                item->setText(tr("%1 level %2")
                            .arg(QString::fromStdString(QtDatapackClientLoader::datapackLoader->monsterSkillsExtra.at(i.key()).name)
                            .arg(level)
                        ));
        }
        if(CommonSettingsServer::commonSettingsServer.useSP &&
CommonDatapack::commonDatapack.monsterSkills.at(i.key()).level.at(i.value()-1).sp_to_learn>monster.sp)
        {
            item->setFont(MissingQuantity);
            item->setForeground(QBrush(QColor(200,20,20)));
            item->setToolTip(tr("You need more sp"));
        }
        attack_to_learn_graphical[item]=i.key();
        learnAttackList->addItem(item);
    }
    if(attack_to_learn_graphical.empty())
        learnDescription->setText(tr("No more attack to learn"));
    else
        learnDescription->setText(tr("Select attack to learn"));
    return true;
}

void Battle::sendBattleReturn(const std::vector<Skill::AttackReturn>
&attackReturn)
{
    #ifdef CATCHCHALLENGER_DEBUG_FIGHT
    {
        const PlayerMonster *
currentMonster=connexionManager->client->getCurrentMonster();
        if(currentMonster!=NULL)
            qDebug() << "currentMonster was hp" << currentMonster->hp << "and
buff" << currentMonster->buffs.size(); const PublicPlayerMonster *
otherMonster=connexionManager->client->getOtherMonster();
        if(currentMonster!=NULL)
            qDebug() << "otherMonster was hp" << otherMonster->hp << "and buff"
<< otherMonster->buffs.size();
    }
    #endif
    connexionManager->client->addAndApplyAttackReturnList(attackReturn);
    #ifdef CATCHCHALLENGER_DEBUG_FIGHT
    {
        const PlayerMonster *
currentMonster=connexionManager->client->getCurrentMonster();
        if(currentMonster!=NULL)
            qDebug() << "currentMonster have hp" << currentMonster->hp << "and
buff" << currentMonster->buffs.size(); const PublicPlayerMonster *
otherMonster=connexionManager->client->getOtherMonster();
        if(currentMonster!=NULL)
            qDebug() << "otherMonster have hp" << otherMonster->hp << "and buff"
<< otherMonster->buffs.size();
    }
    #endif
    doNextAction();
}*/

void Battle::useTrap(const uint16_t &itemId) {
  updateTrapTime.restart();
  displayTrapProgression = 0;
  trapItemId = itemId;
  if (!client_->tryCatchClient(itemId)) {
    qDebug() << "!connexionManager->client->tryCatchClient(itemId)";
    abort();
  }
  displayTrap();
  // need wait the server reply, monsterCatch(const bool &success)
}

void Battle::monsterCatch(const bool &success) {
  if (client_->playerMonster_catchInProgress.empty()) {
    error("Internal bug: cupture monster list is emtpy");
    return;
  }
  if (!success) {
    trapSuccess = false;
#ifdef CATCHCHALLENGER_DEBUG_FIGHT
    emit message(QStringLiteral("catch is failed"));
#endif
    client_->generateOtherAttack();
  } else {
    trapSuccess = true;
#ifdef CATCHCHALLENGER_DEBUG_FIGHT
    emit message(QStringLiteral("catch is success"));
#endif
    // connexionManager->client->playerMonster_catchInProgress.first().id=newMonsterId;
    if (client_->getPlayerMonster().size() >=
        CommonSettingsCommon::commonSettingsCommon.maxPlayerMonsters) {
      Player_private_and_public_informations &playerInformations =
          client_->get_player_informations();
      if (playerInformations.warehouse_playerMonster.size() >=
          CommonSettingsCommon::commonSettingsCommon
              .maxWarehousePlayerMonsters) {
        ShowMessageDialog(tr("Error"), tr("You have already the maximum number "
                                          "of monster into you warehouse"));
        return;
      }
      playerInformations.warehouse_playerMonster.push_back(
          client_->playerMonster_catchInProgress.front());
    } else {
      client_->addPlayerMonster(client_->playerMonster_catchInProgress.front());
      load_monsters();
    }
  }
  client_->playerMonster_catchInProgress.erase(
      client_->playerMonster_catchInProgress.cbegin());
  displayTrap();
}

/*
void Battle::battleAcceptedByOther(const std::string &pseudo,const uint8_t
&skinId,const std::vector<uint8_t> &stat, const uint8_t &monsterPlace,const
PublicPlayerMonster &publicPlayerMonster)
{
    BattleInformations battleInformations;
    battleInformations.pseudo=pseudo;
    battleInformations.skinId=skinId;
    battleInformations.stat=stat;
    battleInformations.monsterPlace=monsterPlace;
    battleInformations.publicPlayerMonster=publicPlayerMonster;
    battleInformationsList.push_back(battleInformations);
    if(battleInformationsList.size()>1 || !botFightList.empty())
        return;
    battleAcceptedByOtherFull(battleInformations);
}*/

void Battle::battleAcceptedByOtherFull(
    const BattleInformations &battleInformations) {
  if (client_->isInFight()) {
    qDebug() << "already in fight";
    client_->battleRefused();
    return;
  }
  prepareFight();
  battleType = BattleType_OtherPlayer;
  // stackedWidget->setCurrentWidget(page_battle);

  // skinFolderList=FacilityLib::skinIdList(client->get_datapack_base_name()+QStringLiteral(DATAPACK_BASE_PATH_SKIN));
  QPixmap otherFrontImage = getFrontSkin(battleInformations.skinId);

  // reset the other player info
  enemy_->SetPixmap(otherFrontImage.scaled(160, 160));
  // battleOtherPseudo->setText(lastBattleQuery.first().first);
  frameFightTop->hide();
  frameFightBottom->hide();
  player_->SetPixmap(playerBackImage.scaled(160, 160));
  ShowStatusMessage(tr("%1 wish fight with you")
                        .arg(QString::fromStdString(battleInformations.pseudo)),
                    false, false);

  resetPosition(true, true, true);
  moveType = MoveType_Enter;
  battleStep = BattleStep_Presentation;
  moveFightMonsterBoth();
  client_->setBattleMonster(battleInformations.stat,
                            battleInformations.monsterPlace,
                            battleInformations.publicPlayerMonster);

  // init_environement_display(map, map->getX(), map->getY());
}

/*void Battle::battleCanceledByOther(){
    connexionManager->client->fightFinished();
    stackedWidget->setCurrentWidget(page_map);
    showTip(tr("The other player have canceled the battle").toStdString());
    load_monsters();
    if(battleInformationsList.empty())
    {
        emit error("battle info not found at collision");
        return;
    }
    battleInformationsList.erase(battleInformationsList.cbegin());
    if(!battleInformationsList.empty())
    {
        const BattleInformations
&battleInformations=battleInformationsList.front();
        battleInformationsList.erase(battleInformationsList.cbegin());
        battleAcceptedByOtherFull(battleInformations);
    }
    else if(!botFightList.empty())
    {
        const uint16_t fightId=botFightList.front();
        botFightList.erase(botFightList.cbegin());
        botFight(fightId);
    }
}*/

void Battle::doNextAction() {
  toolButtonFightQuit->setVisible(battleType == BattleType_Wild);
  if (escape) {
    doNextActionStep = DoNextActionStep_Start;
    if (!escapeSuccess) {  // the other attack
      escape = false;      // need be dropped to have text to escape
      fightTimerFinish = false;
      if (!client_->getAttackReturnList().empty()) {
        qDebug() << "doNextAction(): escape failed: you take damage";
        ShowStatusMessage(tr("You have failed to escape!"));
        return;
      } else {
        PublicPlayerMonster *otherMonster = client_->getOtherMonster();
        if (otherMonster == NULL) {
          emit error("NULL pointer for other monster at doNextAction()");
          return;
        }
        qDebug()
            << "doNextAction(): escape fail but the wild monster can't attack";
        ShowStatusMessage(
            tr("The wild %1 can't attack")
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->monsterExtra
                        .at(otherMonster->monster)
                        .name)));
        return;
      }
    } else {
      qDebug() << "doNextAction(): escape success";
      win();
    }
    return;  // useful to quit correctly
  }
  // apply the effect
  if (!client_->getAttackReturnList().empty()) {
    const Skill::AttackReturn returnAction =
        client_->getAttackReturnList().front();
    PublicPlayerMonster *otherMonster = client_->getOtherMonster();
    if (otherMonster) {
      qDebug() << "doNextAction(): apply the effect and display it";
      StartAttack();
    } else if (returnAction.publicPlayerMonster.monster !=
               0) {  // do the monster change
      if (!client_->addBattleMonster(returnAction.monsterPlace,
                                     returnAction.publicPlayerMonster))
        return;
      // sendBattleReturn(returnAction);:already added because the information
      // is into connexionManager->client->getAttackReturnList()
      init_other_monster_display();
      updateOtherMonsterInformation();
      resetPosition(true, true, false);
      battleStep = BattleStep_Middle;
      enemy_->RunAction(enemy_enter_);
    } else {
      newError("Internal bug", "No other monster to display attack");
    }
    return;
  }

  if (doNextActionStep == DoNextActionStep_Loose) {
    qDebug() << "doNextAction(): do step loose";
#ifdef DEBUG_CLIENT_BATTLE
    qDebug() << "doNextActionStep==DoNextActionStep_Loose, fightTimerFinish: "
             << fightTimerFinish;
#endif
    if (fightTimerFinish)
      loose();
    else
      fightTimerFinish = true;
    return;
  }

  if (CommonDatapack::commonDatapack.monsters.empty()) return;
  // if the current monster is KO
  if (client_->currentMonsterIsKO()) {
    if (CommonDatapack::commonDatapack.monsters.empty()) return;
    if (!client_->isInFight()) {
      if (CommonDatapack::commonDatapack.monsters.empty()) return;
      loose();
      return;
    }
    qDebug() << "doNextAction(): remplace KO current monster";
    PublicPlayerMonster *currentMonster = client_->getCurrentMonster();
    ShowStatusMessage(
        tr("Your %1 have lost!")
            .arg(QString::fromStdString(
                QtDatapackClientLoader::datapackLoader->monsterExtra
                    .at(currentMonster->monster)
                    .name)),
        false, false);
    doNextActionStep = DoNextActionStep_Start;
    player_->RunAction(player_dead_);
    return;
  }

  // if the other monster is KO
  if (client_->otherMonsterIsKO()) {
    uint32_t returnedLastGivenXP = client_->lastGivenXP();
    if (returnedLastGivenXP > 2 * 1000 * 1000) {
      newError(tr("Internal error").toStdString() + ", file: " +
                   std::string(__FILE__) + ":" + std::to_string(__LINE__),
               "returnedLastGivenXP is negative");
      doNextAction();
      return;
    }
    if (returnedLastGivenXP > 0 || mLastGivenXP > 0) {
      if (returnedLastGivenXP > 0) mLastGivenXP = returnedLastGivenXP;
      displayAttackProgression = 0;
      displayExperienceGain();
      return;
    } else {
      updateCurrentMonsterInformation();
    }
    if (!client_->isInFight()) {
      win();
      return;
    }
    updateCurrentMonsterInformationXp();
#ifdef CATCHCHALLENGER_EXTRA_CHECK
    if (client_->getAttackReturnList().empty()) {
      PublicPlayerMonster *otherMonster = client_->getOtherMonster();
      PublicPlayerMonster *currentMonster = client_->getCurrentMonster();
      if (currentMonster != NULL)
        if ((int)currentMonster->hp != player_hp_bar_->Value()) {
          emit error(QStringLiteral("Current monster hp don't match with the "
                                    "internal value (do next action): %1!=%2")
                         .arg(currentMonster->hp)
                         .arg(player_hp_bar_->Value())
                         .toStdString());
          return;
        }
      if (otherMonster != NULL)
        if ((int)otherMonster->hp != enemy_hp_bar_->Value()) {
          emit error(QStringLiteral("Other monster hp don't match with the "
                                    "internal value (do next action): %1!=%2")
                         .arg(otherMonster->hp)
                         .arg(enemy_hp_bar_->Value())
                         .toStdString());
          return;
        }
    }
#endif
    qDebug() << "doNextAction(): remplace KO other monster";
    PublicPlayerMonster *otherMonster = client_->getOtherMonster();
    if (otherMonster == NULL) {
      emit error("No other monster into doNextAction()");
      return;
    }
    ShowStatusMessage(
        tr("The other %1 have lost!")
            .arg(QString::fromStdString(
                QtDatapackClientLoader::datapackLoader->monsterExtra
                    .at(otherMonster->monster)
                    .name)),
        false, false);
    doNextActionStep = DoNextActionStep_Start;
    // current player monster is KO
    enemy_->RunAction(enemy_dead_);
    return;
  }

  // nothing to done, show the menu
  qDebug() << "doNextAction(): show the menu";
  stackedWidgetFightBottomBarsetCurrentWidget(1);
#ifdef CATCHCHALLENGER_EXTRA_CHECK
  PublicPlayerMonster *currentMonster = client_->getCurrentMonster();
  if (currentMonster != NULL) {
    if (CommonDatapack::commonDatapack.monsters.find(currentMonster->monster) ==
        CommonDatapack::commonDatapack.monsters.cend()) {
      emit error(QStringLiteral("Current monster don't exists: %1")
                     .arg(currentMonster->monster)
                     .toStdString());
      return;
    }
    if ((int)currentMonster->hp != player_hp_bar_->Value()) {
      emit error(
          QStringLiteral("Current monster damage don't match with the internal "
                         "value (doNextAction && currentMonster): %1!=%2")
              .arg(currentMonster->hp)
              .arg(player_hp_bar_->Value())
              .toStdString());
      return;
    }
  }
  PublicPlayerMonster *otherMonster = client_->getOtherMonster();
  if (otherMonster != NULL) {
    if (CommonDatapack::commonDatapack.monsters.find(otherMonster->monster) ==
        CommonDatapack::commonDatapack.monsters.cend()) {
      emit error(QStringLiteral("Current monster don't exists: %1")
                     .arg(otherMonster->monster)
                     .toStdString());
      return;
    }
    if ((int)otherMonster->hp != enemy_hp_bar_->Value()) {
      emit error(
          QStringLiteral("Other monster damage don't match with the internal "
                         "value (doNextAction && otherMonster): %1!=%2")
              .arg(otherMonster->hp)
              .arg(enemy_hp_bar_->Value())
              .toStdString());
      return;
    }
  }
#endif
  return;
}

bool Battle::displayFirstAttackText(bool firstText) {
  PublicPlayerMonster *otherMonster = client_->getOtherMonster();
  PublicPlayerMonster *currentMonster = client_->getCurrentMonster();
  if (otherMonster == NULL) {
    error(
        "displayAttack(): crash: unable to get the other monster to display an "
        "attack");
    doNextAction();
    return false;
  }
  if (currentMonster == NULL) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "displayAttack(): crash: unable to get the current monster");
    doNextAction();
    return false;
  }
  if (client_->getAttackReturnList().empty()) {
    emit error("Display text for not existant attack");
    return false;
  }
  const Skill::AttackReturn currentAttack = client_->getFirstAttackReturn();
  qDebug()
      << QStringLiteral(
             "displayFirstAttackText(): before display text, "
             "lifeEffectMonster.size(): %1, buffLifeEffectMonster.size(): %2, "
             "addBuffEffectMonster.size(): %3, removeBuffEffectMonster.size(): "
             "%4, attackReturnList.size(): %5")
             .arg(currentAttack.lifeEffectMonster.size())
             .arg(currentAttack.buffLifeEffectMonster.size())
             .arg(currentAttack.addBuffEffectMonster.size())
             .arg(currentAttack.removeBuffEffectMonster.size())
             .arg(client_->getAttackReturnList().size());

  // in case of failed
  if (!currentAttack.success) {
    if (currentAttack.doByTheCurrentMonster)
      ShowStatusMessage(
          tr("Your %1 have failed the attack %2")
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterExtra
                      .at(currentMonster->monster)
                      .name))
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterSkillsExtra
                      .at(currentAttack.attack)
                      .name)));
    else
      ShowStatusMessage(
          tr("The other %1 have failed the attack %2")
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterExtra
                      .at(otherMonster->monster)
                      .name))
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterSkillsExtra
                      .at(currentAttack.attack)
                      .name)));
    client_->removeTheFirstAttackReturn();
    return false;
  }

  QString attackOwner;
  if (firstText) {
    if (currentAttack.doByTheCurrentMonster)
      attackOwner =
          tr("Your %1 do the attack %2 and ")
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterExtra
                      .at(currentMonster->monster)
                      .name))
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterSkillsExtra
                      .at(currentAttack.attack)
                      .name));
    else
      attackOwner =
          tr("The other %1 do the attack %2 and ")
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterExtra
                      .at(otherMonster->monster)
                      .name))
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterSkillsExtra
                      .at(currentAttack.attack)
                      .name));
  } else {
    if (currentAttack.doByTheCurrentMonster) {
      attackOwner = tr("Your %1").arg(QString::fromStdString(
          QtDatapackClientLoader::datapackLoader->monsterExtra
              .at(currentMonster->monster)
              .name));
    } else {
      attackOwner = tr("The other %1")
                        .arg(QString::fromStdString(
                            QtDatapackClientLoader::datapackLoader->monsterExtra
                                .at(otherMonster->monster)
                                .name));
    }
  }

  // the life effect
  if (!currentAttack.lifeEffectMonster.empty()) {
    const Skill::LifeEffectReturn &lifeEffectReturn =
        currentAttack.lifeEffectMonster.front();
    if (currentAttack.doByTheCurrentMonster) {
      if ((lifeEffectReturn.on & ApplyOn::ApplyOn_AllEnemy) ||
          (lifeEffectReturn.on & ApplyOn::ApplyOn_AloneEnemy)) {
        if (lifeEffectReturn.quantity > 0)
          attackOwner +=
              tr("heal of %2 the other %1")
                  .arg(QString::fromStdString(
                      QtDatapackClientLoader::datapackLoader->monsterExtra
                          .at(otherMonster->monster)
                          .name))
                  .arg(lifeEffectReturn.quantity);
        else if (lifeEffectReturn.quantity < 0)
          attackOwner +=
              tr("hurt of %2 the other %1")
                  .arg(QString::fromStdString(
                      QtDatapackClientLoader::datapackLoader->monsterExtra
                          .at(otherMonster->monster)
                          .name))
                  .arg(-lifeEffectReturn.quantity);
      }
      if ((lifeEffectReturn.on & ApplyOn::ApplyOn_AllAlly) ||
          (lifeEffectReturn.on & ApplyOn::ApplyOn_Themself)) {
        if (lifeEffectReturn.quantity > 0)
          attackOwner +=
              tr("heal themself of %1").arg(lifeEffectReturn.quantity);
        else if (lifeEffectReturn.quantity < 0)
          attackOwner +=
              tr("hurt themself of %1").arg(-lifeEffectReturn.quantity);
      }
    } else {
      if ((lifeEffectReturn.on & ApplyOn::ApplyOn_AllEnemy) ||
          (lifeEffectReturn.on & ApplyOn::ApplyOn_AloneEnemy)) {
        if (lifeEffectReturn.quantity > 0)
          attackOwner +=
              tr("heal of %2 your %1")
                  .arg(QString::fromStdString(
                      QtDatapackClientLoader::datapackLoader->monsterExtra
                          .at(currentMonster->monster)
                          .name))
                  .arg(lifeEffectReturn.quantity);
        else if (lifeEffectReturn.quantity < 0)
          attackOwner +=
              tr("hurt of %2 your %1")
                  .arg(QString::fromStdString(
                      QtDatapackClientLoader::datapackLoader->monsterExtra
                          .at(currentMonster->monster)
                          .name))
                  .arg(-lifeEffectReturn.quantity);
      }
      if ((lifeEffectReturn.on & ApplyOn::ApplyOn_AllAlly) ||
          (lifeEffectReturn.on & ApplyOn::ApplyOn_Themself)) {
        if (lifeEffectReturn.quantity > 0)
          attackOwner +=
              tr("heal themself of %1").arg(lifeEffectReturn.quantity);
        else if (lifeEffectReturn.quantity < 0)
          attackOwner +=
              tr("hurt themself of %1").arg(-lifeEffectReturn.quantity);
      }
    }
    if (lifeEffectReturn.effective != 1 && lifeEffectReturn.critical) {
      if (lifeEffectReturn.effective > 1)
        attackOwner +=
            QStringLiteral("\n") + tr("It's very effective and critical throw");
      else
        attackOwner += QStringLiteral("\n") +
                       tr("It's not very effective but it's critical throw");
    } else if (lifeEffectReturn.effective != 1) {
      if (lifeEffectReturn.effective > 1)
        attackOwner += QStringLiteral("\n") + tr("It's very effective");
      else
        attackOwner += QStringLiteral("\n") + tr("It's not very effective");
    } else if (lifeEffectReturn.critical) {
      attackOwner += QStringLiteral("\n") + tr("Critical throw");
    }
    ShowStatusMessage(attackOwner, false, false);
    qDebug() << "display the life effect";
    qDebug()
        << QStringLiteral(
               "displayFirstAttackText(): after display text, "
               "lifeEffectMonster.size(): %1, buffLifeEffectMonster.size(): "
               "%2, addBuffEffectMonster.size(): %3, "
               "removeBuffEffectMonster.size(): %4, attackReturnList.size(): "
               "%5")
               .arg(currentAttack.lifeEffectMonster.size())
               .arg(currentAttack.buffLifeEffectMonster.size())
               .arg(currentAttack.addBuffEffectMonster.size())
               .arg(currentAttack.removeBuffEffectMonster.size())
               .arg(client_->getAttackReturnList().size());
    return true;
  }

  // the add buff
  if (!currentAttack.addBuffEffectMonster.empty()) {
    return ApplyBuffEffect(currentMonster, otherMonster, currentAttack);
  }

  // the remove buff
  if (!currentAttack.removeBuffEffectMonster.empty()) {
    const Skill::BuffEffect &removeBuffEffectMonster =
        currentAttack.removeBuffEffectMonster.front();
    UI::ListView *buff_list = nullptr;
    if (currentAttack.doByTheCurrentMonster) {
      if ((removeBuffEffectMonster.on & ApplyOn::ApplyOn_AllEnemy) ||
          (removeBuffEffectMonster.on & ApplyOn::ApplyOn_AloneEnemy)) {
        attackOwner +=
            tr("add the buff %2 on the other %1")
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->monsterExtra
                        .at(otherMonster->monster)
                        .name))
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->monsterBuffsExtra
                        .at(removeBuffEffectMonster.buff)
                        .name));
        buff_list = enemy_buff_;
      }
      if ((removeBuffEffectMonster.on & ApplyOn::ApplyOn_AllAlly) ||
          (removeBuffEffectMonster.on & ApplyOn::ApplyOn_Themself)) {
        attackOwner +=
            tr("add the buff %1 on themself")
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->monsterBuffsExtra
                        .at(removeBuffEffectMonster.buff)
                        .name));
        buff_list = player_buff_;
      }
    } else {
      if ((removeBuffEffectMonster.on & ApplyOn::ApplyOn_AllEnemy) ||
          (removeBuffEffectMonster.on & ApplyOn::ApplyOn_AloneEnemy)) {
        attackOwner +=
            tr("add the buff %2 on your %1")
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->monsterExtra
                        .at(currentMonster->monster)
                        .name))
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->monsterBuffsExtra
                        .at(removeBuffEffectMonster.buff)
                        .name));
        buff_list = player_buff_;
      }
      if ((removeBuffEffectMonster.on & ApplyOn::ApplyOn_AllAlly) ||
          (removeBuffEffectMonster.on & ApplyOn::ApplyOn_Themself)) {
        attackOwner +=
            tr("add the buff %1 on themself")
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->monsterBuffsExtra
                        .at(removeBuffEffectMonster.buff)
                        .name));
        buff_list = enemy_buff_;
      }
    }
    ShowStatusMessage(attackOwner);
    if (buff_list != nullptr) {
      uint8_t index = 0;
      while (index < buff_list->Count()) {
        auto tmp = buff_list->GetItem(index);
        if (tmp->Data(99) == removeBuffEffectMonster.buff) {
          buff_list->RemoveItem(index);
          delete tmp;
        }
      }
    }
    client_->removeTheFirstRemoveBuffEffectAttackReturn();
    if (!client_->firstAttackReturnHaveMoreEffect())
      client_->removeTheFirstAttackReturn();
    qDebug() << "display the remove buff";
    qDebug()
        << QStringLiteral(
               "displayFirstAttackText(): after display text, "
               "lifeEffectMonster.size(): %1, buffLifeEffectMonster.size(): "
               "%2, addBuffEffectMonster.size(): %3, "
               "removeBuffEffectMonster.size(): %4, attackReturnList.size(): "
               "%5")
               .arg(currentAttack.lifeEffectMonster.size())
               .arg(currentAttack.buffLifeEffectMonster.size())
               .arg(currentAttack.addBuffEffectMonster.size())
               .arg(currentAttack.removeBuffEffectMonster.size())
               .arg(client_->getAttackReturnList().size());
    return false;
  }

  // the apply buff effect
  if (!currentAttack.buffLifeEffectMonster.empty()) {
    const Skill::LifeEffectReturn &buffLifeEffectMonster =
        currentAttack.buffLifeEffectMonster.front();
    if (currentAttack.doByTheCurrentMonster) {
      if ((buffLifeEffectMonster.on & ApplyOn::ApplyOn_AllEnemy) ||
          (buffLifeEffectMonster.on & ApplyOn::ApplyOn_AloneEnemy)) {
        if (buffLifeEffectMonster.quantity > 0)
          attackOwner +=
              tr("heal of %2 the other %1")
                  .arg(QString::fromStdString(
                      QtDatapackClientLoader::datapackLoader->monsterExtra
                          .at(otherMonster->monster)
                          .name))
                  .arg(buffLifeEffectMonster.quantity);
        else if (buffLifeEffectMonster.quantity < 0)
          attackOwner +=
              tr("hurt of %2 the other %1")
                  .arg(QString::fromStdString(
                      QtDatapackClientLoader::datapackLoader->monsterExtra
                          .at(otherMonster->monster)
                          .name))
                  .arg(-buffLifeEffectMonster.quantity);
      }
      if ((buffLifeEffectMonster.on & ApplyOn::ApplyOn_AllAlly) ||
          (buffLifeEffectMonster.on & ApplyOn::ApplyOn_Themself)) {
        if (buffLifeEffectMonster.quantity > 0)
          attackOwner +=
              tr("heal themself of %1").arg(buffLifeEffectMonster.quantity);
        else if (buffLifeEffectMonster.quantity < 0)
          attackOwner +=
              tr("hurt themself of %1").arg(-buffLifeEffectMonster.quantity);
      }
    } else {
      if ((buffLifeEffectMonster.on & ApplyOn::ApplyOn_AllEnemy) ||
          (buffLifeEffectMonster.on & ApplyOn::ApplyOn_AloneEnemy)) {
        if (buffLifeEffectMonster.quantity > 0)
          attackOwner +=
              tr("heal of %2 your %1")
                  .arg(QString::fromStdString(
                      QtDatapackClientLoader::datapackLoader->monsterExtra
                          .at(currentMonster->monster)
                          .name))
                  .arg(buffLifeEffectMonster.quantity);
        else if (buffLifeEffectMonster.quantity < 0)
          attackOwner +=
              tr("hurt of %2 your %1")
                  .arg(QString::fromStdString(
                      QtDatapackClientLoader::datapackLoader->monsterExtra
                          .at(currentMonster->monster)
                          .name))
                  .arg(-buffLifeEffectMonster.quantity);
      }
      if ((buffLifeEffectMonster.on & ApplyOn::ApplyOn_AllAlly) ||
          (buffLifeEffectMonster.on & ApplyOn::ApplyOn_Themself)) {
        if (buffLifeEffectMonster.quantity > 0)
          attackOwner +=
              tr("heal themself of %1").arg(buffLifeEffectMonster.quantity);
        else if (buffLifeEffectMonster.quantity < 0)
          attackOwner +=
              tr("hurt themself of %1").arg(-buffLifeEffectMonster.quantity);
      }
    }
    if (buffLifeEffectMonster.effective != 1 &&
        buffLifeEffectMonster.critical) {
      if (buffLifeEffectMonster.effective > 1) {
        attackOwner +=
            QStringLiteral("\n") + tr("It's very effective and critical throw");
      } else {
        attackOwner += QStringLiteral("\n") +
                       tr("It's not very effective but it's critical throw");
      }
    } else if (buffLifeEffectMonster.effective != 1) {
      if (buffLifeEffectMonster.effective > 1) {
        attackOwner += QStringLiteral("\n") + tr("It's very effective");
      } else {
        attackOwner += QStringLiteral("\n") + tr("It's not very effective");
      }
    } else if (buffLifeEffectMonster.critical) {
      attackOwner += QStringLiteral("\n") + tr("Critical throw");
    }
    qDebug() << "display the buff effect";
    qDebug()
        << QStringLiteral(
               "displayFirstAttackText(): after display text, "
               "lifeEffectMonster.size(): %1, buffLifeEffectMonster.size(): "
               "%2, addBuffEffectMonster.size(): %3, "
               "removeBuffEffectMonster.size(): %4, attackReturnList.size(): "
               "%5")
               .arg(currentAttack.lifeEffectMonster.size())
               .arg(currentAttack.buffLifeEffectMonster.size())
               .arg(currentAttack.addBuffEffectMonster.size())
               .arg(currentAttack.removeBuffEffectMonster.size())
               .arg(client_->getAttackReturnList().size());
    return true;
  }
  emit error("Can't display text without effect");
  return false;
}

void Battle::error(std::string error) {
  qDebug() << QString::fromStdString(error);
  // emit newError(tr("Error with the protocol").toStdString(),error);
}

void Battle::ShowMessageDialog(const QString &title, const QString &message) {
  if (message_dialog_ == nullptr) {
    message_dialog_ = new MessageDialog();
    message_dialog_->SetParent(this);
  }
  message_dialog_->SetMessage(message);
  emit setAbove(message_dialog_);
}

void Battle::ShowMonsterDialog() {
  if (monsters_bag_ == nullptr) {
    monsters_bag_ = new MonsterBag();
    monsters_bag_->setParent(this);
    monsters_bag_->SetVar(connexionManager);
    if (!connect(monsters_bag_, &MonsterBag::OnSelectMonster, this,
                 &Battle::OnMonsterSelect))
      abort();
  }
  emit setAbove(monsters_bag_);
}

void Battle::ShowBagDialog() {
  if (inventory_ == nullptr) {
    inventory_ = new Inventory();
    inventory_->SetParent(this);
    if (!connect(inventory_, &Inventory::useItem, this, &Battle::UseBagItem))
      abort();
  }
  waitedObjectType = Inventory::ObjectType::ObjectType_UseInFight;
  inventory_->setVar(connexionManager, waitedObjectType, true);
  emit setAbove(inventory_);
}

void Battle::UseBagItem(uint16_t item) {
  emit setAbove(nullptr);
  const uint8_t monsterPosition = 0;
  const PlayerMonster *const monster =
      client_->monsterByPosition(monsterPosition);
  const QtDatapackClientLoader::MonsterExtra &monsterInformationsExtra =
      QtDatapackClientLoader::datapackLoader->monsterExtra.at(monster->monster);

  // it's trap
  if (CommonDatapack::commonDatapack.items.trap.find(item) !=
          CommonDatapack::commonDatapack.items.trap.cend() &&
      client_->isInFightWithWild()) {
    useTrap(item);
    return;
  } else {  // else it's to use on current monster
    const uint8_t &monsterPosition = client_->getCurrentSelectedMonsterNumber();
    if (client_->useObjectOnMonsterByPosition(item, monsterPosition)) {
      if (CommonDatapack::commonDatapack.items.monsterItemEffect.find(item) !=
          CommonDatapack::commonDatapack.items.monsterItemEffect.cend()) {
        client_->useObjectOnMonsterByPosition(item, monsterPosition);
        UpdateAttackSkills();
        displayAttackProgression = 0;
        attack_quantity_changed = 0;
        if (battleType != BattleType_OtherPlayer) {
          doNextAction();
        } else {
          ShowStatusMessage(tr("In waiting of the other player action"), false,
                            false);
        }
      } else {
        error(tr("You have selected a buggy object").toStdString());
      }
    } else {
      ShowMessageDialog(tr("Warning"), tr("Can't be used now!"));
    }
  }
  QString text;
  if (client_->useObjectOnMonsterByPosition(item, monsterPosition)) {
    text += tr("Using <b>%1</b> on <b>%2</b>")
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->itemsExtra.at(item)
                        .name))
                .arg(QString::fromStdString(monsterInformationsExtra.name));
  } else {
    text += tr("Failed to use <b>%1</b> on <b>%2</b>")
                .arg(QString::fromStdString(
                    QtDatapackClientLoader::datapackLoader->itemsExtra.at(item)
                        .name))
                .arg(QString::fromStdString(monsterInformationsExtra.name));
  }

  ShowStatusMessage(text);
}

QPixmap Battle::getFrontSkin(const std::string &skinName) {
  (void)skinName;
  /*
    if(frontSkinCacheString.find(skinName)!=frontSkinCacheString.cend())
        return frontSkinCacheString.at(skinName);
    const QPixmap skin(QString::fromStdString(getFrontSkinPath(skinName)));
    if(!skin.isNull())
        frontSkinCacheString[skinName]=skin;
    else
        frontSkinCacheString[skinName]=QPixmap(":/CC/images/player_default/front.png");
    return frontSkinCacheString.at(skinName);*/
  return QPixmap(":/CC/images/player_default/front.png");
}

QPixmap Battle::getFrontSkin(const uint32_t &skinId) {
  if (skinId < (uint32_t)QtDatapackClientLoader::datapackLoader->skins.size()) {
    return getFrontSkin(
        QtDatapackClientLoader::datapackLoader->skins.at(skinId));
  } else {
    return getFrontSkin(std::string());
  }
}

void Battle::ConfigurePlayer() {
  const auto &informations = client_->player_informations.public_informations;
  playerBackImage = GetBackSkin(informations.skinId);
  playerBackImage = playerBackImage.transformed(QTransform().scale(-1, 1));
  playerBackImage = playerBackImage.scaledToHeight(600);
  player_->SetPixmap(playerBackImage);
  player_->SetPos(-150, 290);
  player_->StoreState(kOutScreen);

  player_->SetPos(60, 290);
  player_->StoreState(kReadyForBattle);

  player_exit_ = MoveTo::Create(1500, -400, 290);
  player_exit_->SetTag(kStatusExit);
  player_dead_ = MoveTo::Create(1500, -400, 290);
  player_dead_->SetTag(kStatusDead);
  player_enter_ = MoveTo::Create(2500, 60, 290);
  player_enter_->SetTag(kStatusEnter);

  player_->SetOnActionDone(std::bind(&Battle::OnPlayerActionDone, this, _1));

  player_buff_ = UI::ListView::Create(frameFightBottom);
  // player_buff_->SetHorizontal(true);
  player_buff_->SetPos(200, 25);
  player_buff_->SetSize(180, 16);
}

void Battle::ConfigureEnemy() {
  enemy_->SetPos(1500, 90);
  enemy_->StoreState(kOutScreen);

  enemy_->SetPos(910, 90);
  enemy_->StoreState(kReadyForBattle);

  enemy_exit_ = MoveTo::Create(1500, 1500, 90);
  enemy_exit_->SetTag(kStatusExit);
  enemy_dead_ = MoveTo::Create(1500, 1500, 90);
  enemy_dead_->SetTag(kStatusDead);
  enemy_enter_ = MoveTo::Create(1500, 910, 90);
  enemy_enter_->SetTag(kStatusEnter);

  enemy_->SetOnActionDone(std::bind(&Battle::OnEnemyActionDone, this, _1));

  enemy_buff_ = UI::ListView::Create(frameFightTop);
  // enemy_buff_->SetHorizontal(true);
  enemy_buff_->SetPos(200, 20);
  enemy_buff_->SetSize(180, 16);
}

void Battle::ConfigureCommons() {
  status_timeout_ = Timeout::Create(1500);

  status_label_->SetOnActionDone(
      std::bind(&Battle::OnStatusActionDone, this, _1));

  sprite_damage_ = Blink::Create(100, 10);
  sprite_damage_->SetTag(kStatusDamage);

  attack_ = Sprite::Create(this);
  attack_->SetOnActionDone(std::bind(&Battle::OnAttackDone, this, _1));

  std::vector<QPixmap> frames;
  attack_animation_ = Animation::Create(frames, 100);
}

void Battle::OnPlayerActionDone(int tag) {
  if (tag == kStatusEnter) {
    updateCurrentMonsterInformation();
    doNextAction();
  } else if (tag == kStatusExit) {
    updateCurrentMonsterInformation();
    updateCurrentMonsterInformationXp();
    if (battleStep == BattleStep_Presentation) {
      resetPosition(true, false, true);
      battleStep = BattleStep_PresentationMonster;
      PlayerMonster *monster = client_->getCurrentMonster();
      if (monster == NULL) {
        newError(tr("Internal error").toStdString() + ", file: " +
                     std::string(__FILE__) + ":" + std::to_string(__LINE__),
                 "NULL pointer at updateCurrentMonsterInformation()");
        return;
      }
      ShowStatusMessage(tr("Go %1").arg(QString::fromStdString(
          QtDatapackClientLoader::datapackLoader->monsterExtra
              .at(monster->monster)
              .name)));
      player_->RunAction(player_enter_);
    }
  } else if (tag == kStatusDead) {
    client_->dropKOCurrentMonster();
    if (client_->haveAnotherMonsterOnThePlayerToFight()) {
      if (client_->isInFight()) {
#ifdef DEBUG_CLIENT_BATTLE
        qDebug() << "Your current monster is KO, select another";
#endif
        // TODO(lanstat): Verificar como llamar al overmaplogic
        // selectObject(ObjectType_MonsterToFightKO);
      } else {
#ifdef DEBUG_CLIENT_BATTLE
        qDebug() << "You win";
#endif
        doNextActionStep = DoNextActionStep_Win;
        ShowStatusMessage(tr("You win!"), false, true);
      }
    } else {
#ifdef DEBUG_CLIENT_BATTLE
      qDebug() << "You lose";
#endif
      doNextActionStep = DoNextActionStep_Loose;
      ShowStatusMessage(tr("You lose!"), false, true);
    }
  } else if (tag == kStatusDamage) {
    OnDamageDone();
  }
}

void Battle::OnEnemyActionDone(int tag) {
  if (tag == kStatusEnter) {
    updateOtherMonsterInformation();
    doNextAction();
  } else if (tag == kStatusExit) {
    updateOtherMonsterInformation();
    doNextAction();
  } else if (tag == kStatusDead) {
    client_->dropKOOtherMonster();
    if (client_->isInFight()) {
      if (client_->isInBattle() && !client_->haveBattleOtherMonster()) {
        ShowStatusMessage(tr("In waiting of other monster selection"), false,
                          false);
        return;
      }
      pushButtonFightEnterNext->setVisible(false);
      init_other_monster_display();
      frameFightTop->setVisible(false);
      updateOtherMonsterInformation();

      QString status;
      PublicPlayerMonster *otherMonster = client_->getOtherMonster();
      if (QtDatapackClientLoader::datapackLoader->monsterExtra.find(
              otherMonster->monster) !=
          QtDatapackClientLoader::datapackLoader->monsterExtra.cend())
        status = tr("The other player call %1")
                     .arg(QString::fromStdString(
                         QtDatapackClientLoader::datapackLoader->monsterExtra
                             .at(otherMonster->monster)
                             .name));
      else
        status = tr("The other player call %1").arg(tr("(Unknown monster)"));
      ShowStatusMessage(status, false, false);
      resetPosition(true, true, false);
      enemy_->RunAction(enemy_enter_);
    } else {
      qDebug() << "doNextAction(): you win";
      doNextActionStep = DoNextActionStep_Win;
      if (!escape) {
        PlayerMonster *currentMonster = client_->getCurrentMonster();
        if (currentMonster != NULL) {
          progressBarFightBottomExp->setValue(currentMonster->remaining_xp);
          labelFightBottomLevel->setPlainText(
              tr("Level %1").arg(currentMonster->level));
        }
        ShowStatusMessage(tr("You win!"));
      } else {
        if (escapeSuccess)
          ShowStatusMessage(tr("Your escape is successful"));
        else
          ShowStatusMessage(tr("Your escape have failed but you win"));
      }
      if (escape) {
        fightTimerFinish = true;
        doNextAction();
      }
    }
  } else if (tag == kStatusDamage) {
    OnDamageDone();
  }
}

void Battle::OnStatusActionDone(int tag) {
  (void)tag;
  doNextAction();
}

UI::Button *Battle::CreateSkillButton() {
  auto item = UI::Button::Create(":/CC/images/interface/redbutton.png");
  item->SetPixelSize(10);
  item->SetSize(500, 25);
  item->SetOnClick(std::bind(&Battle::OnSkillClick, this, _1));
  return item;
}

void Battle::OnSkillClick(Node *button) {
  if (!client_->canDoFightAction()) {
    ShowMessageDialog(
        "Communication problem",
        tr("Sorry but the client wait more data from the server to do it"));
    return;
  }
  const uint8_t skill_endurance =
      static_cast<uint8_t>(button->Data(kSkillEndurance));
  const uint16_t skill_used = button->Data(kSkillId);
  if (skill_endurance <= 0 && !useTheRescueSkill) {
    ShowMessageDialog(tr("No endurance"),
                      tr("You have no more endurance to use this skill"));
    return;
  }
  doNextActionStep = DoNextActionStep_Start;
  escape = false;
  PlayerMonster *monster = client_->getCurrentMonster();
  if (monster == NULL) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "NULL pointer at updateCurrentMonsterInformation()");
    return;
  }
  if (useTheRescueSkill)
    client_->useSkill(0);
  else
    client_->useSkill(skill_used);
  UpdateAttackSkills();
  displayAttackProgression = 0;
  attack_quantity_changed = 0;
  if (battleType != BattleType_OtherPlayer) {
    doNextAction();
  } else {
    ShowStatusMessage(tr("In waiting of the other player action"), false,
                      false);
  }
}

void Battle::ShowStatusMessage(const std::string &text, bool show_next_btn,
                               bool use_timeout) {
  ShowStatusMessage(QString::fromStdString(text), show_next_btn, use_timeout);
}

void Battle::ShowStatusMessage(const QString &text, bool show_next_btn,
                               bool use_timeout) {
  stackedWidgetFightBottomBarsetCurrentWidget(0);
  status_label_->SetText(text);
  if (use_timeout) {
    status_label_->RunAction(status_timeout_);
    pushButtonFightEnterNext->setVisible(false);
    return;
  }
  pushButtonFightEnterNext->setVisible(show_next_btn);
}

void Battle::UpdateAttackSkills() {
  if (!client_->getAbleToFight()) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "Try update the monster when have not any ready monster");
    return;
  }
  PlayerMonster *monster = client_->getCurrentMonster();
  if (monster == NULL) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "NULL pointer at updateCurrentMonsterInformation()");
    return;
  }
  // list the attack
  player_skills_->Clear();
  unsigned int index = 0;
  useTheRescueSkill = true;
  while (index < monster->skills.size()) {
    auto item = CreateSkillButton();
    const PlayerMonster::PlayerSkill &skill = monster->skills.at(index);
    if (skill.level > 1)
      item->SetText(
          QStringLiteral("%1, level %2 (remaining endurance: %3)")
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterSkillsExtra
                      .at(skill.skill)
                      .name))
              .arg(skill.level)
              .arg(skill.endurance));
    else
      item->SetText(
          QStringLiteral("%1 (remaining endurance: %2)")
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterSkillsExtra
                      .at(skill.skill)
                      .name))
              .arg(skill.endurance));
    item->SetData(kSkillId, skill.skill);
    item->SetData(kSkillEndurance, skill.endurance);
    if (skill.endurance > 0) useTheRescueSkill = false;
    player_skills_->AddItem(item);
    index++;
  }
  if (useTheRescueSkill &&
      CommonDatapack::commonDatapack.monsterSkills.find(0) !=
          CommonDatapack::commonDatapack.monsterSkills.cend()) {
    if (!CommonDatapack::commonDatapack.monsterSkills.at(0).level.empty()) {
      auto item = CreateSkillButton();
      item->SetText(
          QStringLiteral("Rescue skill: %1")
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterSkillsExtra
                      .at(0)
                      .name)));
      item->SetData(kSkillId, 0);
      player_skills_->AddItem(item);
    }
  }
}

void Battle::OnMonsterSelect(uint8_t monster_index) {
  resetPosition(true, false, true);
  // do copie here because the call of changeOfMonsterInFight apply the skill /
  // buff effect
  const PlayerMonster *tempMonster = client_->monsterByPosition(monster_index);
  if (tempMonster == NULL) {
    qDebug() << "Monster not found";
    return;
  }
  PlayerMonster copiedMonster = *tempMonster;
  if (!client_->changeOfMonsterInFight(monster_index)) return;
  client_->changeOfMonsterInFightByPosition(monster_index);
  PlayerMonster *playerMonster = client_->getCurrentMonster();
  init_current_monster_display(&copiedMonster);
  if (QtDatapackClientLoader::datapackLoader->monsterExtra.find(
          playerMonster->monster) !=
      QtDatapackClientLoader::datapackLoader->monsterExtra.cend()) {
    player_->SetPixmap(QtDatapackClientLoader::datapackLoader
                           ->getMonsterExtra(playerMonster->monster)
                           .back.scaled(160, 160));
    ShowStatusMessage(tr("Go %1").arg(QString::fromStdString(
                          QtDatapackClientLoader::datapackLoader->monsterExtra
                              .at(playerMonster->monster)
                              .name)),
                      false, false);
  } else {
    ShowStatusMessage(tr("You change of monster"), false, false);
    player_->SetPixmap(QPixmap(":/CC/images/monsters/default/back.png"));
  }
  battleStep = BattleStep_Presentation;
  player_->RunAction(player_enter_);
}

void Battle::StartAttack() {
  PublicPlayerMonster *other_monster = client_->getOtherMonster();
  PublicPlayerMonster *current_monster = client_->getCurrentMonster();
  if (other_monster == NULL) {
    error(
        "displayAttack(): crash: unable to get the other monster to display "
        "an "
        "attack");
    doNextAction();
    return;
  }
  if (current_monster == NULL) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "displayAttack(): crash: unable to get the current monster");
    doNextAction();
    return;
  }

  const Skill::AttackReturn &attack_return = client_->getFirstAttackReturn();
  // get the life effect to display
  CatchChallenger::ApplyOn apply_on;
  if (!attack_return.lifeEffectMonster.empty()) {
    apply_on = attack_return.lifeEffectMonster.front().on;
  } else if (!attack_return.buffLifeEffectMonster.empty()) {
    apply_on = attack_return.buffLifeEffectMonster.front().on;
  } else if (!attack_return.addBuffEffectMonster.empty()) {
    apply_on = attack_return.addBuffEffectMonster.front().on;
  } else if (!attack_return.removeBuffEffectMonster.empty()) {
    apply_on = attack_return.removeBuffEffectMonster.front().on;
  } else {
    error(QStringLiteral(
              "displayAttack(): strange: nothing to display, "
              "lifeEffectMonster.size(): %1, buffLifeEffectMonster.size(): "
              "%2, addBuffEffectMonster.size(): %3, "
              "removeBuffEffectMonster.size(): %4, AttackReturnList: %5")
              .arg(attack_return.lifeEffectMonster.size())
              .arg(attack_return.buffLifeEffectMonster.size())
              .arg(attack_return.addBuffEffectMonster.size())
              .arg(attack_return.removeBuffEffectMonster.size())
              .arg(client_->getAttackReturnList().size())
              .toStdString());
    client_->removeTheFirstAttackReturn();
    // doNextAction();
    return;
  }

  bool apply_on_enemy = false;
  if (attack_return.doByTheCurrentMonster) {
    if ((apply_on == ApplyOn::ApplyOn_AloneEnemy) ||
        (apply_on == ApplyOn::ApplyOn_AllEnemy))
      apply_on_enemy = true;
  } else {
    if ((apply_on == ApplyOn::ApplyOn_Themself) ||
        (apply_on == ApplyOn::ApplyOn_AllAlly))
      apply_on_enemy = true;
  }
  if (apply_on_enemy)
    attack_->SetPos(enemy_->X(), enemy_->Y());
  else
    attack_->SetPos(player_->X(), player_->Y());

  // attack animation
  {
    uint32_t attack_id = client_->getFirstAttackReturn().attack;
    QString skill_animation =
        QString::fromStdString(
            QtDatapackClientLoader::datapackLoader->getDatapackPath()) +
        DATAPACK_BASE_PATH_SKILLANIMATION;
    QString file_animation =
        skill_animation + QStringLiteral("%1.gif").arg(attack_id);
    if (QFile(file_animation).exists()) {
      std::vector<QPixmap> frames;
      attack_animation_->SetFrames(frames);
    }
  }

  if (!displayFirstAttackText(true)) {
    return;
  }

  attack_->SetVisible(true);
  attack_->RunAction(attack_animation_);
}

void Battle::OnAttackDone(int tag) {
  (void)tag;
  attack_->SetVisible(false);
  if (client_->getAttackReturnList().empty()) {
    newError(tr("Internal error").toStdString() + ", file: " +
                 std::string(__FILE__) + ":" + std::to_string(__LINE__),
             "displayAttack(): crash: display an empty attack return");
    doNextAction();
    return;
  }
  if (client_->getAttackReturnList().front().lifeEffectMonster.empty() &&
      client_->getAttackReturnList().front().buffLifeEffectMonster.empty() &&
      client_->getAttackReturnList().front().addBuffEffectMonster.empty() &&
      client_->getAttackReturnList().front().removeBuffEffectMonster.empty() &&
      client_->getAttackReturnList().front().success) {
    qDebug() << QStringLiteral(
                    "displayAttack(): strange: display an empty lifeEffect "
                    "list into attack return, attack: %1, "
                    "doByTheCurrentMonster: %2, success: %3")
                    .arg(connexionManager->client->getAttackReturnList()
                             .front()
                             .attack)
                    .arg(connexionManager->client->getAttackReturnList()
                             .front()
                             .doByTheCurrentMonster)
                    .arg(connexionManager->client->getAttackReturnList()
                             .front()
                             .success);
    client_->removeTheFirstAttackReturn();
    doNextAction();
    return;
  }

  const Skill::AttackReturn &attack_return = client_->getFirstAttackReturn();
  // get the life effect to display
  CatchChallenger::ApplyOn apply_on;
  if (!attack_return.lifeEffectMonster.empty()) {
    apply_on = attack_return.lifeEffectMonster.front().on;
  } else if (!attack_return.buffLifeEffectMonster.empty()) {
    apply_on = attack_return.buffLifeEffectMonster.front().on;
  } else if (!attack_return.addBuffEffectMonster.empty()) {
    apply_on = attack_return.addBuffEffectMonster.front().on;
  } else if (!attack_return.removeBuffEffectMonster.empty()) {
    apply_on = attack_return.removeBuffEffectMonster.front().on;
  } else {
    error(QStringLiteral(
              "displayAttack(): strange: nothing to display, "
              "lifeEffectMonster.size(): %1, buffLifeEffectMonster.size(): "
              "%2, addBuffEffectMonster.size(): %3, "
              "removeBuffEffectMonster.size(): %4, AttackReturnList: %5")
              .arg(attack_return.lifeEffectMonster.size())
              .arg(attack_return.buffLifeEffectMonster.size())
              .arg(attack_return.addBuffEffectMonster.size())
              .arg(attack_return.removeBuffEffectMonster.size())
              .arg(client_->getAttackReturnList().size())
              .toStdString());
    client_->removeTheFirstAttackReturn();
    // doNextAction();
    return;
  }

  bool apply_on_enemy = false;
  if (attack_return.doByTheCurrentMonster) {
    if ((apply_on == ApplyOn::ApplyOn_AloneEnemy) ||
        (apply_on == ApplyOn::ApplyOn_AllEnemy))
      apply_on_enemy = true;
  } else {
    if ((apply_on == ApplyOn::ApplyOn_Themself) ||
        (apply_on == ApplyOn::ApplyOn_AllAlly))
      apply_on_enemy = true;
  }
  if (apply_on_enemy) {
    enemy_->RunAction(sprite_damage_);
  } else {
    player_->RunAction(sprite_damage_);
  }

  if (!attack_return.lifeEffectMonster.empty()) {
    int hp_to_change = attack_return.lifeEffectMonster.front().quantity;
    if (hp_to_change != 0) {
      client_->firstLifeEffectQuantityChange(-hp_to_change);
      if (apply_on_enemy) {
        enemy_hp_bar_->IncrementValue(hp_to_change, true);
      } else {
        player_hp_bar_->IncrementValue(hp_to_change, true);
      }
    }
  }
}

void Battle::OnDamageDone() {
  const Skill::AttackReturn &attack_return = client_->getFirstAttackReturn();
  if (!attack_return.lifeEffectMonster.empty())
    client_->removeTheFirstLifeEffectAttackReturn();
  else
    client_->removeTheFirstBuffEffectAttackReturn();
  if (!client_->firstAttackReturnHaveMoreEffect()) {
#ifdef CATCHCHALLENGER_DEBUG_FIGHT
    {
      qDebug() << "after display attack: currentMonster have hp "
               << player_hp_bar_->Value() << "and buff" << bottomBuff->count();
      qDebug() << "after display attack: otherMonster have hp"
               << enemy_hp_bar_->value() << "and buff" << enemy_buff_->Count();
    }
#endif
    client_->removeTheFirstAttackReturn();
  }
  // attack is finish
  doNextAction();
}

void Battle::newError(std::string error, std::string detailedError) {
  qDebug() << QString::fromStdString(error + ": " + detailedError);
}

bool Battle::ApplyBuffEffect(PublicPlayerMonster *current_monster,
                             PublicPlayerMonster *other_monster,
                             const Skill::AttackReturn current_attack) {
  QString attack_owner;
  const auto &buff_effect = current_attack.addBuffEffectMonster.front();
  UI::ListView *buff_list = nullptr;
#ifdef CATCHCHALLENGER_EXTRA_CHECK
  bool on_buff_current_monster = false;
#endif
  if (current_attack.doByTheCurrentMonster) {
    if ((buff_effect.on & ApplyOn::ApplyOn_AllEnemy) ||
        (buff_effect.on & ApplyOn::ApplyOn_AloneEnemy)) {
      attack_owner +=
          tr("add the buff %2 on the other %1")
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterExtra
                      .at(other_monster->monster)
                      .name))
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterBuffsExtra
                      .at(buff_effect.buff)
                      .name));
      buff_list = enemy_buff_;
#ifdef CATCHCHALLENGER_EXTRA_CHECK
      on_buff_current_monster = false;
#endif
    }
    if ((buff_effect.on & ApplyOn::ApplyOn_AllAlly) ||
        (buff_effect.on & ApplyOn::ApplyOn_Themself)) {
      attack_owner +=
          tr("add the buff %1 on themself")
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterBuffsExtra
                      .at(buff_effect.buff)
                      .name));
      buff_list = player_buff_;
#ifdef CATCHCHALLENGER_EXTRA_CHECK
      on_buff_current_monster = true;
#endif
    }
  } else {
    if ((buff_effect.on & ApplyOn::ApplyOn_AllEnemy) ||
        (buff_effect.on & ApplyOn::ApplyOn_AloneEnemy)) {
      attack_owner +=
          tr("add the buff %2 on your %1")
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterExtra
                      .at(current_monster->monster)
                      .name))
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterBuffsExtra
                      .at(buff_effect.buff)
                      .name));
      buff_list = player_buff_;
#ifdef CATCHCHALLENGER_EXTRA_CHECK
      on_buff_current_monster = true;
#endif
    }
    if ((buff_effect.on & ApplyOn::ApplyOn_AllAlly) ||
        (buff_effect.on & ApplyOn::ApplyOn_Themself)) {
      attack_owner +=
          tr("add the buff %1 on themself")
              .arg(QString::fromStdString(
                  QtDatapackClientLoader::datapackLoader->monsterBuffsExtra
                      .at(buff_effect.buff)
                      .name));
      buff_list = enemy_buff_;
#ifdef CATCHCHALLENGER_EXTRA_CHECK
      on_buff_current_monster = false;
#endif
    }
  }
  ShowStatusMessage(attack_owner);
  if (buff_list != nullptr) {
    // search the buff
    Sprite *item = nullptr;
    int index = 0;
    while (index < buff_list->Count()) {
      if (buff_list->GetItem(index)->Data(99) == buff_effect.buff) {
        item = static_cast<Sprite *>(buff_list->GetItem(index));
        break;
      }
      index++;
    }
    // add new buff because don't exist
    if (index == buff_list->Count()) {
      item = Sprite::Create();
      item->SetData(99,
                    buff_effect.buff);  // to prevent duplicate buff, because
                                        // add can be to re-enable an already
                                        // enable buff (for larger period then)
    }
    if (QtDatapackClientLoader::datapackLoader->monsterBuffsExtra.find(
            buff_effect.buff) ==
        QtDatapackClientLoader::datapackLoader->monsterBuffsExtra.cend()) {
      item->SetToolTip(tr("Unknown buff"));
      item->SetPixmap(QPixmap(":/CC/images/interface/buff.png"));
    } else {
      const QtDatapackClientLoader::MonsterExtra::Buff &buff_extra =
          QtDatapackClientLoader::datapackLoader->monsterBuffsExtra.at(
              buff_effect.buff);
      const QtDatapackClientLoader::QtBuff &qt_buff_extra =
          QtDatapackClientLoader::datapackLoader->QtmonsterBuffsExtra.at(
              buff_effect.buff);
      if (!qt_buff_extra.icon.isNull()) {
        item->SetPixmap(qt_buff_extra.icon);
      } else {
        item->SetPixmap(QPixmap(":/CC/images/interface/buff.png"));
      }
      if (buff_effect.level <= 1) {
        item->SetToolTip(QString::fromStdString(buff_extra.name));
      } else {
        item->SetToolTip(tr("%1 at level %2")
                             .arg(QString::fromStdString(buff_extra.name))
                             .arg(buff_effect.level));
      }
      item->SetToolTip(item->ToolTip() + "\n" +
                       QString::fromStdString(buff_extra.description));
    }
    item->SetSize(16, 16);
    if (index == buff_list->Count()) buff_list->AddItem(item);
  }
  client_->removeTheFirstAddBuffEffectAttackReturn();
  if (!client_->firstAttackReturnHaveMoreEffect())
    client_->removeTheFirstAttackReturn();
#ifdef CATCHCHALLENGER_EXTRA_CHECK
  if (!on_buff_current_monster && other_monster != NULL) {
    if (other_monster->buffs.size() != buff_list->Count()) {
      error(
          "displayFirstAttackText(): bug: buffs number displayed != than "
          "real buff count on other monter");
      doNextAction();
      return false;
    }
  }
  if (on_buff_current_monster && current_monster != NULL) {
    if (current_monster->buffs.size() != buff_list->Count()) {
      error(
          "displayFirstAttackText(): bug: buffs number displayed != than "
          "real buff count on current monter");
      doNextAction();
      return false;
    }
  }
#endif
  qDebug() << "display the add buff";
  qDebug()
      << QStringLiteral(
             "displayFirstAttackText(): after display text, "
             "lifeEffectMonster.size(): %1, buffLifeEffectMonster.size(): "
             "%2, buff_effect.size(): %3, "
             "removeBuffEffectMonster.size(): %4, attackReturnList.size(): "
             "%5")
             .arg(current_attack.lifeEffectMonster.size())
             .arg(current_attack.buffLifeEffectMonster.size())
             .arg(current_attack.addBuffEffectMonster.size())
             .arg(current_attack.removeBuffEffectMonster.size())
             .arg(client_->getAttackReturnList().size());
  return false;
}
