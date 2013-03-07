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
#include <QStyleOptionGraphicsItem>

#include "engine.h"
#include "program.h"
#include "mapdata.h"

bool MapTileExt::sortLevel1(const MapTileExt* mte1, const MapTileExt* mte2)
{
    return (mte1->level % 4) > (mte2->level % 4);
}

bool MapTileExt::sortLevel2(const MapTileExt* mte1, const MapTileExt* mte2)
{
    return (mte1->level % 4) < (mte2->level % 4);
}

bool MapTile::isValid(void) const
{
    return true;
}

MapTile::MapTile(const mp2til_t & mp2, AGG::File & agg, const QPoint & offset)
    : spriteIndex(mp2.tileSprite), shape(mp2.tileShape)
{

    setOffset(offset);
    setFlags(QGraphicsItem::ItemIsSelectable);

    QPixmap sprite = agg.getImageTIL("GROUND32", spriteIndex);

    switch(shape % 4)
    {
	case 1: setPixmap(sprite.transformed(QTransform().scale( 1, -1))); break;
	case 2: setPixmap(sprite.transformed(QTransform().scale(-1,  1))); break;
	case 3: setPixmap(sprite.transformed(QTransform().scale(-1, -1))); break;
	default: setPixmap(sprite); break;
    }

    // level1
    loadSpritelevel(spritesLevel1, mp2.level1, agg);

    // level2
    loadSpritelevel(spritesLevel2, mp2.level2, agg);
}

MapTile::~MapTile()
{
    qDeleteAll(spritesLevel1.begin(), spritesLevel1.end());
    qDeleteAll(spritesLevel2.begin(), spritesLevel2.end());
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

void MapTile::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    // draw tile
    painter->drawPixmap(offset(), pixmap());

    // draw level1
    for(QList<MapTileExt*>::const_iterator
	it = spritesLevel1.begin(); it != spritesLevel1.end(); ++it)
	painter->drawPixmap(offset() + (*it)->offset, (*it)->pixmap);

    // draw level2
    for(QList<MapTileExt*>::const_iterator
	it = spritesLevel2.begin(); it != spritesLevel2.end(); ++it)
	painter->drawPixmap(offset() + (*it)->offset, (*it)->pixmap);

    if(option->state & QStyle::State_Selected)
    {
	QPixmap selPixmap(pixmap().size());
	selPixmap.fill(QColor(40, 40, 100, 150));

        QPainter paint(& selPixmap);
	painter->drawPixmap(offset(), selPixmap);
    }
}

void MapTile::showInfo(void) const
{
    QString str, msg;
    QTextStream ss1(& str), ss2(& msg);

    ss1 << "Tile";
    ss2 << "tile sprite: " << spriteIndex << endl << "tile shape: " << shape << endl;

    QMessageBox::information(NULL, str, msg);
}

void MapTile::loadSpritelevel(QList<MapTileExt*> & list, const mp2lev_t & level, AGG::File & agg)
{
    if(level.object && level.index < 0xFF)
    {
	const QString & icn = H2::mapICN(level.object);

	if(! icn.isEmpty())
	{
	    list << new MapTileExt(agg.getImageICN(icn, level.index));

	    if(! list.back() || list.back()->pixmap.isNull())
		qWarning() << "MapTile::loadSpriteLevel: pixmap is null" << level.object << level.index;
	}
    }
}

void MapTile::loadSpriteLevels(const mp2ext_t & mp2, AGG::File & agg)
{
    // level1
    loadSpritelevel(spritesLevel1, mp2.level1, agg);

    // level2
    loadSpritelevel(spritesLevel2, mp2.level2, agg);
}

void MapTile::sortSpritesLevels(void)
{
    qSort(spritesLevel1.begin(), spritesLevel1.end(), MapTileExt::sortLevel1);
    qSort(spritesLevel2.begin(), spritesLevel2.end(), MapTileExt::sortLevel2);
}

const QString & MapData::name(void) const
{
    return mapName;
}

const QString & MapData::description(void) const
{
    return mapDescription;
}

const QSize & MapData::size(void) const
{
    return mapSize;
}

int MapData::indexLimit(void) const
{
    return mapSize.width() * mapSize.height();
}

