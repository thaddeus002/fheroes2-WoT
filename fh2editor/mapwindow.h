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
#include <QAction>

#include "mapdata.h"

class MainWindow;

QT_BEGIN_NAMESPACE
class QContextMenuEvent;
class QActionGroup;
QT_END_NAMESPACE

class ActionFillGround : public QAction
{
    Q_OBJECT

public:
    ActionFillGround(int, QObject*);

    int ground(void) const { return type; }

protected:
    int type;
};

class MapWindow : public QGraphicsView
{
    Q_OBJECT

public:
    MapWindow(MainWindow*);

    void	newFile(const QSize &, int);
    bool	loadFile(const QString &);
    bool	save(void);
    bool	saveAs(void);
    bool	saveFile(const QString &);
    QString	userFriendlyCurrentFile(void);
    QString	currentFile(void);

signals:
    void	selectedItems(bool);
    void	validBuffer(bool);

protected:
    void	closeEvent(QCloseEvent*);
    void	contextMenuEvent(QContextMenuEvent*);

private slots:
    void	mapWasSelectionChanged(void);
    void	mapWasModified(void);
    void	fillGroundAction(QAction*);
    void	clearObjectsAction(QAction*);
    void	copyToBuffer(void);
    void	pasteFromBuffer(void);
    void	editPassableDialog(void);
    void	cellInfoDialog(void);

private:
    friend class MainWindow;
    friend class MapData;

    bool	maybeSave(void);
    void	setCurrentFile(const QString &);
    QString	strippedName(const QString &);

    QString	curFile;
    bool	isUntitled;
    bool	isModified;

    MainWindow*		mainWindow;
    MapData		mapData;

    QAction*            editCopyAct;
    QAction*            editPasteAct;
    QAction*            editPassableAct;
    QAction*            cellInfoAct;

    QActionGroup*	fillGroundAct;
    QActionGroup*	clearObjectsAct;
};

#endif
