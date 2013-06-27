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

#ifndef _EDITOR_MAPDATA_H_
#define _EDITOR_MAPDATA_H_

#include <QVector>
#include <QList>
#include <QMap>
#include <QString>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
class QDomElement;
class QAction;
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

#include "engine.h"

class MapTile;
class MapTileLevels;
class MapData;
class MapArea;
class MapWindow;
class AroundGrounds;

class MP2Format
{
public:
    bool		loadMap(const QString &);
    bool		loadTiles(const QVector<mp2til_t> &, const QVector<mp2ext_t> &);
    QPoint		positionExtBlockFromNumber(int) const;

    QSize		size;

    QString		name;
    QString		description;

    quint16		difficulty;
    quint8		kingdomColor[6];
    quint8		humanAllow[6];
    quint8		compAllow[6];
    quint8		conditionWins;
    quint8		conditionWinsData1;
    quint8		conditionWinsData2;
    quint16		conditionWinsData3;
    quint16		conditionWinsData4;
    quint8		conditionLoss;
    quint16		conditionLossData1;
    quint16		conditionLossData2;
    bool		startWithHero;
    quint8		raceColor[6];
    quint32		uniq;

    QVector<mp2til_t>	tiles;
    QVector<mp2ext_t>	sprites;

    QVector<H2::TownPos>	castles;
    QVector<H2::HeroPos>	heroes;
    QVector<H2::SignPos>	signs;
    QVector<H2::EventPos>	mapEvents;
    QVector<H2::SphinxPos>	sphinxes;
    QVector<mp2dayevent_t>	dayEvents;
    QVector<mp2rumor_t>		rumors;
};

class MapTileExt
{
    quint8		spriteICN;
    quint8		spriteExt;
    quint8		spriteIndex;
    quint8		spriteLevel;
    quint32		spriteUID;

    friend		QDomElement & operator<< (QDomElement &, const MapTileExt &);
    friend		QDomElement & operator>> (QDomElement &, MapTileExt &);

public:
    MapTileExt() : spriteICN(0), spriteExt(0), spriteIndex(0), spriteLevel(0), spriteUID(0) {}
    MapTileExt(int lv, const mp2lev_t &);
    MapTileExt(const CompositeObject &, const CompositeSprite &, quint32);

    static bool		sortLevel1(const MapTileExt*, const MapTileExt*);
    static bool		sortLevel2(const MapTileExt*, const MapTileExt*);

    int			uid(void) const { return spriteUID; }
    int			icn(void) const { return spriteICN; }
    int			ext(void) const { return spriteExt; }
    int			index(void) const { return spriteIndex; }
    int			level(void) const { return spriteLevel; }

    bool		isUID(quint32 uid) const { return uid == spriteUID; }

    static bool		isAnimation(const MapTileExt*);
    static bool		isMapEvent(const MapTileExt*);
    static bool		isSphinx(const MapTileExt*);
    static bool		isSign(const MapTileExt*);
    static bool		isButtle(const MapTileExt*);
    static bool		isMiniHero(const MapTileExt*);
    static bool		isTown(const MapTileExt*);
    static bool		isRandomTown(const MapTileExt*);
    static int		loyaltyObject(const MapTileExt*);
    static bool		isResource(const MapTileExt*);
    static int		resource(const MapTileExt*);
};

QDomElement & operator<< (QDomElement &, const MapTileExt &);
QDomElement & operator>> (QDomElement &, MapTileExt &);

class MapTileLevels : public QList<MapTileExt*>
{
public:
    MapTileLevels() {}
    MapTileLevels(const MapTileLevels &);
    ~MapTileLevels();
    MapTileLevels & operator=(const MapTileLevels &);

    const MapTileExt*	find(bool (*pf)(const MapTileExt*)) const;
    void		paint(QPainter &, const QPoint &, const QPoint &) const;
    QString		infoString(void) const;
    int			topObjectID(void) const;
    bool		removeSprite(quint32);
};

