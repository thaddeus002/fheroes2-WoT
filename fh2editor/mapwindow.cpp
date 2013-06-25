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

#include <QtGui>
#include <QContextMenuEvent>

#include "dialogs.h"
#include "mainwindow.h"
#include "mapwindow.h"

MapWindow::MapWindow(MainWindow* parent) : mainWindow(parent), mapData(this)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setMouseTracking(true);

    isUntitled = true;
    isModified = false;

    // init: copy, paste
    editCopyAct = new QAction(QIcon(":/images/menu_copy.png"), tr("Copy"), this);
    editCopyAct->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    editCopyAct->setEnabled(false);
    connect(editCopyAct, SIGNAL(triggered()), &mapData, SLOT(copyToBuffer()));

    editPasteAct = new QAction(QIcon(":/images/menu_paste.png"), tr("Paste"), this);
    editPasteAct->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    editPasteAct->setEnabled(false);
    connect(editPasteAct, SIGNAL(triggered()), this, SLOT(mapWasModified()));
    connect(editPasteAct, SIGNAL(triggered()), &mapData, SLOT(pasteFromBuffer()));

    connect(&mapData, SIGNAL(validBuffer(bool)), editPasteAct, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(selectedItems(bool)), editCopyAct, SLOT(setEnabled(bool)));

    //
    addObjectAct = new QAction(QIcon(":/images/add_objects.png"), tr("Add object..."), this);
    addObjectAct->setStatusTip(tr("Select map object"));
    connect(addObjectAct, SIGNAL(triggered()), &mapData, SLOT(selectObjectImage()));

    editObjectAct = new QAction(QIcon(":/images/edit_objects.png"), tr("Edit object..."), this);
    editObjectAct->setStatusTip(tr("Edit map object"));
    connect(editObjectAct, SIGNAL(triggered()), &mapData, SLOT(editObjectAttributes()));

    QAction* curAct;

    // init: fill ground
    fillGroundAct = new QActionGroup(this);

    curAct = new QAction(QIcon(":/images/ground_desert.png"), tr("Desert"), this);
    curAct->setStatusTip(tr("Fill desert ground"));
    curAct->setData(Ground::Desert);
    fillGroundAct->addAction(curAct);

    curAct = new QAction(QIcon(":/images/ground_snow.png"), tr("Snow"), this);
    curAct->setStatusTip(tr("Fill snow ground"));
    curAct->setData(Ground::Snow);
    fillGroundAct->addAction(curAct);

    curAct = new QAction(QIcon(":/images/ground_swamp.png"), tr("Swamp"), this);
    curAct->setStatusTip(tr("Fill swamp ground"));
    curAct->setData(Ground::Swamp);
    fillGroundAct->addAction(curAct);

    curAct = new QAction(QIcon(":/images/ground_wasteland.png"), tr("Wasteland"), this);
    curAct->setStatusTip(tr("Fill wasteland ground"));
    curAct->setData(Ground::Wasteland);
    fillGroundAct->addAction(curAct);

    curAct = new QAction(QIcon(":/images/ground_beach.png"), tr("Beach"), this);
    curAct->setStatusTip(tr("Fill beach ground"));
    curAct->setData(Ground::Beach);
    fillGroundAct->addAction(curAct);

    curAct = new QAction(QIcon(":/images/ground_lava.png"), tr("Lava"), this);
    curAct->setStatusTip(tr("Fill lava ground"));
    curAct->setData(Ground::Lava);
    fillGroundAct->addAction(curAct);

    curAct = new QAction(QIcon(":/images/ground_dirt.png"), tr("Dirt"), this);
    curAct->setStatusTip(tr("Fill dirt ground"));
    curAct->setData(Ground::Dirt);
    fillGroundAct->addAction(curAct);

    curAct = new QAction(QIcon(":/images/ground_grass.png"), tr("Grass"), this);
    curAct->setStatusTip(tr("Fill grass ground"));
    curAct->setData(Ground::Grass);
    fillGroundAct->addAction(curAct);

    curAct = new QAction(QIcon(":/images/ground_water.png"), tr("Water"), this);
    curAct->setStatusTip(tr("Fill water"));
    curAct->setData(Ground::Water);
    fillGroundAct->addAction(curAct);

    connect(fillGroundAct, SIGNAL(triggered(QAction*)), &mapData, SLOT(fillGroundAction(QAction*)));

    // init: clear objects
    clearObjectsAct = new QActionGroup(this);

    curAct = new QAction(tr("Buildings"), this);
    curAct->setStatusTip(tr("Remove buildings"));
    curAct->setData(1);
    clearObjectsAct->addAction(curAct);

    curAct = new QAction(tr("Mounts/Rocs"), this);
    curAct->setStatusTip(tr("Remove mounts/rocs"));
    curAct->setData(2);
    clearObjectsAct->addAction(curAct);

    curAct = new QAction(tr("Trees/Shrubs"), this);
    curAct->setStatusTip(tr("Remove trees/shrubs"));
    curAct->setData(3);
    clearObjectsAct->addAction(curAct);

    curAct = new QAction(tr("Pickup resources"), this);
    curAct->setStatusTip(tr("Remove resources"));
    curAct->setData(4);
    clearObjectsAct->addAction(curAct);

    curAct = new QAction(tr("Artifacts"), this);
    curAct->setStatusTip(tr("Remove artifacts"));
    curAct->setData(5);
    clearObjectsAct->addAction(curAct);

    curAct = new QAction(tr("Monsters"), this);
    curAct->setStatusTip(tr("Remove monsters"));
    curAct->setData(6);
    clearObjectsAct->addAction(curAct);

    curAct = new QAction(tr("Heroes"), this);
    curAct->setStatusTip(tr("Remove heroes"));
    curAct->setData(7);
    clearObjectsAct->addAction(curAct);

    curAct = new QAction(this);
    curAct->setSeparator(true);
    clearObjectsAct->addAction(curAct);

    curAct = new QAction(tr("All"), this);
    curAct->setStatusTip(tr("Remove all objects"));
    curAct->setData(10);
    clearObjectsAct->addAction(curAct);

    connect(clearObjectsAct, SIGNAL(triggered(QAction*)), &mapData, SLOT(removeObjectsAction(QAction*)));

    // init other
    editPassableAct = new QAction(QIcon(":/images/edit_cell.png"), tr("Edit passable"), this);
    editPassableAct->setStatusTip(tr("Edit cell passable"));
    connect(editPassableAct, SIGNAL(triggered()), &mapData, SLOT(editPassableDialog()));

    cellInfoAct = new QAction(QIcon(":/images/cell_info.png"), tr("Cell info"), this);
    cellInfoAct->setStatusTip(tr("Show cell info"));
    connect(cellInfoAct, SIGNAL(triggered()), &mapData, SLOT(cellInfoDialog()));

    selectAllAct = new QAction(QIcon(":/images/menu_fill.png"), tr("Select All"), this);
    selectAllAct->setStatusTip(tr("Select all tiles"));
    connect(selectAllAct, SIGNAL(triggered()), &mapData, SLOT(selectAllTiles()));

    miniMap = new Form::MiniMap(this);
}

