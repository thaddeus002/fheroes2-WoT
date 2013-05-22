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

QString townName(int num)
{
    const char* towns[] = { "Blackridge", "Pinehurst", "Woodhaven", "Hillstone", "Whiteshield", "Bloodreign", "Dragontooth", "Greywind", "Blackwind", "Portsmith", "Middle Gate", "Tundara", 
	"Vulcania", "Sansobar", "Atlantium", "Baywatch", "Wildabar", "Fountainhead", "Vertigo", "Winterkill", "Nightshadow", "Sandcaster", "Lakeside", "Olympus", 
	"Brindamoor", "Burlock", "Xabran", "Dragadune", "Alamar", "Kalindra", "Blackfang", "Basenji", "Algary", "Sorpigal", "New Dawn", "Erliquin", 
	"Avone", "Big Oak", "Hampshire", "Chandler", "South Mill", "Weed Patch", "Roc Haven", "Avalon", "Antioch", "Brownston", "Weddington", "Whittingham",
	"Westfork", "Hilltop", "Yorksford", "Sherman", "Roscomon", "Elk's Head", "Cathcart", "Viper's Nest", "Pig's Eye", "Blacksford", "Burton", "Blackburn",
	"Lankershire", "Lombard", "Timberhill", "Fenton", "Troy", "Forder Oaks", "Meramec", "Quick Silver", "Westmoor", "Willow", "Sheltemburg", "Corackston" };

    return QString(towns[num % 72]);
}

QString Portrait::transcribe(int port)
{
    const char* ports[] = { "Unknown",
	"Lord Kilburn", "Sir Gallanth", "Ector", "Gwenneth", "Tyro", "Ambrose", "Ruby", "Maximus", "Dimitry",
	"Thundax", "Fineous", "Jojosh", "Crag Hack", "Jezebel", "Jaclyn", "Ergon", "Tsabu", "Atlas",
	"Astra", "Natasha", "Troyan", "Vatawna", "Rebecca", "Gem", "Ariel", "Carlawn", "Luna",
	"Arie", "Alamar", "Vesper", "Crodo", "Barok", "Kastore", "Agar", "Falagar", "Wrathmont",
	"Myra", "Flint", "Dawn", "Halon", "Myrini", "Wilfrey", "Sarakin", "Kalindra", "Mandigal",
	"Zom", "Darlana", "Zam", "Ranloo", "Charity", "Rialdo", "Roxana", "Sandro", "Celia",
	"Roland", "Lord Corlagon", "Sister Eliza", "Archibald", "Lord Halton", "Brother Bax",
	"Solmyr", "Dainwin", "Mog", "Uncle Ivan", "Joseph", "Gallavant", "Elderian", "Ceallach", "Drakonia", "Martine", "Jarkonas",
	"Random Hero" };

    const int count = sizeof(ports) / sizeof(ports[0]);

    return count > port ? QString(ports[port]) : QString(ports[0]);
}

QString Resource::transcribe(int res)
{
    switch(res)
    {
	case Resource::Wood:	return "Wood";
	case Resource::Mercury:	return "Mercury";
	case Resource::Ore:	return "Ore";
	case Resource::Sulfur:	return "Sulfur";
	case Resource::Crystal:	return "Crystal";
	case Resource::Gems:	return "Gems";
	case Resource::Gold:	return "Gold";
	case Resource::Random:	return "Random";

	default: break;
    }

    return "Unknown";
}

