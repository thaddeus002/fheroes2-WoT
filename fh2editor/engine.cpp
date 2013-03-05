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

#include <QDebug>
#include <QtEndian>

#include "program.h"
#include "engine.h"

H2::File::File()
{
}

H2::File::File(const QString & fn) : QFile(fn)
{
}

qint16 H2::File::readLE16(void)
{
    qint16 res = 0;

    if(pos() + sizeof(res) <= size())
    {
	readData((char*) &res, sizeof(res));
	res = qFromLittleEndian(res);
    }

    return res;
}

qint32 H2::File::readLE32(void)
{
    qint32 res = 0;

    if(pos() + sizeof(res) <= size())
    {
	readData((char*) &res, sizeof(res));
	res = qFromLittleEndian(res);
    }

    return res;
}

qint8 H2::File::readByte(void)
{
    qint8 res = 0;

    if(pos() + sizeof(res) <= size())
    {
	readData((char*) &res, sizeof(res));
    }

    return res;
}

QString H2::File::readString(size_t sz)
{
    return QString(readBlock(sz));
}

QByteArray H2::File::readBlock(size_t sz)
{
    QByteArray res;

    if(pos() + sz <= size())
    {
	res.reserve(sz);
	res.fill(0, sz);
	readData(res.data(), sz);
    }

    return res;
}

mp2til_t::mp2til_t()
{
    tileSprite = Rand(16, 19);
    objectName1 = 0;
    indexName1 = 0;
    quantity1 = 0;
    quantity2 = 0;
    objectName2 = 0;
    indexName2 = 0;
    tileShape = 0;
    tileObject = 0;
    indexExt = 0;
    uniq1 = 0;
    uniq2 = 0;
}

mp2til_t H2::File::readMP2Til(void)
{
    mp2til_t res;

    res.tileSprite = readLE16();
    res.objectName1 = readByte();
    res.indexName1 = readByte();
    res.quantity1 = readByte();
    res.quantity2 = readByte();
    res.objectName2 = readByte();
    res.indexName2 = readByte();
    res.tileShape = readByte();
    res.tileObject = readByte();
    res.indexExt = readLE16();
    res.uniq1 = readLE32();
    res.uniq2 = readLE32();

    return res;
}

mp2ext_t H2::File::readMP2Ext(void)
{
    mp2ext_t res;

    res.indexExt = readLE16();
    res.objectName1 = readByte();
    res.indexName1 = readByte();
    res.quantity = readByte();
    res.objectName2 = readByte();
    res.indexName2 = readByte();
    res.uniq1 = readLE32();
    res.uniq2 = readLE32();

    return res;
}

AGG::File::File(const QString & file)
{
    loadFile(file);
}

QByteArray AGG::File::readRawData(const QString & name)
{
    QByteArray res;
    QMap<QString, Item>::const_iterator it = items.find(name);

    if(items.end() != it)
    {
	res.resize((*it).size);

	seek((*it).offset);
	readData(res.data(), (*it).size);
    }
    else
	qCritical() << "Item" << qPrintable(name) << "not found";

    return res;
}

bool AGG::File::loadFile(const QString & fn)
{
    if(isOpen()) close();
    if(fn.isNull()) return false;

    setFileName(fn);
    if(open(QIODevice::ReadOnly))
    {
	qDebug() << "AGG::File::loadFile:" << qPrintable(fn);

	quint16 countItems = readLE16();

	qDebug() << "AGG::File::loadFile:" << "count items:" << countItems;

	const int sizeName = 15;
	char buf[sizeName + 1];

	for(int it = 0; it < countItems; ++it)
	{
	    seek(size() - sizeName * (countItems - it));

	    qFill(buf, buf + sizeName + 1, 0);
	    readData(buf, sizeName);

	    Item & item = items[QString(buf)];

	    seek(sizeof(countItems) + it * (3 * sizeof(quint32) /* crcItem + offsetItem + sizeItem */)); 

	    item.crc = readLE32();
	    item.offset = readLE32();
	    item.size = readLE32();

	    //qDebug() << "parse: " << buf << ", size: " << item.size;
	}

	// load palette
	QMap<QString, Item>::const_iterator pal = items.find("KB.PAL");

	if(items.end() != pal)
	{
	    char r, g, b;
	    const quint32 palSize = (*pal).size / 3;
	    colors.reserve(palSize);

	    seek((*pal).offset);

	    for(quint32 num = 0; num < palSize; ++num)
	    {
		readData(& r, 1);
		readData(& g, 1);
		readData(& b, 1);

		colors.push_back(qRgb(r << 2, g << 2, b << 2));
	    }

	    qDebug() << "AGG::File::loadFile:" << "loaded palette: " << colors.size();

	    return true;
	}
	else
	    qCritical() << "palette not found";
    }
    else
	qCritical() << "Can not read file " << qPrintable(fn);

    return false;
}

QPixmap AGG::File::getImageTIL(const QString & id, quint16 index)
{
    if(items.isEmpty())
	return NULL;

    QByteArray buf = readRawData(id + ".TIL");

    if(buf.size())
    {
	quint16 tileCount = qFromLittleEndian(*((quint16*) buf.data()));

	if(index < tileCount)
	{
	    quint16 tileWidth = qFromLittleEndian(*((quint16*) (buf.data() + 2)));
	    quint16 tileHeight = qFromLittleEndian(*((quint16*) (buf.data() + 4)));

	    QImage image((uchar*) buf.data() + 6 + index * tileWidth * tileHeight, tileWidth, tileHeight, QImage::Format_Indexed8);
	    image.setColorTable(colors);

	    return QPixmap::fromImage(image);
	}
	else
	    qCritical() << "Out of range: " << index;
    }

    return NULL;
}

quint32 Rand(quint32 min, quint32 max)
{
    if(min > max) qSwap(min, max);
    return min + Rand(max - min);
}

quint32 Rand(quint32 max)
{
    return static_cast<quint32>((max + 1) * (qrand() / (RAND_MAX + 1.0)));
}
