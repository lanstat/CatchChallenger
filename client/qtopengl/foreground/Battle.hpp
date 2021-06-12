#ifndef Battle_H
#define Battle_H

#include <QObject>
#include <QTimer>
#include <unordered_map>

#include "../../../general/base/CommonDatapackServerSpec.hpp"
#include "../../../general/base/FacilityLib.hpp"
#include "../../../general/base/GeneralStructures.hpp"
#include "../ConnexionManager.hpp"
#include "../Map_client.hpp"
#include "../ScreenInput.hpp"
#include "../above/MessageDialog.hpp"
#include "../above/inventory/Inventory.hpp"
#include "../above/monster/MonsterBag.hpp"
#include "../cc/ClientFightEngine.hpp"
#include "../core/ActionManager.hpp"
#include "../core/Sprite.hpp"
#include "../ui/Label.hpp"
#include "../ui/ListView.hpp"
class CCprogressbar;
class ImagesStrechMiddle;
class CustomButton;
class CustomText;
class QGraphicsProxyWidget;
class QListWidget;
class QListWidgetItem;

class Battle : public ScreenInput {
  Q_OBJECT

 public:
  explicit Battle();
  ~Battle();
  static Battle *battle;
  void newLanguage();
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget = nullptr) override;
  void mousePressEventXY(const QPointF &p, bool &pressValidated,
                         bool &callParentClass) override;
  void mouseReleaseEventXY(const QPointF &p, bool &pressValidated,
                           bool &callParentClass) override;
  void mouseMoveEventXY(const QPointF &p, bool &pressValidated,
                        bool &callParentClass) override;
  void stackedWidgetFightBottomBarsetCurrentWidget(int index);

  bool check_monsters();
  bool WildFightInitialize(CatchChallenger::Map_client *map, const uint8_t &x,
                           const uint8_t &y);
  void botFight(CatchChallenger::Map_client *map, const uint8_t &x,
                const uint8_t &y, const uint16_t &fightId);
  void setVar(ConnexionManager *connexionManager);

  enum BattleType { BattleType_Wild, BattleType_Bot, BattleType_OtherPlayer };
  enum BattleStep {
    BattleStep_Presentation,
    BattleStep_PresentationMonster,
    BattleStep_Middle,
    BattleStep_Final
  };
  enum MoveType { MoveType_Enter, MoveType_Leave, MoveType_Dead };
  enum DoNextActionStep {
    DoNextActionStep_Start,
    DoNextActionStep_Loose,
    DoNextActionStep_Win
  };
  enum SpriteAction { kEnter = 1, kExit = 2, kDead = 3 };
  enum SpriteState { kOutScreen = 0, kReadyForBattle = 1 };
  struct BattleInformations {
    std::string pseudo;
    uint8_t skinId;
    std::vector<uint8_t> stat;
    uint8_t monsterPlace;
    CatchChallenger::PublicPlayerMonster publicPlayerMonster;
  };
 public slots:
  void on_pushButtonFightEnterNext_clicked();
  void on_toolButtonFightQuit_clicked();
  void on_pushButtonFightAttack_clicked();
  void on_pushButtonFightMonster_clicked();
  void on_pushButtonFightAttackConfirmed_clicked();
  void on_pushButtonFightReturn_clicked();
  void on_listWidgetFightAttack_itemSelectionChanged();
  void on_listWidgetFightAttack_itemActivated(QListWidgetItem *item);
  void displayAttack();
  void displayExperienceGain();
  void botFightFullDiffered();
  void OnBagButtonClick();
 signals:
  void Finished();

 private:
  ImagesStrechMiddle *frameFightBottom;
  QList<QListWidgetItem *> bottomBuff;
  QGraphicsTextItem *labelFightBottomName;
  CCprogressbar *progressBarFightBottomHP;
  CCprogressbar *progressBarFightBottomExp;

  ImagesStrechMiddle *frameFightTop;
  QList<QListWidgetItem *> topBuff;
  QGraphicsTextItem *labelFightTopName;
  QGraphicsTextItem *labelFightBottomLevel;
  CCprogressbar *progressBarFightTopHP;

  QGraphicsPixmapItem *labelFightBackground;
  QPixmap labelFightBackgroundPix;
  QGraphicsPixmapItem *labelFightForeground;
  QPixmap labelFightForegroundPix;
  QGraphicsPixmapItem *labelFightPlateformBottom;
  QPixmap labelFightPlateformBottomPix;
  QGraphicsPixmapItem *labelFightPlateformTop;
  QPixmap labelFightPlateformTopPix;
  CatchChallenger::Map_client *map;

  Inventory::ObjectType waitedObjectType;
  // bot
  CatchChallenger::Bot actualBot;

  QTimer moveFightMonsterBothTimer;
  QTimer displayAttackTimer;
  QTimer displayExpTimer;
  QTime updateAttackTime;
  QTimer botFightTimer;
  QTime updateTrapTime;
  QTimer displayTrapTimer;
  QMovie *movie;

  QGraphicsPixmapItem *labelFightMonsterAttackBottom;
  QGraphicsPixmapItem *labelFightMonsterAttackTop;
  QGraphicsPixmapItem *labelFightTrap;

  QPixmap playerBackImage;
  QPixmap playerFrontImage;

  ImagesStrechMiddle *stackedWidgetFightBottomBar;

  UI::Label *status_label_;
  CustomButton *pushButtonFightEnterNext;

  CustomButton *pushButtonFightAttack;
  CustomButton *pushButtonFightBag;
  CustomButton *pushButtonFightMonster;
  CustomButton *toolButtonFightQuit;
  MessageDialog *message_dialog_;

  CustomButton *pushButtonFightAttackConfirmed;
  CustomButton *pushButtonFightReturn;
  QGraphicsTextItem *labelFightAttackDetails;
  QGraphicsProxyWidget *listWidgetFightAttackProxy;
  QListWidget *listWidgetFightAttack;

  int m_stackedWidgetFightBottomBarIndex = 0;
  uint32_t mLastGivenXP = 0;
  uint8_t currentMonsterLevel;
  int displayAttackProgression;
  int32_t attack_quantity_changed;
  uint8_t monsterEvolutionPostion;
  uint32_t trapItemId;

  bool escape;
  bool escapeSuccess;
  bool useTheRescueSkill;
  bool trapSuccess;
  ConnexionManager *connexionManager;
  bool fightTimerFinish;
  uint16_t fightId;
  int displayTrapProgression;
  uint16_t screen_width_;
  uint16_t screen_height_;

  BattleStep battleStep;
  MoveType moveType;
  BattleType battleType;
  DoNextActionStep doNextActionStep;

  Inventory *inventory_;
  MonsterBag *monsters_bag_;
  Sprite *player_;
  Sprite *enemy_;
  UI::ListView *player_skills_;
  ActionManager *action_manager_;

  CatchChallenger::CommonFightEngine *fightEngine;
  CatchChallenger::Api_protocol_Qt *client_;
  std::vector<uint16_t> botFightList;
  std::vector<BattleInformations> battleInformationsList;
  std::unordered_map<QListWidgetItem *, uint16_t> fight_attacks_graphical;
  std::unordered_map<uint8_t, QListWidgetItem *> buffToGraphicalItemTop;
  std::unordered_map<uint8_t, QListWidgetItem *> buffToGraphicalItemBottom;
  std::vector<uint8_t> tradeEvolutionMonsters;
  std::vector<QListWidgetItem> monsterList;

  void ConfigurePlayer();
  void ConfigureEnemy();
  void ConfigureStatusLabel();
  void OnPlayerActionDone(int action_id);
  void OnEnemyActionDone(int action_id);
  void OnStatusActionDone(int action_id);

  void moveFightMonsterBoth();
  void doNextAction();
  void updateCurrentMonsterInformation();
  void updateOtherMonsterInformation();
  void updateCurrentMonsterInformationXp();
  void updateAttackList();
  void win();
  void loose();
  void displayText(const std::string &text);
  void resetPosition(bool outOfScreen, bool topMonster, bool bottomMonster);
  void botFightFull(const uint16_t &fightId);
  void prepareFight();
  QPixmap GetBackSkin(const uint32_t &skinId);
  QPixmap getFrontSkin(const std::string &skinName);
  QPixmap getFrontSkin(const uint32_t &skinId);
  void load_monsters();
  void checkEvolution();
  bool displayFirstAttackText(bool firstText);

  void init_environement_display(CatchChallenger::Map_client *map,
                                 const uint8_t &x, const uint8_t &y);
  void init_current_monster_display(
      CatchChallenger::PlayerMonster *fightMonster);
  void init_other_monster_display();
  void battleAcceptedByOtherFull(const BattleInformations &battleInformations);
  void ShowMessageDialog(const QString &title, const QString &message);
  void ShowBagDialog();
  void ShowMonsterDialog();
  void UseBagItem(uint16_t);
  void on_monsterList_itemActivated(uint8_t monsterPosition);
  void monsterCatch(const bool &success);
  void displayTrap();
  void useTrap(const uint16_t &itemId);
 private slots:
  void error(std::string error);

 signals:
  void newError(std::string error, std::string detailedError);
};
#endif  // MULTI_H
