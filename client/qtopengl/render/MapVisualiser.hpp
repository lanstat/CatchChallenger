#ifndef MAP_VISUALISER_H
#define MAP_VISUALISER_H

#include "MapItem.hpp"
#include "MapObjectItem.hpp"
#include "ObjectGroupItem.hpp"
#include "MapMark.hpp"

#include "../../../general/base/GeneralStructures.hpp"
#include "../../../general/base/CommonMap.hpp"
#include "../Map_client.hpp"
#include "../DisplayStructures.hpp"
#include "../../../general/base/Map_loader.hpp"

#include <QGraphicsSimpleTextItem>
#include <QTimer>
#include <QList>
#include <QKeyEvent>
#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QSet>
#include <QString>
#include <QMultiMap>
#include <QHash>
#include <QTime>
#include <QDateTime>

#include "MapVisualiserThread.hpp"

class MapVisualiser : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit MapVisualiser(const bool &debugTags=false);
    ~MapVisualiser();

    virtual void eventOnMap(CatchChallenger::MapEvent event,Map_full * tempMapObject,uint8_t x,uint8_t y);
    QRectF boundingRect() const;

    Map_full * getMap(const std::string &map) const;

    std::string current_map;
    std::unordered_map<std::string,Map_full *> all_map,old_all_map;
    std::unordered_map<std::string,QDateTime> old_all_map_time;
    MapItem* mapItem;
protected:
    Tiled::MapReader reader;

    Tiled::Tileset * markPathFinding;
    int tagTilesetIndex;

    bool debugTags;
    std::string mLastError;

    Tiled::Layer *grass;
    Tiled::Layer *grassOver;
    MapMark *mark;

    MapVisualiserThread mapVisualiserThread;
    std::vector<std::string> asyncMap;
    std::unordered_map<uint16_t/*intervale*/,QTimer *> animationTimer;

    virtual void destroyMap(Map_full *map);
protected slots:
    virtual void resetAll();
public slots:
    void loadOtherMap(const std::string &resolvedFileName);
    virtual void loadBotOnTheMap(Map_full *parsedMap, const uint32_t &botId, const uint8_t &x, const uint8_t &y,
                                 const std::string &lookAt, const std::string &skin);
    //virtual std::unordered_set<QString> loadMap(Map_full *map, const bool &display);
    virtual void removeUnusedMap();
    virtual void hideNotloadedMap();
private slots:
    void loadTeleporter(Map_full *map);
    void asyncDetectBorder(Map_full * tempMapObject);
    void applyTheAnimationTimer();
protected slots:
    virtual bool asyncMapLoaded(const std::string &fileName,Map_full * tempMapObject);
    void passMapIntoOld();
signals:
    void loadOtherMapAsync(const std::string &fileName);
    void mapDisplayed(const std::string &fileName);
};

#endif
