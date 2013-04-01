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

#include <algorithm>
#include <QtGui>
#include <QPainter>
#include <QDomDocument>
#include <QDebug>
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

#include "engine.h"
#include "program.h"
#include "mainwindow.h"
#include "mapwindow.h"
#include "mapdata.h"

MapTileExt::MapTileExt(int lvl, const mp2lev_t & ext, const QPair<QPixmap, QPoint> & pair)
    : QPair<QPixmap, QPoint>(pair.first, pair.second), spriteICN(H2::mapICN(ext.object)), spriteIndex(ext.index), level(lvl), tmp(0), spriteUID(ext.uniq)
{
}

bool MapTileExt::sortLevel1(const MapTileExt* mte1, const MapTileExt* mte2)
{
    return (mte1->level % 4) > (mte2->level % 4) ||
	((mte1->level % 4) == (mte2->level % 4) && mte1->level < mte2->level);
}

bool MapTileExt::sortLevel2(const MapTileExt* mte1, const MapTileExt* mte2)
{
    return (mte1->level % 4) < (mte2->level % 4);
}

bool MapTileExt::isTown(const MapTileExt* te)
{
    return ICN::OBJNTOWN == te->spriteICN ? true : isRandomTown(te);
}

bool MapTileExt::isRandomTown(const MapTileExt* te)
{
    return ICN::OBJNTWRD == te->spriteICN && 32 > te->spriteIndex;
}

bool MapTileExt::isMiniHero(const MapTileExt* te)
{
    return ICN::MINIHERO == te->spriteICN;
}

bool MapTileExt::isSign(const MapTileExt* te)
{
    return (ICN::OBJNMUL2 == te->spriteICN && 114 == te->spriteIndex) ||
	    (ICN::OBJNDSRT == te->spriteICN && 119 == te->spriteIndex) ||
	    (ICN::OBJNLAVA == te->spriteICN && 117 == te->spriteIndex) ||
	    (ICN::OBJNSNOW == te->spriteICN && 143 == te->spriteIndex) ||
	    (ICN::OBJNSWMP == te->spriteICN && 140 == te->spriteIndex);
}

bool MapTileExt::isButtle(const MapTileExt* te)
{
    return ICN::OBJNWATR == te->spriteICN && 0 == te->spriteIndex;
}

bool MapTileExt::isSphinx(const MapTileExt* te)
{
    return ICN::OBJNDSRT == te->spriteICN && (85 <= te->spriteIndex || 88 <= te->spriteIndex);
}

bool MapTileExt::isMapEvent(const MapTileExt* te)
{
    return ICN::OBJNMUL2 == te->spriteICN && 163 == te->spriteIndex;
}

MapTile::MapTile(const mp2til_t & mp2, const QPoint & pos, Editor::Theme & theme)
    : themeContent(theme), til(mp2), mpos(pos), passableBase(0), passableLocal(0xFFFF)
{
    QPoint offset(mpos.x() * theme.tileSize().width(), mpos.y() * theme.tileSize().height());
    setOffset(offset);
    setFlags(QGraphicsItem::ItemIsSelectable);
    setTileSprite(mp2.tileSprite, mp2.tileShape);
    loadSpriteLevel(spritesLevel1, 0, mp2.level1);
    loadSpriteLevel(spritesLevel2, 0, mp2.level2);
}

void MapTile::setTileSprite(int index, int rotate)
{
    til.tileSprite = index;
    til.tileShape = rotate;

    QPixmap sprite = themeContent.getImageTIL("GROUND32.TIL", til.tileSprite);

    switch(til.tileShape % 4)
    {
	case 1: setPixmap(sprite.transformed(QTransform().scale( 1, -1))); break;
	case 2: setPixmap(sprite.transformed(QTransform().scale(-1,  1))); break;
	case 3: setPixmap(sprite.transformed(QTransform().scale(-1, -1))); break;
	default: setPixmap(sprite); break;
    }
}

QRectF MapTile::boundingRect(void) const
{
    return QRectF(offset(), pixmap().size());
}

MapTile::~MapTile()
{
    qDeleteAll(spritesLevel1.begin(), spritesLevel1.end());
    qDeleteAll(spritesLevel2.begin(), spritesLevel2.end());
}

