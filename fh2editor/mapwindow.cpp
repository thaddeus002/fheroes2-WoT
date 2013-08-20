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

MapWindow::MapWindow(MainWindow* parent) : QGraphicsView(parent), mapData(this)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setMouseTracking(true);

    isUntitled = true;
    isModified = false;

    miniMap = new Form::MiniMap(this);
    connect(miniMap, SIGNAL(windowPositionNeedChange(const QPoint &)), this, SLOT(viewportSetPositionFromMiniMap(const QPoint &)));

    townList = new Form::TownList(this);
    connect(townList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(viewportSetPositionFromListWidget(QListWidgetItem*)));

    heroList = new Form::HeroList(this);
    connect(heroList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(viewportSetPositionFromListWidget(QListWidgetItem*)));

    infoForm = new Form::InfoForm(this);

    // change size
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(changeViewedRect()));
    connect(verticalScrollBar() , SIGNAL(valueChanged(int)), this, SLOT(changeViewedRect()));
    connect(horizontalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(changeViewedRect()));
    connect(verticalScrollBar() , SIGNAL(rangeChanged(int, int)), this, SLOT(changeViewedRect()));
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

    miniMap->generateFromTiles(mapData.tiles());
    townList->load(mapData.objects());
    heroList->load(mapData.objects());

    QPair<int, int> vers = mapData.versions();

    if(vers.first != vers.second)
	mapWasModified();

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

void MapWindow::mapWasModified(void)
{
    isModified = true;
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
    setWindowModified(true);
    if(miniMap) miniMap->generateFromTiles(mapData.tiles());
    if(townList) townList->load(mapData.objects());
    if(heroList) heroList->load(mapData.objects());
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

Form::MiniMap* MapWindow::miniMapWidget(void)
{
    return miniMap;
}

Form::TownList* MapWindow::townListWidget(void)
{
    return townList;
}

Form::HeroList* MapWindow::heroListWidget(void)
{
    return heroList;
}

Form::InfoForm* MapWindow::infoWidget(void)
{
    return infoForm;
}

void MapWindow::viewportSetPositionFromMiniMap(const QPoint & miniPos)
{
    if(miniMap)
    {
	QPointF pos(mapData.size().width() * miniPos.x(), mapData.size().height() * miniPos.y());

	pos.rx() /= miniMap->mapSize().width();
	pos.ry() /= miniMap->mapSize().height();

	if(0 <= pos.x() && 0 <= pos.y() &&
	    pos.x() < mapData.size().width() && pos.y() < mapData.size().height())
	{
	    const QSize & ts = EditorTheme::tileSize();
	    centerOn(pos.x() * ts.width(), pos.y() * ts.height());
	}
    }
}

void MapWindow::viewportSetPositionFromListWidget(QListWidgetItem* item)
{
    if(item)
    {
	QPoint pos = qvariant_cast<QPoint>(item->data(Qt::UserRole));
        const QSize & ts = EditorTheme::tileSize();
	centerOn(pos.x() * ts.width(), pos.y() * ts.height());
    }
}

void MapWindow::changeViewedRect(void)
{
    if(miniMap)
    {
	const QSize & ts = EditorTheme::tileSize();
	const QSize absSize = QSize(mapData.size().width() * ts.width(), mapData.size().height() * ts.height());
	const QSize tmpSize = QSize(size().width() * miniMap->mapSize().width(), size().height() * miniMap->mapSize().height());
	int mw = tmpSize.width() / absSize.width();
	int mh = tmpSize.height() / absSize.height();
	miniMap->setWindowPos(horizontalScrollBar()->value() * miniMap->mapSize().width() / absSize.width(),
				verticalScrollBar()->value() * miniMap->mapSize().height() / absSize.height(),
				mw ? mw : 1, mh ? mh : 1);
    }
}
