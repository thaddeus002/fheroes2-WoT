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

#ifndef _EDITOR_ENGINE_H_
#define _EDITOR_ENGINE_H_

#include <QFile>
#include <QMap>
#include <QString>
#include <QPixmap>
#include <QByteArray>
#include <QPair>
#include <QVector>

namespace Ground
{
    enum { Unknown = 0, Desert = 0x01, Snow = 0x02, Swamp = 0x04, Wasteland = 0x08, Beach = 0x10, Lava = 0x20, Dirt = 0x40, Grass = 0x80, Water = 0x100,
	    All = Desert | Snow | Swamp | Wasteland | Beach | Lava | Dirt | Grass | Water };
}

namespace Direction
{
    enum { Unknown = 0x0000, TopLeft = 0x0001, Top = 0x0002, TopRight = 0x0004, Right = 0x0008, BottomRight = 0x0010, Bottom = 0x0020, BottomLeft = 0x0040, Left = 0x0080, Center = 0x0100,
	    TopRow = TopLeft | Top | TopRight, BottomRow = BottomLeft | Bottom | BottomRight, CenterRow = Left | Center | Right, All = TopRow | CenterRow | BottomRow };
}

struct mp2icn_t
{
    mp2icn_t(const char*);

    quint16	offsetX;
    quint16	offsetY;
    quint16	width;
    quint16	height;
    quint8	type;
    quint32	offsetData;

    static int	sizeOf(void) { return 13; };
};

struct mp2lev_t
{
    mp2lev_t();

    quint8	object;
    quint8	index;
    quint32	uniq;
};

struct mp2til_t
{
    mp2til_t();

    quint16     tileSprite;
    quint8      quantity1;
    quint8      quantity2;
    quint8      tileShape;
    quint8      tileObject;
    quint16     indexExt;
    mp2lev_t	level1;
    mp2lev_t	level2;
};

struct mp2ext_t
{
    mp2ext_t();

    quint16     indexExt;
    quint8      quantity;
    mp2lev_t	level1;
    mp2lev_t	level2;
};

struct mp2pos_t
{
    quint8	posx;
    quint8	posy;
    quint8	type;
};

struct mp2castle_t
{
    quint8	color;
    quint8	customBuilding;
    quint32	building;
    quint8	magicTower;
    quint8	customTroops;
    quint8	troopId[5];
    quint16	troopCount[5];
    quint8	captainPresent;
    quint8	customName;
    QString	name; /* string: 13 byte */
    quint8	race;
    quint8	forceTown;
    quint8      unknown1[29];
};

struct mp2hero_t
{
    quint8	unknown1;
    quint8	customTroops;
    quint8	troopId[5];
    quint16	troopCount[5];
    quint8	customPortrate;
    quint8	portrateType;
    quint8	artifacts[3];
    quint8	unknown2;
    quint32	exerience;
    quint8	customSkills;
    quint8	skillId[8];
    quint8	skillLevel[8];
    quint8	unknown3;
    quint8	customName;
    QString	name; /* string: 13 byte */
    quint8	patrol;
    quint8	patrolSquare;
    quint8      unknown4[15];
};

struct mp2sign_t
{
    quint8	id; /* 0x01 */
    quint8	zero[8];
    QString	text;
};

struct mp2mapevent_t
{
    quint8	id; /* 0x01 */
    quint32	resources[7]; /* wood, mercury, ore, sulfur, crystal, gems, golds */
    quint16	artifact; /* 0xffff - none */
    quint8	allowComputer;
    quint8	cancelAfterFirstVisit;
    quint8	zero[10];
    quint8	colors[6]; /* blue, green, red, yellow, orange, purple */
    QString	text;
};

struct mp2dayevent_t
{
    quint8	id; /* 0 */
    quint32	resources[7]; /* wood, mercury, ore, sulfur, crystal, gems, golds */
    quint16	artifact; /* always 0xffff - none */
    quint16	allowComputer;
    quint16	dayFirstOccurent;
    quint16	subsequentOccurrences;
    quint8	zero[6];
    quint8	colors[6]; /* blue, green, red, yellow, orange, purple */
    QString	text;
};

