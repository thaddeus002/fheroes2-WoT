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

#include <QtEndian>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QPixmapCache>
#include <QDomDocument>
#include <QPainter>

#include "program.h"
#include "engine.h"
#include "mapdata.h"

QString readStringFromStream(QDataStream & ds, int count = 0)
{
    QString str;
    quint8 byte;

    if(count)
    {
	str.reserve(128);

	for(int ii = 0; ii < count; ++ii)
	{
	    ds >> byte;
	    str.push_back(byte);
	}
    }
    else
    {
	str.reserve(512);

	while(! ds.atEnd())
	{
	    ds >> byte;
	    str.push_back(byte);
	    if(0 == byte) break;
	}
    }

    return str;
}

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
	read((char*) &res, sizeof(res));
	res = qFromLittleEndian(res);
    }
    else
	qWarning() << "H2::File::readLE16:" << "out of range";

    return res;
}

qint32 H2::File::readLE32(void)
{
    qint32 res = 0;

    if(pos() + sizeof(res) <= size())
    {
	read((char*) &res, sizeof(res));
	res = qFromLittleEndian(res);
    }
    else
	qWarning() << "H2::File::readLE32:" << "out of range";

    return res;
}

qint8 H2::File::readByte(void)
{
    qint8 res = 0;

    if(pos() + sizeof(res) <= size())
    {
	read((char*) &res, 1);
    }
    else
	qWarning() << "H2::File::readByte:" << "out of range";

    return res;
}

QString H2::File::readString(size_t sz)
{
    return QString(readBlock(sz));
}

QByteArray H2::File::readBlock(size_t sz, int ps)
{
    if(0 <= ps)
	seek(ps);

    if(pos() + sz <= size())
	return read(sz);
    else
	qWarning() << "H2::File::readBlock:" << "out of range";

    return NULL;
}

H2::ICNSprite::ICNSprite(const mp2icn_t & icn, const char* buf, quint32 size, const QVector<QRgb> & pals)
    : QImage(icn.width, icn.height, QImage::Format_ARGB32)
{
    const quint8* ptr = (const quint8*) buf;
    const quint8* outOfRange = ptr + size;

    fill(Qt::transparent);

    if(0x20 == icn.type)
	DrawVariant2(ptr, outOfRange, pals);
    else
	DrawVariant1(ptr, outOfRange, pals);
}

void H2::ICNSprite::DrawVariant1(const quint8* ptr, const quint8* outOfRange, const QVector<QRgb> & pals)
{
    int col = 0;
    int posX = 0;
    int posY = 0;

    QRgb shadow = qRgba(0, 0, 0, 0x40);

    while(1)
    {
        // 0x00 - end line
        if(0 == *ptr)
        {
            posY++;
            posX = 0;
            ptr++;
        }
        else
        // 0x7F - count data
        if(0x80 > *ptr)
        {
            col = *ptr;
            ptr++;
            while(col-- && ptr < outOfRange)
            {
                setPixel(posX, posY, pals[*ptr]);
                posX++;
                ptr++;
            }
        }
        else
        // 0x80 - end data
        if(0x80 == *ptr)
        {
            break;
        }
        else
        // 0xBF - skip data
        if(0xC0 > *ptr)
        {
            posX += *ptr - 0x80;
            ptr++;
        }
        else
        // 0xC0 - shadow
        if(0xC0 == *ptr)
        {
            ptr++;
            col = *ptr % 4 ? *ptr % 4 : *(++ptr);
            while(col--){ setPixel(posX, posY, shadow); posX++; }
            ptr++;
        }
        else
        // 0xC1
        if(0xC1 == *ptr)
        {
            ptr++;
            col = *ptr;
            ptr++;
            while(col--){ setPixel(posY, posY, pals[*ptr]); posX++; }
            ptr++;
        }
        else
        {
            col = *ptr - 0xC0;
            ptr++;
            while(col--){ setPixel(posX, posY, pals[*ptr]); posX++; }
            ptr++;
        }

        if(ptr >= outOfRange)
        {
            qWarning() << "H2::ICNSprite:DrawVariant1:" << "parse out of range";
            break;
        }
    }
}

void H2::ICNSprite::DrawVariant2(const quint8* ptr, const quint8* outOfRange, const QVector<QRgb> & pals)
{
    int col = 0;
    int posX = 0;
    int posY = 0;

    while(1)
    {
        // 0x00 - end line
        if(0 == *ptr)
        {
            posY++;
            posX = 0;
            ptr++;
        }
        else
        // 0x7F - count data
        if(0x80 > *ptr)
        {
	    col = *ptr;
            while(col--) { setPixel(posX, posY, pals[1]); posX++; }
            ptr++;
        }
        else
        // 0x80 - end data
        if(0x80 == *ptr)
        {
            break;
        }
        else
        // other - skip data
        {
	    posX += *ptr - 0x80;
            ptr++;
        }

        if(ptr >= outOfRange)
        {
            qWarning() << "H2::ICNSprite:DrawVariant2:" << "parse out of range";
            break;
        }
    }
}


mp2icn_t::mp2icn_t(const char* data)
{
    QByteArray buf(data, sizeOf());
    QDataStream ds(buf);
    ds >> *this;
}

QDataStream & operator>> (QDataStream & ds, mp2icn_t & icn)
{
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> icn.offsetX >> icn.offsetY >>
	icn.width >> icn.height >> icn.type >> icn.offsetData;
    return ds;
}

mp2lev_t::mp2lev_t()
{
    object = 0;
    index = 0;
    uniq = 0;
}

mp2til_t::mp2til_t()
{
    tileSprite = Editor::Rand(16, 19);
    quantity1 = 0;
    quantity2 = 0;
    tileShape = 0;
    tileObject = 0;
    indexExt = 0;
}

mp2ext_t::mp2ext_t()
{
    indexExt = 0;
    quantity = 0;
}

QDataStream & operator>> (QDataStream & ds, mp2til_t & til)
{
    ds.setByteOrder(QDataStream::LittleEndian);
    return ds >> til.tileSprite >>
	til.level1.object >> til.level1.index >>
	til.quantity1 >> til.quantity2 >>
	til.level2.object >> til.level2.index >>
	til.tileShape >> til.tileObject >>
	til.indexExt >> til.level1.uniq >> til.level2.uniq;
}

QDataStream & operator>> (QDataStream & ds, mp2ext_t & ext)
{
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> ext.indexExt >> ext.level1.object >> ext.level1.index >>
	ext.quantity >> ext.level2.object >> ext.level2.index >> ext.level1.uniq >> ext.level2.uniq;
    ext.level1.object *= 2;
    return ds;
}

