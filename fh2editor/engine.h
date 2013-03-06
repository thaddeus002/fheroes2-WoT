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
		readBlock(size_t);

	qint16	readLE16(void);
	qint32	readLE32(void);

	mp2til_t readMP2Til(void);
	mp2ext_t readMP2Ext(void);
    };

    class ICNSprite : public QImage
    {
    public:
	ICNSprite(const mp2icn_t &, const char*, quint32, const QVector<QRgb> &);
    };

    QString mapICN(int);
}

namespace AGG
{
    struct Item
    {
	quint32 crc;
	quint32 offset;
	quint32 size;
    };

    class File : protected H2::File
    {
    protected:
	QFile			file;
	QMap<QString, Item>	items;
	QVector<QRgb>		colors;

	QByteArray	readRawData(const QString &);
	bool		loadFile(const QString &);

    public:
	File(const QString &);

	QPixmap getImageTIL(const QString &, quint16);
	QPair<QPixmap, QPoint> getImageICN(const QString &, quint16);
    };
}

#endif
