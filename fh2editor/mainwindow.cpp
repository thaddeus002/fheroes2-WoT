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

MainWindow::MainWindow(const QString & dataFile) : sequenceMapNumber(0), aggContent(dataFile)
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

void MainWindow::copy(void)
{
    if(activeMapWindow())
	activeMapWindow()->copy();
}

void MainWindow::paste(void)
{
    if(activeMapWindow())
        activeMapWindow()->paste();
}

void MainWindow::fill(void)
{
    if(activeMapWindow())
        activeMapWindow()->fill();
}

void MainWindow::about(void)
{
   QMessageBox::about(this, tr("Map Editor"),
            tr("<b>Version 0.1.</b>"));
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

    bool hasSelection = false; /* activeMapWindow() &&
                         activeMapWindow()->textCursor().hasSelection()); */

    editCopyAct->setEnabled(hasSelection);
    editPasteAct->setEnabled(false);
    editFillAct->setEnabled(hasSelection);

    if(hasMapWindow)
    {
	switch(activeMapWindow()->modeView())
	{
	    case 1:	viewExploreModeAct->setChecked(true); break;
	    case 2:	viewSelectModeAct->setChecked(true); break;
	    default:	break;
	}

	switch(activeMapWindow()->currentGround())
	{
	    case 1:	groundDesertAct->setChecked(true); break;
	    case 2:	groundSnowAct->setChecked(true); break;
	    case 3:	groundSwampAct->setChecked(true); break;
	    case 4:	groundWastelandAct->setChecked(true); break;
	    case 5:	groundBeachAct->setChecked(true); break;
	    case 6:	groundLavaAct->setChecked(true); break;
	    case 7:	groundDirtAct->setChecked(true); break;
	    case 8:	groundGrassAct->setChecked(true); break;
	    case 9:	groundWaterAct->setChecked(true); break;
	    default:	break;
	}
    }

    switchViewAct->setEnabled(hasMapWindow);
    switchGroundAct->setEnabled(hasMapWindow);
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
    child->parentWidget()->setGeometry(0, 0, 640, 480);

    connect(child, SIGNAL(copyAvailable(bool)), editCopyAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)), editFillAct, SLOT(setEnabled(bool)));

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

    // select view mode
    viewExploreModeAct = new QAction(QIcon(":/images/mode_explore.png"), tr("Ex&plore"), this);
    //viewExploreModeAct->setShortcuts(QKeySequence::);
    viewExploreModeAct->setStatusTip(tr("Switch to explore view"));
    viewExploreModeAct->setCheckable(true);
    connect(viewExploreModeAct, SIGNAL(triggered()), this, SLOT(switchModeViewGroup()));

    viewSelectModeAct = new QAction(QIcon(":/images/mode_select.png"), tr("Se&lect"), this);
    //viewSelectModeAct->setShortcuts(QKeySequence::);
    viewSelectModeAct->setStatusTip(tr("Switch to select view"));
    viewSelectModeAct->setCheckable(true);
    connect(viewSelectModeAct, SIGNAL(triggered()), this, SLOT(switchModeViewGroup()));

    switchViewAct = new QActionGroup(this);
    switchViewAct->addAction(viewExploreModeAct);
    switchViewAct->addAction(viewSelectModeAct);

    // select ground
    groundDesertAct = new QAction(QIcon(":/images/ground_desert.png"), tr("Desert"), this);
    groundDesertAct->setStatusTip(tr("Select desert ground"));
    groundDesertAct->setCheckable(true);
    connect(groundDesertAct, SIGNAL(triggered()), this, SLOT(switchGroundGroup()));

    groundSnowAct = new QAction(QIcon(":/images/ground_snow.png"), tr("Snow"), this);
    groundSnowAct->setStatusTip(tr("Select snow ground"));
    groundSnowAct->setCheckable(true);
    connect(groundSnowAct, SIGNAL(triggered()), this, SLOT(switchGroundGroup()));

    groundSwampAct = new QAction(QIcon(":/images/ground_swamp.png"), tr("Swamp"), this);
    groundSwampAct->setStatusTip(tr("Select swamp ground"));
    groundSwampAct->setCheckable(true);
    connect(groundSwampAct, SIGNAL(triggered()), this, SLOT(switchGroundGroup()));

    groundWastelandAct = new QAction(QIcon(":/images/ground_wasteland.png"), tr("Wasteland"), this);
    groundWastelandAct->setStatusTip(tr("Select wasteland ground"));
    groundWastelandAct->setCheckable(true);
    connect(groundWastelandAct, SIGNAL(triggered()), this, SLOT(switchGroundGroup()));

    groundBeachAct = new QAction(QIcon(":/images/ground_beach.png"), tr("Beach"), this);
    groundBeachAct->setStatusTip(tr("Select beach ground"));
    groundBeachAct->setCheckable(true);
    connect(groundBeachAct, SIGNAL(triggered()), this, SLOT(switchGroundGroup()));

    groundLavaAct = new QAction(QIcon(":/images/ground_lava.png"), tr("Lava"), this);
    groundLavaAct->setStatusTip(tr("Select lava ground"));
    groundLavaAct->setCheckable(true);
    connect(groundLavaAct, SIGNAL(triggered()), this, SLOT(switchGroundGroup()));

    groundDirtAct = new QAction(QIcon(":/images/ground_dirt.png"), tr("Dirt"), this);
    groundDirtAct->setStatusTip(tr("Select dirt ground"));
    groundDirtAct->setCheckable(true);
    connect(groundDirtAct, SIGNAL(triggered()), this, SLOT(switchGroundGroup()));

    groundGrassAct = new QAction(QIcon(":/images/ground_grass.png"), tr("Grass"), this);
    groundGrassAct->setStatusTip(tr("Select grass ground"));
    groundGrassAct->setCheckable(true);
    connect(groundGrassAct, SIGNAL(triggered()), this, SLOT(switchGroundGroup()));

    groundWaterAct = new QAction(QIcon(":/images/ground_water.png"), tr("Water"), this);
    groundWaterAct->setStatusTip(tr("Select water"));
    groundWaterAct->setCheckable(true);
    connect(groundWaterAct, SIGNAL(triggered()), this, SLOT(switchGroundGroup()));

    switchGroundAct = new QActionGroup(this);
    switchGroundAct->addAction(groundDesertAct);
    switchGroundAct->addAction(groundSnowAct);
    switchGroundAct->addAction(groundSwampAct);
    switchGroundAct->addAction(groundWastelandAct);
    switchGroundAct->addAction(groundBeachAct);
    switchGroundAct->addAction(groundLavaAct);
    switchGroundAct->addAction(groundDirtAct);
    switchGroundAct->addAction(groundGrassAct);
    switchGroundAct->addAction(groundWaterAct);

