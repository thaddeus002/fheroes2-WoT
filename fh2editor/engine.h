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
#include <QMap>
#include <QDomElement>
#include <QMenu>

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

namespace ICN
{
    enum { UNKNOWN = 0, OBJNARTI = 0x2C, MONS32 = 0x30, FLAG32 = 0x38, MINIHERO = 0x54, MTNSNOW = 0x58, MTNSWMP = 0x5C, MTNLAVA = 0x60, MTNDSRT = 0x64, MTNDIRT = 0x68, MTNMULT = 0x6C,
	    EXTRAOVR = 0x74, ROAD = 0x78, MTNCRCK = 0x7C, MTNGRAS = 0x80, TREJNGL = 0x84, TREEVIL = 0x88, OBJNTOWN = 0x8C, OBJNTWBA = 0x90, OBJNTWSH = 0x94, OBJNTWRD = 0x98, OBJNWAT2 = 0xA0,
	    OBJNMUL2 = 0xA4, TRESNOW = 0xA8, TREFIR = 0xAC, TREFALL = 0xB0, STREAM = 0xB4, OBJNRSRC = 0xB8, OBJNGRA2 = 0xC0, TREDECI = 0xC4, OBJNWATR = 0xC8, OBJNGRAS = 0xCC, OBJNSNOW = 0xD0,
	    OBJNSWMP = 0xD4, OBJNLAVA = 0xD8, OBJNDSRT = 0xDC, OBJNDIRT = 0xE0, OBJNCRCK = 0xE4, OBJNLAV3 = 0xE8, OBJNMULT = 0xEC, OBJNLAV2 = 0xF0, X_LOC1 = 0xF4, X_LOC2 = 0xF8, X_LOC3 = 0xFC };
}

namespace SpriteLevel
{
    enum { Uknown, Bottom, Action, Top };

    int fromString(const QString &);
}

struct mp2icn_t
{
    mp2icn_t(const char*);

    quint16	offsetX;
    quint16	offsetY;
    quint16	width;
    quint16	height;
    quint8	type;	/* animation: 0x01, */
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

struct mp2town_t
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

struct mp2sphinx_t
{
    quint8	id; /* 0x00 */
    quint32	resources[7]; /* wood, mercury, ore, sulfur, crystal, gems, golds */
    quint16	artifact; /* 0xffff - none */
    quint8	answersCount;
    QVector<QString> answers; /* 8 blocks, 13 byte string */
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

QDataStream & operator>> (QDataStream &, mp2icn_t &);
QDataStream & operator>> (QDataStream &, mp2til_t &);
QDataStream & operator>> (QDataStream &, mp2ext_t &);
QDataStream & operator>> (QDataStream &, mp2town_t &);
QDataStream & operator>> (QDataStream &, mp2hero_t &);
QDataStream & operator>> (QDataStream &, mp2sign_t &);
QDataStream & operator>> (QDataStream &, mp2mapevent_t &);
QDataStream & operator>> (QDataStream &, mp2dayevent_t &);
QDataStream & operator>> (QDataStream &, mp2rumor_t &);
QDataStream & operator>> (QDataStream &, mp2sphinx_t &);

struct CompositeSprite
{
    int		spriteIndex;
    QPoint	spritePos;
    int		spriteLevel;
    int		spritePassable;
    int		spriteAnimation;

    CompositeSprite(){}
    CompositeSprite(const QDomElement &, int = -1);
};

struct CompositeObject : public QVector<CompositeSprite>
{
    QString	name;
    QSize	size;
    QPair<QString, int> icn;

    CompositeObject(){}
    CompositeObject(const QString &, const QDomElement &, int);

    bool	isValid(void) const;
};

Q_DECLARE_METATYPE(CompositeObject);


namespace Editor
{
    quint32 Rand(quint32 max);
    quint32 Rand(quint32 min, quint32 max);
    QPixmap pixmapBorder(const QSize &, const QColor &);
    QPixmap pixmapBorderPassable(const QSize &, int passable);

    class MyXML : public QDomElement
    {
    public:
	MyXML(const QString &, const QString &);
    };
}

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
    };

    class ICNSprite : public QImage
    {
    public:
	ICNSprite(const mp2icn_t &, const char*, quint32, const QVector<QRgb> &);

    private:
	void DrawVariant1(const quint8*, const quint8*, const QVector<QRgb> &);
	void DrawVariant2(const quint8*, const quint8*, const QVector<QRgb> &);
    };

    int          mapICN(const QString &);
    int          mapICN(int);

    QString      icnString(int);
    int          isAnimationICN(int, int, int);

    struct TownPos : QPair<mp2town_t, QPoint>
    {
	TownPos() {}
	TownPos(const mp2town_t & t, const QPoint & p) : QPair<mp2town_t, QPoint>(t, p) {}

	const mp2town_t & town(void) const { return first; }
	const QPoint & pos(void) const { return second; }
    };

    struct HeroPos : QPair<mp2hero_t, QPoint>
    {
	HeroPos() {}
	HeroPos(const mp2hero_t & t, const QPoint & p) : QPair<mp2hero_t, QPoint>(t, p) {}

	const mp2hero_t & hero(void) const { return first; }
	const QPoint & pos(void) const { return second; }
    };

