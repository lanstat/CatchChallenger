#include "LoadMapAll.h"

#include "../map-procedural-generation-terrain/LoadMap.h"
#include "../map-procedural-generation-terrain/MapBrush.h"

#include <iostream>

void LoadMapAll::addCityContent(Tiled::Map &worldMap, const unsigned int &mapXCount, const unsigned int &mapYCount)
{
    if(MapBrush::mapMask==NULL)
    {
        std::cerr << "MapBrush::mapMask==NULL (abort) into LoadMapAll::addCityContent" << std::endl;
        abort();
    }
    const unsigned int mapWidth=worldMap.width()/mapXCount;
    const unsigned int mapHeight=worldMap.height()/mapYCount;
    const Tiled::Map *map=LoadMap::readMap("template/city.tmx");
    if((unsigned int)map->width()>mapWidth)
    {
        std::cout << "map->width()>mapWitdh for city" << std::endl;
        abort();
    }
    if((unsigned int)map->height()>mapHeight)
    {
        std::cout << "map->height()>mapHeight for city" << std::endl;
        abort();
    }
    MapBrush::MapTemplate mapTemplate=MapBrush::tiledMapToMapTemplate(map,worldMap);
    unsigned int index=0;
    while(index<cities.size())
    {
        const City &city=cities.at(index);

        const uint32_t x=city.x;
        const uint32_t y=city.y;
        const uint8_t xoffset=(mapWidth-map->width())/2;
        const uint8_t yoffset=(mapHeight-map->height())/2;
        MapBrush::brushTheMap(worldMap,mapTemplate,x*mapWidth+xoffset,y*mapHeight+yoffset,MapBrush::mapMask);
        index++;
    }
}