int MapTile::groundType(void) const
{
    return themeContent.ground(til.tileSprite);
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
    Q_UNUSED(option);

    // draw tile
    painter->drawPixmap(offset(), pixmap());

    // draw level1
    for(QList<MapTileExt*>::const_iterator
	it = spritesLevel1.begin(); it != spritesLevel1.end(); ++it)
    {
	painter->drawPixmap(offset() + (*it)->offset(), (*it)->pixmap());

	if(ICN::UNKNOWN != (*it)->spriteICN)
	{
	    int anim = H2::isAnimationICN((*it)->spriteICN, (*it)->spriteIndex, 0);

	    if(0 < anim)
	    {
		QPair<QPixmap, QPoint> p = themeContent.getImageICN(H2::icnString((*it)->spriteICN), anim);
		painter->drawPixmap(offset() + p.second, p.first);
	    }
	}
    }

    // draw level2
    for(QList<MapTileExt*>::const_iterator
	it = spritesLevel2.begin(); it != spritesLevel2.end(); ++it)
    {
	painter->drawPixmap(offset() + (*it)->offset(), (*it)->pixmap());

	if(ICN::UNKNOWN != (*it)->spriteICN)
	{
	    int anim = H2::isAnimationICN((*it)->spriteICN, (*it)->spriteIndex, 0);

	    if(0 < anim)
	    {
		QPair<QPixmap, QPoint> p = themeContent.getImageICN(H2::icnString((*it)->spriteICN), anim);
		painter->drawPixmap(offset() + p.second, p.first);
	    }
	}
    }
}

void MapTile::showInfo(void) const
{
    QString str, msg;
    QTextStream ss1(& str), ss2(& msg);

    ss1 << "Tile" << "(" << mpos.x() << "," << mpos.y() << ")";
    ss2 << "tile sprite: " << til.tileSprite << endl \
	<< "tile rotate:  " << til.tileShape << endl;

    ss2 << "----------------------" << endl;

    // draw level2
    if(spritesLevel1.size())
    {
	for(QList<MapTileExt*>::const_iterator
	    it = spritesLevel1.begin(); it != spritesLevel1.end(); ++it)
	{
	    ss2 <<
		"uniq:   " << (*it)->spriteUID << endl <<
		"object: " << H2::icnString((*it)->spriteICN) << endl <<
		"index:  " << (*it)->spriteIndex << endl <<
		"level:  " << (*it)->level << endl <<
		"----------------------" << endl;
	}

    }

    // draw level2
    if(spritesLevel2.size())
    {
	for(QList<MapTileExt*>::const_iterator
	    it = spritesLevel2.begin(); it != spritesLevel2.end(); ++it)
	{
	    ss2 <<
		"uniq:   " << (*it)->spriteUID << endl <<
		"object: " << H2::icnString((*it)->spriteICN) << endl <<
		"index:  " << (*it)->spriteIndex << endl <<
		"level:  " << (*it)->level << endl <<
		"----------------------" << endl;
	}
    }

    QMessageBox::information(NULL, str, msg);
}

void MapTile::loadSpriteLevel(QList<MapTileExt*> & list, int level, const mp2lev_t & ext)
{
    if(ext.object && ext.index < 0xFF)
    {
	const QString & icn = H2::icnString(H2::mapICN(ext.object));

	if(! icn.isEmpty())
	{
	    list << new MapTileExt(level, ext, themeContent.getImageICN(icn, ext.index));

	    if(! list.back() || list.back()->pixmap().isNull())
		qWarning() << "MapTile::loadSpriteLevel: pixmap is null" << ext.object << ext.index;
	}
    }
}

void MapTile::loadSpriteLevels(const mp2ext_t & mp2)
{
    // level1
    loadSpriteLevel(spritesLevel1, mp2.quantity, mp2.level1);

    // level2
    loadSpriteLevel(spritesLevel2, mp2.quantity, mp2.level2);
}

void MapTile::sortSpritesLevels(void)
{
    qSort(spritesLevel1.begin(), spritesLevel1.end(), MapTileExt::sortLevel1);
    qSort(spritesLevel2.begin(), spritesLevel2.end(), MapTileExt::sortLevel2);
}