QDataStream & operator>> (QDataStream & ds, mp2town_t & cstl)
{
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> cstl.color >> cstl.customBuilding >> cstl.building >> cstl.magicTower >> cstl.customTroops;

    for(int ii = 0; ii < 5; ++ii)
	ds >> cstl.troopId[ii];

    for(int ii = 0; ii < 5; ++ii)
	ds >> cstl.troopCount[ii];

    ds >> cstl.captainPresent >> cstl.customName;

    cstl.name = readStringFromStream(ds, 13);

    ds >> cstl.race >> cstl.forceTown;
    return ds;
}

QDataStream & operator>> (QDataStream & ds, mp2hero_t & hero)
{
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> hero.unknown1 >> hero.customTroops;

    for(int ii = 0; ii < 5; ++ii)
	ds >> hero.troopId[ii];

    for(int ii = 0; ii < 5; ++ii)
	ds >> hero.troopCount[ii];

    ds >> hero.customPortrate >> hero.portrateType;

    for(int ii = 0; ii < 3; ++ii)
	ds >> hero.artifacts[ii];

    ds >> hero.unknown2 >> hero.exerience >> hero.customSkills;

    for(int ii = 0; ii < 8; ++ii)
	ds >> hero.skillId[ii];

    for(int ii = 0; ii < 8; ++ii)
	ds >> hero.skillLevel[ii];

    ds >> hero.unknown3 >> hero.customName;

    hero.name = readStringFromStream(ds, 13);

    ds >> hero.patrol >> hero.patrolSquare;
    return ds;
}

QDataStream & operator>> (QDataStream & ds, mp2sign_t & sign)
{
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> sign.id;

    for(int ii = 0; ii < 8; ++ii)
	ds >> sign.zero[ii];

    sign.text = readStringFromStream(ds);
    return ds;
}

QDataStream & operator>> (QDataStream & ds, mp2mapevent_t & evnt)
{
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> evnt.id;

    for(int ii = 0; ii < 7; ++ii)
	ds >> evnt.resources[ii];

    ds >> evnt.artifact >> evnt.allowComputer >> evnt.cancelAfterFirstVisit;

    for(int ii = 0; ii < 10; ++ii)
	ds >> evnt.zero[ii];

    for(int ii = 0; ii < 6; ++ii)
	ds >> evnt.colors[ii];

    evnt.text = readStringFromStream(ds);
    return ds;
}

QDataStream & operator>> (QDataStream & ds, mp2dayevent_t & evnt)
{
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> evnt.id;

    for(int ii = 0; ii < 7; ++ii)
	ds >> evnt.resources[ii];

    ds >> evnt.artifact >> evnt.allowComputer >> evnt.dayFirstOccurent >> evnt.subsequentOccurrences;

    for(int ii = 0; ii < 6; ++ii)
	ds >> evnt.zero[ii];

    for(int ii = 0; ii < 6; ++ii)
	ds >> evnt.colors[ii];

    evnt.text = readStringFromStream(ds);
    return ds;
}

QDataStream & operator>> (QDataStream & ds, mp2rumor_t & rumor)
{
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> rumor.id;

    for(int ii = 0; ii < 7; ++ii)
	ds >> rumor.zero[ii];

    rumor.text = readStringFromStream(ds);
    return ds;
}

QDataStream & operator>> (QDataStream & ds, mp2sphinx_t & sphinx)
{
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> sphinx.id;

    for(int ii = 0; ii < 7; ++ii)
	ds >> sphinx.resources[ii];

    ds >> sphinx.artifact >> sphinx.answersCount;

    for(int ii = 0; ii < 8; ++ii)
    {
	QString str = readStringFromStream(ds, 13);

	if(! str.isEmpty())
	    sphinx.answers.push_back(str);
    }

    sphinx.text = readStringFromStream(ds);
    return ds;
}


bool AGG::File::exists(const QString & str) const
{
    return items.end() != items.find(str);
}

QByteArray AGG::File::readRawData(const QString & name)
{
    QMap<QString, Item>::const_iterator it = items.find(name);

    if(items.end() != it)
	return readBlock((*it).size, (*it).offset);
    else
	qCritical() << "AGG::File::readRawData:" << "item" << qPrintable(name) << "not found";

    return NULL;
}

bool AGG::File::loadFile(const QString & fn)
{
    if(isOpen()) close();
    if(fn.isEmpty()) return false;

    setFileName(fn);
    if(open(QIODevice::ReadOnly))
    {
	qDebug() << "AGG::File::loadFile:" << qPrintable(fn);
	quint16 countItems = readLE16();

	qDebug() << "AGG::File::loadFile:" << "count items:" << countItems;
	const int sizeName = 15;

	for(int it = 0; it < countItems; ++it)
	{
	    int posname = size() - sizeName * (countItems - it);

	    Item & item = items[QString(readBlock(sizeName, posname))];

	    seek(sizeof(countItems) + it * (3 * sizeof(quint32) /* crcItem + offsetItem + sizeItem */)); 

	    item.crc = readLE32();
	    item.offset = readLE32();
	    item.size = readLE32();
	}

	return true;
    }
    else
	qCritical() << "AGG::File::loadFile:" << "Can not read file " << qPrintable(fn);

    return false;
}

QPixmap AGG::File::getImageTIL(const QString & id, int index, QVector<QRgb> & colors)
{
    QByteArray buf = readRawData(id);

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
	    qCritical() << "AGG::File::getImageTIL:" << "out of range" << index;
    }

    return NULL;
}

QPair<QPixmap, QPoint> AGG::File::getImageICN(const QString & id, int index, QVector<QRgb> & colors)
{
    QPair<QPixmap, QPoint> result;
    QByteArray buf = readRawData(id);

    if(buf.size())
    {
	quint16 icnCount = qFromLittleEndian(*((quint16*) buf.data()));

	if(index < icnCount)
	{
	    mp2icn_t header(buf.data() + 6 + index * mp2icn_t::sizeOf());
	    quint32 sizeData = 0;

	    if(index + 1 < icnCount)
	    {
		mp2icn_t headerNext(buf.data() + 6 + (index + 1) * mp2icn_t::sizeOf());
		sizeData = headerNext.offsetData - header.offsetData;
	    }
	    else
		sizeData = qFromLittleEndian(*((quint32*) (buf.data() + 2))) - header.offsetData;

	    H2::ICNSprite image(header, buf.data() + 6 + header.offsetData, sizeData, colors);

	    result.first = QPixmap::fromImage(image);
	    result.second = QPoint(header.offsetX, header.offsetY);
	}
	else
	    qCritical() << "AGG::File::getImageICN:" << "out of range" << index;
    }

    return result;
}


