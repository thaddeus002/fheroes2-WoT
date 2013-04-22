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
#include "dialogs.h"
#include "mainwindow.h"
#include "mapwindow.h"
#include "mapdata.h"

MapTileExt::MapTileExt(int lvl, const mp2lev_t & ext)
    : spriteICN(H2::mapICN(ext.object)), spriteExt(0x03 & ext.object), spriteIndex(ext.index), spriteLevel(lvl), tmp(0), spriteUID(ext.uniq)
{
}

MapTileExt::MapTileExt(int icn, const CompositeSprite & cs, quint32 uid)
    : spriteICN(icn), spriteExt(0), spriteIndex(cs.spriteIndex), spriteLevel(0), tmp(0), spriteUID(uid)
{
    if(cs.spriteAnimation)
	spriteExt |= 0x01;
}

bool MapTileExt::sortLevel1(const MapTileExt* mte1, const MapTileExt* mte2)
{
    return (mte1->spriteLevel % 4) > (mte2->spriteLevel % 4);
}

bool MapTileExt::sortLevel2(const MapTileExt* mte1, const MapTileExt* mte2)
{
    return (mte1->spriteLevel % 4) < (mte2->spriteLevel % 4);
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

MapTileLevels::MapTileLevels(const MapTileLevels & other)
    : QList<MapTileExt*>()
{
    for(const_iterator it = other.begin(); it != other.end(); ++it)
	*this << new MapTileExt(**it);
}

MapTileLevels::~MapTileLevels()
{
    qDeleteAll(begin(), end());
}

void MapTileLevels::paint(QPainter & painter, const QPoint & offset, const QPoint & mpos, EditorTheme & theme) const
{
    for(const_iterator it = begin(); it != end(); ++it)
    {
	QPair<QPixmap, QPoint> p1 = theme.getImageICN((*it)->icn(), (*it)->index());
	painter.drawPixmap(offset + p1.second, p1.first);

	int anim = H2::isAnimationICN((*it)->icn(), (*it)->index(), 0);

	if(0 < anim)
	{
	    QPair<QPixmap, QPoint> p2 = theme.getImageICN((*it)->icn(), anim);
	    painter.drawPixmap(offset + p2.second, p2.first);
	}
	else
	if((*it)->ext() & 0x01)
	    qDebug() << "H2::isAnimationICN:" << "incorrect animation" << mpos << "icn:" << (*it)->icn() << "index:" << (*it)->index();
    }
}

QString MapTileLevels::infoString(void) const
{
    QString str;
    QTextStream ss(& str);

    for(const_iterator it = begin(); it != end(); ++it)
    {
	ss <<
	    "uniq:   " << (*it)->uid() << endl <<
	    "object: " << H2::icnString((*it)->icn()) << ", " << (*it)->ext() << endl <<
	    "index:  " << (*it)->index() << endl <<
	    "level:  " << (*it)->level() << endl;
    }

    return str;
}

const MapTileExt* MapTileLevels::find(bool (*pf)(const MapTileExt*)) const
{
    const_iterator it = std::find_if(begin(), end(), pf);
    return it != end() ? *it : NULL;
}

MapTile::MapTile(const mp2til_t & mp2, const QPoint & pos, EditorTheme & theme)
    : themeContent(theme), til(mp2), mpos(pos), passableBase(0), passableLocal(0xFFFF)
{
    QPoint offset(mpos.x() * theme.tileSize().width(), mpos.y() * theme.tileSize().height());
    setOffset(offset);
    setFlags(QGraphicsItem::ItemIsSelectable);
    setTileSprite(til.tileSprite, til.tileShape);
    loadSpriteLevel(spritesLevel1, 0, mp2.level1);
    loadSpriteLevel(spritesLevel2, 0, mp2.level2);
}

MapTile::MapTile(const MapTile & other)
    : QGraphicsPixmapItem(), themeContent(other.themeContent), til(other.til),
	mpos(other.mpos), spritesLevel1(other.spritesLevel1), spritesLevel2(other.spritesLevel2),
	passableBase(other.passableBase), passableLocal(other.passableLocal)
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    setTileSprite(til.tileSprite, til.tileShape);
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
    spritesLevel1.paint(*painter, offset().toPoint(), mpos, themeContent);

    // draw level2
    spritesLevel2.paint(*painter, offset().toPoint(), mpos, themeContent);
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
	ss2 << spritesLevel1.infoString() <<
	    "----------------------" << endl;

    // draw level2
    if(spritesLevel2.size())
	ss2 << spritesLevel2.infoString() <<
	    "----------------------" << endl;

    QMessageBox::information(NULL, str, msg);
}