void MapData::newMap(const QSize & msz, const QString &)
{
    tilesetSize = QSize(32, 32);

    mapDifficulty = 1;

    mapSize = QSize(msz);
    mapStartWithHero = false;

    mapName = "New Map";

    for(int ii = 0; ii < 6; ++ii)
    {
	mapKingdomColor[ii] = 0;
	mapHumanAllow[ii] = 0;
	mapCompAllow[ii] = 0;
	mapRaceColor[ii] = 0;
    }

/*
    quint8              mapConditionWins;
    quint8              mapConditionWinsData1;
    quint8              mapConditionWinsData2;
    quint16             mapConditionWinsData3;
    quint16             mapConditionWinsData4;
    quint8              mapConditionLoss;
    quint16             mapConditionLossData1;
    quint16             mapConditionLossData2;
*/

    mapUniq = 1;

    mapAuthors = "unknown";
    mapLicense = "unknown";

    // fill tiles
    for(int yy = 0; yy < mapSize.height(); ++yy)
    {
	for(int xx = 0; xx < mapSize.width(); ++xx)
        {
    	    tilesetItems.push_back(new MapTile(mp2til_t(), aggContent, QPoint(xx * tilesetSize.width(), yy * tilesetSize.height())));
	    addItem(tilesetItems.back());
	}
    }
}

bool MapData::loadMap(const QString & mapFile)
{
    return loadMP2Map(mapFile);

}

bool MapData::loadMP2Map(const QString & mapFile)
{
    H2::File map(mapFile);

    tilesetSize = QSize(32, 32);

    if(map.open(QIODevice::ReadOnly))
    {
	// 4 byte: orig ver
	if(map.readLE32() != 0x0000005C)
	{
	    qDebug() << "Incorrect map file: " << mapFile;
    	    map.close();
    	    return false;
	}

	// difficulty: 0: easy, 1: normal, 2: hard, 3: expert
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
	if(map.readLE32() != mapSize.width())
	    qDebug() << "MapData::loadMP2Map:" << "incorrect size";
	if(map.readLE32() != mapSize.height())
	    qDebug() << "MapData::loadMP2Map:" << "incorrect size";

	setSceneRect(QRect(QPoint(0, 0),
		QSize(mapSize.width() * tilesetSize.width(), mapSize.height() * tilesetSize.height())));

	// data map: mp2tile, part1
	// count blocks: width * heigth
	QVector<mp2til_t> tilBlocks(indexLimit());

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

		tilesetItems.push_back(new MapTile(mp2, aggContent, QPoint(xx * tilesetSize.width(), yy * tilesetSize.height())));

		quint16 ext = mp2.indexExt;

		while(ext)
		{
		    if(ext >= extBlocks.size())
		    {
			qDebug() << "ext block: out of range" << ext;
    			map.close();
    			return false;
		    }

		    tilesetItems.back()->loadSpriteLevels(extBlocks[ext], aggContent);

		    ext = extBlocks[ext].indexExt;
		}

		tilesetItems.back()->sortSpritesLevels();
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
    // explore mode
    if(1 == modeView)
    {
	// show tile info
	if(event->buttons() & Qt::LeftButton)
	{
	    MapTile* item = qgraphicsitem_cast<MapTile*>(itemAt(event->scenePos()));
	    if(item) item->showInfo();
	}
    }
    else
    // select mode
    if(2 == modeView)
    {
	clearSelection();
    }

    //QGraphicsScene::mousePressEvent(event);
}

void MapData::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseReleaseEvent(event);
}

void MapData::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // select mode
    if(2 == modeView)
    {
	// select area
	if(event->buttons() & Qt::LeftButton)
	{
	    clearSelection();
	    selectArea(event->buttonDownScenePos(Qt::LeftButton), event->scenePos());
	}
    }

    QGraphicsScene::mouseMoveEvent(event);
}

int MapData::sceneModeView(void) const
{
    return modeView;
}

void MapData::setSceneModeView(int mode)
{
    modeView = mode;
}

int MapData::sceneCurrentGround(void) const
{
    return currentGround;
}

void MapData::setSceneCurrentGround(int ground)
{
    currentGround = ground;
}

void MapData::selectArea(QPointF ptdn, QPointF ptup)
{
    if(ptup.x() < ptdn.x())
	qSwap(ptup.rx(), ptdn.rx());

    if(ptup.y() < ptdn.y())
	qSwap(ptup.ry(), ptdn.ry());

    QList<QGraphicsItem*> list = items(QRectF(ptdn, ptup), Qt::IntersectsItemShape, Qt::AscendingOrder);

    if(list.isEmpty() && ptdn == ptup)
    {
	QGraphicsItem* item = itemAt(ptup);
	if(item) list << item;
    }

    for(QList<QGraphicsItem*>::iterator
	it = list.begin(); it != list.end(); ++it)
	(*it)->setSelected(true);
}
