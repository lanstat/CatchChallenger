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
#include "../above/MessageDialog.hpp"
#include "../ScreenInput.hpp"
#include "../cc/ClientFightEngine.hpp"
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
  void stackedWidgetFightBottomBarsetCurrentWidget(int index);

  void on_monsterList_itemActivated(uint8_t monsterPosition);
  bool check_monsters();
  bool WildFightInitialize(ConnexionManager *connexionManager,
                           CatchChallenger::Map_client *map, const uint8_t &x,
                           const uint8_t &y);
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
  enum ObjectType {
    ObjectType_All,
    ObjectType_Seed,
    ObjectType_Sell,
    ObjectType_SellToMarket,
    ObjectType_Trade,
    ObjectType_MonsterToTrade,
    ObjectType_MonsterToTradeToMarket,
    ObjectType_MonsterToLearn,
    ObjectType_MonsterToFight,
    ObjectType_MonsterToFightKO,
    ObjectType_ItemOnMonsterOutOfFight,
    ObjectType_ItemOnMonster,
    ObjectType_ItemEvolutionOnMonster,
    ObjectType_ItemLearnOnMonster,
    ObjectType_UseInFight
  };
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
  //void on_pushButtonFightMonster_clicked();
  void on_pushButtonFightAttackConfirmed_clicked();
  void on_pushButtonFightReturn_clicked();
  void on_listWidgetFightAttack_itemSelectionChanged();
  void on_listWidgetFightAttack_itemActivated(QListWidgetItem *item);
 signals:
  void Finished();

 private:
  ImagesStrechMiddle *frameFightBottom;
  QList<QListWidgetItem *> bottomBuff;
  QGraphicsTextItem *labelFightBottomName;
  CCprogressbar *progressBarFightBottomHP;

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

  ObjectType waitedObjectType;
  // bot
  CatchChallenger::Bot actualBot;

  QTimer moveFightMonsterBottomTimer;
  QTimer moveFightMonsterTopTimer;
  QTimer doNextActionTimer;

  QGraphicsPixmapItem *labelFightMonsterAttackBottom;
  QGraphicsPixmapItem *labelFightMonsterAttackTop;
  QGraphicsPixmapItem *labelFightMonsterBottom;
  QGraphicsPixmapItem *labelFightMonsterTop;
  QGraphicsPixmapItem *labelFightTrap;

  QPixmap playerBackImage;
  QPixmap playerFrontImage;

  ImagesStrechMiddle *stackedWidgetFightBottomBar;

  QGraphicsTextItem *labelFightEnter;
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

  bool escape;
  bool escapeSuccess;
  bool useTheRescueSkill;
  ConnexionManager *connexionManager;
  bool fightTimerFinish;
  uint16_t fightId;

  BattleStep battleStep;
  MoveType moveType;
  BattleType battleType;
  DoNextActionStep doNextActionStep;

  CatchChallenger::CommonFightEngine *fightEngine;
  CatchChallenger::Api_protocol_Qt *client_;
  std::vector<uint16_t> botFightList;
  std::vector<BattleInformations> battleInformationsList;
  std::unordered_map<QListWidgetItem *, uint16_t> fight_attacks_graphical;
  std::unordered_map<uint8_t, QListWidgetItem *> buffToGraphicalItemTop;
  std::unordered_map<uint8_t, QListWidgetItem *> buffToGraphicalItemBottom;

  void PrepareEngine();
  void moveFightMonsterBoth();
  void moveFightMonsterBottom();
  void initializeText(QGraphicsTextItem *);
  void doNextAction();
  void updateCurrentMonsterInformation();
  void updateOtherMonsterInformation();
  void updateCurrentMonsterInformationXp();
  void updateAttackList();
  void moveFightMonsterTop();
  void win();
  void loose();
  void displayText(const std::string &text);
  void displayAttack();
  void displayExperienceGain();
  void resetPosition(bool outOfScreen, bool topMonster, bool bottomMonster);
  void botFightFullDiffered();
  void prepareFight();
  void LoadPlayerData();
  QPixmap GetBackSkin(const uint32_t &skinId);
  void load_monsters();
  void checkEvolution();

  void init_environement_display(CatchChallenger::Map_client *map,
                                 const uint8_t &x, const uint8_t &y);
  void init_current_monster_display(
      CatchChallenger::PlayerMonster *fightMonster);
  void init_other_monster_display();
  void ShowMessageDialog(const QString &text);
  void CloseMessageDialog();
 private slots:
  void error(std::string error);

 signals:
  void newError(std::string error, std::string detailedError);
};
#endif  // MULTI_H