QDomElement & operator<< (QDomElement &, const MapTileLevels &);
QDomElement & operator>> (QDomElement &, MapTileLevels &);

class MapTile : public QGraphicsPixmapItem
{
public:
    MapTile();
    MapTile(const mp2til_t &, const QPoint &);
    MapTile(const MapTile &);

    MapTile &		operator=(const MapTile &);

    QRectF		boundingRect(void) const;

    void		showInfo(void) const;
    int			groundType(void) const;

    static QString	indexString(int);

    const QPoint &	mapPos(void) const { return mpos; }
    void		setMapPos(const QPoint & pos) { mpos = pos; }

    int			basePassable(void) const { return passableBase; }
    int			localPassable(void) const { return passableLocal; }
    void		paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = 0);
    void		loadSpriteLevels(const mp2ext_t &);
    void		sortSpritesLevels(void);
    void		setTileSprite(int, int);
    void		addSpriteSection(const CompositeObject &, const CompositeSprite &, quint32);
    void		removeSpriteSection(quint32);

    const MapTileLevels & levels1(void) const { return spritesLevel1; }
    const MapTileLevels & levels2(void) const { return spritesLevel2; }

    bool		isAction(void) const;
    int			object(void) const;

protected:
    static void		loadSpriteLevel(MapTileLevels &, int, const mp2lev_t &);

    friend		QDomElement & operator<< (QDomElement &, const MapTile &);
    friend		QDomElement & operator>> (QDomElement &, MapTile &);


    QPoint		mpos;

    int			tileSprite;
    int			tileShape;
    int			objectID;

    MapTileLevels	spritesLevel1;
    MapTileLevels	spritesLevel2;

    quint16		passableBase;
    quint16		passableLocal;
};

QDomElement & operator<< (QDomElement &, const MapTile &);
QDomElement & operator>> (QDomElement &, MapTile &);

class MapTiles : public QList<MapTile*>
{
    QSize		msize;

    friend		QDomElement & operator<< (QDomElement &, const MapTiles &);
    friend		QDomElement & operator>> (QDomElement &, MapTiles &);

public:
    MapTiles() {}
    MapTiles(const MapTiles &, const QRect &);

    void		newMap(const QSize &);
    bool		importMap(const QSize &, const QVector<mp2til_t> &, const QVector<mp2ext_t> &);

    const QSize &	mapSize(void) const { return msize; }
    int			indexPoint(const QPoint &) const;
    bool		isValidPoint(const QPoint &) const;

    const MapTile*	tileConst(const QPoint &) const;
    MapTile*		tile(const QPoint &);
    const MapTile*	mapToTileConst(const QPoint &) const;
    MapTile*		mapToTile(const QPoint &);

    void		removeSprites(quint32);

    const MapTile*	tileFromDirectionConst(const MapTile*, int direct) const;
    MapTile*		tileFromDirection(const MapTile*, int direct);
    const MapTile*	tileFromDirectionConst(const QPoint &, int direct) const;
    MapTile*		tileFromDirection(const QPoint &, int direct);

    void		insertToScene(QGraphicsScene &) const;
    QString		sizeDescription(void) const;

    void		fixedOffset(void);
};

QDomElement & operator<< (QDomElement &, const MapTiles &);
QDomElement & operator>> (QDomElement &, MapTiles &);

class MapArea
{
public:
    MapTiles		tiles;
    MapObjects		objects;
    quint32		uniq;

    MapArea() : uniq(1) {}
    MapArea(const MapArea &, const QRect &);

    void		importArea(const MapArea &, const QPoint &);
    void		importMP2Towns(const QVector<H2::TownPos> &);
    void		importMP2Heroes(const QVector<H2::HeroPos> &);
    void		importMP2Signs(const QVector<H2::SignPos> &);
    void		importMP2MapEvents(const QVector<H2::EventPos> &);
    void		importMP2SphinxRiddles(const QVector<H2::SphinxPos> &);
};

