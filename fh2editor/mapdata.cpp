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
#include <QGraphicsSceneMouseEvent>

#include "engine.h"
#include "program.h"
#include "mapdata.h"

bool MapTile::isValid(void) const
{
    return true;
}

MapTile::MapTile(const QPoint & inpos, const mp2til_t & mp2, AGG::File & agg, const QPoint & offset)
    : pos(inpos), sprite(mp2.tileSprite), shape(mp2.tileShape)
{
    QPixmap pixmap = agg.getImageTIL("GROUND32", sprite);

    area.setTopLeft(offset);
    area.setSize(pixmap.size());

    setFlags(QGraphicsItem::ItemIsSelectable);

    switch(shape % 4)
    {
	case 1: pixmapTile = pixmap.transformed(QTransform().scale( 1, -1)); break;
	case 2: pixmapTile = pixmap.transformed(QTransform().scale(-1,  1)); break;
	case 3: pixmapTile = pixmap.transformed(QTransform().scale(-1, -1)); break;
	default: pixmapTile= pixmap; break;
    }
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

QRectF MapTile::boundingRect(void) const
{
    return area;
}

void MapTile::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if(painter)
	painter->drawPixmap(area.x(), area.y(), pixmapTile);
}

void MapTile::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(event)
    {
	QString str, msg;
	QTextStream ss1(& str), ss2(& msg);

	ss1 << "Tile(" << pos.x() << "," << pos.y() << ")";
	ss2 << "tile sprite: " << sprite << endl << "tile shape: " << shape << endl;

	QMessageBox::information(NULL, str, msg);
    }

    QGraphicsItem::mousePressEvent(event);
}

void MapTile::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseMoveEvent(event);
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
    return loadMP2Map(mapFile);
}

bool MapData::loadMP2Map(const QString & mapFile)
{
    H2::File map(mapFile);

    tilesetSize.setWidth(32);
    tilesetSize.setHeight(32);

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

	setSceneRect(QRect(QPoint(0, 0),
		QSize(mapSize.width() * tilesetSize.width(), mapSize.height() * tilesetSize.height())));

	// data map: mp2tile, part1
	// count blocks: width * heigth
	QVector<mp2til_t> tilBlocks(mapSize.width() * mapSize.height());

	for(QVector<mp2til_t>::iterator
	    it = tilBlocks.begin(); it != tilBlocks.end(); ++it)
	    (*it) = map.readMP2Til();

	// data map: mp2ext, part2
	// count blocks: 4 byte
	QVector<mp2ext_t> extBlocks(map.readLE32());

	for(QVector<mp2ext_t>::iterator
	    it = extBlocks.begin(); it != extBlocks.end(); ++it)
	    (*it) = map.readMP2Ext();

	// cood castles
	QVector<mp2pos_t> townPosBlocks;
	townPosBlocks.reserve(72);

	// 72 x 3 byte (px, py, id)
	for(int ii = 0; ii < 72; ++ii)
	{
	    mp2pos_t twn;

	    twn.posx = map.readByte();
	    twn.posy = map.readByte();
	    twn.type = map.readByte();

	    if(0xFF != twn.posx && 0xFF != twn.posy)
		townPosBlocks.push_back(twn);
	}

	// cood resource kingdoms
	QVector<mp2pos_t> resourcePosBlocks;
	resourcePosBlocks.reserve(144);

	// 144 x 3 byte (px, py, id)
	for(int ii = 0; ii < 144; ++ii)
	{
	    mp2pos_t res;

	    res.posx = map.readByte();
	    res.posy = map.readByte();
	    res.type = map.readByte();

	    if(0xFF != res.posx && 0xFF != res.posy)
		resourcePosBlocks.push_back(res);
	}

	// byte: numObelisks
	map.readByte();

	// find count latest blocks: unknown byte ?? ?? ?? LO HI 00 00
	int blocksCount = 0;

	while(1)
	{
    	    quint8 lo = map.readByte();
    	    quint8 hi = map.readByte();

    	    if(0 == hi && 0 == lo)
		break;
    	    else
    		blocksCount = 256 * hi + lo - 1;
	}

	// read latest blocks
	for(int ii = 0; ii < blocksCount; ++ii)
	{
	    QByteArray block = map.readBlock(map.readLE16());

	    // parse block
	}

	mapUniq = map.readLE32();

	map.close();

	// load tiles
	for(int yy = 0; yy < mapSize.height(); ++yy)
	{
	    for(int xx = 0; xx < mapSize.width(); ++xx)
	    {
		const mp2til_t & mp2 = tilBlocks[xx + yy * mapSize.width()];

		tilesetItems.push_back(new MapTile(QPoint(xx, yy), mp2, aggContent, QPoint(xx * tilesetSize.width(), yy * tilesetSize.height())));

		quint16 ext = mp2.indexExt;

		while(ext)
		{
		    if(ext >= extBlocks.size())
		    {
			qDebug() << "ext block: out of range" << ext;
    			map.close();
    			return false;
		    }

		    //loadLevel1(extBlocks[ext]);
		    //loadLevel2(extBlocks[ext]);

		    ext = extBlocks[ext].indexExt;
		}

		addItem(tilesetItems.back());
	    }
	}

	//
	mapAuthors = "unknown";
	mapLicense = "unknown";

	return true;
    }

    return false;
}

void MapData::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mousePressEvent(event);
}

void MapData::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseMoveEvent(event);
}