MapData::MapData(MapWindow* parent) : QGraphicsScene(parent), themeContent(parent->mainWindow->aggContent), tileOverMouse(NULL)
{
}

Editor::Theme & MapData::theme(void)
{
    return themeContent;
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

quint32 MapData::uniq(void)
{
    return mapUniq++;
}

int MapData::indexLimit(void) const
{
    return size().width() * size().height();
}

void MapData::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // clear selected
    if(selectedItems().size())
    {
	if((event->buttons() & Qt::LeftButton) ||
	    ((event->buttons() & Qt::RightButton) && ! selectionArea().contains(event->scenePos())))
	    clearSelection();
    }

    event->accept();
}

void MapData::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // select area
    if(event->buttons() & Qt::LeftButton)
    {
	if(selectedItems().size())
	    clearSelection();

	selectArea(event->buttonDownScenePos(Qt::LeftButton), event->scenePos());
    }

    // update select item over cursor
    if(tileOverMouse)
	update(tileOverMouse->boundingRect());

    tileOverMouse = qgraphicsitem_cast<MapTile*>(itemAt(event->scenePos()));

    if(tileOverMouse)
	update(tileOverMouse->boundingRect());

    event->accept();
}

void MapData::selectArea(QPointF ptdn, QPointF ptup)
{
    if(ptup != ptdn)
    {
	if(ptup.x() < ptdn.x())
	    qSwap(ptup.rx(), ptdn.rx());

	if(ptup.y() < ptdn.y())
	    qSwap(ptup.ry(), ptdn.ry());

	QRect selRect = QRectF(ptdn, ptup).toRect();

	const QSize & tileSize = themeContent.tileSize();

	int sl = selRect.left() / tileSize.width();
	int st = selRect.top() / tileSize.height();
	int sr = selRect.right() / tileSize.width();
	int sb = selRect.bottom() / tileSize.height();

	if(selRect.left() > sl * tileSize.width())
	    selRect.setLeft(sl * tileSize.width());

	if(selRect.top() > st * tileSize.height())
	    selRect.setTop(st * tileSize.height());

	if(selRect.right() > sr * tileSize.width())
	    selRect.setRight((sr + 1) * tileSize.width());

	if(selRect.bottom() > sb * tileSize.height())
	    selRect.setBottom((sb + 1) * tileSize.height());

	QPainterPath path;
	path.addRect(selRect);

	setSelectionArea(path);
    }
}

void MapData::selectAllTiles(void)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    const QSize & sz = themeContent.tileSize();
    selectArea(QPointF(0, 0), QPointF(sz.width() * size().width(), sz.height() * size().height()));
    QApplication::restoreOverrideCursor();
}

void MapData::drawForeground(QPainter* painter, const QRectF & rect)
{
    Q_UNUSED(rect);

    // paint: selected area
    if(selectedItems().size())
    {
	painter->setPen(QPen(QColor(40, 40, 100), 1));
	painter->setBrush(QBrush(QColor(40, 40, 100, 150), Qt::Dense4Pattern));
	painter->drawRoundedRect(selectionArea().boundingRect(), 6.0, 6.0);
    }
    else
    // paint: selected item over mouse
    if(tileOverMouse)
    {
	painter->setPen(QPen(QColor(255, 255, 0), 1));
	painter->setBrush(QBrush(QColor(0, 0, 0, 0)));
	const QRectF & rt = tileOverMouse->boundingRect();
	painter->drawRect(QRectF(rt.x() + 1, rt.y() + 1, rt.width() - 2, rt.height() - 2));
    }
}

void MapData::editPassableDialog(void)
{
    qDebug() << "edit passable dialog";
}

void MapData::cellInfoDialog(void)
{
    if(tileOverMouse)
	tileOverMouse->showInfo();
}

void MapData::copyToBuffer(void)
{
    QList<QGraphicsItem*> selected = selectedItems();

    if(selected.size())
    {
        qDebug() << "copy action";
        emit validBuffer(true);
    }
}

void MapData::pasteFromBuffer(void)
{
    emit dataModified();
    qDebug() << "paste action";
}

