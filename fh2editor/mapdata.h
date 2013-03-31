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
class MapData;
class MapWindow;
class AroundGrounds;

class MapTileExt : protected QPair<QPixmap, QPoint>
{
    friend class MapTile;
    friend class MapData;

    quint8		spriteICN;
    quint8		spriteIndex;
    quint8		level;
    quint8		tmp;
    quint32		spriteUID;

public:
    MapTileExt(int lv, const mp2lev_t &, const QPair<QPixmap, QPoint> & pair);

    static bool		sortLevel1(const MapTileExt*, const MapTileExt*);
    static bool		sortLevel2(const MapTileExt*, const MapTileExt*);

    QPixmap &		pixmap(void) { return first; };
    QPoint &		offset(void) { return second; };

    static bool		isMapEvent(const MapTileExt*);
    static bool		isSphinx(const MapTileExt*);
    static bool		isSign(const MapTileExt*);
    static bool		isButtle(const MapTileExt*);
    static bool		isMiniHero(const MapTileExt*);
    static bool		isTown(const MapTileExt*);
    static bool		isRandomTown(const MapTileExt*);
};

class MapTile : public QGraphicsPixmapItem
{
public:
    MapTile(const mp2til_t &, const QPoint &, H2::Theme &);
    ~MapTile();

    QRectF		boundingRect(void) const;

    void		showInfo(void) const;
    int			groundType(void) const;

    static QString	indexString(int);

    const QPoint &	mapPos(void) const { return mpos; }
    void		paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = 0);
    void		loadSpriteLevels(const mp2ext_t &);
    void		loadSpriteLevel(QList<MapTileExt*> &, int, const mp2lev_t &);
    void		sortSpritesLevels(void);

    void		setTileSprite(int, int);

protected:
    friend class MapData;

    H2::Theme &		themeContent;
    mp2til_t		til;
    QPoint		mpos;

    QList<MapTileExt*>	spritesLevel1;
    QList<MapTileExt*>	spritesLevel2;

    quint16		passableBase;
    quint16		passableLocal;
};

class MapKey : public QPoint
{
public:
    MapKey(const QPoint & pos) : QPoint(pos) {}

    bool operator< (const QPoint & pt) const { return x() + y() < pt.x() + pt.y(); }
};

class MapData : public QGraphicsScene
{
    Q_OBJECT

public:
    MapData(MapWindow*);

    const QString &	name(void) const;
    const QString &	description(void) const;
    const QSize &	size(void) const;

    int			indexLimit(void) const;
    quint32		uniq(void);

    void		newMap(const QSize &, const QString &);
    bool		loadMap(const QString &);

    H2::Theme &		theme(void);

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

protected:
    void                mousePressEvent(QGraphicsSceneMouseEvent*);
    void                mouseMoveEvent(QGraphicsSceneMouseEvent*);
    void		drawForeground(QPainter*, const QRectF &);

    void		selectArea(QPointF, QPointF);
    bool		loadMP2Map(const QString &);
    bool		loadMapTiles(const QVector<mp2til_t> &, const QVector<mp2ext_t> &);

    const MapTile*	mapTileConst(const QPoint &) const;
    MapTile*		mapTile(const QPoint &);
    const MapTile*	mapTileFromDirectionConst(const MapTile*, int) const;
    MapTile*		mapTileFromDirection(const MapTile*, int);
    QPoint		positionExtBlockFromNumber(const QVector<mp2til_t> &, int) const;

    AroundGrounds	aroundGrounds(const MapTile*) const;

    friend class	MapTile;

    H2::Theme		themeContent;
    QSize		mapSize;

    QString		mapName;
    QString		mapDescription;
    QString		mapAuthors;
    QString		mapLicense;
    quint16		mapDifficulty;
    quint8		mapKingdomColor[6];
    quint8		mapHumanAllow[6];
    quint8		mapCompAllow[6];
    quint8		mapConditionWins;
    quint8		mapConditionWinsData1;
    quint8		mapConditionWinsData2;
    quint16		mapConditionWinsData3;
    quint16		mapConditionWinsData4;
    quint8		mapConditionLoss;
    quint16		mapConditionLossData1;
    quint16		mapConditionLossData2;
    bool		mapStartWithHero;
    quint8		mapRaceColor[6];
    quint32		mapUniq;

    QList<MapTile*>	tilesetItems;
    MapTile*		tileOverMouse;

    QMap<MapKey, QSharedPointer<H2::Object> >
			mapObjects;
    QVector<QSharedPointer<H2::DayEvent> >
			dayEvents;
    QVector<QSharedPointer<H2::Rumor> >
			tavernRumors;
};

#endif
