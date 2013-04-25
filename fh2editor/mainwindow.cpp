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

#include "dialogs.h"
#include "mainwindow.h"
#include "mapwindow.h"

MainWindow::MainWindow() : sequenceMapNumber(0)
{
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateStatusBar()));

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    updateMenus();
    updateStatusBar();

    readSettings();

    setWindowTitle(tr("FHeroes2 Map Editor"));
    setUnifiedTitleAndToolBarOnMac(true);
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    mdiArea->closeAllSubWindows();

    if(mdiArea->currentSubWindow())
        event->ignore();
    else
    {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newFile(void)
{
    Form::SelectMapSize form;

    if(QDialog::Accepted == form.exec())
    {
	MapWindow* child = createMapWindow();
	child->newFile(form.result, sequenceMapNumber++);
	child->show();
    }
}

void MainWindow::open(void)
{
    QString fileName = QFileDialog::getOpenFileName(this);

    if(!fileName.isEmpty())
    {
        QMdiSubWindow* existing = findMapWindow(fileName);

        if(existing)
            mdiArea->setActiveSubWindow(existing);
	else
	{
	    MapWindow* child = createMapWindow();

    	    if(child->loadFile(fileName))
	    {
        	statusBar()->showMessage(tr("File loaded"), 2000);
        	child->show();
    	    }
	    else
        	child->close();
	}
    }
}

void MainWindow::save(void)
{
    if(activeMapWindow() && activeMapWindow()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs(void)
{
    if(activeMapWindow() && activeMapWindow()->saveAs())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::about(void)
{
   QMessageBox::about(this, tr("Map Editor"), tr("<b>Demo version 0.1.</b>"));
}

void MainWindow::updateMenus(void)
{
    bool hasMapWindow = (activeMapWindow() != 0);

    fileSaveAct->setEnabled(hasMapWindow);
    fileSaveAsAct->setEnabled(hasMapWindow);
    closeAct->setEnabled(hasMapWindow);
    closeAllAct->setEnabled(hasMapWindow);
    tileAct->setEnabled(hasMapWindow);
    cascadeAct->setEnabled(hasMapWindow);
    nextAct->setEnabled(hasMapWindow);
    previousAct->setEnabled(hasMapWindow);
    separatorAct->setVisible(hasMapWindow);
    mapOptionsAct->setEnabled(hasMapWindow);
    showRadarAct->setEnabled(hasMapWindow);
    showPassableAct->setEnabled(hasMapWindow);
}

void MainWindow::updateWindowMenu(void)
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow*> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for(int i = 0; i < windows.size(); ++i)
    {
        MapWindow* child = qobject_cast<MapWindow*>(windows.at(i)->widget());

        QString text;

        if(i < 9)
            text = tr("&%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
	else
            text = tr("%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());

        QAction* action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMapWindow());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

MapWindow* MainWindow::createMapWindow(void)
{
    MapWindow* child = new MapWindow(this);
    mdiArea->addSubWindow(child);
    child->parentWidget()->setGeometry(child->parentWidget()->pos().x(), child->parentWidget()->pos().y(), 480, 320);
    return child;
}

void MainWindow::createActions(void)
{
    fileNewAct = new QAction(QIcon(":/images/menu_new.png"), tr("&New"), this);
    fileNewAct->setShortcuts(QKeySequence::New);
    fileNewAct->setStatusTip(tr("Create a new file"));
    connect(fileNewAct, SIGNAL(triggered()), this, SLOT(newFile()));

    fileOpenAct = new QAction(QIcon(":/images/menu_open.png"), tr("&Open..."), this);
    fileOpenAct->setShortcuts(QKeySequence::Open);
    fileOpenAct->setStatusTip(tr("Open an existing file"));
    connect(fileOpenAct, SIGNAL(triggered()), this, SLOT(open()));

    fileSaveAct = new QAction(QIcon(":/images/menu_save.png"), tr("&Save"), this);
    fileSaveAct->setShortcuts(QKeySequence::Save);
    fileSaveAct->setStatusTip(tr("Save the map to disk"));
    connect(fileSaveAct, SIGNAL(triggered()), this, SLOT(save()));

    fileSaveAsAct = new QAction(tr("Save &As..."), this);
    fileSaveAsAct->setShortcuts(QKeySequence::SaveAs);
    fileSaveAsAct->setStatusTip(tr("Save the map under a new name"));
    connect(fileSaveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    mapOptionsAct = new QAction(QIcon(":/images/map_options.png"), tr("&Options"), this);
    mapOptionsAct->setStatusTip(tr("Show map options"));
    connect(mapOptionsAct, SIGNAL(triggered()), this, SLOT(mapOptions()));

    fileExitAct = new QAction(tr("E&xit"), this);
    fileExitAct->setShortcuts(QKeySequence::Quit);
    fileExitAct->setStatusTip(tr("Exit the application"));
    connect(fileExitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous window"));
    connect(previousAct, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    menuAboutAct = new QAction(tr("&About"), this);
    menuAboutAct->setStatusTip(tr("Show the application's About box"));
    connect(menuAboutAct, SIGNAL(triggered()), this, SLOT(about()));

    showRadarAct = new QAction(tr("Radar"), this);
    showRadarAct->setStatusTip(tr("Show the mini map"));

    showPassableAct = new QAction(tr("Passable Mode"), this);
    showPassableAct->setStatusTip(tr("Show the passable mode"));
}

void MainWindow::createMenus(void)
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(fileNewAct);
    fileMenu->addAction(fileOpenAct);
    fileMenu->addAction(fileSaveAct);
    fileMenu->addAction(fileSaveAsAct);
    fileMenu->addSeparator();

    fileMenu->addAction(fileExitAct);

    mapMenu = menuBar()->addMenu(tr("&Map"));
    mapMenu->addAction(mapOptionsAct);
    mapMenu->addSeparator()->setText(tr("View Mode"));
    mapMenu->addAction(showRadarAct);
    mapMenu->addAction(showPassableAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();
    menuBar()->addAction(menuAboutAct);
}

void MainWindow::createToolBars(void)
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(fileNewAct);
    fileToolBar->addAction(fileOpenAct);
    fileToolBar->addAction(fileSaveAct);
}

void MainWindow::createStatusBar(void)
{
    labelTileX = new QLabel(this);
    labelTileY = new QLabel(this);

    statusBar()->addPermanentWidget(new QLabel("tile x:", this));
    statusBar()->addPermanentWidget(labelTileX);
    statusBar()->addPermanentWidget(new QLabel("tile y:", this));
    statusBar()->addPermanentWidget(labelTileY);
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::updateStatusBar(void)
{
    MapWindow* mapWindow = activeMapWindow();

    disconnect(labelTileX, SLOT(setNum(int)));
    disconnect(labelTileY, SLOT(setNum(int)));

    if(mapWindow)
    {
	connect(mapWindow, SIGNAL(cursorTileXPosChanged(int)), labelTileX, SLOT(setNum(int)));
	connect(mapWindow, SIGNAL(cursorTileYPosChanged(int)), labelTileY, SLOT(setNum(int)));
    }
}

void MainWindow::readSettings(void)
{
    QSettings settings("fheroes2", "editor");

    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    sequenceMapNumber = settings.value("sequenceMapNumber", 1).toInt();

    move(pos);
    resize(size);
}

void MainWindow::writeSettings(void)
{
    QSettings settings("fheroes2", "editor");

    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("sequenceMapNumber", sequenceMapNumber);
}

MapWindow* MainWindow::activeMapWindow(void)
{
    QMdiSubWindow* activeSubWindow = mdiArea->activeSubWindow();

    return activeSubWindow ? qobject_cast<MapWindow*>(activeSubWindow->widget()) : 0;
}

QMdiSubWindow* MainWindow::findMapWindow(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach(QMdiSubWindow* window, mdiArea->subWindowList())
    {
        MapWindow* child = qobject_cast<MapWindow*>(window->widget());

        if(child && child->currentFile() == canonicalFilePath)
            return window;
    }

    return 0;
}

void MainWindow::setActiveSubWindow(QWidget* window)
{
    if(window)
	mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(window));
}

void MainWindow::mapOptions(void)
{
    if(activeMapWindow())
	activeMapWindow()->mapData.showMapOptions();
}