void MapData::fillGroundAction(QAction* act)
{
    if(act)
    {
	int ground = act->data().toInt();
	QList<QGraphicsItem*> selected = selectedItems();


	// fill default
	for(QList<QGraphicsItem*>::iterator
	    it = selected.begin(); it != selected.end(); ++it)
	{
	    MapTile* tile = qgraphicsitem_cast<MapTile*>(*it);

    	    if(tile)
		tile->setTileSprite(themeContent.startFilledTile(ground), 0);
	}

	// fixed border
	QSize tileSize = themeContent.tileSize();
        QRectF rectArea = selectionArea().boundingRect();

	rectArea.setTopLeft(rectArea.topLeft() - QPoint(tileSize.width() / 2, tileSize.height() / 2));
	rectArea.setBottomRight(rectArea.bottomRight() + QPoint(tileSize.width() / 2, tileSize.height() / 2));

	QList<QGraphicsItem*> listItems = items(rectArea);

	for(QList<QGraphicsItem*>::iterator
	    it = listItems.begin(); it != listItems.end(); ++it)
	{
	    MapTile* tile = qgraphicsitem_cast<MapTile*>(*it);

    	    if(tile)
	    {
		QPair<int, int> indexGroundRotate = themeContent.indexGroundRotateFix(aroundGrounds(tile), tile->groundType());

		if(0 <= indexGroundRotate.first)
            	    tile->setTileSprite(indexGroundRotate.first, indexGroundRotate.second);
	    }
	}

	emit dataModified();
    }
}

void MapData::removeObjectsAction(QAction* act)
{
    QList<QGraphicsItem*> selected = selectedItems();

    if(act)
    {
        //int type = act->data().toInt();
	QList<QGraphicsItem*> selected = selectedItems();

	for(QList<QGraphicsItem*>::iterator
	    it = selected.begin(); it != selected.end(); ++it)
	{
	    // code: remove objects
	}

	emit dataModified();
    }
}

bool MapData::isValidPoint(const QPoint & pos) const
{
    int index = pos.x() + pos.y() * size().width();
    return 0 <= index && index < mapTiles.size();
}

const MapTile* MapData::mapTileFromDirectionConst(const MapTile* center, int direct) const
{
    if(center)
    {
	QPoint directPos = center->mapPos();

	switch(direct)
	{
    	    case Direction::Top:         if(directPos.y()) directPos.setY(directPos.y() - 1); break;
    	    case Direction::Bottom:      if(directPos.y() < size().height()) directPos.setY(directPos.y() + 1); break;
    	    case Direction::Left:        if(directPos.x()) directPos.setX(directPos.x() - 1); break;
    	    case Direction::Right:       if(directPos.x() < size().width()) directPos.setX(directPos.x() + 1); break;

    	    case Direction::TopRight:    return mapTileFromDirectionConst(mapTileFromDirectionConst(center, Direction::Top), Direction::Right);
    	    case Direction::BottomRight: return mapTileFromDirectionConst(mapTileFromDirectionConst(center, Direction::Bottom), Direction::Right);
    	    case Direction::BottomLeft:  return mapTileFromDirectionConst(mapTileFromDirectionConst(center, Direction::Bottom), Direction::Left);
    	    case Direction::TopLeft:     return mapTileFromDirectionConst(mapTileFromDirectionConst(center, Direction::Top), Direction::Left);
    	    default: break;
	}

	QList<MapTile*>::const_iterator it = mapTiles.begin();
	for(; it != mapTiles.end(); ++it) if((*it)->mapPos() == directPos) break;

	return it != mapTiles.end() ? *it : NULL;
    }

    return NULL;
}

MapTile* MapData::mapTileFromDirection(const MapTile* center, int direct)
{
    return const_cast<MapTile*>(mapTileFromDirectionConst(center, direct));
}

const MapTile* MapData::mapTileConst(const QPoint & pos) const
{
    return isValidPoint(pos) ? mapTiles[pos.x() + pos.y() * size().width()] : NULL;
}

MapTile* MapData::mapTile(const QPoint & pos)
{
    return const_cast<MapTile*>(mapTileConst(pos));
}