inline bool IS_EQUAL_VALS(int A, int B)
{
    return (A & B) == A;
}

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
	    if(0 == byte) break;
	    str.push_back(byte);
	}
    }
    else
    {
	str.reserve(512);

	while(! ds.atEnd())
	{
	    ds >> byte;
	    if(0 == byte) break;
	    str.push_back(byte);
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

quint32 H2::File::readLE16(void)
{
    quint16 res = 0;

    if(pos() + sizeof(res) <= size())
    {
	read((char*) &res, sizeof(res));
	res = qFromLittleEndian(res);
    }
    else
	qWarning() << "H2::File::readLE16:" << "out of range";

    return res;
}

quint32 H2::File::readLE32(void)
{
    quint32 res = 0;

    if(pos() + sizeof(res) <= size())
    {
	read((char*) &res, sizeof(res));
	res = qFromLittleEndian(res);
    }
    else
	qWarning() << "H2::File::readLE32:" << "out of range";

    return res;
}

quint32 H2::File::readByte(void)
{
    quint8 res = 0;

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
    objectID = 0;
}

mp2ext_t::mp2ext_t()
{
    indexExt = 0;
    quantity = 0;
}

QDataStream & operator>> (QDataStream & ds, mp2til_t & til)
{
    ds.setByteOrder(QDataStream::LittleEndian);
    til.ext.quantity = 0;
    return ds >> til.tileSprite >>
	til.ext.level1.object >> til.ext.level1.index >>
	til.quantity1 >> til.quantity2 >>
	til.ext.level2.object >> til.ext.level2.index >>
	til.tileShape >> til.objectID >>
	til.ext.indexExt >> til.ext.level1.uniq >> til.ext.level2.uniq;
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
    { ds >> cstl.troopId[ii]; cstl.troopId[ii] += 1; }

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
    { ds >> hero.troopId[ii]; hero.troopId[ii] += 1; }

    for(int ii = 0; ii < 5; ++ii)
	ds >> hero.troopCount[ii];

    ds >> hero.customPortrate >> hero.portrateType;

    for(int ii = 0; ii < 3; ++ii)
    {
	ds >> hero.artifacts[ii];
	hero.artifacts[ii] = 0xFF == hero.artifacts[ii] ? Artifact::None : hero.artifacts[ii] + 1;
    }

    ds >> hero.unknown2 >> hero.experience >> hero.customSkills;

    for(int ii = 0; ii < 8; ++ii)
    {
	ds >> hero.skillId[ii];
	hero.skillId[ii] = 0xFF == hero.skillId[ii] ? SkillType::Unknown : hero.skillId[ii] + 1;
    }

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
    evnt.artifact = 0xFF == evnt.artifact ? Artifact::None : evnt.artifact + 1;

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
    evnt.artifact = 0xFF == evnt.artifact ? Artifact::None : evnt.artifact + 1;

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

    quint8 answersCount = 0;
    ds >> sphinx.artifact >> answersCount;
    sphinx.artifact = 0xFF == sphinx.artifact ? Artifact::None : sphinx.artifact + 1;

    for(int ii = 0; ii < 8; ++ii)
    {
	QString str = readStringFromStream(ds, 13);

	if(! str.isEmpty())
	    sphinx.answers << str;
    }

    sphinx.text = readStringFromStream(ds);
    return ds;
}

bool AGG::File::exists(const QString & str) const
{
    return items.end() != items.find(str);
}

int AGG::File::seekToData(const QString & name)
{
    QMap<QString, Item>::const_iterator it = items.find(name);

    if(items.end() != it)
	seek((*it).offset);
    else
	qCritical() << "AGG::File::seekToData:" << "item" << qPrintable(name) << "not found";

    return items.end() != it ? (*it).size : 0;
}

QByteArray AGG::File::readRawData(const QString & name)
{
    return readBlock(seekToData(name));
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
    int blockSize = seekToData(id);

    if(blockSize)
    {
	quint16 tileCount = readLE16();

	if(index < tileCount)
	{
	    quint16 tileWidth = readLE16();
	    quint16 tileHeight = readLE16();
	    QByteArray buf = readBlock(blockSize - 6);

	    QImage image((uchar*) buf.data() + index * tileWidth * tileHeight, tileWidth, tileHeight, QImage::Format_Indexed8);
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
    int blockSize = seekToData(id);

    if(blockSize)
    {
	quint16 icnCount = readLE16();

	if(index < icnCount)
	{
	    quint32 sizeData = readLE32();
	    QByteArray buf = readBlock(blockSize - 6);
	    mp2icn_t header(buf.data() + index * mp2icn_t::sizeOf());

	    if(index + 1 < icnCount)
	    {
		mp2icn_t headerNext(buf.data() + (index + 1) * mp2icn_t::sizeOf());
		sizeData = headerNext.offsetData - header.offsetData;
	    }
	    else
		sizeData = sizeData - header.offsetData;

	    H2::ICNSprite image(header, buf.data() + header.offsetData, sizeData, colors);

	    result.first = QPixmap::fromImage(image);
	    result.second = QPoint(header.offsetX, header.offsetY);
	}
	else
	    qCritical() << "AGG::File::getImageICN:" << "out of range" << index;
    }

    return result;
}

QString AGG::Spool::dirName(void) const
{
    return first.isReadable() ?
	QDir::toNativeSeparators(QFileInfo(first.fileName()).absolutePath()) : NULL;
}

bool AGG::Spool::setData(const QString & file)
{
    if(first.loadFile(file))
    {
	QFileInfo fileInfo(file);
	QStringList list = fileInfo.absoluteDir().entryList(QStringList() << "heroes2x.agg", QDir::Files | QDir::Readable);

	if(list.size())
	    second.loadFile(QDir::toNativeSeparators(fileInfo.absolutePath() + QDir::separator() + list.front()));

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

	    return true;
	}
	else
	    qCritical() << "AGG::Spool:" << "palette not found";
    }

    return false;
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

    if(0 <= index)
    {
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
    }

    return result;
}

QPixmap AGG::Spool::getImage(const CompositeObject & obj, const QSize & tileSize)
{
    const QString key = obj.icn.first + obj.name;
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
	    QPoint offset((*it).spritePos.x() * tileSize.width(), (*it).spritePos.y() * tileSize.height());
	    QPair<QPixmap, QPoint> sprite = getImageICN(obj.icn.first, (*it).spriteIndex);
	    paint.drawPixmap(offset + sprite.second, sprite.first);

	    if((*it).spriteAnimation)
	    {
		sprite = getImageICN(obj.icn.first, (*it).spriteIndex + 1);
		paint.drawPixmap(offset + sprite.second, sprite.first);
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

QPixmap Editor::pixmapBorder(const QSize & size, const QColor & color)
{
    QPixmap result(size);
    result.fill(Qt::transparent);

    QPainter paint(& result);
    paint.setPen(QPen(color, 1));
    paint.setBrush(QBrush(QColor(0, 0, 0, 0)));
    paint.drawRect(0, 0, size.width() - 1, size.height() - 1);

    return result;
}

QPixmap Editor::pixmapBorderPassable(const QSize & size, int passable)
{
    QColor redColor(255, 0, 0);
    QColor greenColor(0, 255, 0);

    if(Direction::Unknown == passable || Direction::Center == passable)
	return pixmapBorder(size, redColor);
    else
    if(IS_EQUAL_VALS(Direction::All, passable))
	return pixmapBorder(size, greenColor);

    QPixmap result(size);
    result.fill(Qt::transparent);

    int cw = (size.width() - 3) / 3;
    int ch = (size.height() - 3) / 3;

    QPainter paint(& result);
    paint.setBrush(QBrush(QColor(0, 0, 0, 0)));

    for(int xx = 1; xx < size.width() - 2; ++xx)
    {
	if(xx < cw)
	{
	    paint.setPen(QPen((passable & Direction::TopLeft ? greenColor : redColor), 1));
	    paint.drawPoint(xx, 1);
	    paint.setPen(QPen((passable & Direction::BottomLeft ? greenColor : redColor), 1));
	    paint.drawPoint(xx, size.height() - 2);
	}
	else
	if(xx < 2 * cw)
	{
	    paint.setPen(QPen((passable & Direction::Top ? greenColor : redColor), 1));
	    paint.drawPoint(xx, 1);
	    paint.setPen(QPen((passable & Direction::Bottom ? greenColor : redColor), 1));
	    paint.drawPoint(xx, size.height() - 2);
	}
	else
	{
	    paint.setPen(QPen((passable & Direction::TopRight ? greenColor : redColor), 1));
	    paint.drawPoint(xx, 1);
	    paint.setPen(QPen((passable & Direction::BottomRight ? greenColor : redColor), 1));
	    paint.drawPoint(xx, size.height() - 2);
	}
    }

    for(int yy = 1; yy < size.height() - 2; ++yy)
    {
	if(yy < ch)
	{
	    paint.setPen(QPen((passable & Direction::TopLeft ? greenColor : redColor), 1));
	    paint.drawPoint(1, yy);
	    paint.setPen(QPen((passable & Direction::TopRight ? greenColor : redColor), 1));
	    paint.drawPoint(size.width() - 2, yy);
	}
	else
	if(yy < 2 * ch)
	{
	    paint.setPen(QPen((passable & Direction::Left ? greenColor : redColor), 1));
	    paint.drawPoint(1, yy);
	    paint.setPen(QPen((passable & Direction::Right ? greenColor : redColor), 1));
	    paint.drawPoint(size.width() - 2, yy);
	}
	else
	{
	    paint.setPen(QPen((passable & Direction::BottomLeft ? greenColor : redColor), 1));
	    paint.drawPoint(1, yy);
	    paint.setPen(QPen((passable & Direction::BottomRight ? greenColor : redColor), 1));
	    paint.drawPoint(size.width() - 2, yy);
	}
    }

    return result;
}

Editor::MyXML::MyXML(const QString & xml, const QString & root, bool debug)
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
	if(debug)
	    qDebug() << "MyXML:" << xml << "unknown tag:" << tagName();
	clear();
    }
}

Editor::MyObjectsXML::MyObjectsXML(const QString & xml, bool debug)
{
    Editor::MyXML templateObjects(QDir::toNativeSeparators(QFileInfo(xml).absolutePath() + QDir::separator() + "template.xml"), "template");
    Editor::MyXML objectsElem(xml, "objects", debug);

    if(! objectsElem.isNull())
    {
	QString icn = objectsElem.hasAttribute("icn") ? objectsElem.attribute("icn") : NULL;
	int cid = objectsElem.hasAttribute("cid") ? objectsElem.attribute("cid").toInt(NULL, 0) : MapObj::None;
    
	// parse element: object
        QDomNodeList objectsList = objectsElem.elementsByTagName("object");

        for(int pos = 0; pos < objectsList.size(); ++pos)
        {
            QDomElement objElem = objectsList.item(pos).toElement();

            if(! objElem.hasAttribute("cid")) objElem.setAttribute("cid", cid);
            if(! objElem.hasAttribute("icn")) objElem.setAttribute("icn", icn.isNull() ? "unknown" : icn);

	    push_back(objElem);
        }

        if(! templateObjects.isNull())
	{
    	    // parse element: template
    	    objectsList = objectsElem.elementsByTagName("template");

    	    for(int pos = 0; pos < objectsList.size(); ++pos)
    	    {
        	QDomElement tmplElem(objectsList.item(pos).toElement());

        	if(tmplElem.hasAttribute("section"))
        	{
            	    QDomElement objElem = templateObjects.firstChildElement(tmplElem.attribute("section")).cloneNode(true).toElement();

            	    if(objElem.isNull())
            	    {
			if(debug)
                	    qDebug() << "Editor::MyObjectsXML:" << "unknown xml section:" << tmplElem.attribute("section") << "file:" << xml;
                	continue;
            	    }

		    if(tmplElem.hasAttribute("icn")) objElem.setAttribute("icn", tmplElem.attribute("icn"));
        	    else
		    if(!icn.isNull()) objElem.setAttribute("icn", icn);

		    if(tmplElem.hasAttribute("cid")) objElem.setAttribute("cid", tmplElem.attribute("cid").toInt(NULL, 0));
        	    else
		    if(cid != MapObj::None) objElem.setAttribute("cid", cid);

            	    objElem.setAttribute("name", tmplElem.hasAttribute("name") ? tmplElem.attribute("name") : objElem.tagName());
            	    objElem.setTagName("object");
		    
		    // fix index offset
		    if(tmplElem.hasAttribute("index"))
		    {
            		int startIndex = tmplElem.attribute("index").toInt();
    			QDomNodeList spritesList = objElem.elementsByTagName("sprite");

    			for(int pos2 = 0; pos2 < spritesList.size(); ++pos2)
    			{
        		    QDomElement spriteElem = spritesList.item(pos2).toElement();
        		    int offsetIndex = spriteElem.hasAttribute("index") ? spriteElem.attribute("index").toInt(NULL, 0) : 0;
			    spriteElem.setAttribute("index", startIndex + offsetIndex);
			}
		    }

		    push_back(objElem);
		}
	    }
	}
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
	default: break;
    }

    return NULL;
}

int H2::mapICN(const QString & str)
{
    for(int ii = 0; ii < 0xFF; ++ii)
    {
	QString icn = H2::icnString(ii);
	if(! icn.isNull() && str == icn) return ii;
    }

    return ICN::UNKNOWN;
}

int H2::isAnimationICN(int spriteClass, int spriteIndex, int ticket)
{
    switch(spriteClass)
    {
	case ICN::OBJNLAVA:
	    switch(spriteIndex)
	    {
		// shadow of lava
		case 0x4E: case 0x58: case 0x62:
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
		case 0x76: case 0x86: case 0x96: case 0xA6:
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

struct SpriteInfo
{
    int		oid;
    int		level;
    int		passable;

    SpriteInfo() : oid(MapObj::None), level(SpriteLevel::Unknown), passable(Direction::Unknown) {}
    SpriteInfo(int id, int lv, int ps) : oid(id), level(lv), passable(ps) {}
};

/*Themes section */
namespace EditorTheme
{
    AGG::Spool			aggSpool;
    QString			themeName("unknown");
    QSize			themeTile(0, 0);
    QMap<int, SpriteInfo>	mapSpriteInfoCache;
}

bool EditorTheme::load(const QString & data)
{
    if(aggSpool.setData(data))
    {
	themeName = "agg";
	themeTile = QSize(32, 32);

	mapSpriteInfoCache.clear();
	QStringList files = resourceFiles("objects", "*.xml");
	for(QStringList::const_iterator
	    it = files.begin(); it != files.end(); ++it)
	{
	    Editor::MyObjectsXML objectsElem(*it, false);

	    for(QList<QDomElement>::const_iterator
		it = objectsElem.begin(); it != objectsElem.end(); ++it)
	    {
		QString icnStr = (*it).attribute("icn").toUpper();
		if(0 > icnStr.lastIndexOf(".ICN")) icnStr.append(".ICN");
		int icn = H2::mapICN(icnStr);
		int gcid = (*it).attribute("cid").toInt(NULL, 0);

    		QDomNodeList spritesList = (*it).elementsByTagName("sprite");

    		for(int pos = 0; pos < spritesList.size(); ++pos)
    		{
        	    QDomElement spriteElem = spritesList.item(pos).toElement();
		    int index = spriteElem.attribute("index").toInt();
		    QString level = spriteElem.attribute("level");
		    int passable = spriteElem.attribute("passable").toInt(NULL, 0);
		    int key = (icn << 16) | (0x0000FFFF & index);
		    int cid = spriteElem.hasAttribute("cid") ? spriteElem.attribute("cid").toInt(NULL, 0) : gcid;
		    mapSpriteInfoCache[key] = SpriteInfo(cid, SpriteLevel::fromString(level), passable);
		}
	    }
	}

	return true;
    }

    return false;
}

QString EditorTheme::resourceFile(const QString & dir, const QString & file)
{
    return Resource::FindFile(QDir::toNativeSeparators(QString("themes") + QDir::separator() + themeName + QDir::separator() + dir), file);
}

QStringList EditorTheme::resourceFiles(const QString & dir, const QString & file)
{
    return Resource::FindFiles(QDir::toNativeSeparators(QString("themes") + QDir::separator() + themeName+ QDir::separator() + dir), file);
}

int EditorTheme::getObjectID(const QString & icn, int index)
{
    return getObjectID(H2::mapICN(icn), index);
}

int EditorTheme::getObjectID(int icn, int index)
{
    int key = (icn << 16) | (0x0000FFFF & index);
    QMap<int,SpriteInfo>::const_iterator it = mapSpriteInfoCache.find(key);
    return it != mapSpriteInfoCache.end() ? (*it).oid : ICN::UNKNOWN;
}

QPixmap EditorTheme::getImageTIL(const QString & til, int index)
{
    return aggSpool.getImageTIL(til, index);
}

QPair<QPixmap, QPoint> EditorTheme::getImageICN(const QString & icn, int index)
{
    return aggSpool.getImageICN(icn, index);
}

QPair<QPixmap, QPoint> EditorTheme::getImageICN(int icn, int index)
{
    return aggSpool.getImageICN(H2::icnString(icn), index);
}

QPixmap EditorTheme::getImage(const CompositeObject & obj)
{
    return aggSpool.getImage(obj, themeTile);
}

const QSize & EditorTheme::tileSize(void)
{
    return themeTile;
}

int EditorTheme::startFilledTile(int ground)
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

int EditorTheme::startFilledOriginalTile(int ground)
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

int EditorTheme::ground(int index)
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

int EditorTheme::startGroundTile(int ground)
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

int EditorTheme::startGroundOriginalTile(int ground)
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

/* return pair, first: index tile, second: shape - 0: none, 1: vert, 2: horz, 3: both */
QPair<int, int> EditorTheme::groundBoundariesFix(const MapTile & tile, const MapTiles & tiles)
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

QDomElement & operator<< (QDomElement & el, const MapObject & obj)
{
    el.setAttribute("uid", obj.objUid);
    el << static_cast<const QPoint &>(obj);
    return el;
}

QDomElement & operator>> (QDomElement & el, MapObject & obj)
{
    obj.objUid = el.hasAttribute("uid") ? el.attribute("uid").toInt() : 0;
    el >> static_cast<QPoint &>(obj);
    return el;
}

QDomElement & operator<< (QDomElement & el, const Troops & troops)
{
    for(Troops::const_iterator
	it = troops.begin(); it != troops.end(); ++it)
    {
	QDomElement troopElem = el.ownerDocument().createElement("troop");
	el.appendChild(troopElem);
	troopElem.setAttribute("type", (*it).type());
	troopElem.setAttribute("count", (*it).count());
    }

    return el;
}

QDomElement & operator>> (QDomElement & el, Troops & troops)
{
    troops.clear();

    QDomNodeList troopList = el.elementsByTagName("troop");
    for(int pos = 0; pos < troopList.size(); ++pos)
    {
	QDomElement troopElem = troopList.item(pos).toElement();
	int type = troopElem.hasAttribute("type") ? troopElem.attribute("type").toInt() : 0;
	int count = troopElem.hasAttribute("count") ? troopElem.attribute("count").toInt() : 0;
	if(type && count) troops.push_back(Troop(type, count));
    }

    return el;
}

QDomElement & operator<< (QDomElement & el, const Skills & skills)
{
    for(Skills::const_iterator
	it = skills.begin(); it != skills.end(); ++it)
    {
	QDomElement skillElem = el.ownerDocument().createElement("skill");
	el.appendChild(skillElem);
	skillElem.setAttribute("id", (*it).skill());
	skillElem.setAttribute("level", (*it).level());
    }

    return el;
}

QDomElement & operator>> (QDomElement & el, Skills & skills)
{
    skills.clear();

    QDomNodeList skillList = el.elementsByTagName("skill");
    for(int pos = 0; pos < skillList.size(); ++pos)
    {
	QDomElement skillElem = skillList.item(pos).toElement();
	int id = skillElem.hasAttribute("id") ? skillElem.attribute("id").toInt() : 0;
	int level = skillElem.hasAttribute("level") ? skillElem.attribute("level").toInt() : 0;
	if(id && level) skills.push_back(Skill(id, level));
    }

    return el;
}

MapTown::MapTown(const QPoint & pos, quint32 id)
    : MapObject(pos, id, MapObj::Castle), color(Color::Unknown), race(Race::Unknown), buildings(0), forceTown(false)
{
}

MapTown::MapTown(const QPoint & pos, quint32 id, const mp2town_t & mp2)
    : MapObject(pos, id, MapObj::Castle), nameTown(mp2.name), forceTown(mp2.forceTown)
{
    switch(mp2.color)
    {
        case 0:	color = Color::Blue; break;
        case 1: color = Color::Green; break;
        case 2: color = Color::Red; break;
        case 3: color = Color::Yellow; break;
        case 4: color = Color::Orange; break;
        case 5: color = Color::Purple; break;
        default: color = Color::Unknown; break;
    }

    switch(mp2.race)
    {
        case 0: race = Race::Knight; break;
        case 1: race = Race::Barbarian; break;
        case 2: race = Race::Sorceress; break;
        case 3: race = Race::Warlock; break;
        case 4: race = Race::Wizard; break;
        case 5: race = Race::Necromancer; break;
        default: race = Race::Random; break;
    }

    if(mp2.customBuilding)
    {
	if(0x00000002 & mp2.building) buildings |= Building::ThievesGuild;
        if(0x00000004 & mp2.building) buildings |= Building::Tavern;
	if(0x00000008 & mp2.building) buildings |= Building::Shipyard;
        if(0x00000010 & mp2.building) buildings |= Building::Well;
        if(0x00000080 & mp2.building) buildings |= Building::Statue;
        if(0x00000100 & mp2.building) buildings |= Building::LeftTurret;
        if(0x00000200 & mp2.building) buildings |= Building::RightTurret;
        if(0x00000400 & mp2.building) buildings |= Building::Marketplace;
        if(0x00001000 & mp2.building) buildings |= Building::Moat;
        if(0x00000800 & mp2.building) buildings |= Building::ExtraWel2;
        if(0x00002000 & mp2.building) buildings |= Building::ExtraSpec;
        if(0x00080000 & mp2.building) buildings |= Building::Dwelling1;
        if(0x00100000 & mp2.building) buildings |= Building::Dwelling2;
        if(0x00200000 & mp2.building) buildings |= Building::Dwelling3;
        if(0x00400000 & mp2.building) buildings |= Building::Dwelling4;
        if(0x00800000 & mp2.building) buildings |= Building::Dwelling5;
        if(0x01000000 & mp2.building) buildings |= Building::Dwelling6;
        if(0x02000000 & mp2.building) buildings |= Building::Upgrade2 | Building::Dwelling2;
        if(0x04000000 & mp2.building) buildings |= Building::Upgrade3 | Building::Dwelling3;
        if(0x08000000 & mp2.building) buildings |= Building::Upgrade4 | Building::Dwelling4;
        if(0x10000000 & mp2.building) buildings |= Building::Upgrade5 | Building::Dwelling5;
        if(0x20000000 & mp2.building) buildings |= Building::Upgrade6 | Building::Dwelling6;
    }

    if(0 < mp2.magicTower) buildings |= Building::MageGuild1;
    if(1 < mp2.magicTower) buildings |= Building::MageGuild2;
    if(2 < mp2.magicTower) buildings |= Building::MageGuild3;
    if(3 < mp2.magicTower) buildings |= Building::MageGuild4;
    if(4 < mp2.magicTower) buildings |= Building::MageGuild5;

    if(mp2.captainPresent) buildings |= Building::Captain;

    if(mp2.customTroops)
    {
	for(int ii = 0; ii < 5; ++ii)
	    if(mp2.troopId[ii] && mp2.troopCount[ii])
		troops.push_back(Troop(mp2.troopId[ii], mp2.troopCount[ii]));
    }
}

QDomElement & operator<< (QDomElement & el, const MapTown & town)
{
    el << static_cast<const MapObject &>(town);

    el.setAttribute("name", town.nameTown);
    el.setAttribute("color", town.color);
    el.setAttribute("race", town.race);
    el.setAttribute("buildings", town.buildings);
    el.setAttribute("forceTown", town.forceTown);

    QDomDocument doc = el.ownerDocument();

    if(town.troops.size())
    {
	QDomElement troopsElem = doc.createElement("troops");
	el.appendChild(troopsElem);
	troopsElem << town.troops;
    }

    return el;
}

QDomElement & operator>> (QDomElement & el, MapTown & town)
{
    el >> static_cast<MapObject &>(town);

    town.nameTown = el.hasAttribute("name") ? el.attribute("name") : "Unknown";
    town.color = el.hasAttribute("color") ? el.attribute("color").toInt() : Color::Unknown;
    town.race =  el.hasAttribute("race") ? el.attribute("race").toInt() : Race::Unknown;
    town.buildings = el.hasAttribute("buildings") ? el.attribute("buildings").toInt() : 0;
    town.forceTown = el.hasAttribute("forceTown") ? el.attribute("forceTown").toInt() : false;

    QDomElement troopsElem = el.firstChildElement("troops");
    troopsElem >> town.troops;

    return el;
}

MapHero::MapHero(const QPoint & pos, quint32 id)
    : MapObject(pos, id, MapObj::Heroes), color(Color::Unknown), race(Race::Unknown),
    portrait(Portrait::Random), experience(0), patrolMode(false), patrolSquare(0)
{
}

MapHero::MapHero(const QPoint & pos, quint32 id, const mp2hero_t & mp2)
    : MapObject(pos, id, MapObj::Heroes), color(Color::Unknown), race(Race::Unknown), portrait(Portrait::Random), nameHero(mp2.name)
{
    if(mp2.customTroops)
    {
	for(int ii = 0; ii < 5; ++ii)
	    if(mp2.troopId[ii] && mp2.troopCount[ii])
		troops.push_back(Troop(mp2.troopId[ii], mp2.troopCount[ii]));
    }

    for(int index = 0; index < 3; ++index)
	if(Artifact::None != mp2.artifacts[index])
	    artifacts.push_back(mp2.artifacts[index]);

    experience = mp2.experience;
    patrolMode = mp2.patrol;
    patrolSquare = mp2.patrolSquare;

    if(mp2.customPortrate)
	portrait = mp2.portrateType + 1;

    if(nameHero.isEmpty())
	nameHero = Portrait::transcribe(portrait);

    if(mp2.customSkills)
    {
	for(int ii = 0; ii < 8; ++ii)
	    if(mp2.skillId[ii] && mp2.skillLevel[ii])
		skills.push_back(Skill(mp2.skillId[ii], mp2.skillLevel[ii]));
    }
}

QDomElement & operator<< (QDomElement & el, const MapHero & hero)
{
    el << static_cast<const MapObject &>(hero);

    el.setAttribute("name", hero.nameHero);
    el.setAttribute("color", hero.color);
    el.setAttribute("race", hero.race);
    el.setAttribute("portrait", hero.portrait);
    el.setAttribute("experience", hero.experience);
    el.setAttribute("patrolMode", hero.patrolMode);
    el.setAttribute("patrolSquare", hero.patrolSquare);

    QDomDocument doc = el.ownerDocument();

    if(hero.artifacts.size())
    {
	QDomElement artifactsElem = doc.createElement("artifacts");
	el.appendChild(artifactsElem);

	for(QVector<int>::const_iterator
	    it = hero.artifacts.begin(); it != hero.artifacts.end(); ++it)
	{
	    QDomElement artElem = doc.createElement("artifact");
	    artifactsElem.appendChild(artElem);
	    artElem.setAttribute("id", *it);
	}
    }

    if(hero.skills.size())
    {
	QDomElement skillsElem = doc.createElement("skills");
	el.appendChild(skillsElem);
	skillsElem << hero.skills;
    }

    if(hero.troops.size())
    {
	QDomElement troopsElem = doc.createElement("troops");
	el.appendChild(troopsElem);
	troopsElem << hero.troops;
    }

    return el;
}

QDomElement & operator>> (QDomElement & el, MapHero & hero)
{
    el >> static_cast<MapObject &>(hero);

    hero.nameHero = el.hasAttribute("name") ? el.attribute("name") : "Unknown";
    hero.color = el.hasAttribute("color") ? el.attribute("color").toInt() : Color::Unknown;
    hero.race =  el.hasAttribute("race") ? el.attribute("race").toInt() : Race::Unknown;
    hero.portrait = el.hasAttribute("portrait") ? el.attribute("portrait").toInt() : Portrait::Random;
    hero.experience = el.hasAttribute("experience") ? el.attribute("experience").toInt() : 0;
    hero.patrolMode = el.hasAttribute("patrolMode") ? el.attribute("patrolMode").toInt() : false;
    hero.patrolSquare = el.hasAttribute("patrolSquare") ? el.attribute("patrolSquare").toInt() : 0;

    hero.artifacts.clear();
    QDomNodeList artList = el.firstChildElement("artifacts").elementsByTagName("artifact");
    for(int pos = 0; pos < artList.size(); ++pos)
    {
	QDomElement artElem = artList.item(pos).toElement();
	if(artElem.hasAttribute("id"))
	    hero.artifacts.push_back(artElem.attribute("id").toInt());
    }

    QDomElement skillsElem = el.firstChildElement("skills");
    skillsElem >> hero.skills;

    QDomElement troopsElem = el.firstChildElement("troops");
    troopsElem >> hero.troops;

    return el;
}

MapSign::MapSign(const QPoint & pos, quint32 id)
    : MapObject(pos, id, MapObj::Sign)
{
}

MapSign::MapSign(const QPoint & pos, quint32 id, const mp2sign_t & mp2)
    : MapObject(pos, id, MapObj::Sign), message(mp2.text)
{
}

QDomElement & operator<< (QDomElement & el, const MapSign & sign)
{
    el << static_cast<const MapObject &>(sign);
    el.appendChild(el.ownerDocument().createTextNode(sign.message));
    return el;
}

QDomElement & operator>> (QDomElement & el, MapSign & sign)
{
    el >> static_cast<MapObject &>(sign);
    sign.message = el.text();
    return el;
}

MapEvent::MapEvent(const QPoint & pos, quint32 id)
    : MapObject(pos, id, MapObj::Event), artifact(Artifact::Unknown), allowComputer(false),
	cancelAfterFirstVisit(true), colors(0)
{
}

MapEvent::MapEvent(const QPoint & pos, quint32 id, const mp2mapevent_t & mp2)
    : MapObject(pos, id, MapObj::Event), artifact(mp2.artifact), allowComputer(mp2.allowComputer),
	cancelAfterFirstVisit(mp2.cancelAfterFirstVisit), colors(0), message(mp2.text)
{
    resources.wood = mp2.resources[0];
    resources.mercury = mp2.resources[1];
    resources.ore = mp2.resources[2];
    resources.sulfur = mp2.resources[3];
    resources.crystal = mp2.resources[4];
    resources.gems = mp2.resources[5];
    resources.gold = mp2.resources[6];

    if(mp2.colors[0]) colors |= Color::Blue;
    if(mp2.colors[1]) colors |= Color::Red;
    if(mp2.colors[2]) colors |= Color::Green;
    if(mp2.colors[3]) colors |= Color::Yellow;
    if(mp2.colors[4]) colors |= Color::Orange;
    if(mp2.colors[5]) colors |= Color::Purple;
}

QDomElement & operator<< (QDomElement & el, const MapEvent & event)
{
    el << static_cast<const MapObject &>(event);

    QDomDocument doc = el.ownerDocument();

    QDomElement resourcesElem = doc.createElement("resources");
    el.appendChild(resourcesElem);
    resourcesElem << event.resources;

    el.setAttribute("artifact", event.artifact);
    el.setAttribute("colors", event.colors);
    el.setAttribute("allowComputer", event.allowComputer);
    el.setAttribute("cancelAfterFirstVisit", event.cancelAfterFirstVisit);

    el.appendChild(doc.createElement("msg")).appendChild(doc.createTextNode(event.message));

    return el;
}

QDomElement & operator>> (QDomElement & el, MapEvent & event)
{
    el >> static_cast<MapObject &>(event);

    QDomElement resourcesElem = el.firstChildElement("resources");
    resourcesElem >> event.resources;

    event.artifact = el.hasAttribute("artifact") ? el.attribute("artifact").toInt() : 0;
    event.colors = el.hasAttribute("colors") ? el.attribute("colors").toInt() : 0;
    event.allowComputer = el.hasAttribute("allowComputer") ? el.attribute("allowComputer").toInt() : false;
    event.cancelAfterFirstVisit = el.hasAttribute("cancelAfterFirstVisit") ? el.attribute("cancelAfterFirstVisit").toInt() : true;

    QDomElement msgElem = el.firstChildElement("msg");
    event.message = msgElem.text();

    return el;
}

MapSphinx::MapSphinx(const QPoint & pos, quint32 id, const mp2sphinx_t & mp2)
    : MapObject(pos, id, MapObj::Sphinx), artifact(mp2.artifact), answers(mp2.answers), message(mp2.text)
{
    resources.wood = mp2.resources[0];
    resources.mercury = mp2.resources[1];
    resources.ore = mp2.resources[2];
    resources.sulfur = mp2.resources[3];
    resources.crystal = mp2.resources[4];
    resources.gems = mp2.resources[5];
    resources.gold = mp2.resources[6];
}

MapSphinx::MapSphinx(const QPoint & pos, quint32 id)
    : MapObject(pos, id, MapObj::Sphinx), artifact(Artifact::Unknown)
{
}

QDomElement & operator<< (QDomElement & el, const MapSphinx & sphinx)
{
    el << static_cast<const MapObject &>(sphinx);

    QDomDocument doc = el.ownerDocument();

    QDomElement resourcesElem = doc.createElement("resources");
    el.appendChild(resourcesElem);
    resourcesElem << sphinx.resources;

    el.setAttribute("artifact", sphinx.artifact);

    QDomElement answersElem = doc.createElement("answers");
    el.appendChild(answersElem);

    for(QStringList::const_iterator
	it = sphinx.answers.begin(); it != sphinx.answers.end(); ++it)
	answersElem.appendChild(doc.createElement("answer")).appendChild(doc.createTextNode(*it));

    el.appendChild(doc.createElement("msg")).appendChild(doc.createTextNode(sphinx.message));

    return el;
}

QDomElement & operator>> (QDomElement & el, MapSphinx & sphinx)
{
    el >> static_cast<MapObject &>(sphinx);

    QDomElement resourcesElem = el.firstChildElement("resources");
    resourcesElem >> sphinx.resources;

    sphinx.artifact = el.hasAttribute("artifact") ? el.attribute("artifact").toInt() : 0;

    sphinx.answers.clear();
    QDomElement answersElem = el.firstChildElement("answers");
    QDomNodeList list = answersElem.elementsByTagName("answer");

    for(int pos = 0; pos < list.size(); ++pos)
	sphinx.answers << list.item(pos).toElement().text();

    QDomElement msgElem = el.firstChildElement("msg");
    sphinx.message = msgElem.text();

    return el;
}

MapResource::MapResource(const QPoint & pos, quint32 id, int res ,int val)
    : MapObject(pos, id, MapObj::Resource), type(res), count(val)
{
}

MapResource::MapResource(const QPoint & pos, quint32 id)
    : MapObject(pos, id, MapObj::Resource), type(Resource::Unknown), count(0)
{
}

QDomElement & operator<< (QDomElement & el, const MapResource & res)
{
    el << static_cast<const MapObject &>(res);

    el.setAttribute("type", res.type);
    el.setAttribute("count", res.count);

    return el;
}

QDomElement & operator>> (QDomElement & el, MapResource & res)
{
    el >> static_cast<MapObject &>(res);

    res.type = el.hasAttribute("type") ? el.attribute("type").toInt() : 0;
    res.count = el.hasAttribute("count") ? el.attribute("count").toInt() : 0;

    return el;
}

DayEvent::DayEvent(const mp2dayevent_t & mp2)
    : allowComputer(mp2.allowComputer), dayFirstOccurent(mp2.dayFirstOccurent),
	daySubsequentOccurrences(mp2.subsequentOccurrences), colors(0), message(mp2.text)
{
    resources.wood = mp2.resources[0];
    resources.mercury = mp2.resources[1];
    resources.ore = mp2.resources[2];
    resources.sulfur = mp2.resources[3];
    resources.crystal = mp2.resources[4];
    resources.gems = mp2.resources[5];
    resources.gold = mp2.resources[6];

    if(mp2.colors[0]) colors |= Color::Blue;
    if(mp2.colors[1]) colors |= Color::Red;
    if(mp2.colors[2]) colors |= Color::Green;
    if(mp2.colors[3]) colors |= Color::Yellow;
    if(mp2.colors[4]) colors |= Color::Orange;
    if(mp2.colors[5]) colors |= Color::Purple;
}

QString DayEvent::header(void) const
{
    QString header;
    QTextStream ts(& header);
    ts << "Day " << dayFirstOccurent << " - " << message;
    return header;
}

QDomElement & operator<< (QDomElement & el, const DayEvent & event)
{
    el.setAttribute("colors", event.colors);
    el.setAttribute("allowComputer", event.allowComputer);

    el.setAttribute("dayFirst", event.dayFirstOccurent);
    el.setAttribute("daySubsequent", event.daySubsequentOccurrences);

    QDomDocument doc = el.ownerDocument();

    QDomElement resourcesElem = doc.createElement("resources");
    el.appendChild(resourcesElem);
    resourcesElem << event.resources;

    el.appendChild(doc.createElement("msg")).appendChild(doc.createTextNode(event.message));

    return el;
}

QDomElement & operator>> (QDomElement & el, DayEvent & event)
{
    event.colors = el.hasAttribute("colors") ? el.attribute("colors").toInt() : 0;
    event.allowComputer = el.hasAttribute("allowComputer") ? el.attribute("allowComputer").toInt() : false;

    event.dayFirstOccurent = el.hasAttribute("dayFirst") ? el.attribute("dayFirst").toInt() : 0;
    event.daySubsequentOccurrences = el.hasAttribute("daySubsequent") ? el.attribute("daySubsequent").toInt() : 0;

    QDomElement resourcesElem = el.firstChildElement("resources");
    resourcesElem >> event.resources;

    QDomElement msgElem = el.firstChildElement("msg");
    event.message = msgElem.text();

    return el;
}

MapObjects::MapObjects()
{
}

MapObjects::MapObjects(const MapObjects & mo, const QRect & rt)
{
    //QMap<MapKey, QSharedPointer<MapObject> >
}

void MapObjects::remove(const QPoint & pos)
{
    erase(std::remove(begin(), end(), pos), end());
}

void MapObjects::remove(int uid)
{
    erase(std::remove(begin(), end(), uid), end());
}

SharedMapObject MapObjects::find(const QPoint & pos) const
{
    const_iterator it = std::find(begin(), end(), pos);
    return it != end() ? *it : NULL;
}

QList<SharedMapObject> MapObjects::list(int type) const
{
    QList<SharedMapObject> result;

    for(const_iterator it = begin(); it != end(); ++it)
	if(*it == type) result.push_back(*it);

    return result;
}

QDomElement & operator<< (QDomElement & el, const MapObjects & objects)
{
    QDomDocument doc = el.ownerDocument();

    for(QList<SharedMapObject>::const_iterator
	it = objects.begin(); it != objects.end(); ++it)
    {
	QDomElement elem = doc.createElement((*it).data()->object());
	el.appendChild(elem);

	switch((*it).data()->type())
	{
	    case MapObj::Castle: { MapTown* obj = dynamic_cast<MapTown*>((*it).data()); if(obj) elem << *obj; } break;
	    case MapObj::Heroes: { MapHero* obj = dynamic_cast<MapHero*>((*it).data());	if(obj) elem << *obj; } break;
	    case MapObj::Sign:   { MapSign* obj = dynamic_cast<MapSign*>((*it).data());	if(obj) elem << *obj; } break;
	    case MapObj::Event:  { MapEvent* obj = dynamic_cast<MapEvent*>((*it).data()); if(obj) elem << *obj; } break;
	    case MapObj::Sphinx: { MapSphinx* obj = dynamic_cast<MapSphinx*>((*it).data()); if(obj) elem << *obj; } break;
	    case MapObj::Resource: { MapResource* obj = dynamic_cast<MapResource*>((*it).data()); if(obj) elem << *obj; } break;
	    default: elem << *(*it).data(); break;
	}
    }

    return el;
}

QDomElement & operator>> (QDomElement & el, MapObjects & objects)
{
    QDomDocument doc = el.ownerDocument();
    objects.clear();

    for(QDomElement elem = el.firstChildElement(); ! elem.isNull(); elem = el.nextSiblingElement())
    {
	if(elem.tagName() == "town")
	{ MapTown* obj = new MapTown(); elem >> *obj; objects.push_back(obj); }
	else
	if(elem.tagName() == "hero")
	{ MapHero* obj = new MapHero(); elem >> *obj; objects.push_back(obj); }
	else
	if(elem.tagName() == "sign")
	{ MapSign* obj = new MapSign(); elem >> *obj; objects.push_back(obj); }
	else
	if(elem.tagName() == "event")
	{ MapEvent* obj = new MapEvent(); elem >> *obj; objects.push_back(obj); }
	else
	if(elem.tagName() == "sphinx")
	{ MapSphinx* obj = new MapSphinx(); elem >> *obj; objects.push_back(obj); }
	else
	if(elem.tagName() == "resource")
	{ MapResource* obj = new MapResource(); elem >> *obj; objects.push_back(obj); }
    }

    return el;
}

DayEvents::DayEvents()
{
}

QDomElement & operator<< (QDomElement & el, const DayEvents & events)
{
    for(DayEvents::const_iterator
        it = events.begin(); it != events.end(); ++it)
    {
	QDomElement elem = el.ownerDocument().createElement("event");
        el.appendChild(elem);
        elem << *it;
    }

    return el;
}

QDomElement & operator>> (QDomElement & el, DayEvents & events)
{
    QDomDocument doc = el.ownerDocument();
    events.clear();

    QDomNodeList list = el.elementsByTagName("event");
    for(int pos = 0; pos < list.size(); ++pos)
    {
	DayEvent event;
	QDomElement elem = list.item(pos).toElement();
	elem >> event;
	events.push_back(event);
    }

    return el;
}

QDomElement & operator<< (QDomElement & el, const Resources & res)
{
    el.setAttribute("wood", res.wood);
    el.setAttribute("mercury", res.mercury);
    el.setAttribute("ore", res.ore);
    el.setAttribute("sulfur", res.sulfur);
    el.setAttribute("crystal", res.crystal);
    el.setAttribute("gems", res.gems);
    el.setAttribute("gold", res.gold);

    return el;
}

QDomElement & operator>> (QDomElement & el, Resources & res)
{
    res.wood = el.hasAttribute("wood") ? el.attribute("wood").toInt() : 0;
    res.mercury = el.hasAttribute("mercury") ? el.attribute("mercury").toInt() : 0;
    res.ore = el.hasAttribute("ore") ? el.attribute("ore").toInt() : 0;
    res.sulfur = el.hasAttribute("sulfur") ? el.attribute("sulfur").toInt() : 0;
    res.crystal = el.hasAttribute("crystal") ? el.attribute("crystal").toInt() : 0;
    res.gems = el.hasAttribute("gems") ? el.attribute("gems").toInt() : 0;
    res.gold = el.hasAttribute("gold") ? el.attribute("gold").toInt() : 0;

    return el;
}

QDomElement & operator<< (QDomElement & el, const TavernRumors & rumors)
{
    QDomDocument doc = el.ownerDocument();

    for(TavernRumors::const_iterator
        it = rumors.begin(); it != rumors.end(); ++it)
        el.appendChild(doc.createElement("msg")).appendChild(doc.createTextNode(*it));

    return el;
}

QDomElement & operator>> (QDomElement & el, TavernRumors & rumors)
{
    rumors.clear();
    QDomNodeList list = el.elementsByTagName("msg");

    for(int pos = 0; pos < list.size(); ++pos)
	rumors << list.item(pos).toElement().text();

    return el;
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
	return Top;
    else
    if(level == "top")
	return Top;

    qDebug() << "SpriteLevel::fromString:" << "unknown sprite level";
    return Unknown;
}

CompositeSprite::CompositeSprite(const QDomElement & elem)
    : spriteIndex(elem.attribute("index").toInt()), spriteLevel(0), spritePassable(Direction::All), spriteAnimation(0)
{
    spritePos.setX(elem.attribute("px").toInt());
    spritePos.setY(elem.attribute("py").toInt());

    spriteLevel = SpriteLevel::fromString(elem.attribute("level").toLower());

    if(elem.hasAttribute("passable"))
	spritePassable = elem.attribute("passable").toInt(NULL, 0);

    if(elem.hasAttribute("animation"))
	spriteAnimation = elem.attribute("animation").toInt();
}

CompositeObject::CompositeObject(const QDomElement & elem)
    : name(elem.attribute("name")), size(elem.attribute("width").toInt(), elem.attribute("height").toInt()), classId(elem.attribute("cid").toInt(NULL, 0))
{
    QString icnStr = elem.attribute("icn").toUpper();
    if(0 > icnStr.lastIndexOf(".ICN")) icnStr.append(".ICN");
    icn = qMakePair(icnStr, H2::mapICN(icnStr));

    QDomNodeList list = elem.elementsByTagName("sprite");
    for(int pos = 0; pos < list.size(); ++pos)
	push_back(CompositeSprite(list.item(pos).toElement()));
}

bool CompositeObject::isValid(void) const
{
    return ! name.isEmpty();
}

CompositeObjectCursor::CompositeObjectCursor(const CompositeObject & obj) : CompositeObject(obj), valid(true)
{
    const QSize & tileSize = EditorTheme::tileSize();
    const QSize areaSize(tileSize.width() * size.width(), tileSize.height() * size.height());

    objectArea = EditorTheme::getImage(obj);
    centerOffset = QPoint(areaSize.width() - tileSize.width(),
				areaSize.height() - tileSize.height());

    // generate passable color map
    passableMap = QPixmap(areaSize);
    passableMap.fill(Qt::transparent);

    QPixmap yellowBound = Editor::pixmapBorder(tileSize - QSize(2, 2), QColor(255, 255, 0));
    QPainter paint(& passableMap);

    for(CompositeObject::const_iterator
	it = begin(); it != end(); ++it)
    {
	const QPoint offset((*it).spritePos.x() * tileSize.width(),
				(*it).spritePos.y() * tileSize.height());

	QPixmap tileP = Editor::pixmapBorderPassable(tileSize - QSize(2, 2), (*it).spritePassable);

	switch((*it).spriteLevel)
	{
	    case SpriteLevel::Bottom:
		paint.drawPixmap(offset + QPoint(1, 1), tileP);
		break;

	    case SpriteLevel::Action:
		paint.drawPixmap(offset + QPoint(1, 1), tileP);
		centerOffset = offset;
		break;

	    case SpriteLevel::Top:
		paint.drawPixmap(offset + QPoint(1, 1), yellowBound);
		break;

	    default: break;
	}
    }
}

void CompositeObjectCursor::paint(QPainter & painter, const QPoint & pos, bool allow)
{
    Q_UNUSED(allow);
    scenePos = pos;

    painter.drawPixmap(pos, objectArea);
    painter.drawPixmap(pos, passableMap);
}

QRect CompositeObjectCursor::area(void) const
{
    return QRect(scenePos, objectArea.size());
}

QPoint CompositeObjectCursor::center(void) const
{
    return centerOffset;
}

void CompositeObjectCursor::reset(void)
{
    valid = false;
}

bool CompositeObjectCursor::isValid(void) const
{
    return valid;
}

QString GameCondition::variantString(void) const
{
    QString res;
    if(QVariant::Point == variant().type())
    {
        QPoint pt = variant().toPoint();
        QTextStream ts(& res);
        ts << pt.x() << "," << pt.y();
    }
    else
    if(QVariant::Int == variant().type())
        res = QString::number(variant().toInt());
    return res;
}

int Color::count(int v)
{
    int res = 0;
    if(Blue & v) ++res;
    if(Red & v) ++res;
    if(Green & v) ++res;
    if(Yellow & v) ++res;
    if(Orange & v) ++res;
    if(Purple & v) ++res;
    return res;
}

QColor Color::convert(int v)
{
    switch(v)
    {
	case Blue:	return QColor(0, 0, 0xFF);
	case Red:	return QColor(0xFF, 0, 0);
	case Green:	return QColor(0, 0xFF, 0);
	case Yellow:	return QColor(0xFF, 0xFF, 0);
	case Orange:	return QColor(0xFF, 0x66, 0);
	case Purple:	return QColor(0xFF, 0, 0xFF);
	default: break;
    }

    return QColor(0, 0, 0);
}

QPixmap Color::pixmap(int v, const QSize & sz)
{
    QPixmap pixmap(sz);
    pixmap.fill(convert(v));
    return pixmap;
}

QVector<int> Color::colors(int v)
{
    QVector<int> res;
    res.reserve(6);
    if(Blue & v) res.push_back(Blue);
    if(Green & v) res.push_back(Green);
    if(Red & v) res.push_back(Red);
    if(Yellow & v) res.push_back(Yellow);
    if(Orange & v) res.push_back(Orange);
    if(Purple & v) res.push_back(Purple);
    return res;
}

QDomElement & operator<< (QDomElement & el, const QSize & sz)
{
    el.setAttribute("width", sz.width());
    el.setAttribute("height", sz.height());
    return el;
}

QDomElement & operator>> (QDomElement & el, QSize & sz)
{
    sz.setWidth(el.hasAttribute("width") ? el.attribute("width").toInt() : 0);
    sz.setHeight(el.hasAttribute("height") ? el.attribute("height").toInt() : 0);
    return el;
}

QDomElement & operator<< (QDomElement & el, const QPoint & pt)
{
    el.setAttribute("posx", pt.x());
    el.setAttribute("posy", pt.y());
    return el;
}

QDomElement & operator>> (QDomElement & el, QPoint & pt)
{
    pt.setX(el.hasAttribute("posx") ? el.attribute("posx").toInt() : 0);
    pt.setY(el.hasAttribute("posy") ? el.attribute("posy").toInt() : 0);
    return el;
}

QString MapObj::transcribe(int index)
{
    const char* names[] = { "None", "AlchemyLab", "Sign", "Buoy", "Skeleton", "DaemonCave", "TreasureChest", "FaerieRing", "CampFire", "Fountain",
	"Gazebo", "AncientLamp", "Graveyard", "ArcherHouse", "GoblinHut", "DwarfCott", "PeasantHut", "Unused17", "Unused18", "Event", "DragonCity",
	"LightHouse", "WaterWheel", "Mines", "Monster", "Obelisk", "Oasis", "Resource", "Coast", "SawMill", "Oracle", "Shrine1", "ShipWreck",
	"Unused33", "DesertTent", "Castle", "StoneLights", "WagonCamp", "WaterChest", "WhirlPool", "WindMill", "Artifact", "Reefs", "Boat",
	"RndUltimateArtifact", "RndArtifact", "RndResource", "RndMonster", "RndTown", "RndCastle", "Mermaid", "RndMonster1", "RndMonster2",
	"RndMonster3", "RndMonster4", "Heroes", "Sirens", "HutMagi", "WatchTower", "TreeHouse", "TreeCity", "Ruins", "Fort", "TradingPost",
	"AbandonedMine", "ThatchedHut", "StandingStones", "Idol", "TreeKnowledge", "DoctorHut", "Temple", "HillFort", "HalflingHole", "MercenaryCamp",
	"Shrine2", "Shrine3", "Pyramid", "CityDead", "Excavation", "Sphinx", "Wagon", "Tarpit", "ArtesianSpring", "TrollBridge", "WateringHole",
	"WitchsHut", "Xanadu", "Cave", "Leanto", "MagellanMaps", "FlotSam", "DerelictShip", "ShipwreckSurviror", "Bottle", "MagicWell", "MagicGarden",
	"ObservationTower", "FreemanFoundry", "EyeMagi", "Trees", "Mounts", "Volcano", "Flowers", "Stones", "WaterLake", "Mandrake", "DeadTree", "Stump",
	"Crater", "Cactus", "Mound", "Dune", "LavaPool", "Shrub", "Arena", "BarrowMounds", "RndArtifact1", "RndArtifact2", "RndArtifact3", "Barrier",
	"TravellerTent", "AlchemyTower", "Stables", "Jail", "FireAltar", "AirAltar", "EarthAltar", "WaterAltar" };

    return QString(names[index & 0x7F]);
}

QString Artifact::transcribe(int index)
{
    const char* names[] = { "None", "Ultimate Book of Knowledge", "Ultimate Sword of Dominion", "Ultimate Cloak of Protection", "Ultimate Wand of Magic",
	"Ultimate Shield", "Ultimate Staff", "Ultimate Crown", "Golden Goose", "Arcane Necklace of Magic", "Caster's Bracelet of Magic", "Mage's Ring of Power",
	"Witch's Broach of Magic", "Medal of Valor", "Medal of Courage", "Medal of Honor", "Medal of Distinction", "Fizbin of Misfortune", "Thunder Mace of Dominion",
	"Armored Gauntlets of Protection", "Defender Helm of Protection", "Giant Flail of Dominion", "Ballista of Quickness", "Stealth Shield of Protection",
	"Dragon Sword of Dominion", "Power Axe of Dominion", "Divine Breastplate of Protection", "Minor Scroll of Knowledge", "Major Scroll of Knowledge",
	"Superior Scroll of Knowledge", "Foremost Scroll of Knowledge", "Endless Sack of Gold", "Endless Bag of Gold", "Endless Purse of Gold", "Nomad Boots of Mobility",
	"Traveler's Boots of Mobility", "Lucky Rabbit's Foot", "Golden Horseshoe", "Gambler's Lucky Coin", "Four-Leaf Clover", "True Compass of Mobility",
	"Sailor's Astrolabe of Mobility", "Evil Eye", "Enchanted Hourglass", "Gold Watch", "Skullcap", "Ice Cloak", "Fire Cloak", "Lightning Helm", "Evercold Icicle",
	"Everhot Lava Rock", "Lightning Rod", "Snake-Ring", "Ankh", "Book of Elements", "Elemental Ring", "Holy Pendant", "Pendant of Free Will", "Pendant of Life",
	"Serenity Pendant", "Seeing-eye Pendant", "Kinetic Pendant", "Pendant of Death", "Wand of Negation", "Golden Bow", "Telescope", "Statesman's Quill",
	"Wizard's Hat", "Power Ring", "Ammo Cart", "Tax Lien", "Hideous Mask", "Endless Pouch of Sulfur", "Endless Vial of Mercury", "Endless Pouch of Gems",
	"Endless Cord of Wood", "Endless Cart of Ore", "Endless Pouch of Crystal", "Spiked Helm", "Spiked Shield", "White Pearl", "Black Pearl", "Magic Book",
	"Dummy 1", "Dummy 2", "Dummy 3", "Dummy 4", "Spell Scroll", "Arm of the Martyr", "Breastplate of Anduran", "Broach of Shielding", "Battle Garb of Anduran",
	"Crystal Ball", "Heart of Fire", "Heart of Ice", "Helmet of Anduran", "Holy Hammer", "Legendary Scepter", "Masthead", "Sphere of Negation", "Staff of Wizardry",
	"Sword Breaker", "Sword of Anduran", "Spade of Necromancy", "Unknown" };

    return isValid(index) ? QString(names[index]) : QString(names[Unknown]);
}

bool Artifact::isValid(int index)
{
    return 0 <= index && Unknown > index;
}
