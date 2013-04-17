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
#include <QAction>

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
    quint8		tmp;
    quint32		spriteUID;

public:
    MapTileExt(int lv, const mp2lev_t &);
    MapTileExt(int, const CompositeSprite &, quint32);

    static bool		sortLevel1(const MapTileExt*, const MapTileExt*);
    static bool		sortLevel2(const MapTileExt*, const MapTileExt*);

    int			uid(void) const { return spriteUID; }
    int			icn(void) const { return spriteICN; }
    int			ext(void) const { return spriteExt; }
    int			index(void) const { return spriteIndex; }
    int			level(void) const { return spriteLevel; }

    static bool		isMapEvent(const MapTileExt*);
    static bool		isSphinx(const MapTileExt*);
    static bool		isSign(const MapTileExt*);
    static bool		isButtle(const MapTileExt*);
    static bool		isMiniHero(const MapTileExt*);
    static bool		isTown(const MapTileExt*);
    static bool		isRandomTown(const MapTileExt*);
};

class MapTileLevels : public QList<MapTileExt*>
{
public:
    MapTileLevels() {}
    MapTileLevels(const MapTileLevels &);
    ~MapTileLevels();

    const MapTileExt*	find(bool (*pf)(const MapTileExt*)) const;
    void		paint(QPainter &, const QPoint &, EditorTheme &) const;
    QString		infoString(void) const;
};

class MapTile : public QGraphicsPixmapItem
{
public:
    MapTile(const mp2til_t &, const QPoint &, EditorTheme &);
    MapTile(const MapTile &);

    QRectF		boundingRect(void) const;

    void		showInfo(void) const;
    int			groundType(void) const;

    static QString	indexString(int);

    const QPoint &	mapPos(void) const { return mpos; }
    void		paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = 0);
    void		loadSpriteLevels(const mp2ext_t &);
    void		sortSpritesLevels(void);
    void		setTileSprite(int, int);
    void		addSpriteSection(int, const CompositeSprite &, quint32);

    const MapTileLevels & levels1(void) const { return spritesLevel1; }
    const MapTileLevels & levels2(void) const { return spritesLevel2; }

protected:
    static void		loadSpriteLevel(MapTileLevels &, int, const mp2lev_t &);

    EditorTheme &	themeContent;
    mp2til_t		til;
    QPoint		mpos;

    MapTileLevels	spritesLevel1;
    MapTileLevels	spritesLevel2;

    quint16		passableBase;
    quint16		passableLocal;
};

class MapTiles : protected QList<MapTile*>
{
    QSize		size;

public:
    MapTiles() {}
    MapTiles(const MapTiles &, const QRect &);

    void		newMap(const QSize &, EditorTheme &);
    bool		importMap(const QSize &, const QVector<mp2til_t> &, const QVector<mp2ext_t> &, EditorTheme &);

    const QSize &	mapSize(void) const { return size; }
    int			indexPoint(const QPoint &) const;
    bool		isValidPoint(const QPoint &) const;

    const MapTile*	tileConst(const QPoint &) const;
    MapTile*		tile(const QPoint &);
    const MapTile*	mapToTileConst(const QPoint &) const;
    MapTile*		mapToTile(const QPoint &);

    const MapTile*	tileFromDirectionConst(const MapTile*, int direct) const;
    MapTile*		tileFromDirection(const MapTile*, int direct);
    const MapTile*	tileFromDirectionConst(const QPoint &, int direct) const;
    MapTile*		tileFromDirection(const QPoint &, int direct);

    void		insertToScene(QGraphicsScene &) const;
};

class MapArea
{
public:
    MapTiles		tiles;
    MapObjects		objects;

    MapArea(){}
    MapArea(const MapArea & ma, const QRect & rt) : tiles(ma.tiles, rt), objects(ma.objects, rt) {}

    void		importMP2Towns(const QVector<H2::TownPos> &);
    void		importMP2Heroes(const QVector<H2::HeroPos> &);
    void		importMP2Signs(const QVector<H2::SignPos> &);
    void		importMP2MapEvents(const QVector<H2::EventPos> &);
    void		importMP2SphinxRiddles(const QVector<H2::SphinxPos> &);

    void		addObject(const QPoint &, const CompositeObject &, const QSize &, quint32);
};

class MapSelectedArea : public MapArea
{
public:
    MapSelectedArea(const MapArea & ma, const QRect & rt) : MapArea(ma, rt) {}
};

class MapData : public QGraphicsScene
{
    Q_OBJECT

public:
    MapData(MapWindow*);

    const QString &	name(void) const;
    const QString &	description(void) const;
    const QSize &	size(void) const;

    quint32		uniq(void);

    void		newMap(const QSize &, const QString &);
    bool		loadMap(const QString &);

    QPoint		mapToTile(const QPoint &) const;
    QRect		mapToTile(const QRect &) const;

    EditorTheme &	theme(void);
    void		SaveTest(void) const;

signals:
    void		dataModified(void);
    void		validBuffer(bool);

protected slots:
    void		selectAllTiles(void);
    void		editPassableDialog(void);
    void		cellInfoDialog(void);
    void		copyToBuffer(void);
    void		pasteFromBuffer(void);
    void		fillGroundAction(QAction*);
    void		removeObjectsAction(QAction*);
    void		selectObjectImage(void);

protected:
    void                mousePressEvent(QGraphicsSceneMouseEvent*);
    void                mouseMoveEvent(QGraphicsSceneMouseEvent*);
    void		drawForeground(QPainter*, const QRectF &);
    void		selectArea(QPointF, QPointF);


    friend class	MP2Format;
    friend class	MapTile;

    EditorTheme		themeContent;
    MapTile*		tileOverMouse;

    QString		mapName;
    QString		mapDescription;
    QString		mapAuthors;
    QString		mapLicense;
    int			mapDifficulty;
    quint32		mapUniq;

    MapArea		mapArea;
    MapTiles &		mapTiles;
    MapObjects &	mapObjects;

    DayEvents		dayEvents;
    TavernRumors	tavernRumors;

    CompositeObjectCursor
                        currentObject;
};

#endif