AGG::Spool::Spool(const QString & file)
{
    if(first.loadFile(file))
    {
	QStringList list = QFileInfo(file).absoluteDir().entryList(QStringList() << "heroes2x.agg", QDir::Files | QDir::Readable);

	if(list.size())
	    second.loadFile(QDir::toNativeSeparators(QFileInfo(file).absolutePath() + QDir::separator() + list.front()));

	// load palette
	QByteArray array = first.readRawData("KB.PAL");

	if(array.size())
	{
	    qint8 r, g, b;
	    const quint32 palSize = array.size() / 3;
	    colors.reserve(palSize);

	    for(quint32 num = 0; num < palSize; ++num)
	    {
		r = array[num * 3];
		g = array[num * 3 + 1];
		b = array[num * 3 + 2];

		colors.push_back(qRgb(r << 2, g << 2, b << 2));
	    }

	    qDebug() << "AGG::Spool:" << "loaded palette: " << colors.size();
	}
	else
	    qCritical() << "AGG::Spool:" << "palette not found";
    }
}

bool AGG::Spool::isHeroes2XMode(void) const
{
    return second.isReadable();
}

QPixmap AGG::Spool::getImageTIL(const QString & til, int index)
{
    QString key = til + QString::number(index);
    QPixmap result = NULL;

    if(! QPixmapCache::find(key, & result))
    {
	result = second.isReadable() && second.exists(til) ?
		second.getImageTIL(til, index, colors) : first.getImageTIL(til, index, colors);
	QPixmapCache::insert(key, result);
    }

    return result;
}

QPair<QPixmap, QPoint> AGG::Spool::getImageICN(const QString & icn, int index)
{
    QString key = icn + QString::number(index);
    QPair<QPixmap, QPoint> result;

    if(! QPixmapCache::find(key, & result.first))
    {
	result = second.isReadable() && second.exists(icn) ?
		second.getImageICN(icn, index, colors) : first.getImageICN(icn, index, colors);
	QPixmapCache::insert(key, result.first);
	icnOffsetCache[key] = result.second;
    }
    else
    {
	result.second = icnOffsetCache[key];
    }

    return result;
}

QPixmap AGG::Spool::getImage(const CompositeObject & obj, const QSize & tileSize)
{
    const QString key = obj.icn + obj.name;
    QPixmap result = NULL;

    if(! QPixmapCache::find(key, & result))
    {
	int width = obj.size.width() * tileSize.width();
	int height = obj.size.height() * tileSize.height();

	result = QPixmap(width, height);
	result.fill(Qt::transparent);
	QPainter paint(& result);

	for(QVector<CompositeSprite>::const_iterator
	    it = obj.begin(); it != obj.end(); ++it)
	{
	    QPair<QPixmap, QPoint> sprite = getImageICN(obj.icn, (*it).spriteIndex);
	    paint.drawPixmap((*it).spritePos.x() * tileSize.width() + sprite.second.x(), (*it).spritePos.y() * tileSize.height() + sprite.second.y(), sprite.first);

	    if((*it).spriteAnimation)
	    {
		sprite = getImageICN(obj.icn, (*it).spriteIndex + 1);
		paint.drawPixmap((*it).spritePos.x() * tileSize.width() + sprite.second.x(), (*it).spritePos.y() * tileSize.height() + sprite.second.y(), sprite.first);
	    }
	}

	QPixmapCache::insert(key, result);
    }

    return result;
}

quint32 Editor::Rand(quint32 min, quint32 max)
{
    if(min > max) qSwap(min, max);
    return min + Rand(max - min);
}

quint32 Editor::Rand(quint32 max)
{
    return static_cast<quint32>((max + 1) * (qrand() / (RAND_MAX + 1.0)));
}

QPixmap Editor::pixmapBorder(const QSize & size, const QColor & color, int offset)
{
    QPixmap result(size);
    result.fill(Qt::transparent);

    QPainter paint(& result);
    paint.setPen(QPen(color, 1));
    paint.setBrush(QBrush(QColor(0, 0, 0, 0)));
    paint.drawRect(offset, offset, size.width() - 2 * offset - 1, size.height() - 2 * offset - 1);

    return result;
}

