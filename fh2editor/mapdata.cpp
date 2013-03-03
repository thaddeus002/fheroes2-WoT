/***************************************************************************
 *   Copyright (C) 2013 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QPainter>
#include <QDomDocument>
#include <QDebug>
#include <QMessageBox>

#include "engine.h"
#include "program.h"
#include "mapdata.h"

bool MapTile::isValid(void) const
{
    return true;
}

MapTile::MapTile() : index(-1), sprite(-1), shape(-1)
{
}

MapTile::MapTile(int p1, int p2, int p3) : index(p1), sprite(p2), shape(p3)
{
}

QString MapTile::indexString(int index)
{
    QString str;

    QTextStream out(& str);
    out.setFieldWidth(3);
    out.setPadChar('0');

    out << index;

    return str;
}

QGraphicsPixmapItem* MapTile::loadItem(QPixmap pixmap, const QSize & size, const QPoint & offset)
{
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem();
    item->setOffset(offset);

    if(!pixmap.isNull() && pixmap.size() != size)
    {
	QPixmap temp = pixmap.scaled(size);
	qSwap(temp, pixmap);
    }

    item->setFlags(QGraphicsItem::ItemIsSelectable);

    switch(shape % 4)
    {
	case 1: item->setPixmap(pixmap.transformed(QTransform().scale( 1, -1))); break;
	case 2: item->setPixmap(pixmap.transformed(QTransform().scale(-1,  1))); break;
	case 3: item->setPixmap(pixmap.transformed(QTransform().scale(-1, -1))); break;
	default: item->setPixmap(pixmap); break;
    }

    return item;
}

MapData::MapData() : tilesetItemsGroup(NULL)
{
}

MapData::~MapData()
{
}

const QString & MapData::Name(void) const
{
    return mapName;
}

const QString & MapData::Description(void) const
{
    return mapDescription;
}

const QSize & MapData::Size(void) const
{
    return mapSize;
}

int MapData::indexLimit(void) const
{
    return mapSize.width() * mapSize.height();
}

bool MapData::loadMap(const QString & mapFile)
{
    mapSize.setWidth(0);
    mapSize.setHeight(0);

    mapName.clear();
    mapDescription.clear();
    mapAuthors.clear();
    mapLicense.clear();

    tilesContent.clear();
    tilesetSize.setWidth(0);
    tilesetSize.setHeight(0);

    if(tilesetItemsGroup)
    {
	removeItem(tilesetItemsGroup);
	destroyItemGroup(tilesetItemsGroup);
	tilesetItemsGroup = NULL;
    }

    if(tilesetItems.size())
    {
	for(QList<QGraphicsItem*>::iterator
		it = tilesetItems.begin(); it != tilesetItems.end(); ++it)
	    delete *it;

	tilesetItems.clear();
    }

    if(! loadMP2Map(mapFile))
	return false;

    // load scene
    setSceneRect(QRect(QPoint(0, 0),
		    QSize(mapSize.width() * tilesetSize.width(), mapSize.height() * tilesetSize.height())));

    for(int yy = 0; yy < mapSize.height(); ++yy)
    {
	for(int xx = 0; xx < mapSize.width(); ++xx)
	{
	    const QPoint offset(xx * tilesetSize.width(), yy * tilesetSize.height());
	    const int index = yy * mapSize.height() + xx;

	    MapTile & tile = tilesContent[index];

	    tilesetItems.push_back(
		tile.loadItem(aggContent.getImageTIL("GROUND32", tile.sprite), tilesetSize, offset));
	}
    }

    tilesetItemsGroup = createItemGroup(tilesetItems);
    addItem(tilesetItemsGroup);

    return true;
}

bool MapData::loadMP2Map(const QString & mapFile)
{
    H2::File map(mapFile);

    if(map.open(QIODevice::ReadOnly))
    {
	// 4 byte: orig ver
	if(map.readLE32() != 0x0000005C)
	{
	    qDebug() << "Incorrect map file: " << mapFile;
    	    map.close();
    	    return false;
	}

	// difficulty
	mapDifficulty = map.readLE16();

	// width, height
	mapSize.setWidth(map.readByte());
	mapSize.setHeight(map.readByte());

	// kingdom color: blue, gree, red, yellow, orange, purple
	for(int ii = 0; ii < 6; ++ii)
	    mapKingdomColor[ii] = map.readByte();

	// allow human blue
	for(int ii = 0; ii < 6; ++ii)
	    mapKingdomColor[ii] = map.readByte();

	// allow human: blue, gree, red, yellow, orange, purple
	for(int ii = 0; ii < 6; ++ii)
	    mapHumanAllow[ii] = map.readByte();

	// allow comp: blue, gree, red, yellow, orange, purple
	for(int ii = 0; ii < 6; ++ii)
	    mapCompAllow[ii] = map.readByte();

	// wins
	map.seek(0x1D);
	mapConditionWins = map.readByte();

	// data wins
	mapConditionWinsData1 = map.readByte();
	mapConditionWinsData2 = map.readByte();
	mapConditionWinsData3 = map.readLE16();
	map.seek(0x2C);
	mapConditionWinsData4 = map.readLE16();

	// loss
	map.seek(0x22);
	mapConditionLoss = map.readByte();
	// data loss
	mapConditionLossData1 = map.readLE16();
	map.seek(0x2E);
	mapConditionLossData2 = map.readLE16();

	// start with hero
	map.seek(0x25);
	mapStartWithHero = (0 == map.readByte());

	// race color
	for(int ii = 0; ii < 6; ++ii)
	    mapRaceColor[ii] = map.readByte();

	// name
	map.seek(0x3A);
	mapName = map.readString(16);

	// description
	map.seek(0x76);
	mapDescription = map.readString(143);

	// data map: width, heigth
	map.seek(0x01A4);
	map.readLE32();
	map.readLE32();

	tilesContent.reserve(indexLimit());

	// data map: tiles, part1
	for(int ii = 0; ii < indexLimit(); ++ii)
	{
	    int tileSprite = map.readLE16();
	    int objectName1 = map.readByte();
	    int indexName1 = map.readByte();
	    int quantity1 = map.readByte();
	    int quantity2 = map.readByte();
	    int objectName2 = map.readByte();
	    int indexName2 = map.readByte();
	    int tileShape = map.readByte();
	    int tileObject = map.readByte();

	    int offsetPart2 = map.readLE16();
	    int uniq1 = map.readLE32();
	    int uniq2 = map.readLE32();

	    tilesContent.push_back(MapTile(ii, tileSprite, tileShape));

	    //qDebug() << "tile index=" << ii << "sprite=" << tileSprite << "shape=" << tileShape;
	}

	map.close();

	//
	mapAuthors = "unknown";
	mapLicense = "unknown";
	//
	tilesetSize.setWidth(32);
	tilesetSize.setHeight(32);

	return true;
    }

    return false;
}
