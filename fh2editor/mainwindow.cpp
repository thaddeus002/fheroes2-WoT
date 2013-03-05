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

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);

    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("FHeroes2 Map Editor"));
    setUnifiedTitleAndToolBarOnMac(true);

//    QMessageBox::critical(this, tr("Error"),
//            tr("<b>File HEROES2.AGG not found!</b>"));
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
    MapWindow* child = createMapWindow();
    child->newFile(Dialog::SelectMapSize(), sequenceMapNumber++);
    child->show();
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
		child->resize(QSize(640, 480));
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

void MainWindow::cut(void)
{
//    if(activeMapWindow())
//        activeMapWindow()->cut();
}

void MainWindow::copy(void)
{
//    if(activeMapWindow())
//        activeMapWindow()->copy();
}

void MainWindow::paste(void)
{
//    if(activeMapWindow())
//        activeMapWindow()->paste();
}

void MainWindow::about(void)
{
   QMessageBox::about(this, tr("Map Editor"),
            tr("<b>Version 0.1.</b>"));
}

void MainWindow::updateMenus(void)
{
    bool hasMapWindow = (activeMapWindow() != 0);

    saveAct->setEnabled(hasMapWindow);
    saveAsAct->setEnabled(hasMapWindow);
    pasteAct->setEnabled(hasMapWindow);
    closeAct->setEnabled(hasMapWindow);
    closeAllAct->setEnabled(hasMapWindow);
    tileAct->setEnabled(hasMapWindow);
    cascadeAct->setEnabled(hasMapWindow);
    nextAct->setEnabled(hasMapWindow);
    previousAct->setEnabled(hasMapWindow);
    separatorAct->setVisible(hasMapWindow);

    bool hasSelection = false; /* activeMapWindow() &&
                         activeMapWindow()->textCursor().hasSelection()); */

    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);

    exploreAct->setEnabled(hasMapWindow);
    selectAct->setEnabled(hasMapWindow);

    if(hasMapWindow)
    {
	switch(activeMapWindow()->modeView())
	{
	    case 1:	exploreAct->setChecked(true); break;
	    case 2:	selectAct->setChecked(true); break;
	    default:	break;
	}
    }
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
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
	else
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());

        QAction* action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMapWindow());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

MapWindow* MainWindow::createMapWindow(void)
{
    MapWindow* child = new MapWindow(aggContent);
    mdiArea->addSubWindow(child);

    connect(child, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));

    return child;
}

void MainWindow::createActions(void)
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the map to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the map under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exploreAct = new QAction(QIcon(":/images/explore.png"), tr("Ex&plore"), this);
    //exploreAct->setShortcuts(QKeySequence::);
    exploreAct->setStatusTip(tr("Switch to explore view"));
    exploreAct->setCheckable(true);
    connect(exploreAct, SIGNAL(triggered()), this, SLOT(switchExploreView()));

    selectAct = new QAction(QIcon(":/images/select.png"), tr("Se&lect"), this);
    //selectAct->setShortcuts(QKeySequence::);
    selectAct->setStatusTip(tr("Switch to select view"));
    selectAct->setCheckable(true);
    connect(selectAct, SIGNAL(triggered()), this, SLOT(switchSelectView()));

    modeViewAct = new QActionGroup(this);
    modeViewAct->addAction(exploreAct);
    modeViewAct->addAction(selectAct);
    //connect(modeViewAct, SIGNAL(triggered(QAction*)), this, SLOT());

//! [0]
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
//! [0]

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()),
            mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, SIGNAL(triggered()),
            mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus(void)
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();

    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    modeMenu = menuBar()->addMenu(tr("&Mode"));
    modeMenu->addSeparator()->setText(tr("View"));
    modeMenu->addAction(exploreAct);
    modeMenu->addAction(selectAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();
    menuBar()->addAction(aboutAct);
}

void MainWindow::createToolBars(void)
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);

    modeToolBar = addToolBar(tr("Mode"));
    modeToolBar->addAction(exploreAct);
    modeToolBar->addAction(selectAct);
}

void MainWindow::createStatusBar(void)
{
    statusBar()->showMessage(tr("Ready"));
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

void MainWindow::switchExploreView(void)
{
    if(activeMapWindow())
	activeMapWindow()->setModeView(1);
}

void MainWindow::switchSelectView(void)
{
    if(activeMapWindow())
	activeMapWindow()->setModeView(2);
}
