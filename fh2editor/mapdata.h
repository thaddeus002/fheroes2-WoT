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

class MapTile : public QGraphicsItem
{
public:
    MapTile(const QPoint &, const mp2til_t &, AGG::File &, const QPoint &);

    bool		isValid(void) const;
    static QString	indexString(int);

    QRectF		boundingRect(void) const;
    void		paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* = 0);

protected:
    void		mousePressEvent(QGraphicsSceneMouseEvent*);
    void		mouseMoveEvent(QGraphicsSceneMouseEvent*);

    QPoint		pos;

    int			sprite;
    int			shape;

    QPixmap		pixmapTile;
    QRectF		area;
};

class MapData : public QGraphicsScene
{
    Q_OBJECT

public:
    MapData(AGG::File & agg) : aggContent(agg) {}

    const QString &	name(void) const;
    const QString &	description(void) const;
    const QSize &	size(void) const;
    int			indexLimit(void) const;

    void		newMap(const QSize &, const QString &);
    bool		loadMap(const QString &);

signals:
    void		dataModified(void);

protected:
    bool		loadMP2Map(const QString &);

    void		mousePressEvent(QGraphicsSceneMouseEvent*);
    void		mouseMoveEvent(QGraphicsSceneMouseEvent*);

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
};

#endif