    struct SignPos : QPair<mp2sign_t, QPoint>
    {
	SignPos() {}
	SignPos(const mp2sign_t & t, const QPoint & p) : QPair<mp2sign_t, QPoint>(t, p) {}

	const mp2sign_t & sign(void) const { return first; }
	const QPoint & pos(void) const { return second; }
    };

    struct EventPos : QPair<mp2mapevent_t, QPoint>
    {
	EventPos() {}
	EventPos(const mp2mapevent_t & t, const QPoint & p) : QPair<mp2mapevent_t, QPoint>(t, p) {}

	const mp2mapevent_t & event(void) const { return first; }
	const QPoint & pos(void) const { return second; }
    };

    struct SphinxPos : QPair<mp2sphinx_t, QPoint>
    {
	SphinxPos() {}
	SphinxPos(const mp2sphinx_t & t, const QPoint & p) : QPair<mp2sphinx_t, QPoint>(t, p) {}

	const mp2sphinx_t & sphinx(void) const { return first; }
	const QPoint & pos(void) const { return second; }
    };
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

	QPixmap			getImageTIL(const QString &, int, QVector<QRgb> &);
	QPair<QPixmap, QPoint>	getImageICN(const QString &, int, QVector<QRgb> &);
    };

    class Spool
    {
	AGG::File		first;
	AGG::File		second; /* first: heroes2.agg, second: heroes2x.agg */
	QVector<QRgb>           colors;
	QMap<QString, QPoint>	icnOffsetCache;

    public:
	Spool(const QString &);

	QPixmap			getImageTIL(const QString &, int);
	QPair<QPixmap, QPoint>	getImageICN(const QString &, int);
	QPixmap			getImage(const CompositeObject &, const QSize &);

	bool			isHeroes2XMode(void) const;
    };
}

class MapData;
class MapTile;
class MapTiles;

class AroundGrounds: public QVector<int>
{
public:
    AroundGrounds() : QVector<int>(9, Ground::Unknown){} /* ground: top left, top, top right, right, bottom right, bottom, bottom left, left, center */
    AroundGrounds(const MapTiles &, const QPoint &);

    int operator() (void) const;
    int groundsDirects(int) const;
    int directionsAroundGround(int) const;
};

class EditorTheme
{
protected:
    AGG::Spool &		aggSpool;
    QString			name;
    QSize			tile;

public:
    EditorTheme(AGG::Spool &);

    QPixmap			getImageTIL(const QString &, int);
    QPair<QPixmap, QPoint>	getImageICN(int, int);
    QPixmap			getImage(const CompositeObject &);

    const QSize &		tileSize(void) const;

    int				startFilledTile(int) const;
    int				startGroundTile(int) const;
    int				startFilledOriginalTile(int) const;
    int				startGroundOriginalTile(int) const;

    int				ground(int) const;
    QPair<int, int>		groundBoundariesFix(const MapTile &, const MapTiles &) const;

    QString			resourceFile(const QString & dir, const QString & file) const;
};

class MapObject : public QPoint
{
    int	uniq;

public:
    MapObject(const QPoint & pos, int id) : QPoint(pos), uniq(id) {}
    virtual ~MapObject() {}

    const int & uid(void) const { return uniq; }
};

class MapTown : public MapObject
{
public:
    MapTown(const QPoint & pos, quint32 id, const mp2town_t &);
};

class MapHero : public MapObject
{
public:
    MapHero(const QPoint & pos, quint32 id, const mp2hero_t &);
};

class MapSign : public MapObject
{
public:
    MapSign(const QPoint & pos, quint32 id, const mp2sign_t &);
};

class MapEvent : public MapObject
{
public:
    MapEvent(const QPoint & pos, quint32 id, const mp2mapevent_t &);
};

class MapSphinx : public MapObject
{
public:
    MapSphinx(const QPoint & pos, quint32 id, const mp2sphinx_t &);
};

class DayEvent
{
public:
    DayEvent(const mp2dayevent_t &);
};

class Rumor : public QString
{
public:
    Rumor(const mp2rumor_t &);
};

class MapKey : public QPoint
{
public:
    MapKey(const QPoint & pos) : QPoint(pos) {}

    bool operator< (const QPoint & pt) const { return x() + y() < pt.x() + pt.y(); }
};

class MapObjects : public QMap<MapKey, QSharedPointer<MapObject> >
{
public:
    MapObjects();
    MapObjects(const MapObjects &, const QRect &);
};

class DayEvents : public QVector<QSharedPointer<DayEvent> >
{
public:
    DayEvents();
};

class TavernRumors : public QVector<QSharedPointer<Rumor> >
{
public:
    TavernRumors();
};

struct CompositeObjectCursor : public CompositeObject
{
    QPoint		scenePos;
    QPoint		centerOffset;
    QPixmap		objectArea;
    QPixmap		passableMap;
    bool		valid;

    CompositeObjectCursor() : valid(false) {}
    CompositeObjectCursor(const CompositeObject &, EditorTheme &);

    void		reset(void);
    bool		isValid(void) const;
    void		paint(QPainter &, const QPoint &, bool allow);
    QRect		area(void) const;
    QPoint		center(void) const;
};

#endif
