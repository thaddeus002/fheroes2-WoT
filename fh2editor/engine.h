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
#include <QPixmap>

namespace H2
{
    class File : public QFile
    {
    public:
	File();
	File(const QString &);

	qint8	readByte(void);
	QString	readString(size_t);
	qint16	readLE16(void);
	qint32	readLE32(void);
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

    class File : protected H2::File
    {
    protected:
	QFile			file;
	QMap<QString, Item>	items;
	QVector<QRgb>		colors;

	QByteArray	readRawData(const QString &);
	bool		loadFile(const QString &);

    public:
	File();

	QPixmap getImageTIL(const QString &, quint16);
    };
}

#endif