void MapData::newMap(const QSize & msz, const QString &)
{
    mapDifficulty = 1;
    mapUniq = 1;

    mapSize = QSize(msz);

    mapName = "New Map";
    mapAuthors = "unknown";
    mapLicense = "unknown";

    // fill tiles
    for(int yy = 0; yy < size().height(); ++yy)
    {
	for(int xx = 0; xx < size().width(); ++xx)
        {
    	    mapTiles.push_back(new MapTile(mp2til_t(), QPoint(xx, yy), themeContent));
	    addItem(mapTiles.back());
	}
    }
}

bool MapData::loadMap(const QString & mapFile)
{
    const QSize & tileSize = themeContent.tileSize();

    MP2Format mp2;

    if(mp2.loadMap(mapFile))
    {
	mapSize = mp2.size;
	mapName = mp2.name;
	mapDescription = mp2.description;

	setSceneRect(QRect(QPoint(0, 0),
		QSize(size().width() * tileSize.width(), size().height() * tileSize.height())));

	// import tiles
	for(int yy = 0; yy < size().height(); ++yy)
	{
	    for(int xx = 0; xx < size().width(); ++xx)
	    {
		const mp2til_t & mp2til = mp2.tiles[xx + yy * size().width()];
		mapTiles.push_back(new MapTile(mp2til, QPoint(xx, yy), themeContent));
		int ext = mp2til.indexExt;

		while(ext)
		{
		    if(ext >= mp2.sprites.size())
		    {
			qDebug() << "ext block: out of range" << ext;
    			return false;
		    }

		    mapTiles.back()->loadSpriteLevels(mp2.sprites[ext]);
		    ext = mp2.sprites[ext].indexExt;
		}

		mapTiles.back()->sortSpritesLevels();
		addItem(mapTiles.back());
	    }

	}

	// import towns
	for(QVector<H2::TownPos>::const_iterator
	    it = mp2.castles.begin(); it != mp2.castles.end(); ++it) if(isValidPoint((*it).pos()))
	{
	    const MapTile* tile = mapTileConst((*it).pos());
	    QList<MapTileExt*>::const_iterator ext = std::find_if(tile->spritesLevel1.begin(), tile->spritesLevel1.end(), MapTileExt::isTown);
	    int uid = ext != tile->spritesLevel1.end() ? (*ext)->spriteUID : -1;
	    mapObjects[(*it).pos()] = QSharedPointer<Editor::Object>(new Editor::Town((*it).pos(), uid, (*it).town()));
	}

	// import heroes
	for(QVector<H2::HeroPos>::const_iterator
	    it = mp2.heroes.begin(); it != mp2.heroes.end(); ++it) if(isValidPoint((*it).pos()))
	{
	    const MapTile* tile = mapTileConst((*it).pos());
	    QList<MapTileExt*>::const_iterator ext = std::find_if(tile->spritesLevel1.begin(), tile->spritesLevel1.end(), MapTileExt::isMiniHero);
	    int uid = ext != tile->spritesLevel1.end() ? (*ext)->spriteUID : -1;
	    mapObjects[(*it).pos()] = QSharedPointer<Editor::Object>(new Editor::Hero((*it).pos(), uid, (*it).hero()));
	}

	// import signs
	for(QVector<H2::SignPos>::const_iterator
	    it = mp2.signs.begin(); it != mp2.signs.end(); ++it) if(isValidPoint((*it).pos()))
	{
	    const MapTile* tile = mapTileConst((*it).pos());
	    QList<MapTileExt*>::const_iterator ext = std::find_if(tile->spritesLevel1.begin(), tile->spritesLevel1.end(), MapTileExt::isSign);
	    int uid = ext != tile->spritesLevel1.end() ? (*ext)->spriteUID : -1;
	    mapObjects[(*it).pos()] = QSharedPointer<Editor::Object>(new Editor::Sign((*it).pos(), uid, (*it).sign()));
	}

	// import map events
	for(QVector<H2::EventPos>::const_iterator
	    it = mp2.mapEvents.begin(); it != mp2.mapEvents.end(); ++it) if(isValidPoint((*it).pos()))
	{
	    const MapTile* tile = mapTileConst((*it).pos());
	    QList<MapTileExt*>::const_iterator ext = std::find_if(tile->spritesLevel1.begin(), tile->spritesLevel1.end(), MapTileExt::isMapEvent);
	    int uid = ext != tile->spritesLevel1.end() ? (*ext)->spriteUID : -1;
	    mapObjects[(*it).pos()] = QSharedPointer<Editor::Object>(new Editor::MapEvent((*it).pos(), uid, (*it).event()));
	}

	// import sphinx riddles
	for(QVector<H2::SphinxPos>::const_iterator
	    it = mp2.sphinxes.begin(); it != mp2.sphinxes.end(); ++it) if(isValidPoint((*it).pos()))
	{
	    const MapTile* tile = mapTileConst((*it).pos());
	    QList<MapTileExt*>::const_iterator ext = std::find_if(tile->spritesLevel1.begin(), tile->spritesLevel1.end(), MapTileExt::isSphinx);
	    int uid = ext != tile->spritesLevel1.end() ? (*ext)->spriteUID : -1;
	    mapObjects[(*it).pos()] = QSharedPointer<Editor::Object>(new Editor::Sphinx((*it).pos(), uid, (*it).sphinx()));
	}

	// import day events
	for(QVector<mp2dayevent_t>::const_iterator
	    it = mp2.dayEvents.begin(); it != mp2.dayEvents.end(); ++it)
	{
	    dayEvents.push_back(QSharedPointer<Editor::DayEvent>(new Editor::DayEvent(*it)));
	}

	// import rumors
	for(QVector<mp2rumor_t>::const_iterator
	    it = mp2.rumors.begin(); it != mp2.rumors.end(); ++it)
	{
	    tavernRumors.push_back(QSharedPointer<Editor::Rumor>(new Editor::Rumor(*it)));
	}
	return true;
    }

    return false;
}