Editor::MyXML::MyXML(const QString & xml, const QString & root)
{
    QDomDocument dom;
    QFile file(xml);

    if(file.open(QIODevice::ReadOnly))
    {
        QString errorStr;
        int errorLine;
        int errorColumn;

	if(! dom.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
    	    qDebug() << "MyXML:" << xml << errorStr << errorLine << errorColumn;
    }
    else
	qDebug() << "MyXML:" << "error open:" << xml;

    file.close();

    static_cast<QDomElement &>(*this) = dom.documentElement();

    if(tagName() != root)
    {
	qDebug() << "MyXML:" << xml << "unknown tag:" << tagName();
	clear();
    }
}

int H2::mapICN(int type)
{
    switch(type & 0xFC)
    {
	// artifact
	case 0x2C: return ICN::OBJNARTI;
	// monster
	case 0x30: return ICN::MONS32;
	// castle flags
	case 0x38: return ICN::FLAG32;
	// heroes
	case 0x54: return ICN::MINIHERO;
	// relief: snow
	case 0x58: return ICN::MTNSNOW;
	// relief: swamp
	case 0x5C: return ICN::MTNSWMP;
	// relief: lava
	case 0x60: return ICN::MTNLAVA;
	// relief: desert
	case 0x64: return ICN::MTNDSRT;
	// relief: dirt
	case 0x68: return ICN::MTNDIRT;
	// relief: others
	case 0x6C: return ICN::MTNMULT;
	// mines
	case 0x74: return ICN::EXTRAOVR;
	// road
	case 0x78: return ICN::ROAD;
	// relief: crck
	case 0x7C: return ICN::MTNCRCK;
	// relief: gras
	case 0x80: return ICN::MTNGRAS;
	// trees jungle
	case 0x84: return ICN::TREJNGL;
	// trees evil
	case 0x88: return ICN::TREEVIL;
	// castle and tower
	case 0x8C: return ICN::OBJNTOWN;
	// castle lands
	case 0x90: return ICN::OBJNTWBA;
	// castle shadow
	case 0x94: return ICN::OBJNTWSH;
	// random castle
	case 0x98: return ICN::OBJNTWRD;
	// water object
	case 0xA0: return ICN::OBJNWAT2;
	// object other
	case 0xA4: return ICN::OBJNMUL2;
	// trees snow
	case 0xA8: return ICN::TRESNOW;
	// trees trefir
	case 0xAC: return ICN::TREFIR;
	// trees
	case 0xB0: return ICN::TREFALL;
	// river
	case 0xB4: return ICN::STREAM;
	// resource
	case 0xB8: return ICN::OBJNRSRC;
	// gras object
	case 0xC0: return ICN::OBJNGRA2;
	// trees tredeci
	case 0xC4: return ICN::TREDECI;
	// sea object
	case 0xC8: return ICN::OBJNWATR;
        // vegetation gras                                            
        case 0xCC: return ICN::OBJNGRAS;
	// object on snow                                             
	case 0xD0: return ICN::OBJNSNOW;
        // object on swamp                                            
        case 0xD4: return ICN::OBJNSWMP;
	// object on lava                                             
	case 0xD8: return ICN::OBJNLAVA;
        // object on desert                                           
        case 0xDC: return ICN::OBJNDSRT;
        // object on dirt                                             
	case 0xE0: return ICN::OBJNDIRT;
	// object on crck
	case 0xE4: return ICN::OBJNCRCK;
	// object on lava
	case 0xE8: return ICN::OBJNLAV3;
	// object on earth
	case 0xEC: return ICN::OBJNMULT;
	//  object on lava                                            
	case 0xF0: return ICN::OBJNLAV2;
	// extra objects for loyalty version
	case 0xF4: return ICN::X_LOC1;
	// extra objects for loyalty version
	case 0xF8: return ICN::X_LOC2;
	// extra objects for loyalty version
	case 0xFC: return ICN::X_LOC3;
	// unknown
	default: qWarning() << "H2::mapICN:unknown object:" << type; break;
    }

    return ICN::UNKNOWN;
}

QString H2::icnString(int type)
{
    switch(type)
    {
	case ICN::OBJNARTI: return "OBJNARTI.ICN";
	case ICN::MONS32: return "MONS32.ICN";
	case ICN::FLAG32: return "FLAG32.ICN";
	case ICN::MINIHERO: return "MINIHERO.ICN";
	case ICN::MTNSNOW: return "MTNSNOW.ICN";
	case ICN::MTNSWMP: return "MTNSWMP.ICN";
	case ICN::MTNLAVA: return "MTNLAVA.ICN";
	case ICN::MTNDSRT: return "MTNDSRT.ICN";
	case ICN::MTNDIRT: return "MTNDIRT.ICN";
	case ICN::MTNMULT: return "MTNMULT.ICN";
	case ICN::EXTRAOVR: return "EXTRAOVR.ICN";
	case ICN::ROAD: return "ROAD.ICN";
	case ICN::MTNCRCK: return "MTNCRCK.ICN";
	case ICN::MTNGRAS: return "MTNGRAS.ICN";
	case ICN::TREJNGL: return "TREJNGL.ICN";
	case ICN::TREEVIL: return "TREEVIL.ICN";
	case ICN::OBJNTOWN: return "OBJNTOWN.ICN";
	case ICN::OBJNTWBA: return "OBJNTWBA.ICN";
	case ICN::OBJNTWSH: return "OBJNTWSH.ICN";
	case ICN::OBJNTWRD: return "OBJNTWRD.ICN";
	case ICN::OBJNWAT2: return "OBJNWAT2.ICN";
	case ICN::OBJNMUL2: return "OBJNMUL2.ICN";
	case ICN::TRESNOW: return "TRESNOW.ICN";
	case ICN::TREFIR: return "TREFIR.ICN";
	case ICN::TREFALL: return "TREFALL.ICN";
	case ICN::STREAM: return "STREAM.ICN";
	case ICN::OBJNRSRC: return "OBJNRSRC.ICN";
	case ICN::OBJNGRA2: return "OBJNGRA2.ICN";
	case ICN::TREDECI: return "TREDECI.ICN";
	case ICN::OBJNWATR: return "OBJNWATR.ICN";
        case ICN::OBJNGRAS: return "OBJNGRAS.ICN";
	case ICN::OBJNSNOW: return "OBJNSNOW.ICN";
        case ICN::OBJNSWMP: return "OBJNSWMP.ICN";
	case ICN::OBJNLAVA: return "OBJNLAVA.ICN";
        case ICN::OBJNDSRT: return "OBJNDSRT.ICN";
	case ICN::OBJNDIRT: return "OBJNDIRT.ICN";
	case ICN::OBJNCRCK: return "OBJNCRCK.ICN";
	case ICN::OBJNLAV3: return "OBJNLAV3.ICN";
	case ICN::OBJNMULT: return "OBJNMULT.ICN";
	case ICN::OBJNLAV2: return "OBJNLAV2.ICN";
	case ICN::X_LOC1: return "X_LOC1.ICN";
	case ICN::X_LOC2: return "X_LOC2.ICN";
	case ICN::X_LOC3: return "X_LOC3.ICN";
	// unknown
	default: qWarning() << "H2::icnString: return NULL, object:" << type; break;
    }

    return NULL;
}

int H2::isAnimationICN(int spriteClass, int spriteIndex, int ticket)
{
    switch(spriteClass)
    {
	case ICN::OBJNLAVA:
	    switch(spriteIndex)
	    {
		// shadow of lava
		case 0x4F: case 0x58: case 0x62:
		    return spriteIndex + (ticket % 9) + 1;
		default: break;
	    }
	    break;

	case ICN::OBJNLAV2:
	    switch(spriteIndex)
	    {
		// middle volcano
		case 0x00:
		// shadow
		case 0x07: case 0x0E:
		// lava
		case 0x15:
		    return spriteIndex + (ticket % 6) + 1;
		// small volcano
		// shadow
		case 0x21: case 0x2C:
		// lava
		case 0x37: case 0x43:
		    return spriteIndex + (ticket % 10) + 1;
		default: break;
	    }
	    break;

	case ICN::OBJNLAV3:
	    // big volcano
	    switch(spriteIndex)
	    {
		// smoke
		case 0x00: case 0x0F: case 0x1E: case 0x2D: case 0x3C: case 0x4B: case 0x5A: case 0x69: case 0x87: case 0x96: case 0xA5:
		// shadow
		case 0x78: case 0xB4: case 0xC3: case 0xD2: case 0xE1:
		    return spriteIndex + (ticket % 14) + 1;
		default: break;
	    }
	    break;

	case ICN::OBJNMUL2:
	    switch(spriteIndex)
	    {
		// lighthouse
		case 0x3D:
		    return spriteIndex + (ticket % 9) + 1;
		// alchemytower
		case 0x1B:
		// watermill
		case 0x53: case 0x5A: case 0x62: case 0x69:
		// fire in wagoncamp
		case 0x81:
		// smoke smithy (2 chimney)
		case 0xA6:
		// smoke smithy (1 chimney)
		case 0xAD:
		// shadow smoke
		case 0xB4:
		    return spriteIndex + (ticket % 6) + 1;
		// magic garden
		case 0xBE:
		    return spriteIndex + (ticket % 6) + 1;
		default: break;
	    }
	    break;

	case ICN::OBJNSNOW:
	    switch(spriteIndex)
	    {
		// firecamp
		case 0x04:
		// alchemytower
		case 0x97:
		// watermill
		case 0xA2: case 0xA9: case 0xB1: case 0xB8:
		    return spriteIndex + (ticket % 6) + 1;
		// mill
		case 0x60: case 0x64: case 0x68: case 0x6C: case 0x70: case 0x74: case 0x78: case 0x7C: case 0x80: case 0x84:
		    return spriteIndex + (ticket % 3) + 1;
		default: break;
	    }
	    break;

	case ICN::OBJNSWMP:
	    switch(spriteIndex)
	    {
		// shadow
		case 0x00: case 0x0E: case 0x2B:
		// smoke
		case 0x07: case 0x22: case 0x33:
		// light in window
		case 0x16: case 0x3A: case 0x43: case 0x4A:
		    return spriteIndex + (ticket % 6) + 1;
		default: break;
	    }
	    break;

	case ICN::OBJNDSRT:
	    switch(spriteIndex)
	    {
		// campfire
		case 0x36: case 0x3D:
		    return spriteIndex + (ticket % 6) + 1;
		default: break;
	    }
	    break;

	case ICN::OBJNGRA2:
	    switch(spriteIndex)
	    {
		// mill
		case 0x17: case 0x1B: case 0x1F: case 0x23: case 0x27: case 0x2B: case 0x2F: case 0x33: case 0x37: case 0x3B:
		    return spriteIndex + (ticket % 3) + 1;
		// smoke from chimney
		case 0x3F: case 0x46: case 0x4D:
		// archerhouse
		case 0x54:
		// smoke from chimney
		case 0x5D: case 0x64:
		// shadow smoke
		case 0x6B:
		// peasanthunt
		case 0x72:
		    return spriteIndex + (ticket % 6) + 1;
		default: break;
	    }
	    break;

	case ICN::OBJNCRCK:
	    switch(spriteIndex)
	    {
		// pool of oil
		case 0x50: case 0x5B: case 0x66: case 0x71: case 0x7C: case 0x89: case 0x94: case 0x9F: case 0xAA:
		// smoke from chimney
		case 0xBE:
		// shadow smoke
		case 0xCA:
		    return spriteIndex + (ticket % 10) + 1;
		default: break;
	    }
	    break;

	case ICN::OBJNDIRT:
	    switch(spriteIndex)
	    {
		// mill
		case 0x99: case 0x9D: case 0xA1: case 0xA5: case 0xA9: case 0xAD: case 0xB1: case 0xB5: case 0xB9: case 0xBD:
		    return spriteIndex + (ticket % 3) + 1;
		default: break;
	    }
	    break;

	case ICN::OBJNMULT:
	    switch(spriteIndex)
	    {
		// smoke
		case 0x05:
		// shadow
		case 0x0F: case 0x19:
		    return spriteIndex + (ticket % 9) + 1;
		// smoke
		case 0x24:
		// shadow
		case 0x2D:
		    return spriteIndex + (ticket % 8) + 1;
		// smoke
		case 0x5A:
		// shadow
		case 0x61: case 0x68: case 0x7C:
		// campfire
		case 0x83:
		    return spriteIndex + (ticket % 6) + 1;
		default: break;
	    }
	    break;

	case ICN::OBJNWATR:
	    switch(spriteIndex)
	    {
		// buttle
		case 0x00:
		    return spriteIndex + (ticket % 11) + 1;
		// shadow
		case 0x0C:
		// chest
		case 0x13:
		// shadow
		case 0x26:
		// flotsam
		case 0x2D:
		// unkn
		case 0x37:
		// boat
		case 0x3E:
		// waves
		case 0x45:
		// seaweed
		case 0x4C: case 0x53: case 0x5A: case 0x61: case 0x68:
		// sailor-man
		case 0x6F:
		// shadow
		case 0xBC:
		// buoy
		case 0xC3:
		// broken ship (right)
		case 0xE2: case 0xE9: case 0xF1: case 0xF8:
		    return spriteIndex + (ticket % 6) + 1;
		// seagull on stones
		case 0x76: case 0x86: case 0x96:
		    return spriteIndex + (ticket % 15) + 1;
		// whirlpool
		case 0xCA: case 0xCE: case 0xD2: case 0xD6: case 0xDA: case 0xDE:
		    return spriteIndex + (ticket % 3) + 1;
		default: break;

	    }
	    break;

	case ICN::OBJNWAT2:
	    switch(spriteIndex)
	    {
		// sail broken ship (left)
		case 0x03: case 0x0C:
		    return spriteIndex + (ticket % 6) + 1;
		default: break;
	    }
	    break;

	case ICN::X_LOC1:
	    switch(spriteIndex)
	    {
		// alchemist tower
		case 0x04: case 0x0D: case 0x16:
		// arena
		case 0x1F: case 0x28: case 0x32: case 0x3B:
		// earth altar
		case 0x55: case 0x5E: case 0x67:
		    return spriteIndex + (ticket % 8) + 1;
		default: break;
	    }
	    break;

	case ICN::X_LOC2:
	    switch(spriteIndex)
	    {
		// mermaid
		case 0x0A: case 0x13: case 0x1C: case 0x25: 
		// sirens
		case 0x2F: case 0x38: case 0x41: case 0x4A: case 0x53: case 0x5C: case 0x66:
		    return spriteIndex + (ticket % 8) + 1;
		default: break;
	    }
	    break;

	case ICN::X_LOC3:
	    switch(spriteIndex)
	    {
		// hut magi
		case 0x00: case 0x0A: case 0x14:
		// eye magi
		case 0x20: case 0x29: case 0x32:
		    return spriteIndex + (ticket % 8) + 1;
		// barrier
		case 0x3C: case 0x42: case 0x48: case 0x4E: case 0x54: case 0x5A: case 0x60: case 0x66:
		    return spriteIndex + (ticket % 4) + 1;
		default: break;
	    }

	default: break;
    }

    return 0;
}


/*Themes section */
EditorTheme::EditorTheme(AGG::Spool & spool) : aggSpool(spool), name("agg"), tile(32, 32)
{
}

QString EditorTheme::resourceFile(const QString & dir, const QString & file) const
{
    return Resource::FindFile(QDir::toNativeSeparators(QString("themes") + QDir::separator() + name),
	    QDir::toNativeSeparators(dir + QDir::separator() + file));
}

QPixmap EditorTheme::getImageTIL(const QString & til, int index)
{
    return aggSpool.getImageTIL(til, index);
}

QPair<QPixmap, QPoint> EditorTheme::getImageICN(int icn, int index)
{
    return aggSpool.getImageICN(H2::icnString(icn), index);
}

QPixmap EditorTheme::getImage(const CompositeObject & obj)
{
    return aggSpool.getImage(obj, tile);
}

const QSize & EditorTheme::tileSize(void) const
{
    return tile;
}

int EditorTheme::startFilledTile(int ground) const
{
    // 30%
    if(0 == Editor::Rand(6))
        return startFilledOriginalTile(ground);

    int res = 0;

    switch(ground)
    {
        case Ground::Desert:      res = 300; break;
        case Ground::Snow:        res = 130; break;
        case Ground::Swamp:       res = 184; break;
        case Ground::Wasteland:   res = 399; break;
        case Ground::Beach:       res = 415; break;
        case Ground::Lava:        res = 246; break;
        case Ground::Dirt:        res = 337; break;
        case Ground::Grass:       res = 68;  break;
        case Ground::Water:       res = 16;  break;
        default: qCritical() << "EditorTheme::startFilledTile:" << "unknown ground"; break;
    }

    return res + Editor::Rand(7);
}

int EditorTheme::startFilledOriginalTile(int ground) const
{
    int res = 0;
    int count = 8;

    switch(ground)
    {
        case Ground::Desert:      res = 308; count = 13; break;
        case Ground::Snow:        res = 138; break;
        case Ground::Swamp:       res = 192; count = 16; break;
        case Ground::Wasteland:   res = 407; break;
        case Ground::Beach:       res = 423; count = 9; break;
        case Ground::Lava:        res = 254; break;
        case Ground::Dirt:        res = 345; count = 16; break;
        case Ground::Grass:       res = 76; count = 16; break;
        case Ground::Water:       res = 24; count = 6; break;
        default: qCritical() << "EditorTheme::startFilledOriginalTile:" << "unknown ground"; break;
    }

    return res + Editor::Rand(count - 1);
}

int EditorTheme::ground(int index) const
{
    // list grounds from GROUND32.TIL
    if(30 > index)
        return Ground::Water;
    else
    if(92 > index)
        return Ground::Grass;
    else
    if(146 > index)
        return Ground::Snow;
    else
    if(208 > index)
        return Ground::Swamp;
    else
    if(262 > index)
        return Ground::Lava;
    else
    if(321 > index)
        return Ground::Desert;
    else
    if(361 > index)
        return Ground::Dirt;
    else
    if(415 > index)
        return Ground::Wasteland;
    else
    if(432 > index)
        return Ground::Beach;

    return Ground::Unknown;
}

int EditorTheme::startGroundTile(int ground) const
{
    int res = 0;

    // from GROUND32.TIL
    switch(ground)
    {
        case Ground::Desert:    return 262;
        case Ground::Snow:      return 92;
        case Ground::Swamp:     return 146;
        case Ground::Wasteland: return 361;
        case Ground::Beach:     return 415;
        case Ground::Lava:      return 208;
        case Ground::Dirt:      return 321;
        case Ground::Grass:     return 30;
        case Ground::Water:     return 0;
        default: break;
    }

    return res;
}

int EditorTheme::startGroundOriginalTile(int ground) const
{
    return startGroundTile(ground) + Editor::Rand(3);
}

AroundGrounds::AroundGrounds(const MapTiles & tiles, const QPoint & center) : QVector<int>(9, Ground::Unknown)
{
    QVector<int> & v = *this;
    const MapTile* tile = NULL;

    tile = tiles.tileFromDirectionConst(center, Direction::TopLeft);
    if(tile) v[0] = tile->groundType();

    tile = tiles.tileFromDirectionConst(center, Direction::Top);
    if(tile) v[1] = tile->groundType();

    tile = tiles.tileFromDirectionConst(center, Direction::TopRight);
    if(tile) v[2] = tile->groundType();

    tile = tiles.tileFromDirectionConst(center, Direction::Right);
    if(tile) v[3] = tile->groundType();

    tile = tiles.tileFromDirectionConst(center, Direction::BottomRight);
    if(tile) v[4] = tile->groundType();

    tile = tiles.tileFromDirectionConst(center, Direction::Bottom);
    if(tile) v[5] = tile->groundType();

    tile = tiles.tileFromDirectionConst(center, Direction::BottomLeft);
    if(tile) v[6] = tile->groundType();

    tile = tiles.tileFromDirectionConst(center, Direction::Left);
    if(tile) v[7] = tile->groundType();

    tile = tiles.tileConst(center);
    if(tile) v[8] = tile->groundType();
}

int AroundGrounds::operator() (void) const
{
    int res = 0;

    for(QVector<int>::const_iterator
        it = begin(); it != end(); ++it)
        res |= *it;

    return res;
}

int AroundGrounds::groundsDirects(int directs) const
{
    int res = Ground::Unknown;
    const QVector<int> & v = *this;

    if(Direction::TopLeft & directs)     res |= v[0];
    if(Direction::Top & directs)         res |= v[1];
    if(Direction::TopRight & directs)    res |= v[2];
    if(Direction::Right & directs)       res |= v[3];
    if(Direction::BottomRight & directs) res |= v[4];
    if(Direction::Bottom & directs)      res |= v[5];
    if(Direction::BottomLeft & directs)  res |= v[6];
    if(Direction::Left & directs)        res |= v[7];
    if(Direction::Center & directs)      res |= v[8];

    return res;
}

int AroundGrounds::directionsAroundGround(int ground) const
{
    int res = 0;
    const QVector<int> & v = *this;

    if(v[0] & ground) res |= Direction::TopLeft;
    if(v[1] & ground) res |= Direction::Top;
    if(v[2] & ground) res |= Direction::TopRight;
    if(v[3] & ground) res |= Direction::Right;
    if(v[4] & ground) res |= Direction::BottomRight;
    if(v[5] & ground) res |= Direction::Bottom;
    if(v[6] & ground) res |= Direction::BottomLeft;
    if(v[7] & ground) res |= Direction::Left;

    return res;
}

inline bool IS_EQUAL_VALS(int A, int B)
{
    return (A & B) == A;
}

/* return pair, first: index tile, second: shape - 0: none, 1: vert, 2: horz, 3: both */
QPair<int, int> EditorTheme::groundBoundariesFix(const MapTile & tile, const MapTiles & tiles) const
{
    QPair<int, int> res(-1, 0);
    const int & ground = tile.groundType();

    if(ground == Ground::Beach) return res;
    AroundGrounds around(tiles, tile.mapPos());

    /*
	1. water - any ground (startGroundTile(ground))
	2. ground - other ground (startGroundTile(ground))
	3. ground - water (+16)
    */

    const int ground_and = around.directionsAroundGround(ground);
    const int ground_not = ground == Ground::Water ? around.directionsAroundGround(Ground::All) :
				around.directionsAroundGround(Ground::Water | (Ground::All & ~ground));
    int marker_id = 0;

    // corner: top right
    if(IS_EQUAL_VALS(Direction::All & ~(Direction::TopRight | Direction::Center), ground_and) &&
	IS_EQUAL_VALS(Direction::TopRight, ground_not))
    {
	marker_id = around.groundsDirects(Direction::TopRight);
	res = qMakePair(startGroundOriginalTile(ground) + 12, 0);
    }
    else
    // corner: top left
    if(IS_EQUAL_VALS(Direction::All & ~(Direction::TopLeft | Direction::Center), ground_and) &&
	IS_EQUAL_VALS(Direction::TopLeft, ground_not))
    {
	marker_id = around.groundsDirects(Direction::TopLeft);
	res = qMakePair(startGroundOriginalTile(ground) + 12, 2);
    }
    else
    // corner: bottom right
    if(IS_EQUAL_VALS(Direction::All & ~(Direction::BottomRight | Direction::Center), ground_and) &&
	IS_EQUAL_VALS(Direction::BottomRight, ground_not))
    {
	marker_id = around.groundsDirects(Direction::BottomRight);
	res = qMakePair(startGroundOriginalTile(ground) + 12, 1);
    }
    else
    // corner: bottom left
    if(IS_EQUAL_VALS(Direction::All & ~(Direction::BottomLeft | Direction::Center), ground_and) &&
	IS_EQUAL_VALS(Direction::BottomLeft, ground_not))
    {
	marker_id = around.groundsDirects(Direction::BottomLeft);
	res = qMakePair(startGroundOriginalTile(ground) + 12, 3);
    }
    else
    // top
    if(IS_EQUAL_VALS(Direction::Left | Direction::Right | Direction::Bottom, ground_and) &&
	IS_EQUAL_VALS(Direction::Top, ground_not))
    {
	marker_id = around.groundsDirects(Direction::Top);
	res = qMakePair(startGroundOriginalTile(ground), 0);
    }
    else
    // bottom
    if(IS_EQUAL_VALS(Direction::Left | Direction::Right | Direction::Top, ground_and) &&
	IS_EQUAL_VALS(Direction::Bottom, ground_not))
    {
	marker_id = around.groundsDirects(Direction::Bottom);
	res = qMakePair(startGroundOriginalTile(ground), 1);
    }
    else
    // right
    if(IS_EQUAL_VALS(Direction::Left | Direction::Top | Direction::Bottom, ground_and) &&
	IS_EQUAL_VALS(Direction::Right, ground_not))
    {
	marker_id = around.groundsDirects(Direction::Right);
	res = qMakePair(startGroundOriginalTile(ground) + 8, 0);
    }
    else
    // left
    if(IS_EQUAL_VALS(Direction::Right | Direction::Top | Direction::Bottom, ground_and) &&
	IS_EQUAL_VALS(Direction::Left, ground_not))
    {
	marker_id = around.groundsDirects(Direction::Left);
	res = qMakePair(startGroundOriginalTile(ground) + 8, 2);
    }
    else
    // corner: top + top right + right
    if(IS_EQUAL_VALS(Direction::Left | Direction::Bottom | Direction::BottomLeft, ground_and) &&
	IS_EQUAL_VALS(Direction::Top | Direction::Right, ground_not))
    {
	if(Ground::Water != ground &&
	    around.groundsDirects(Direction::Top) != around.groundsDirects(Direction::Right))
	{
	    res = Ground::Water == around.groundsDirects(Direction::Top) ?
		    qMakePair(startGroundTile(ground) + 36, 0) :
		    qMakePair(startGroundTile(ground) + 37, 0);
	}
	else
	{
	    marker_id = around.groundsDirects(Direction::Top | Direction::Right);
	    res = qMakePair(startGroundOriginalTile(ground) + 4, 0);
	}
    }
    else
    // corner: top + top left + left
    if(IS_EQUAL_VALS(Direction::Right | Direction::Bottom | Direction::BottomRight, ground_and) &&
	IS_EQUAL_VALS(Direction::Top | Direction::Left, ground_not))
    {
	if(Ground::Water != ground &&
	    around.groundsDirects(Direction::Top) != around.groundsDirects(Direction::Left))
	{
	    res = Ground::Water == around.groundsDirects(Direction::Top) ?
		    qMakePair(startGroundTile(ground) + 36, 2) :
		    qMakePair(startGroundTile(ground) + 37, 2);
	}
	else
	{
	    marker_id = around.groundsDirects(Direction::Top | Direction::Left);
	    res = qMakePair(startGroundOriginalTile(ground) + 4, 2);
	}
    }
    else
    // corner: bottom + bottom right + right
    if(IS_EQUAL_VALS(Direction::Left | Direction::Top | Direction::TopLeft, ground_and) &&
	IS_EQUAL_VALS(Direction::Bottom | Direction::Right, ground_not))
    {
	if(Ground::Water != ground &&
	    around.groundsDirects(Direction::Bottom) != around.groundsDirects(Direction::Right))
	{
	    res = Ground::Water == around.groundsDirects(Direction::Bottom) ?
		    qMakePair(startGroundTile(ground) + 36, 1) :
		    qMakePair(startGroundTile(ground) + 37, 1);
	}
	else
	{
	    marker_id = around.groundsDirects(Direction::Bottom | Direction::Right);
	    res = qMakePair(startGroundOriginalTile(ground) + 4, 1);
	}
    }
    else
    // corner: bottom + bottom left + left
    if(IS_EQUAL_VALS(Direction::Right | Direction::Top | Direction::TopRight, ground_and) &&
	IS_EQUAL_VALS(Direction::Bottom | Direction::Left, ground_not))
    {
	if(Ground::Water != ground &&
	    around.groundsDirects(Direction::Bottom) != around.groundsDirects(Direction::Left))
	{
	    res = Ground::Water == around.groundsDirects(Direction::Bottom) ?
		    qMakePair(startGroundTile(ground) + 36, 3) :
		    qMakePair(startGroundTile(ground) + 37, 3);
	}
	else
	{
	    marker_id = around.groundsDirects(Direction::Bottom | Direction::Left);
	    res = qMakePair(startGroundOriginalTile(ground) + 4, 3);
	}
    }
    else
    // filled
    if(IS_EQUAL_VALS(Direction::All & ~Direction::Center, ground_and))
    {
	res = qMakePair(startFilledTile(ground), 0);
    }
    else
    // false
	return qMakePair(-1, 0);

    // dirt fixed
    if(Ground::Dirt == ground)
    {
	if(Ground::Water != marker_id)
	    res.first = startFilledTile(ground);
    }
    else
    // coast fixed
    if(Ground::Water != ground && Ground::Water == marker_id)
	res.first += 16;

    return res;
}

MapTown::MapTown(const QPoint & pos, quint32 id, const mp2town_t &)
    : MapObject(pos, id)
{
}

MapHero::MapHero(const QPoint & pos, quint32 id, const mp2hero_t &)
    : MapObject(pos, id)
{
}

MapSign::MapSign(const QPoint & pos, quint32 id, const mp2sign_t &)
    : MapObject(pos, id)
{
}

MapEvent::MapEvent(const QPoint & pos, quint32 id, const mp2mapevent_t &)
    : MapObject(pos, id)
{
}

MapSphinx::MapSphinx(const QPoint & pos, quint32 id, const mp2sphinx_t &)
    : MapObject(pos, id)
{
}

DayEvent::DayEvent(const mp2dayevent_t &)
{
}

Rumor::Rumor(const mp2rumor_t &)
{
}

MapObjects::MapObjects()
{
}

MapObjects::MapObjects(const MapObjects & mo, const QRect & rt)
{
    //QMap<MapKey, QSharedPointer<MapObject> >
}

DayEvents::DayEvents()
{
}

TavernRumors::TavernRumors()
{
}

int SpriteLevel::fromString(const QString & level)
{
    if(level == "bottom")
	return Bottom;
    else
    if(level == "action")
	return Action;
    else
    if(level == "shadow")
	return Shadow;
    else
    if(level == "top")
	return Top;

    qDebug() << "SpriteLevel::fromString:" << "unknown sprite level";
    return Uknown;
}

CompositeSprite::CompositeSprite(const QDomElement & elem, int templateIndex)
    : spriteIndex(elem.attribute("index").toInt()), spriteLevel(0), spritePassable(Direction::All), spriteAnimation(0)
{
    spritePos.setX(elem.attribute("px").toInt());
    spritePos.setY(elem.attribute("py").toInt());

    if(0 <= templateIndex)
	spriteIndex += templateIndex;

    spriteLevel = SpriteLevel::fromString(elem.attribute("level").toLower());

    if(elem.hasAttribute("passable"))
	spritePassable = elem.attribute("passable").toInt();

    if(elem.hasAttribute("animation"))
	spriteAnimation = elem.attribute("animation").toInt();
}

CompositeObject::CompositeObject(const QString & obj, const QDomElement & elem, int templateIndex)
    : name(elem.attribute("name")), size(elem.attribute("width").toInt(), elem.attribute("height").toInt()), icn(obj.toUpper())
{
    if(0 > icn.lastIndexOf(".ICN")) icn.append(".ICN");

    QDomNodeList list = elem.elementsByTagName("sprite");

    for(int pos = 0; pos < list.size(); ++pos)
	push_back(CompositeSprite(list.item(pos).toElement(), templateIndex));
}

bool CompositeObject::isValid(void) const
{
    return ! name.isEmpty();
}

CompositeObjectPixmap::CompositeObjectPixmap(const CompositeObject & obj, EditorTheme & theme) : object(obj), valid(true)
{
    const QSize & tile = theme.tileSize();

    area = theme.getImage(obj);
    borderRed = Editor::pixmapBorder(QSize(tile.width() * obj.size.width(), tile.height() * obj.size.height()), QColor(255, 0, 0), 0);
    borderGreen = Editor::pixmapBorder(QSize(tile.width() * obj.size.width(), tile.height() * obj.size.height()), QColor(0, 255, 0), 0);

    // generate passable color map
    passableMap = QPixmap(tile.width() * obj.size.width(), tile.height() * obj.size.height());
    passableMap.fill(Qt::transparent);

    QPixmap tileR = Editor::pixmapBorder(tile, QColor(255, 0, 0), 1);
    QPixmap tileG = Editor::pixmapBorder(tile, QColor(0, 255, 0), 1);
    QPixmap tileY = Editor::pixmapBorder(tile, QColor(255, 255, 0), 1);

    QPainter paint(& passableMap);

    for(int yy = 0; yy < obj.size.height(); ++yy)
	for(int xx = 0; xx < obj.size.width(); ++xx)
	    paint.drawPixmap(xx * tile.width(), yy * tile.height(), tileG);

    for(CompositeObject::const_iterator
	it = obj.begin(); it != obj.end(); ++it)
    {
	const QPoint offset((*it).spritePos.x() * tile.width(),
				(*it).spritePos.y() * tile.height());

	switch((*it).spriteLevel)
	{
	    case SpriteLevel::Bottom:
	    case SpriteLevel::Action:
		paint.drawPixmap(offset, tileR);
		break;

// int spritePassable

	    case SpriteLevel::Shadow:
	    case SpriteLevel::Top:
		paint.drawPixmap(offset, tileY);
		break;

	    default:
		paint.drawPixmap(offset, tileG);
		break;
	}
    }
}

void CompositeObjectPixmap::paint(QPainter & painter, const QPoint & pos, bool allow) const
{
    if(valid)
    {
	painter.drawPixmap(pos, area);
	painter.drawPixmap(pos, passableMap);
	painter.drawPixmap(pos, (allow ? borderGreen : borderRed));
    }
}

void CompositeObjectPixmap::reset(void)
{
    valid = false;
}

bool CompositeObjectPixmap::isValid(void) const
{
    return valid;
}