QDomElement & operator<< (QDomElement &, const MapArea &);
QDomElement & operator>> (QDomElement &, MapArea &);

class MapData : public QGraphicsScene
{
    Q_OBJECT

public:
    MapData(MapWindow*);

    const QString &	name(void) const;
    const QString &	description(void) const;
    const QString &	authors(void) const;
    const QString &	license(void) const;
    const QSize &	size(void) const;
    int			difficulty(void) const;
    int			kingdomColors(void) const;
    int			computerColors(void) const;
    int			humanColors(void) const;
    bool		startWithHero(void) const;
    const CondWins &	conditionWins(void) const;
    const CondLoss &	conditionLoss(void) const;
    ListStringPos	conditionHeroList(int) const;
    ListStringPos	conditionTownList(int) const;
    ListStringPos	conditionArtifactList(void) const;
    QList<QString>	conditionSideList(void) const;
    const QStringList & tavernRumorsList(void) const;
    const DayEvents &	dayEvents(void) const;

    quint32		uniq(void);

    void		newMap(const QSize &, const QString &);
    bool		loadMap(const QString &);

    bool		saveMapXML(const QString &) const;

    QPoint		mapToTile(const QPoint &) const;
    QRect		mapToTile(const QRect &) const;

    void		showMapOptions(void);

    MapTile*		currentTile(void) { return tileOverMouse; }

signals:
    void		dataModified(void);
    void		validBuffer(bool);
    void		clickActionObject(MapTile*);

protected slots:
    void		selectAllTiles(void);
    void		editPassableDialog(void);
    void		cellInfoDialog(void);
    void		copyToBuffer(void);
    void		pasteFromBuffer(void);
    void		fillGroundAction(QAction*);
    void		removeObjectsAction(QAction*);
    void		selectObjectImage(void);
    void		editObjectAttributes(void);
    void		removeCurrentObject(void);
    void		generateMiniMap(void);

protected:
    void                mousePressEvent(QGraphicsSceneMouseEvent*);
    void                mouseMoveEvent(QGraphicsSceneMouseEvent*);
    void		mouseDoubleClickEvent(QGraphicsSceneMouseEvent*);
    void		drawForeground(QPainter*, const QRectF &);
    void		selectArea(QPointF, QPointF);

    void               addMapObject(const QPoint &, const CompositeObject &, quint32);

    void		editMapEventDialog(const MapTile &);
    void		editResourceDialog(const MapTile &);
    void		editTownDialog(const MapTile &);
    void		editSignDialog(const MapTile &);
    void		editSphinxDialog(const MapTile &);
    void		editHeroDialog(const MapTile &);

    bool		loadMapMP2(const QString &);
    bool		loadMapXML(const QString &);

    friend class	MP2Format;
    friend class	MapTile;
    friend		QDomElement & operator<< (QDomElement &, const MapData &);
    friend		QDomElement & operator>> (QDomElement &, MapData &);

    MapTile*		tileOverMouse;

    QString		mapName;
    QString		mapDescription;
    QString		mapAuthors;
    QString		mapLicense;
    int			mapDifficulty;
    int			mapKingdomColors;
    int			mapCompColors;
    int			mapHumanColors;
    bool		mapStartWithHero;
    CondWins		mapConditionWins;
    CondLoss		mapConditionLoss;

    MapArea		mapArea;
    MapTiles &		mapTiles;
    MapObjects &	mapObjects;

    DayEvents		mapDayEvents;
    TavernRumors	tavernRumors;

    CompositeObjectCursor
                        currentObject;

    int			engineVersion;
    int			mapVersion;

    static QSharedPointer<MapArea>
    			selectedArea;
};

QDomElement & operator<< (QDomElement &, const MapData &);
QDomElement & operator>> (QDomElement &, MapData &);

QDomElement & operator<< (QDomElement &, const MapData &);
QDomElement & operator>> (QDomElement &, MapData &);

#endif