void MapWindow::newFile(const QSize & sz, int sequenceNumber)
{
    isUntitled = true;

    QString fileName;
    QTextStream ss(& fileName);

    ss << "fh2map_";
    ss.setFieldWidth(4);
    ss.setPadChar('0');
    ss << sequenceNumber;
    ss.setFieldWidth(0);
    ss << ".map";

    QApplication::setOverrideCursor(Qt::WaitCursor);

    mapData.newMap(sz, curFile);
    setScene(& mapData);

    QApplication::restoreOverrideCursor();
    curFile = fileName;

    connect(&mapData, SIGNAL(selectionChanged(void)), this, SLOT(mapWasSelectionChanged(void)));
    connect(&mapData, SIGNAL(dataModified(void)), this, SLOT(mapWasModified(void)));

    mapWasModified();
}

bool MapWindow::loadFile(const QString & fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if(! mapData.loadMap(fileName))
    {
	QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Map Editor"), tr("Cannot read file %1.").arg(fileName));
        return false;
    }

    setScene(& mapData);

    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);

    connect(&mapData, SIGNAL(selectionChanged(void)), this, SLOT(mapWasSelectionChanged(void)));
    connect(&mapData, SIGNAL(dataModified(void)), this, SLOT(mapWasModified(void)));

    return true;
}

