#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include "../../client/qt/render/MapVisualiserPlayer.h"
#include "../../client/qt/render/MapVisualiserOrder.h"
#include "../../client/qt/render/PathFinding.h"

class MapControllerV : public MapVisualiserPlayer
{
    Q_OBJECT
public:
    explicit MapControllerV(const bool &centerOnPlayer=true, const bool &debugTags=false, const bool &useCache=true, const bool &openGL=true);
    ~MapControllerV();

    void setScale(int scaleSize);
    void setBotNumber(quint16 botNumber);

    bool viewMap(const QString &fileName);
    bool asyncMapLoaded(const std::string &fileName, Map_full * tempMapObject) override;
private:
    struct Bot
    {
        quint8 x,y;
        Tiled::MapObject * mapObject;
        QString map;

        int moveStep;
        CatchChallenger::Direction direction;
        bool inMove;
    };
    QList<Bot> botList;
    struct BotSpawnPoint
    {
        Map_full * map;
        quint8 x,y;
    };
    QList<BotSpawnPoint> botSpawnPointList;
    quint16 botNumber;
    Tiled::Tileset * botTileset;

    PathFinding pathFinding;
    std::vector<PathResolved> pathList;
    QTimer timerBotMove;
    QTimer timerBotManagement;
    bool nextPathStep();
private slots:
    void botMove();
    void botManagement();
    bool botMoveStepSlot(Bot *bot);
    CatchChallenger::Direction moveFromPath();
    void eventOnMap(CatchChallenger::MapEvent event,Map_full * tempMapObject,uint8_t x,uint8_t y);
    void pathFindingResult(const std::string &current_map,const uint8_t &x,const uint8_t &y,const std::vector<std::pair<CatchChallenger::Orientation,uint8_t> > &path);
    void keyPressParse();

    //call after enter on new map
    void loadPlayerFromCurrentMap();
    //call before leave the old map (and before loadPlayerFromCurrentMap())
    void unloadPlayerFromCurrentMap();
};

#endif // MAPCONTROLLER_H
