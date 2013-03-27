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

namespace Ground
{
    enum { Desert = 0x01, Snow = 0x02, Swamp = 0x04, Wasteland = 0x08, Beach = 0x10, Lava = 0x20, Dirt = 0x40, Grass = 0x80, Water = 0x100 };
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

namespace H2
{
    class Theme
    {
    protected:
	AGG::Spool &		aggSpool;
	QString			theme;
	QSize			tile;

    public:
	Theme(AGG::Spool & spool) : aggSpool(spool), theme("original"), tile(32, 32) {}

	QPixmap			getImageTIL(const QString & til, quint16 index) { return aggSpool.getImageTIL(til, index); }
	QPair<QPixmap, QPoint>	getImageICN(const QString & icn, quint16 index) { return aggSpool.getImageICN(icn, index); }

	const QSize &		tileSize(void) const { return tile; }
    };
}

#endif
