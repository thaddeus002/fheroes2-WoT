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

#ifndef _EDITOR_MAPWINDOW_H_
#define _EDITOR_MAPWINDOW_H_

#include <QGraphicsView>

#include "mapdata.h"

class MapWindow : public QGraphicsView
{
    Q_OBJECT

public:
    MapWindow(AGG::File &);

    void	newFile(const QSize &, int);
    bool	loadFile(const QString &);
    bool	save(void);
    bool	saveAs(void);
    bool	saveFile(const QString &);
    QString	userFriendlyCurrentFile(void);
    QString	currentFile(void);
    int		modeView(void) const;
    void	copy(void);
    void	paste(void);

signals:
    void	copyAvailable(bool);

public slots:
    void	setModeView(int);

protected:
    void	closeEvent(QCloseEvent*);

private slots:
    void	mapWasSelectionChanged(void);
    void	mapWasModified(void);

private:
    bool	maybeSave(void);
    void	setCurrentFile(const QString &);
    QString	strippedName(const QString &);

    QString	curFile;
    bool	isUntitled;
    bool	isModified;

    MapData	mapData;

    static int sequenceNumber;
};

#endif