bool MapWindow::save(void)
{
    return isUntitled ? saveAs() : saveFile(curFile);
}

bool MapWindow::saveAs(void)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
    return fileName.isEmpty() ? false : saveFile(fileName);
}

bool MapWindow::saveFile(const QString & fileName)
{
    if(! mapData.saveMapXML(fileName))
    {
        QMessageBox::warning(this, tr("Map Editor"), tr("Cannot write file %1.").arg(fileName));
        return saveAs();
    }

    setCurrentFile(fileName);

    return true;
}

QString MapWindow::userFriendlyCurrentFile(void)
{
    return mapData.name() + " (" + strippedName(curFile) + ")";
}

QString MapWindow::currentFile(void)
{
    return curFile;
}

void MapWindow::closeEvent(QCloseEvent* event)
{
    if(mapData.selectedItems().size())
	mapData.clearSelection();

    if(maybeSave())
        event->accept();
    else
        event->ignore();
}

void MapWindow::mapWasSelectionChanged(void)
{
    emit selectedItems(mapData.selectedItems().size());
}

void MapWindow::mapWasModified(void)
{
    isModified = true;
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
    setWindowModified(true);
}

bool MapWindow::maybeSave(void)
{
    if(isModified)
    {
	QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Map Editor"),
		tr("'%1' has been modified.\nDo you want to save your changes?") .arg(userFriendlyCurrentFile()),
		QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

	switch(ret)
	{
	    case QMessageBox::Save:	return save();
	    case QMessageBox::Cancel:	return false;
	    case QMessageBox::Discard:	return true;
	    default: break;
	}
    }

    return true;
}

void MapWindow::setCurrentFile(const QString & fileName)
{
    QFileInfo fileInfo(fileName);
    curFile = QDir::toNativeSeparators(fileInfo.absolutePath() + QDir::separator() + fileInfo.baseName() + ".map");

    isUntitled = false;
    isModified = false;
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString MapWindow::strippedName(const QString & fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MapWindow::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);

    if(mapData.selectedItems().size())
    {
	menu.addAction(editCopyAct);
	menu.addSeparator();

	QMenu* groundSubMenu = menu.addMenu(QIcon(":/images/menu_fill.png"), tr("Fill Ground"));
	QList<QAction*> actions = fillGroundAct->actions();

	for(QList<QAction*>::const_iterator
	    it = actions.begin(); it != actions.end(); ++it)
	    groundSubMenu->addAction(*it);

	menu.addSeparator();

	QMenu* clearSubMenu = menu.addMenu(QIcon(":/images/clear_objects.png"), tr("Remove Objects"));
	actions = clearObjectsAct->actions();

	for(QList<QAction*>::const_iterator
	    it = actions.begin(); it != actions.end(); ++it)
	    clearSubMenu->addAction(*it);
    }
    else
    {
	menu.addAction(editPasteAct);
	menu.addSeparator();

	menu.addAction(addObjectAct);

	menu.addSeparator();
	menu.addAction(editPassableAct);
        menu.addAction(editObjectAct);
	menu.addAction(cellInfoAct);

	editObjectAct->setEnabled(mapData.currentTile() && mapData.currentTile()->isAction());

	menu.addSeparator();
	menu.addAction(selectAllAct);
    }

    if(selectAllAct != menu.exec(event->globalPos()))
	mapData.clearSelection();

    event->accept();
}

Form::MiniMap* MapWindow::miniMapWidget(void)
{
    return miniMap;
}