AroundGrounds MapData::aroundGrounds(const MapTile* center) const
{
    AroundGrounds res;

    if(center)
    {
	const MapTile* tile = NULL;

	tile = mapTileFromDirectionConst(center, Direction::TopLeft);
	if(tile) res[0] = tile->groundType();

	tile = mapTileFromDirectionConst(center, Direction::Top);
	if(tile) res[1] = tile->groundType();

	tile = mapTileFromDirectionConst(center, Direction::TopRight);
	if(tile) res[2] = tile->groundType();

	tile = mapTileFromDirectionConst(center, Direction::Right);
	if(tile) res[3] = tile->groundType();

	tile = mapTileFromDirectionConst(center, Direction::BottomRight);
	if(tile) res[4] = tile->groundType();

	tile = mapTileFromDirectionConst(center, Direction::Bottom);
	if(tile) res[5] = tile->groundType();

	tile = mapTileFromDirectionConst(center, Direction::BottomLeft);
	if(tile) res[6] = tile->groundType();

	tile = mapTileFromDirectionConst(center, Direction::Left);
	if(tile) res[7] = tile->groundType();

	res[8] = center->groundType();
    }

    return res;
}

bool MP2Format::loadMap(const QString & mapFile)
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

	// difficulty: 0: easy, 1: normal, 2: hard, 3: expert
	difficulty = map.readLE16();

	// width, height
	size.setWidth(map.readByte());
	size.setHeight(map.readByte());

	// kingdom color: blue, gree, red, yellow, orange, purple
	for(int ii = 0; ii < 6; ++ii)
	    kingdomColor[ii] = map.readByte();

	// allow human: blue, gree, red, yellow, orange, purple
	for(int ii = 0; ii < 6; ++ii)
	    humanAllow[ii] = map.readByte();

	// allow comp: blue, gree, red, yellow, orange, purple
	for(int ii = 0; ii < 6; ++ii)
	    compAllow[ii] = map.readByte();

	// wins
	map.seek(0x1D);
	conditionWins = map.readByte();

	// data wins
	conditionWinsData1 = map.readByte();
	conditionWinsData2 = map.readByte();
	conditionWinsData3 = map.readLE16();
	map.seek(0x2C);
	conditionWinsData4 = map.readLE16();

	// loss
	map.seek(0x22);
	conditionLoss = map.readByte();
	// data loss
	conditionLossData1 = map.readLE16();
	map.seek(0x2E);
	conditionLossData2 = map.readLE16();

	// start with hero
	map.seek(0x25);
	startWithHero = (0 == map.readByte());

	// race color
	for(int ii = 0; ii < 6; ++ii)
	    raceColor[ii] = map.readByte();

	// name
	map.seek(0x3A);
	name = map.readString(16);

	// description
	map.seek(0x76);
	description = map.readString(143);

	// data map: width, heigth
	map.seek(0x01A4);
	if(map.readLE32() != size.width())
	    qDebug() << "MP2Format::loadMap:" << "incorrect size";
	if(map.readLE32() != size.height())
	    qDebug() << "MP2Format::loadMap:" << "incorrect size";


	// data map: mp2tile, part1
	// count blocks: width * heigth
	tiles.resize(size.width() * size.height());

	for(QVector<mp2til_t>::iterator
	    it = tiles.begin(); it != tiles.end(); ++it)
	{
	    QDataStream ds(map.read(20));
	    ds >> (*it);
	}

	// data map: mp2ext, part2
	// count blocks: 4 byte
	sprites.resize(map.readLE32());

	for(QVector<mp2ext_t>::iterator
	    it = sprites.begin(); it != sprites.end(); ++it)
	{
	    QDataStream ds(map.read(15));
	    ds >> (*it);
	}

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
	    // parse block
	    QByteArray block = map.readBlock(map.readLE16());
	    QDataStream data(block);
	    data.setByteOrder(QDataStream::LittleEndian);
	    const QPoint posBlock = positionExtBlockFromNumber(ii + 1);

	    if(0 <= posBlock.x() && 0 <= posBlock.y())
	    {
		switch(block.size())
		{
		    // castle block: 70 byte
		    case 70:
		    {
			mp2town_t castle; data >> castle;
			castles.push_back(H2::TownPos(castle, posBlock));
		    }
			break;

		    // hero block: 76 byte
		    case 76:
		    {
			mp2hero_t hero; data >> hero;
			heroes.push_back(H2::HeroPos(hero, posBlock));
		    }
			break;

		    default:
			// sign block: 10 byte
			if(10 <= block.size() && 0x01 == block.at(0))
			{
			    mp2sign_t sign; data >> sign;
			    signs.push_back(H2::SignPos(sign, posBlock));
			}
			else
			// map event block: 50 byte
			if(50 <= block.size() && 0x01 == block.at(0))
			{
			    mp2mapevent_t event; data >> event;
			    mapEvents.push_back(H2::EventPos(event, posBlock));
			}
			else
			// sphinx block: 138 byte
			if(138 <= block.size() && 0 == block.at(0))
			{
			    mp2sphinx_t sphinx; data >> sphinx;
			    sphinxes.push_back(H2::SphinxPos(sphinx, posBlock));
			}
			else
			    qCritical() << "unknown block: " << ii << ", size: " << block.size() << ", pos: " << posBlock;
			break;
		}
	    }
	    else
	    if(block.at(0) == 0)
	    {
		// rumor block: 9 byte
		if(9 <= block.size() && block.at(8))
		{
		    mp2rumor_t rumor; data >> rumor;
		    rumors.push_back(rumor);
		}
		else
		// day event block: 50 byte
		if(50 <= block.size() && 0x01 == block.at(42))
		{
		    mp2dayevent_t event; data >> event;
		    dayEvents.push_back(event);
		}
		else
    		    qCritical() << "unknown block: " << ii << ", size: " << block.size();
	    }
	    else
	     qCritical() << "unknown block: " << ii << ", size: " << block.size() << ", byte: " << block[0];
	}

	uniq = map.readLE32();
	map.close();

	//
	return true;
    }

    return false;
}

QPoint MP2Format::positionExtBlockFromNumber(int num) const
{
    for(int yy = 0; yy < size.height(); ++yy)
    {
        for(int xx = 0; xx < size.width(); ++xx)
        {
            const mp2til_t & mp2 = tiles[xx + yy * size.width()];

            quint16 orders = (mp2.quantity2 ? mp2.quantity2 : 0);
            orders <<= 8;
            orders |= mp2.quantity1;

            if(orders && !(orders % 0x08) && (num == orders / 0x08))
		return QPoint(xx, yy);
        }
    }

    return QPoint(-1, -1);
}