struct mp2rumor_t
{
    quint8	id; /* 0 */
    quint8	zero[7];
    QString	text;
};

struct mp2sphinx_t
{
    quint8	id; /* 0x00 */
    quint32	resources[7]; /* wood, mercury, ore, sulfur, crystal, gems, golds */
    quint16	artifact; /* 0xffff - none */
    quint8	answersCount;
    QVector<QString> answers; /* 8 blocks, 13 byte string */
    QString	text;
};

QDataStream & operator>> (QDataStream &, mp2castle_t &);
QDataStream & operator>> (QDataStream &, mp2hero_t &);
QDataStream & operator>> (QDataStream &, mp2sign_t &);
QDataStream & operator>> (QDataStream &, mp2mapevent_t &);
QDataStream & operator>> (QDataStream &, mp2dayevent_t &);
QDataStream & operator>> (QDataStream &, mp2rumor_t &);
QDataStream & operator>> (QDataStream &, mp2sphinx_t &);

quint32 Rand(quint32 max);
quint32 Rand(quint32 min, quint32 max);

namespace H2
{
    class File : public QFile
    {
    public:
	File();
	File(const QString &);

	qint8	readByte(void);
	QString	readString(size_t);
	QByteArray
		readBlock(size_t, int = -1);

	qint16	readLE16(void);
	qint32	readLE32(void);

	mp2til_t readMP2Til(void);
	mp2ext_t readMP2Ext(void);
    };

    class ICNSprite : public QImage
    {
    public:
	ICNSprite(const mp2icn_t &, const char*, quint32, const QVector<QRgb> &);

    private:
	void DrawVariant1(const quint8*, const quint8*, const QVector<QRgb> &);
	void DrawVariant2(const quint8*, const quint8*, const QVector<QRgb> &);
    };

    QString      mapICN(int);
    int          isAnimationICN(const mp2lev_t &, int);
}

namespace AGG
{
    struct Item
    {
	quint32 crc;
	quint32 offset;
	quint32 size;
    };

    class File : public H2::File
    {
    protected:
	QMap<QString, Item>	items;

    public:
	File(){}

	QByteArray		readRawData(const QString &);

	bool			loadFile(const QString &);
	bool			exists(const QString &) const;

	QPixmap			getImageTIL(const QString &, quint16, QVector<QRgb> &);
	QPair<QPixmap, QPoint>	getImageICN(const QString &, quint16, QVector<QRgb> &);
    };

    class Spool
    {
	AGG::File		first;
	AGG::File		second; /* first: heroes2.agg, second: heroes2x.agg */
	QVector<QRgb>           colors;

    public:
	Spool(const QString &);

	QPixmap			getImageTIL(const QString &, quint16);
	QPair<QPixmap, QPoint>	getImageICN(const QString &, quint16);
    };
}

class AroundGrounds: public QVector<int>
{
public:
    AroundGrounds() : QVector<int>(9, Ground::Unknown){} /* ground: top left, top, top right, right, bottom right, bottom, bottom left, left, center */

    int operator() (void) const;
    int groundsDirects(int) const;
    int aroundGround(int) const;
};

namespace H2
{
    class Theme
    {
    protected:
	AGG::Spool &		aggSpool;
	QString			name;
	QSize			tile;

    public:
	Theme(AGG::Spool &);

	QPixmap			getImageTIL(const QString &, quint16);
	QPair<QPixmap, QPoint>	getImageICN(const QString &, quint16);

	const QSize &		tileSize(void) const;

	int			startFilledTile(int) const;
	int			startGroundTile(int) const;
	int			startFilledOriginalTile(int) const;
	int			startGroundOriginalTile(int) const;

	int			ground(int) const;
	QPair<int, int>		indexGroundRotateFix(const AroundGrounds &, int) const;
    };

}

#endif