void MapTile::loadSpriteLevel(MapTileLevels & list, int level, const mp2lev_t & ext)
{
    if(ext.object && ext.index < 0xFF)
    {
	const QString & icn = H2::icnString(H2::mapICN(ext.object));

	if(! icn.isEmpty())
	    list << new MapTileExt(level, ext);
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
    qStableSort(spritesLevel1.begin(), spritesLevel1.end(), MapTileExt::sortLevel1);
    qStableSort(spritesLevel2.begin(), spritesLevel2.end(), MapTileExt::sortLevel2);
}

void MapTile::addSpriteSection(int icn, const CompositeSprite & cs, quint32 uid)
{
    if(cs.spriteLevel == SpriteLevel::Top)
	spritesLevel2 << new MapTileExt(icn, cs, uid);
    else
	spritesLevel1 << new MapTileExt(icn, cs, uid);
}

MapTiles::MapTiles(const MapTiles & tiles, const QRect & area) : size(area.size())
{
    for(int yy = area.y(); yy < area.y() + area.height(); ++yy)
    {
	for(int xx = area.x(); xx < area.x() + area.width(); ++xx)
        {
	    const MapTile* tile = tiles.tileConst(QPoint(xx, yy));
	    if(tile) *this << new MapTile(*tile);
        }
    }
}

void MapTiles::newMap(const QSize & sz, EditorTheme & theme)
{
    size = sz;

    for(int yy = 0; yy < size.height(); ++yy)
    {
	for(int xx = 0; xx < size.width(); ++xx)
    	    push_back(new MapTile(mp2til_t(), QPoint(xx, yy), theme));
    }
}

bool MapTiles::importMap(const QSize & sz, const QVector<mp2til_t> & mp2Tiles, const QVector<mp2ext_t> & mp2Sprites, EditorTheme & theme)
{
    size = sz;

    for(int yy = 0; yy < size.height(); ++yy)
    {
	for(int xx = 0; xx < size.width(); ++xx)
	{
	    const mp2til_t & mp2til = mp2Tiles[indexPoint(QPoint(xx, yy))];
	    push_back(new MapTile(mp2til, QPoint(xx, yy), theme));
	    int ext = mp2til.indexExt;

	    while(ext)
	    {
		if(ext >= mp2Sprites.size())
		{
		    qDebug() << "ext block: out of range" << ext;
    		    return false;
		}

		back()->loadSpriteLevels(mp2Sprites[ext]);
		ext = mp2Sprites[ext].indexExt;
	    }

	    back()->sortSpritesLevels();
	}
    }

    return true;
}

const MapTile* MapTiles::mapToTileConst(const QPoint & pos) const
{
    for(MapTiles::const_iterator
	it = begin(); it != end(); ++it)
	if((*it)->boundingRect().contains(pos)) return *it;

    return NULL;
}

MapTile* MapTiles::mapToTile(const QPoint & pos)
{
    return const_cast<MapTile*>(mapToTileConst(pos));
}

const MapTile* MapTiles::tileConst(const QPoint & pos) const
{
    const QList<MapTile*> & mapTiles = *this;
    return isValidPoint(pos) ? mapTiles[indexPoint(pos)] : NULL;
}

MapTile* MapTiles::tile(const QPoint & pos)
{
    return const_cast<MapTile*>(tileConst(pos));
}

const MapTile* MapTiles::tileFromDirectionConst(const MapTile* tile, int direct) const
{
    return tile ? tileFromDirectionConst(tile->mapPos(), direct) : NULL;
}

MapTile* MapTiles::tileFromDirection(const MapTile* tile, int direct)
{
    return const_cast<MapTile*>(tileFromDirectionConst(tile, direct));
}

const MapTile* MapTiles::tileFromDirectionConst(const QPoint & center, int direct) const
{
    QPoint next(center);

    switch(direct)
    {
	case Direction::Top:         if(center.y()) next.setY(center.y() - 1); break;
    	case Direction::Bottom:      if(center.y() < size.height()) next.setY(center.y() + 1); break;
    	case Direction::Left:        if(center.x()) next.setX(center.x() - 1); break;
    	case Direction::Right:       if(center.x() < size.width()) next.setX(center.x() + 1); break;

    	case Direction::TopRight:    return tileFromDirectionConst(tileFromDirectionConst(center, Direction::Top), Direction::Right);
    	case Direction::BottomRight: return tileFromDirectionConst(tileFromDirectionConst(center, Direction::Bottom), Direction::Right);
    	case Direction::BottomLeft:  return tileFromDirectionConst(tileFromDirectionConst(center, Direction::Bottom), Direction::Left);
    	case Direction::TopLeft:     return tileFromDirectionConst(tileFromDirectionConst(center, Direction::Top), Direction::Left);
    	default: break;
    }

    return tileConst(next);
}

MapTile* MapTiles::tileFromDirection(const QPoint & center, int direct)
{
    return const_cast<MapTile*>(tileFromDirectionConst(center, direct));
}

int MapTiles::indexPoint(const QPoint & pos) const
{
    return pos.x() + pos.y() * size.width();
}

bool MapTiles::isValidPoint(const QPoint & pos) const
{
    return QRect(QPoint(0, 0), size).contains(pos);
}

void MapTiles::insertToScene(QGraphicsScene & scene) const
{
    for(const_iterator it = begin(); it != end(); ++it)
	scene.addItem(*it);
}

void MapArea::importMP2Towns(const QVector<H2::TownPos> & towns)
{
    for(QVector<H2::TownPos>::const_iterator
	it = towns.begin(); it != towns.end(); ++it) if(tiles.isValidPoint((*it).pos()))
    {
	const MapTileExt* ext = tiles.tileConst((*it).pos())->levels1().find(MapTileExt::isTown);
	int uid = ext ? ext->uid() : -1;
	objects[(*it).pos()] = QSharedPointer<MapObject>(new MapTown((*it).pos(), uid, (*it).town()));
    }
}

void MapArea::importMP2Heroes(const QVector<H2::HeroPos> & heroes)
{
    for(QVector<H2::HeroPos>::const_iterator
	it = heroes.begin(); it != heroes.end(); ++it) if(tiles.isValidPoint((*it).pos()))
    {
	const MapTileExt* ext = tiles.tileConst((*it).pos())->levels1().find(MapTileExt::isMiniHero);
	int uid = ext ? ext->uid() : -1;
	objects[(*it).pos()] = QSharedPointer<MapObject>(new MapHero((*it).pos(), uid, (*it).hero()));
    }
}

void MapArea::importMP2Signs(const QVector<H2::SignPos> & signs)
{
    for(QVector<H2::SignPos>::const_iterator
	it = signs.begin(); it != signs.end(); ++it) if(tiles.isValidPoint((*it).pos()))
    {
	const MapTileExt* ext = tiles.tileConst((*it).pos())->levels1().find(MapTileExt::isSign);
	int uid = ext ? ext->uid() : -1;
	objects[(*it).pos()] = QSharedPointer<MapObject>(new MapSign((*it).pos(), uid, (*it).sign()));
    }
}

void MapArea::importMP2MapEvents(const QVector<H2::EventPos> & events)
{
    for(QVector<H2::EventPos>::const_iterator
	it = events.begin(); it != events.end(); ++it) if(tiles.isValidPoint((*it).pos()))
    {
	const MapTileExt* ext = tiles.tileConst((*it).pos())->levels1().find(MapTileExt::isMapEvent);
	int uid = ext ? ext->uid() : -1;
	objects[(*it).pos()] = QSharedPointer<MapObject>(new MapEvent((*it).pos(), uid, (*it).event()));
    }
}

void MapArea::importMP2SphinxRiddles(const QVector<H2::SphinxPos> & sphinxes)
{
    for(QVector<H2::SphinxPos>::const_iterator
	it = sphinxes.begin(); it != sphinxes.end(); ++it) if(tiles.isValidPoint((*it).pos()))
    {
	const MapTileExt* ext = tiles.tileConst((*it).pos())->levels1().find(MapTileExt::isSphinx);
	int uid = ext ? ext->uid() : -1;
	objects[(*it).pos()] = QSharedPointer<MapObject>(new MapSphinx((*it).pos(), uid, (*it).sphinx()));
    }
}

void MapArea::addObject(const QPoint & pos, const CompositeObject & obj, const QSize & tileSize, quint32 uid)
{
    for(CompositeObject::const_iterator
	it = obj.begin(); it != obj.end(); ++it)
    {
	QPoint offset((*it).spritePos.x() * tileSize.width() + 1, (*it).spritePos.y() * tileSize.height() + 1);
	MapTile* tile = tiles.mapToTile(pos + offset);
	tile->addSpriteSection(obj.icn.second, *it, uid);
    }
}

MapData::MapData(MapWindow* parent) : QGraphicsScene(parent), themeContent(parent->mainWindow->aggContent),
    tileOverMouse(NULL), mapName("New Map"), mapAuthors("unknown"), mapLicense("unknown"), mapDifficulty(Difficulty::Normal),
    mapKingdomColors(0), mapCompColors(0), mapHumanColors(0), mapUniq(1), mapStartWithHero(false), mapArea(),
    mapTiles(mapArea.tiles), mapObjects(mapArea.objects)
{
}

EditorTheme & MapData::theme(void)
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

const QString & MapData::authors(void) const
{
    return mapAuthors;
}

const QString & MapData::license(void) const
{
    return mapLicense;
}

int MapData::difficulty(void) const
{
    return mapDifficulty;
}

int MapData::kingdomColors(void) const
{
    return mapKingdomColors;
}

int MapData::humanColors(void) const
{
    return mapHumanColors;
}

int MapData::computerColors(void) const
{
    return mapCompColors;
}

bool MapData::startWithHero(void) const
{
    return mapStartWithHero;
}

const CondWins & MapData::conditionWins(void) const
{
    return mapConditionWins;
}

const CondLoss & MapData::conditionLoss(void) const
{
    return mapConditionLoss;
}

ListStringPos MapData::conditionHeroList(int cond) const
{
    ListStringPos res;

    if(Conditions::Wins & cond)
	res << QPair<QString, QPoint>("Test 1", QPoint(22,33)) << QPair<QString, QPoint>("Hero 2", QPoint(11,67));
    else
    if(Conditions::Loss & cond)
	res << QPair<QString, QPoint>("Test 1", QPoint(22,33)) << QPair<QString, QPoint>("Hero 2", QPoint(11,67));

    return res;
}

ListStringPos MapData::conditionTownList(int cond) const
{
    ListStringPos res;

    if(Conditions::Wins & cond)
	res << QPair<QString, QPoint>("Test 1", QPoint(62,83)) << QPair<QString, QPoint>("Town 2", QPoint(81,27));
    else
    if(Conditions::Loss & cond)
	res << QPair<QString, QPoint>("Test 1", QPoint(62,83)) << QPair<QString, QPoint>("Town 2", QPoint(81,27));

    return res;
}

ListStringPos MapData::conditionArtifactList(void) const
{
    ListStringPos res;

    res << QPair<QString, QPoint>("Test 1", QPoint(62,83)) << QPair<QString, QPoint>("Artifact 2", QPoint(81,27));

    return res;
}

QList<QString> MapData::conditionSideList(void) const
{
    return QList<QString>() << "Left vs Right" << "Right vs Left";
}

const QSize & MapData::size(void) const
{
    return mapTiles.mapSize();
}

quint32 MapData::uniq(void)
{
    return mapUniq++;
}

const QStringList & MapData::tavernRumorsList(void) const
{
    return tavernRumors;
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
    else
    // place object
    if(currentObject.isValid())
    {
	if(event->buttons() & Qt::LeftButton)
	    mapArea.addObject(currentObject.scenePos, currentObject, themeContent.tileSize(), uniq());
	else
	    currentObject.reset();
	update(currentObject.area());
    }

    event->accept();
}

void MapData::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(currentObject.isValid())
    {
	update(currentObject.area());
    }
    else
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

    MapTile* newTileOverMouse = qgraphicsitem_cast<MapTile*>(itemAt(event->scenePos()));

    if(newTileOverMouse)
    {
	if(tileOverMouse != newTileOverMouse)
	{
	    MapWindow* mapWindow = qobject_cast<MapWindow*>(parent());

	    if(mapWindow &&
		(!tileOverMouse || tileOverMouse->mapPos().x() != newTileOverMouse->mapPos().x()))
		    emit mapWindow->cursorTileXPosChanged(newTileOverMouse->mapPos().x());

	    if(mapWindow &&
		(!tileOverMouse || tileOverMouse->mapPos().y() != newTileOverMouse->mapPos().y()))
		    emit mapWindow->cursorTileYPosChanged(newTileOverMouse->mapPos().y());

	    tileOverMouse = newTileOverMouse;
	}

	if(tileOverMouse)
	    update(tileOverMouse->boundingRect());
    }

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
	    selRect.setRight((sr + 1) * tileSize.width() - 1);

	if(selRect.bottom() > sb * tileSize.height())
	    selRect.setBottom((sb + 1) * tileSize.height() - 1);

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
    // paint: selected new object place
    if(currentObject.isValid())
    {
	QPoint pos = tileOverMouse->boundingRect().topLeft().toPoint() - currentObject.center();
	currentObject.paint(*painter, pos, true);
    }
    else
    // paint: selected item over mouse
    if(tileOverMouse)
    {
	painter->setPen(QPen(QColor(255, 255, 0), 1));
	painter->setBrush(QBrush(QColor(0, 0, 0, 0)));
	const QRectF & rt = tileOverMouse->boundingRect();
	painter->drawRect(QRectF(rt.x(), rt.y(), rt.width() - 1, rt.height() - 1));
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
	MapSelectedArea(mapArea, mapToTile(selectionArea().boundingRect().toRect()));
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
	QPoint tile2(tileSize.width() / 2, tileSize.height() / 2);

	rectArea.setTopLeft(rectArea.topLeft() - tile2);
	rectArea.setBottomRight(rectArea.bottomRight() + tile2);

	QList<QGraphicsItem*> listItems = items(rectArea);

	for(QList<QGraphicsItem*>::iterator
	    it = listItems.begin(); it != listItems.end(); ++it)
	{
	    MapTile* tile = qgraphicsitem_cast<MapTile*>(*it);

    	    if(tile)
	    {
		QPair<int, int> indexGroundRotate = themeContent.groundBoundariesFix(*tile, mapTiles);

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

void MapData::newMap(const QSize & msz, const QString &)
{
    const QSize & tileSize = themeContent.tileSize();

    mapTiles.newMap(msz, themeContent);
    mapTiles.insertToScene(*this);

    setSceneRect(QRect(QPoint(0, 0),
	QSize(size().width() * tileSize.width(), size().height() * tileSize.height())));
}

bool MapData::loadMap(const QString & mapFile)
{
    const QSize & tileSize = themeContent.tileSize();

    MP2Format mp2;

    if(mp2.loadMap(mapFile))
    {
	// import tiles
	if(! mapTiles.importMap(mp2.size, mp2.tiles, mp2.sprites, themeContent))
	    return false;

	mapTiles.insertToScene(*this);

	mapName = mp2.name;
	mapDescription = mp2.description;
	mapStartWithHero = mp2.startWithHero;

	switch(mp2.difficulty)
	{
	    case 0:	mapDifficulty = Difficulty::Easy; break;
	    case 2:	mapDifficulty = Difficulty::Tough; break;
	    case 3:	mapDifficulty = Difficulty::Expert; break;
	    default:	mapDifficulty = Difficulty::Normal; break;
	}

	if(mp2.kingdomColor[0]) mapKingdomColors |= Color::Blue;
	if(mp2.kingdomColor[1]) mapKingdomColors |= Color::Green;
	if(mp2.kingdomColor[2]) mapKingdomColors |= Color::Red;
	if(mp2.kingdomColor[3]) mapKingdomColors |= Color::Yellow;
	if(mp2.kingdomColor[4]) mapKingdomColors |= Color::Orange;
	if(mp2.kingdomColor[5]) mapKingdomColors |= Color::Purple;

	if(mp2.humanAllow[0]) mapHumanColors |= Color::Blue;
	if(mp2.humanAllow[1]) mapHumanColors |= Color::Green;
	if(mp2.humanAllow[2]) mapHumanColors |= Color::Red;
	if(mp2.humanAllow[3]) mapHumanColors |= Color::Yellow;
	if(mp2.humanAllow[4]) mapHumanColors |= Color::Orange;
	if(mp2.humanAllow[5]) mapHumanColors |= Color::Purple;

	if(mp2.compAllow[0]) mapCompColors |= Color::Blue;
	if(mp2.compAllow[1]) mapCompColors |= Color::Green;
	if(mp2.compAllow[2]) mapCompColors |= Color::Red;
	if(mp2.compAllow[3]) mapCompColors |= Color::Yellow;
	if(mp2.compAllow[4]) mapCompColors |= Color::Orange;
	if(mp2.compAllow[5]) mapCompColors |= Color::Purple;

	switch(mp2.conditionWins)
	{
	    case 1:	mapConditionWins.set(Conditions::CaptureTown, QPoint(mp2.conditionWinsData3, mp2.conditionWinsData4)); break;
	    case 2:	mapConditionWins.set(Conditions::DefeatHero, QPoint(mp2.conditionWinsData3, mp2.conditionWinsData4)); break;
	    case 3:	mapConditionWins.set(Conditions::FindArtifact, static_cast<int>(mp2.conditionWinsData3)); break;
	    case 4:	mapConditionWins.set(Conditions::SideWins, static_cast<int>(mp2.conditionWinsData3)); break;
	    case 5:	mapConditionWins.set(Conditions::AccumulateGold, 1000 * static_cast<int>(mp2.conditionWinsData3)); break;
	    default:	mapConditionWins.set(Conditions::Wins); break;
	}

	if(mp2.conditionWinsData1)
	    mapConditionWins.first |= Conditions::CompAlsoWins;

	if(mp2.conditionWinsData2)
	    mapConditionWins.first |= Conditions::AllowNormalVictory;

	switch(mp2.conditionLoss)
	{
	    case 1:	mapConditionLoss.set(Conditions::LoseTown, QPoint(mp2.conditionLossData1, mp2.conditionLossData2)); break;
	    case 2:	mapConditionLoss.set(Conditions::LoseHero, QPoint(mp2.conditionLossData1, mp2.conditionLossData2)); break;
	    case 3:	mapConditionLoss.set(Conditions::OutTime, static_cast<int>(mp2.conditionLossData1)); break;
	    default:	mapConditionLoss.set(Conditions::Loss); break;
	}

	mapUniq = mp2.uniq + 1;

	setSceneRect(QRect(QPoint(0, 0),
		QSize(size().width() * tileSize.width(), size().height() * tileSize.height())));

	// import towns
	mapArea.importMP2Towns(mp2.castles);

	// import heroes
	mapArea.importMP2Heroes(mp2.heroes);

	// import signs
	mapArea.importMP2Signs(mp2.signs);

	// import map events
	mapArea.importMP2MapEvents(mp2.mapEvents);

	// import sphinx riddles
	mapArea.importMP2SphinxRiddles(mp2.sphinxes);

	// import day events
	for(QVector<mp2dayevent_t>::const_iterator
	    it = mp2.dayEvents.begin(); it != mp2.dayEvents.end(); ++it)
	    mapDayEvents.push_back(DayEvent(*it));

	// import rumors
	for(QVector<mp2rumor_t>::const_iterator
	    it = mp2.rumors.begin(); it != mp2.rumors.end(); ++it)
	    tavernRumors << Rumor(*it);

	return true;
    }

    return false;
}

QPoint MapData::mapToTile(const QPoint & pt) const
{
    const MapTile* tile = qgraphicsitem_cast<const MapTile*>(itemAt(pt));
    return tile ? tile->mapPos() : QPoint(-1, -1);
}

QRect MapData::mapToTile(const QRect & rt) const
{
    return QRect(mapToTile(rt.topLeft()), mapToTile(rt.bottomRight()));
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
	if(static_cast<int>(map.readLE32()) != size.width())
	{
	    qDebug() << "MP2Format::loadMap:" << "incorrect size";
    	    return false;
	}
	if(static_cast<int>(map.readLE32()) != size.height())
	{
	    qDebug() << "MP2Format::loadMap:" << "incorrect size";
    	    return false;
	}

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

void MapData::SaveTest(void) const
{
    QDomDocument doc;

    QDomElement emap = doc.createElement("map");
    doc.appendChild(emap);

    QDomElement eheader = doc.createElement("header");
    emap.appendChild(eheader);

    eheader.setAttribute("version", 3075);
    eheader.setAttribute("localtime", QDateTime::currentDateTime().toTime_t());

    eheader.appendChild(doc.createElement("name")).appendChild(doc.createTextNode(mapName));
    eheader.appendChild(doc.createElement("description")).appendChild(doc.createTextNode(mapDescription));
    eheader.appendChild(doc.createElement("authors")).appendChild(doc.createTextNode(mapAuthors));
    eheader.appendChild(doc.createElement("license")).appendChild(doc.createTextNode(mapLicense));
    eheader.appendChild(doc.createElement("difficulty")).appendChild(doc.createTextNode(QString::number(mapDifficulty)));

    QDomElement eplayers = doc.createElement("players");
    eplayers.setAttribute("kingdoms", mapKingdomColors);
    eplayers.setAttribute("humans", mapHumanColors);
    eplayers.setAttribute("computers", mapCompColors);
    eplayers.setAttribute("startWithHero", mapStartWithHero);
    eheader.appendChild(eplayers);

    QDomElement ewins = doc.createElement("conditionWins");
    ewins.setAttribute("condition", mapConditionWins.condition());
    ewins.setAttribute("allowNormalVictory", mapConditionWins.allowNormalVictory());
    ewins.setAttribute("computerAlsoWins", mapConditionWins.compAlsoWins());
    ewins.setAttribute("extValue", mapConditionWins.variantString());
    eheader.appendChild(ewins);

    QDomElement eloss = doc.createElement("conditionLoss");
    eloss.setAttribute("condition", mapConditionLoss.condition());
    eloss.setAttribute("extValue", mapConditionLoss.variantString());
    eheader.appendChild(eloss);

/*
    quint32             mapUniq;
*/

    QDomElement erumors = doc.createElement("rumors");
    emap.appendChild(erumors);

    for(TavernRumors::const_iterator
	it = tavernRumors.begin(); it != tavernRumors.end(); ++it)
	erumors.appendChild(doc.createElement("msg")).appendChild(doc.createTextNode(*it));

    QDomElement events = doc.createElement("events");
    emap.appendChild(events);

    for(DayEvents::const_iterator
	it = mapDayEvents.begin(); it != mapDayEvents.end(); ++it)
    {
	QDomElement event = doc.createElement("event");
	events.appendChild(event);

	event.setAttribute("colors", (*it).colors());
	event.setAttribute("allowComputer", (*it).allowComputer());

	QPair<int,int> day = (*it).dayOccurent();
	event.setAttribute("dayFirst", day.first);
	event.setAttribute("daySubsequent", day.second);

	QDomElement resources = doc.createElement("resources");
	event.appendChild(resources);
	resources.setAttribute("wood", (*it).resources().wood);
	resources.setAttribute("mercury", (*it).resources().mercury);
	resources.setAttribute("ore", (*it).resources().ore);
	resources.setAttribute("sulfur", (*it).resources().sulfur);
	resources.setAttribute("crystal", (*it).resources().crystal);
	resources.setAttribute("gems", (*it).resources().gems);
	resources.setAttribute("gold", (*it).resources().gold);

	event.appendChild(doc.createElement("msg")).appendChild(doc.createTextNode((*it).message()));
    }

    doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());

    QFile file("out.xml");
    file.open(QIODevice::WriteOnly);

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF-8"));

    doc.save(out, 5, QDomNode::EncodingFromTextStream);
}

void MapData::selectObjectImage(void)
{
    Form::SelectImage form(themeContent);

    if(QDialog::Accepted == form.exec())
    {
        currentObject = CompositeObjectCursor(form.result, themeContent);
	update();
    }
}
void MapData::showMapOptions(void)
{
    Form::MapOptions form(*this);

    if(QDialog::Accepted == form.exec())
    {
	// tab1
	mapName = form.lineEditName->text();
	mapDescription = form.plainTextEditDescription->toPlainText();
	mapDifficulty = qvariant_cast<int>(comboBoxCurrentData(form.comboBoxDifficulty));

	// tab2
	mapConditionWins.set(qvariant_cast<int>(comboBoxCurrentData(form.comboBoxWinsCond)), comboBoxCurrentData(form.comboBoxWinsCondExt));
	mapConditionWins.setAllowNormalVictory(form.checkBoxAllowNormalVictory->isChecked());
	mapConditionWins.setCompAlsoWins(form.checkBoxCompAlsoWins->isChecked());
	mapConditionLoss.set(qvariant_cast<int>(comboBoxCurrentData(form.comboBoxLossCond)), comboBoxCurrentData(form.comboBoxLossCondExt));
	mapStartWithHero = form.checkBoxStartWithHero->isChecked();

	mapCompColors = 0;
	mapHumanColors = 0;

	for(QVector<Form::PlayerStatus*>::const_iterator
    	    it = form.labelPlayers.begin(); it != form.labelPlayers.end(); ++it)
	{
	    if(mapKingdomColors & (*it)->color())
	    {
		/* 0: n/a, 1: human only, 2: comp only, 3: comp or human */
		if(0x01 & (*it)->status())
		    mapHumanColors |= (*it)->color();
		if(0x02 & (*it)->status())
		    mapCompColors |= (*it)->color();
	    }
	}

	// tab3
	tavernRumors.clear();
	for(int pos = 0; pos < form.listWidgetRumors->count(); ++pos)
	    tavernRumors << form.listWidgetRumors->item(pos)->text();

	mapDayEvents.clear();
	for(int pos = 0; pos < form.listWidgetEvents->count(); ++pos)
	    mapDayEvents.push_back(qvariant_cast<DayEvent>(form.listWidgetEvents->item(pos)->data(Qt::UserRole)));
    }
}

const DayEvents & MapData::dayEvents(void) const
{
    return mapDayEvents;
}
