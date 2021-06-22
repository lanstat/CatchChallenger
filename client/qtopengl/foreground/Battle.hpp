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
#include "../action/Animation.hpp"
#include "../action/Blink.hpp"
#include "../action/MoveTo.hpp"
#include "../action/Timeout.hpp"
#include "../cc/ClientFightEngine.hpp"
#include "../core/ActionManager.hpp"
#include "../core/Node.hpp"
#include "../core/Sprite.hpp"
#include "../ui/Button.hpp"
#include "../ui/Label.hpp"
#include "../ui/ListView.hpp"
#include "../ui/Progressbar.hpp"

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
  enum SkillData { kSkillId = 0, kSkillEndurance = 1 };
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

  enum SpriteStatus {
    kStatusEnter = 0,
    kStatusExit = 1,
    kStatusDead = 2,
    kStatusDamage = 3
  };
  enum SpriteLocation { kOutScreen = 0, kReadyForBattle = 1 };
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
  void on_pushButtonFightReturn_clicked();
  void displayExperienceGain();
  void botFightFullDiffered();
  void OnBagButtonClick();
 signals:
  void Finished();

 private:
  ImagesStrechMiddle *frameFightBottom;
  QGraphicsTextItem *labelFightBottomName;

  ImagesStrechMiddle *frameFightTop;
  QGraphicsTextItem *labelFightTopName;
  QGraphicsTextItem *labelFightBottomLevel;

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
  QTimer displayExpTimer;
  QTimer botFightTimer;
  QTime updateTrapTime;
  QTimer displayTrapTimer;

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
  UI::ListView *player_skills_;
  ActionManager *action_manager_;

  // Player
  Sprite *player_;
  MoveTo *player_enter_;
  MoveTo *player_exit_;
  MoveTo *player_dead_;
  UI::Progressbar *player_hp_bar_;
  CCprogressbar *progressBarFightBottomExp;
  UI::ListView *player_buff_;

  // Enemy
  Sprite *enemy_;
  MoveTo *enemy_enter_;
  MoveTo *enemy_exit_;
  MoveTo *enemy_dead_;
  UI::Progressbar *enemy_hp_bar_;
  UI::ListView *enemy_buff_;

  // Attack
  Sprite *attack_;
  Animation *attack_animation_;

  // Commons
  Blink *sprite_damage_;
  Timeout *status_timeout_;

  CatchChallenger::CommonFightEngine *fightEngine;
  CatchChallenger::Api_protocol_Qt *client_;
  std::vector<uint16_t> botFightList;
  std::vector<BattleInformations> battleInformationsList;
  std::vector<uint8_t> tradeEvolutionMonsters;
  std::vector<QListWidgetItem> monsterList;

  void ConfigurePlayer();
  void ConfigureEnemy();
  void ConfigureCommons();
  void OnPlayerActionDone(int action_id);
  void OnEnemyActionDone(int action_id);
  void OnStatusActionDone(int action_id);
  UI::Button *CreateSkillButton();
  void OnSkillClick(Node *button);
  void ShowStatusMessage(const std::string &text, bool show_next_btn = false,
                         bool use_timeout = true);
  void ShowStatusMessage(const QString &text, bool show_next_btn = false,
                         bool use_timeout = true);
  void UpdateAttackSkills();
  void OnMonsterSelect(uint8_t monster_index);
  void StartAttack();
  void OnAttackDone(int tag);
  void OnDamageDone();
  bool ApplyBuffEffect(
      CatchChallenger::PublicPlayerMonster *current_monster,
      CatchChallenger::PublicPlayerMonster *other_monster,
      const CatchChallenger::Skill::AttackReturn current_attack);

  void moveFightMonsterBoth();
  void doNextAction();
  void updateCurrentMonsterInformation();
  void updateOtherMonsterInformation();
  void updateCurrentMonsterInformationXp();
  void win();
  void loose();
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
  void monsterCatch(const bool &success);
  void displayTrap();
  void useTrap(const uint16_t &itemId);
  void newError(std::string error, std::string detailedError);
 private slots:
  void error(std::string error);
};
#endif  // MULTI_H
