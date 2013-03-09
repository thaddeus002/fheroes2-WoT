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
#include <QString>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

#include "engine.h"

class MapTileExt : public QPair<QPixmap, QPoint>
{
public:
    MapTileExt(quint8 lv, quint32 uid, const QPair<QPixmap, QPoint> & pair);

    static bool		sortLevel1(const MapTileExt*, const MapTileExt*);
    static bool		sortLevel2(const MapTileExt*, const MapTileExt*);

    QPixmap &	pixmap;
    QPoint &	offset;

    quint32	uniq;
    quint8	level;
    quint8	tmp;
};

class MapTile : public QGraphicsPixmapItem
{
public:
    MapTile(const mp2til_t &, AGG::File &, const QPoint &);
    ~MapTile();

    bool		isValid(void) const;
    void		showInfo(void) const;

    static QString	indexString(int);

    void		paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = 0);
    void		loadSpriteLevels(const mp2ext_t &, AGG::File &);
    void		sortSpritesLevels(void);

protected:
    void		loadSpritelevel(QList<MapTileExt*> &, const mp2lev_t &, quint8, AGG::File &);

    int			spriteIndex;
    int			shape;

    QList<MapTileExt*>	spritesLevel1;
    QList<MapTileExt*>	spritesLevel2;
};

class MapData : public QGraphicsScene
{
    Q_OBJECT

public:
    MapData(AGG::File & agg) : aggContent(agg), modeView(1), currentGround(9) {}

    const QString &	name(void) const;
    const QString &	description(void) const;
    const QSize &	size(void) const;

    int			indexLimit(void) const;
    int         	sceneModeView(void) const;
    int         	sceneCurrentGround(void) const;

    void		newMap(const QSize &, const QString &);
    bool		loadMap(const QString &);

    void         	setSceneModeView(int);
    void         	setSceneCurrentGround(int);
signals:
    void		dataModified(void);

protected:
    void                mousePressEvent(QGraphicsSceneMouseEvent*);
    void                mouseReleaseEvent(QGraphicsSceneMouseEvent*);
    void                mouseMoveEvent(QGraphicsSceneMouseEvent*);

    void		selectArea(QPointF, QPointF);
    bool		loadMP2Map(const QString &);

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

    AGG::File &		aggContent;

    QSize		tilesetSize;
    QList<MapTile*>	tilesetItems;

    int			modeView; /* explore: 1, select: 2 */
    int			currentGround; /* desert: 1, snow: 2, swamp: 3, wasteland: 4, beach: 5, lava: 6, dirt: 7, grass: 8, water: 9 */
};

#endif
