#ifndef BOTTARGETLIST_H
#define BOTTARGETLIST_H

#include <QDialog>
#include <QHash>
#include <QListWidgetItem>

#include "../bot/MultipleBotConnection.h"
#include "../bot/actions/ActionsAction.h"
#include "WaitScreen.h"

namespace Ui {
class BotTargetList;
}

class BotTargetList : public QDialog
{
    Q_OBJECT

public:
    explicit BotTargetList(QHash<CatchChallenger::Api_client_real *,MultipleBotConnection::CatchChallengerClient *> apiToCatchChallengerClient,
    QHash<CatchChallenger::ConnectedSocket *,MultipleBotConnection::CatchChallengerClient *> connectedSocketToCatchChallengerClient,
    QHash<QSslSocket *,MultipleBotConnection::CatchChallengerClient *> sslSocketToCatchChallengerClient,
    ActionsAction *actionsAction);
    ~BotTargetList();
    void loadAllBotsInformation();
    void loadAllBotsInformation2();
    void updateLayerElements();
    void updateMapInformation();
    void updatePlayerInformation();
    std::vector<std::string> contentToGUI(const MapServerMini::BlockObject * const blockObject, QListWidget *listGUI=NULL);
    std::vector<std::string> contentToGUI(QListWidget *listGUI, const std::unordered_map<const MapServerMini::BlockObject *, MapServerMini::BlockObjectPathFinding> &resolvedBlockList);
    std::string graphStepNearMap(const MapServerMini::BlockObject * const currentNearBlock, const unsigned int &depth=2);
    std::string graphLocalMap();
signals:
    void start_preload_the_map();
private slots:
    void on_bots_itemSelectionChanged();
    void on_comboBox_Layer_activated(int index);
    void on_localTargets_itemActivated(QListWidgetItem *item);
    void on_comboBoxStep_currentIndexChanged(int index);
    void on_browseMap_clicked();
    void on_searchDeep_editingFinished();
    void on_globalTargets_itemActivated(QListWidgetItem *item);
private:
    Ui::BotTargetList *ui;
    QHash<CatchChallenger::Api_client_real *,MultipleBotConnection::CatchChallengerClient *> apiToCatchChallengerClient;
    QHash<CatchChallenger::ConnectedSocket *,MultipleBotConnection::CatchChallengerClient *> connectedSocketToCatchChallengerClient;
    QHash<QSslSocket *,MultipleBotConnection::CatchChallengerClient *> sslSocketToCatchChallengerClient;
    ActionsAction *actionsAction;
    QHash<QString,MultipleBotConnection::CatchChallengerClient *> pseudoToBot;

    bool botsInformationLoaded;
    uint32_t mapId;
    WaitScreen waitScreen;

    std::vector<uint32_t> mapIdListLocalTarget;
    std::vector<ActionsBotInterface::GlobalTarget> targetListGlobalTarget;
    bool alternateColor;
    static std::string pathFindingToString(const MapServerMini::BlockObjectPathFinding &resolvedBlock);
    static bool isSame(const CatchChallenger::MonstersCollisionValue::MonstersCollisionContent &monstersCollisionContentA,const CatchChallenger::MonstersCollisionValue::MonstersCollisionContent &monstersCollisionContentB);

    //std::unodered_map<const CatchChallenger::MonstersCollisionValue::MonstersCollisionContent,un> monsterCollisionContentDuplicate;
};

#endif // BOTTARGETLIST_H