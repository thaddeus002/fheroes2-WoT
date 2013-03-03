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

#include <QString>
#include <QVector>
#include <QList>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

#include "engine.h"

class MapTile
{
public:
    int				index;
    int				sprite;
    int				shape;


    MapTile();
    MapTile(int, int, int);

    bool			isValid(void) const;
    QGraphicsPixmapItem* 	loadItem(QPixmap, const QSize &, const QPoint &);

    static QString		indexString(int);
};

class MapData : public QGraphicsScene
{
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

    QVector<MapTile>	tilesContent;
    AGG::File		aggContent;

    QSize			tilesetSize;
    QList<QGraphicsItem*>	tilesetItems;
    QGraphicsItemGroup*		tilesetItemsGroup;

public:
    MapData();
    ~MapData();

    const QString &	Name(void) const;
    const QString &	Description(void) const;
    const QSize &	Size(void) const;
    int			indexLimit(void) const;

    bool		loadMap(const QString &);

signals:
    void		dataModified(void);

protected:
    bool		loadMP2Map(const QString &);
};

#endif