//! [0]
    fileExitAct = new QAction(tr("E&xit"), this);
    fileExitAct->setShortcuts(QKeySequence::Quit);
    fileExitAct->setStatusTip(tr("Exit the application"));
    connect(fileExitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
//! [0]

    editCopyAct = new QAction(QIcon(":/images/menu_copy.png"), tr("&Copy"), this);
    editCopyAct->setShortcuts(QKeySequence::Copy);
    editCopyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(editCopyAct, SIGNAL(triggered()), this, SLOT(copy()));

    editPasteAct = new QAction(QIcon(":/images/menu_paste.png"), tr("&Paste"), this);
    editPasteAct->setShortcuts(QKeySequence::Paste);
    editPasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(editPasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    editFillAct = new QAction(QIcon(":/images/menu_fill.png"), tr("&Fill"), this);
    editFillAct->setStatusTip(tr("Fill the ground into the current selection"));
    connect(editFillAct, SIGNAL(triggered()), this, SLOT(fill()));

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

    menuAboutAct = new QAction(tr("&About"), this);
    menuAboutAct->setStatusTip(tr("Show the application's About box"));
    connect(menuAboutAct, SIGNAL(triggered()), this, SLOT(about()));
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

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(editCopyAct);
    editMenu->addAction(editPasteAct);
    editMenu->addAction(editFillAct);

    modeMenu = menuBar()->addMenu(tr("&Select"));
    modeMenu->addSeparator()->setText(tr("View Mode"));
    modeMenu->addAction(viewExploreModeAct);
    modeMenu->addAction(viewSelectModeAct);
    modeMenu->addSeparator()->setText(tr("Fill Ground"));
    modeMenu->addAction(groundDesertAct);
    modeMenu->addAction(groundSnowAct);
    modeMenu->addAction(groundSwampAct);
    modeMenu->addAction(groundWastelandAct);
    modeMenu->addAction(groundBeachAct);
    modeMenu->addAction(groundLavaAct);
    modeMenu->addAction(groundDirtAct);
    modeMenu->addAction(groundGrassAct);
    modeMenu->addAction(groundWaterAct);

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

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(editCopyAct);
    editToolBar->addAction(editPasteAct);
    editToolBar->addAction(editFillAct);

    selectToolBar = addToolBar(tr("Mode"));
    selectToolBar->addAction(viewExploreModeAct);
    selectToolBar->addAction(viewSelectModeAct);
    selectToolBar->addSeparator();
    selectToolBar->addAction(groundDesertAct);
    selectToolBar->addAction(groundSnowAct);
    selectToolBar->addAction(groundSwampAct);
    selectToolBar->addAction(groundWastelandAct);
    selectToolBar->addAction(groundBeachAct);
    selectToolBar->addAction(groundLavaAct);
    selectToolBar->addAction(groundDirtAct);
    selectToolBar->addAction(groundGrassAct);
    selectToolBar->addAction(groundWaterAct);
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

void MainWindow::switchModeViewGroup(void)
{
    if(activeMapWindow())
    {
	if(viewExploreModeAct->isChecked())
	    activeMapWindow()->setModeView(1);
	else
	if(viewSelectModeAct->isChecked())
	    activeMapWindow()->setModeView(2);
    }
}

void MainWindow::switchGroundGroup(void)
{
    if(activeMapWindow())
    {
	if(groundDesertAct->isChecked())
	    activeMapWindow()->setCurrentGround(1);
	else
	if(groundSnowAct->isChecked())
	    activeMapWindow()->setCurrentGround(2);
	else
	if(groundSwampAct->isChecked())
	    activeMapWindow()->setCurrentGround(3);
	else
	if(groundWastelandAct->isChecked())
	    activeMapWindow()->setCurrentGround(4);
	else
	if(groundBeachAct->isChecked())
	    activeMapWindow()->setCurrentGround(5);
	else
	if(groundLavaAct->isChecked())
	    activeMapWindow()->setCurrentGround(6);
	else
	if(groundDirtAct->isChecked())
	    activeMapWindow()->setCurrentGround(7);
	else
	if(groundGrassAct->isChecked())
	    activeMapWindow()->setCurrentGround(8);
	else
	if(groundWaterAct->isChecked())
	    activeMapWindow()->setCurrentGround(9);
    }
}
