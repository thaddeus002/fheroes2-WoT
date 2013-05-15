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
#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QSpacerItem>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLineEdit>
#include <QGroupBox>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QTreeWidget>
#include <QListWidget>

#include "global.h"
#include "program.h"
#include "engine.h"
#include "mapdata.h"
#include "dialogs.h"

QVariant comboBoxCurrentData(const QComboBox* box)
{
    return box->itemData(box->currentIndex());
}

Form::SelectMapSize::SelectMapSize()
{
    setWindowTitle(QApplication::translate("SelectMapSize", "Select Size", 0, QApplication::UnicodeUTF8));

    vboxLayout = new QVBoxLayout(this);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);

    comboBoxSize = new QComboBox(this);
    comboBoxSize->clear();
    comboBoxSize->addItem(QApplication::translate("SelectMapSize", "Small (36x36)", 0, QApplication::UnicodeUTF8), 36);
    comboBoxSize->addItem(QApplication::translate("SelectMapSize", "Medium (72x72)", 0, QApplication::UnicodeUTF8), 72);
    comboBoxSize->addItem(QApplication::translate("SelectMapSize", "Large (108x108)", 0, QApplication::UnicodeUTF8), 108);
    comboBoxSize->addItem(QApplication::translate("SelectMapSize", "Extra Large (144x144)", 0, QApplication::UnicodeUTF8), 144);
    comboBoxSize->setCurrentIndex(1);
    vboxLayout->addWidget(comboBoxSize);

    spacerItem = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vboxLayout->addItem(spacerItem);

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    spinBoxWidth = new QSpinBox(this);
    spinBoxWidth->setMaximum(1024);
    spinBoxWidth->setMinimum(36);
    spinBoxWidth->setSingleStep(2);
    spinBoxWidth->setVisible(false);

    labelWidth = new QLabel(this);
    labelWidth->setEnabled(true);
    labelWidth->setVisible(false);
    labelWidth->setText(QApplication::translate("SelectMapSize", "width", 0, QApplication::UnicodeUTF8));
    labelWidth->setBuddy(spinBoxWidth);

    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->addItem(spacerItem1);
    hboxLayout->addWidget(labelWidth);
    hboxLayout->addWidget(spinBoxWidth);
    hboxLayout->addItem(spacerItem2);
    vboxLayout->addLayout(hboxLayout);

    spacerItem3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    spinBoxHeight = new QSpinBox(this);
    spinBoxHeight->setMaximum(1024);
    spinBoxHeight->setMinimum(36);
    spinBoxHeight->setSingleStep(2);
    spinBoxHeight->setVisible(false);

    labelHeight = new QLabel(this);
    labelHeight->setText(QApplication::translate("SelectMapSize", "height", 0, QApplication::UnicodeUTF8));
    labelHeight->setVisible(false);
    labelHeight->setBuddy(spinBoxHeight);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->addItem(spacerItem3);
    hboxLayout1->addWidget(labelHeight);
    hboxLayout1->addWidget(spinBoxHeight);

    spacerItem4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout1->addItem(spacerItem4);
    vboxLayout->addLayout(hboxLayout1);

    spacerItem5 = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vboxLayout->addItem(spacerItem5);

    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(0);

    spacerItem6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout2->addItem(spacerItem6);

    pushButtonOk = new QPushButton(this);
    pushButtonOk->setText(QApplication::translate("SelectMapSize", "Ok", 0, QApplication::UnicodeUTF8));
    hboxLayout2->addWidget(pushButtonOk);

    spacerItem7 = new QSpacerItem(61, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout2->addItem(spacerItem7);

    pushButtonExpert = new QPushButton(this);
    pushButtonExpert->setText(QApplication::translate("SelectMapSize", "Expert", 0, QApplication::UnicodeUTF8));
    hboxLayout2->addWidget(pushButtonExpert);

    spacerItem8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout2->addItem(spacerItem8);
    vboxLayout->addLayout(hboxLayout2);

    QSize minSize = minimumSizeHint();

    resize(minSize);
    setFixedSize(minSize);

    connect(pushButtonExpert, SIGNAL(clicked()), this, SLOT(clickExpert()));
    connect(pushButtonOk, SIGNAL(clicked()), this, SLOT(clickOk()));
}

void Form::SelectMapSize::clickExpert(void)
{
    // switch to expert
    if(comboBoxSize->isVisible())
    {
	comboBoxSize->hide();
	labelWidth->show();
	spinBoxWidth->show();
	labelHeight->show();
	spinBoxHeight->show();

	pushButtonExpert->setText("Simple");

	resize(minimumSizeHint());
    }
    else
    {
	labelWidth->hide();
	spinBoxWidth->hide();
	labelHeight->hide();
	spinBoxHeight->hide();
	comboBoxSize->show();

	pushButtonExpert->setText("Expert");

	resize(minimumSizeHint());
    }
}

void Form::SelectMapSize::clickOk(void)
{
    if(comboBoxSize->isVisible())
    {
	int size = qvariant_cast<int>(comboBoxCurrentData(comboBoxSize));
	result = QSize(size, size);
    }
    else
    {
	result = QSize(spinBoxWidth->value(), spinBoxHeight->value());
    }

    accept();
}

Form::SelectDataFile::SelectDataFile(const QString & dataFile, const QStringList & dirList)
{
    setWindowTitle(QApplication::translate("DialogSelectDataFile", "Warning", 0, QApplication::UnicodeUTF8));

    verticalLayout = new QVBoxLayout(this);

    labelHeader = new QLabel(this);
    labelHeader->setAlignment(Qt::AlignCenter);
    labelHeader->setText(QApplication::translate("DialogSelectDataFile", "Cannot find resource file: ", 0, QApplication::UnicodeUTF8) + dataFile);
    verticalLayout->addWidget(labelHeader);

    horizontalLayout2 = new QHBoxLayout();

    labelImage = new QLabel(this);
    labelImage->setPixmap(QPixmap(QString::fromUtf8(":/images/cancel.png")));
    labelImage->setScaledContents(false);
    horizontalLayout2->addWidget(labelImage);

    labelBody = new QLabel(this);
    labelBody->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    labelBody->setText(QApplication::translate("DialogSelectDataFile", "Scan directories: ", 0, QApplication::UnicodeUTF8) + "\n" + dirList.join("\n"));
    horizontalLayout2->addWidget(labelBody);
    verticalLayout->addLayout(horizontalLayout2);

    verticalSpacer = new QSpacerItem(288, 6, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    horizontalLayout1 = new QHBoxLayout();

    pushButtonSelect = new QPushButton(this);
    pushButtonSelect->setText(QApplication::translate("DialogSelectDataFile", "Select", 0, QApplication::UnicodeUTF8));
    horizontalLayout1->addWidget(pushButtonSelect);

    pushButtonSave = new QPushButton(this);
    pushButtonSave->setText(QApplication::translate("DialogSelectDataFile", "Save", 0, QApplication::UnicodeUTF8));
    pushButtonSave->setEnabled(false);
    horizontalLayout1->addWidget(pushButtonSave);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout1->addItem(horizontalSpacer);

    pushButtonExit = new QPushButton(this);
    pushButtonExit->setText(QApplication::translate("DialogSelectDataFile", "Exit", 0, QApplication::UnicodeUTF8));
    horizontalLayout1->addWidget(pushButtonExit);
    verticalLayout->addLayout(horizontalLayout1);

    QSize minSize = minimumSizeHint();

    resize(minSize);
    setFixedSize(minSize);

    connect(pushButtonExit, SIGNAL(clicked()), this, SLOT(reject()));
    connect(pushButtonSave, SIGNAL(clicked()), this, SLOT(accept()));
    connect(pushButtonSelect, SIGNAL(clicked()), this, SLOT(clickSelect()));
}

void Form::SelectDataFile::clickSelect(void)
{
    result = QFileDialog::getOpenFileName(this, tr("Open data file"), "", "heroes2.agg");
    pushButtonSave->setEnabled(true);
}

class SelectImageItem : public QListWidgetItem
{
public:
    SelectImageItem(const CompositeObject & obj, const QMap<int, QString> & ids)
    {
	setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	setData(Qt::UserRole, QVariant::fromValue(obj));
	setIcon(EditorTheme::getImage(obj));
	setText(obj.name);
#ifndef QT_NO_TOOLTIP
	setToolTip(QString("class id: ") + (ids[obj.classId].isEmpty() ? QString("unknown") : ids[obj.classId]));
#endif
	setSizeHint(QSize(132, 80));
    }
};

Form::SelectImageTab::SelectImageTab(const QDomElement & groupElem, const QString & dataFolder)
{
    verticalLayout = new QVBoxLayout(this);
    listWidget = new QListWidget(this);
    verticalLayout->addWidget(listWidget);
    listWidget->setIconSize(QSize(64, 64));
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setWrapping(true);
    listWidget->setResizeMode(QListView::Adjust);

    QMap<int, QString> objectsID;

    for(int index = 0; index < 0x80; ++index)
	objectsID[index] = MapObj::transcribe(index);

    Editor::MyObjectsXML objectsElem(EditorTheme::resourceFile(dataFolder, groupElem.attribute("file")));

    for(Editor::MyObjectsXML::const_iterator
	it = objectsElem.begin(); it != objectsElem.end(); ++it)
    {
        CompositeObject obj(*it);

        if(obj.isValid())
	    listWidget->addItem(new SelectImageItem(obj, objectsID));
    }
}

bool Form::SelectImageTab::isSelected(void) const
{
    return listWidget->selectedItems().size();
}

Form::SelectImage::SelectImage()
{
    setWindowTitle(QApplication::translate("SelectImage", "Select Object", 0, QApplication::UnicodeUTF8));

    tabWidget = new QTabWidget(this);

    const QString dataFolder("objects");
    Editor::MyXML groupsElem(EditorTheme::resourceFile(dataFolder, "groups.xml"), "groups");

    if(! groupsElem.isNull())
    {
        QDomNodeList groupsList = groupsElem.elementsByTagName("group");

        for(int pos1 = 0; pos1 < groupsList.size(); ++pos1)
        {
            QDomElement groupElem = groupsList.item(pos1).toElement();
            QString name = groupElem.attribute("name");

            if(! name.isEmpty())
		tabWidget->addTab(new SelectImageTab(groupElem, dataFolder), name);
	}

	tabWidget->setCurrentIndex(0);
    }

    verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(tabWidget);

    pushButtonSelect = new QPushButton(this);
    pushButtonSelect->setText(QApplication::translate("SelectImage", "Select", 0, QApplication::UnicodeUTF8));
    pushButtonSelect->setEnabled(false);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(pushButtonSelect);

    horizontalSpacer = new QSpacerItem(268, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    pushButtonClose = new QPushButton(this);
    pushButtonClose->setText(QApplication::translate("SelectImage", "Close", 0, QApplication::UnicodeUTF8));

    horizontalLayout->addWidget(pushButtonClose);
    verticalLayout->addLayout(horizontalLayout);

    // set size
    QSettings & settings = Resource::localSettings();
    setMinimumSize(QSize(540, 410));
    resize(settings.value("SelectImageDialog/size", minimumSize()).toSize());

    tabSwitched(settings.value("SelectImageDialog/lastTab", 0).toInt());

    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSwitched(int)));
    connect(pushButtonClose, SIGNAL(clicked()), this, SLOT(reject()));
    connect(pushButtonSelect, SIGNAL(clicked()), this, SLOT(clickSelect()));

    connect(this, SIGNAL(finished(int)), this, SLOT(saveSettings()));
}

void Form::SelectImage::saveSettings(void)
{
    QSettings & settings = Resource::localSettings();
    settings.setValue("SelectImageDialog/size", size());
}

void Form::SelectImage::tabSwitched(int num)
{
    SelectImageTab* tab = qobject_cast<SelectImageTab*>(tabWidget->widget(num));

    disconnect(this, SLOT(accept(QListWidgetItem*)));
    disconnect(this, SLOT(selectionChanged()));

    if(tab)
    {
	pushButtonSelect->setEnabled(tab->isSelected());

	connect(tab->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept(QListWidgetItem*)));
	connect(tab->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

	Resource::localSettings().setValue("SelectImageDialog/lastTab", num);
    }
}

void Form::SelectImage::selectionChanged(void)
{
    SelectImageTab* tab = qobject_cast<SelectImageTab*>(tabWidget->currentWidget());

    if(tab)
	pushButtonSelect->setEnabled(tab->isSelected());
}


void Form::SelectImage::clickSelect(void)
{
    SelectImageTab* tab = qobject_cast<SelectImageTab*>(tabWidget->currentWidget());

    if(tab)
	accept(tab->listWidget->currentItem());
}

void Form::SelectImage::accept(QListWidgetItem* item)
{
    if(item)
    {
	result = qvariant_cast<CompositeObject>(item->data(Qt::UserRole));
	QDialog::accept();
    }
}

void fillComboBox(QComboBox & box, const ListStringPos & list)
{
    box.clear();
    for(ListStringPos::const_iterator
	it = list.begin(); it != list.end(); ++it)
    {
	QString str; QTextStream ts(&str);
	ts << (*it).first << " - " << "(" << (*it).second.x() << ", " << (*it).second.y() << ")";
	box.addItem(str, (*it).second);
    }
}

Form::PlayerStatus::PlayerStatus(int c, int v, QWidget* parent) : QLabel(parent), col(c), stat(v)
{
    updatePlayers();
}

void Form::PlayerStatus::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    stat += 1;
    if(0 == (stat % 4)) stat += 1;
    updatePlayers();
    emit mousePressed();
}

void Form::PlayerStatus::updatePlayers(void)
{
    switch(col)
    {
	    case Color::Blue:	setPixmap(EditorTheme::getImageICN("CELLWIN.ICN", 19 + status()).first); break;
    	    case Color::Green:	setPixmap(EditorTheme::getImageICN("CELLWIN.ICN", 23 + status()).first); break;
    	    case Color::Red:	setPixmap(EditorTheme::getImageICN("CELLWIN.ICN", 27 + status()).first); break;
    	    case Color::Yellow:	setPixmap(EditorTheme::getImageICN("CELLWIN.ICN", 31 + status()).first); break;
    	    case Color::Orange:	setPixmap(EditorTheme::getImageICN("CELLWIN.ICN", 35 + status()).first); break;
    	    case Color::Purple:	setPixmap(EditorTheme::getImageICN("CELLWIN.ICN", 39 + status()).first); break;
    	    default: break;
    }
}

Form::MapOptions::MapOptions(MapData & map)
{
    QSettings & settings = Resource::localSettings();
    setWindowTitle(QApplication::translate("MapOptions", "Map Options", 0, QApplication::UnicodeUTF8));

    /* tab info block */
    tabInfo = new QWidget();
    verticalLayout = new QVBoxLayout(tabInfo);

    labelName = new QLabel(tabInfo);
    labelName->setAlignment(Qt::AlignCenter);
    labelName->setText(QApplication::translate("MapOptions", "Map Name:", 0, QApplication::UnicodeUTF8));

    lineEditName = new QLineEdit(tabInfo);
    lineEditName->setText(map.name());

    labelDifficulty = new QLabel(tabInfo);
    labelDifficulty->setAlignment(Qt::AlignCenter);
    labelDifficulty->setText(QApplication::translate("MapOptions", "Map Difficulty:", 0, QApplication::UnicodeUTF8));

    comboBoxDifficulty = new QComboBox(tabInfo);
    comboBoxDifficulty->addItem(QApplication::translate("MapOptions", "Easy", 0, QApplication::UnicodeUTF8), Difficulty::Easy);
    comboBoxDifficulty->addItem(QApplication::translate("MapOptions", "Normal", 0, QApplication::UnicodeUTF8), Difficulty::Normal);
    comboBoxDifficulty->addItem(QApplication::translate("MapOptions", "Tough", 0, QApplication::UnicodeUTF8), Difficulty::Tough);
    comboBoxDifficulty->addItem(QApplication::translate("MapOptions", "Expert", 0, QApplication::UnicodeUTF8), Difficulty::Expert);
    comboBoxDifficulty->setCurrentIndex(map.difficulty());

    labelDescription = new QLabel(tabInfo);
    labelDescription->setAlignment(Qt::AlignCenter);
    labelDescription->setText(QApplication::translate("MapOptions", "Map Description:", 0, QApplication::UnicodeUTF8));

    plainTextEditDescription = new QPlainTextEdit(tabInfo);
    plainTextEditDescription->setPlainText(map.description());

    verticalLayout->addWidget(labelName);
    verticalLayout->addWidget(lineEditName);
    verticalLayout->addWidget(labelDifficulty);
    verticalLayout->addWidget(comboBoxDifficulty);
    verticalLayout->addWidget(labelDescription);
    verticalLayout->addWidget(plainTextEditDescription);

    /* tab condition block */
    tabConditions = new QWidget();

    groupBoxWinsCond = new QGroupBox(tabConditions);
    groupBoxWinsCond->setTitle(QApplication::translate("MapOptions", "Victory Condition", 0, QApplication::UnicodeUTF8));

    comboBoxWinsCond = new QComboBox(groupBoxWinsCond);
    comboBoxWinsCond->addItem(QApplication::translate("MapOptions", "Default", 0, QApplication::UnicodeUTF8), Conditions::Wins);
    comboBoxWinsCond->addItem(QApplication::translate("MapOptions", "Capture a particular castle", 0, QApplication::UnicodeUTF8), Conditions::CaptureTown);
    comboBoxWinsCond->addItem(QApplication::translate("MapOptions", "Defeat a particular hero", 0, QApplication::UnicodeUTF8), Conditions::DefeatHero);
    comboBoxWinsCond->addItem(QApplication::translate("MapOptions", "Find a particular artifact", 0, QApplication::UnicodeUTF8), Conditions::FindArtifact);
    comboBoxWinsCond->addItem(QApplication::translate("MapOptions", "One side defeats another", 0, QApplication::UnicodeUTF8), Conditions::SideWins);
    comboBoxWinsCond->addItem(QApplication::translate("MapOptions", "Accumulate gold", 0, QApplication::UnicodeUTF8), Conditions::AccumulateGold);

    comboBoxWinsCondExt = new QComboBox(groupBoxWinsCond);
    comboBoxWinsCondExt->setEnabled(false);

    horizontalLayoutVictorySlct = new QHBoxLayout();
    horizontalLayoutVictorySlct->addWidget(comboBoxWinsCond);
    horizontalLayoutVictorySlct->addWidget(comboBoxWinsCondExt);

    checkBoxAllowNormalVictory = new QCheckBox(groupBoxWinsCond);
    checkBoxAllowNormalVictory->setEnabled(false);
    checkBoxAllowNormalVictory->setText(QApplication::translate("MapOptions", "Allow normal victory condition", 0, QApplication::UnicodeUTF8));

    checkBoxCompAlsoWins = new QCheckBox(groupBoxWinsCond);
    checkBoxCompAlsoWins->setEnabled(false);
    checkBoxCompAlsoWins->setText(QApplication::translate("MapOptions", "Comp also wins via Special VC", 0, QApplication::UnicodeUTF8));

    horizontalLayoutVictoryCheck = new QHBoxLayout();
    horizontalLayoutVictoryCheck->addWidget(checkBoxAllowNormalVictory);
    horizontalLayoutVictoryCheck->addWidget(checkBoxCompAlsoWins);

    verticalLayout3 = new QVBoxLayout(groupBoxWinsCond);
    verticalLayout3->addLayout(horizontalLayoutVictorySlct);
    verticalLayout3->addLayout(horizontalLayoutVictoryCheck);

    groupBoxLossCond = new QGroupBox(tabConditions);
    groupBoxLossCond->setTitle(QApplication::translate("MapOptions", "Loss Condition", 0, QApplication::UnicodeUTF8));

    comboBoxLossCond = new QComboBox(groupBoxLossCond);
    comboBoxLossCond->addItem(QApplication::translate("MapOptions", "Default", 0, QApplication::UnicodeUTF8), Conditions::Loss);
    comboBoxLossCond->addItem(QApplication::translate("MapOptions", "Lose a particuclar castle", 0, QApplication::UnicodeUTF8), Conditions::LoseTown);
    comboBoxLossCond->addItem(QApplication::translate("MapOptions", "Lose a particular hero", 0, QApplication::UnicodeUTF8), Conditions::LoseHero);
    comboBoxLossCond->addItem(QApplication::translate("MapOptions", "Run out of time", 0, QApplication::UnicodeUTF8), Conditions::OutTime);

    horizontalLayoutLossCond = new QHBoxLayout();
    horizontalLayoutLossCond->addWidget(comboBoxLossCond);

    comboBoxLossCondExt = new QComboBox(groupBoxLossCond);
    comboBoxLossCondExt->setEnabled(false);

    horizontalLayoutLossCond = new QHBoxLayout();
    horizontalLayoutLossCond->addWidget(comboBoxLossCond);
    horizontalLayoutLossCond->addWidget(comboBoxLossCondExt);

    verticalLayout4 = new QVBoxLayout(groupBoxLossCond);
    verticalLayout4->addLayout(horizontalLayoutLossCond);
    groupBoxPlayers = new QGroupBox(tabConditions);
    groupBoxPlayers->setTitle(QApplication::translate("MapOptions", "Players", 0, QApplication::UnicodeUTF8));

    horizontalSpacerPlayersLeft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalSpacerPlayersRight = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    verticalSpacerPage2 = new QSpacerItem(20, 17, QSizePolicy::Minimum, QSizePolicy::Expanding);

    horizontalLayoutPlayers = new QHBoxLayout();
    horizontalLayoutPlayers->addItem(horizontalSpacerPlayersLeft);

    // create players labels
    QVector<int> colors = Color::colors(Color::All);

    for(QVector<int>::const_iterator
	it = colors.begin(); it != colors.end(); ++it)
    {
	int stat = 0; /* 0: n/a, 1: human only, 2: comp only, 3: comp or human */
	if((*it) & map.kingdomColors())
	{
	    if(((*it) & map.computerColors()) &&
		((*it) & map.humanColors()))
		stat = 3;
	    else
	    if((*it) & map.humanColors())
		stat = 1;
	    else
	    if((*it) & map.computerColors())
		stat = 2;
	}
        labelPlayers.push_back(new PlayerStatus(*it, stat, groupBoxPlayers));
	labelPlayers.back()->setEnabled((*it) & map.kingdomColors());
        horizontalLayoutPlayers->addWidget(labelPlayers.back());
    }

    horizontalLayoutPlayers->addItem(horizontalSpacerPlayersRight);

    checkBoxStartWithHero = new QCheckBox(groupBoxPlayers);
    checkBoxStartWithHero->setText(QApplication::translate("MapOptions", "Start with hero in each player's main castle", 0, QApplication::UnicodeUTF8));
    checkBoxStartWithHero->setChecked(map.startWithHero());

    verticalLayout5 = new QVBoxLayout(groupBoxPlayers);
    verticalLayout5->addLayout(horizontalLayoutPlayers);
    verticalLayout5->addWidget(checkBoxStartWithHero);

    verticalLayout6 = new QVBoxLayout(tabConditions);
    verticalLayout6->addWidget(groupBoxWinsCond);
    verticalLayout6->addWidget(groupBoxLossCond);
    verticalLayout6->addWidget(groupBoxPlayers);
    verticalLayout6->addItem(verticalSpacerPage2);

    /* tab rumors/events block */
    tabRumorsEvents = new QWidget();

    groupBoxRumors = new QGroupBox(tabRumorsEvents);
    groupBoxRumors->setTitle(QApplication::translate("MapOptions", "Rumors", 0, QApplication::UnicodeUTF8));

    groupBoxEvents = new QGroupBox(tabRumorsEvents);
    groupBoxEvents->setTitle(QApplication::translate("MapOptions", "Events", 0, QApplication::UnicodeUTF8));

    listWidgetRumors = new RumorsList(map.kingdomColors(), this);
    listWidgetEvents = new EventsList(map.kingdomColors(), this);

    verticalLayout7 = new QVBoxLayout(groupBoxRumors);
    verticalLayout7->addWidget(listWidgetRumors);

    verticalLayout8 = new QVBoxLayout(groupBoxEvents);
    verticalLayout8->addWidget(listWidgetEvents);

    horizontalLayout6 = new QHBoxLayout(tabRumorsEvents);
    horizontalLayout6->addWidget(groupBoxRumors);
    horizontalLayout6->addWidget(groupBoxEvents);

    /* tab: default values */
    tabDefaults = new QWidget();
    verticalLayoutDefaults = new QVBoxLayout(tabDefaults);

    spinBoxResourceGoldMin = new QSpinBox(tabDefaults);
    spinBoxResourceGoldMin->setMaximum(1000);
    spinBoxResourceGoldMin->setMinimum(100);
    spinBoxResourceGoldMin->setSingleStep(100);
    spinBoxResourceGoldMin->setValue(settings.value("MapOptions/defaultGoldMin", DEFAULT_RESOURCE_GOLD_MIN).toInt());

    spinBoxResourceGoldMax = new QSpinBox(tabDefaults);
    spinBoxResourceGoldMax->setMaximum(5000);
    spinBoxResourceGoldMax->setMinimum(300);
    spinBoxResourceGoldMax->setSingleStep(100);
    spinBoxResourceGoldMax->setValue(settings.value("MapOptions/defaultGoldMax", DEFAULT_RESOURCE_GOLD_MAX).toInt());

    labelResourceGold = new QLabel(this);
    labelResourceGold->setText(QApplication::translate("MapOptions", "Resource (gold)", 0, QApplication::UnicodeUTF8));

    horizontalLayoutResourceGold = new QHBoxLayout();
    horizontalLayoutResourceGold->addWidget(labelResourceGold);
    horizontalLayoutResourceGold->addWidget(spinBoxResourceGoldMin);
    horizontalLayoutResourceGold->addWidget(spinBoxResourceGoldMax);

    spinBoxResourceWoodOreMin = new QSpinBox(tabDefaults);
    spinBoxResourceWoodOreMin->setMaximum(10);
    spinBoxResourceWoodOreMin->setMinimum(1);
    spinBoxResourceWoodOreMin->setSingleStep(1);
    spinBoxResourceWoodOreMin->setValue(settings.value("MapOptions/defaultWoodOreMin", DEFAULT_RESOURCE_WOOD_ORE_MIN).toInt());

    spinBoxResourceWoodOreMax = new QSpinBox(tabDefaults);
    spinBoxResourceWoodOreMax->setMaximum(50);
    spinBoxResourceWoodOreMax->setMinimum(3);
    spinBoxResourceWoodOreMax->setSingleStep(1);
    spinBoxResourceWoodOreMax->setValue(settings.value("MapOptions/defaultWoodOreMax", DEFAULT_RESOURCE_WOOD_ORE_MAX).toInt());

    labelResourceWoodOre = new QLabel(this);
    labelResourceWoodOre->setText(QApplication::translate("MapOptions", "Resource (wood, ore)", 0, QApplication::UnicodeUTF8));

    horizontalLayoutResourceWoodOre = new QHBoxLayout();
    horizontalLayoutResourceWoodOre->addWidget(labelResourceWoodOre);
    horizontalLayoutResourceWoodOre->addWidget(spinBoxResourceWoodOreMin);
    horizontalLayoutResourceWoodOre->addWidget(spinBoxResourceWoodOreMax);

    spinBoxResourceOtherMin = new QSpinBox(tabDefaults);
    spinBoxResourceOtherMin->setMaximum(10);
    spinBoxResourceOtherMin->setMinimum(1);
    spinBoxResourceOtherMin->setSingleStep(1);
    spinBoxResourceOtherMin->setValue(settings.value("MapOptions/defaultOtherMin", DEFAULT_RESOURCE_OTHER_MIN).toInt());

    spinBoxResourceOtherMax = new QSpinBox(tabDefaults);
    spinBoxResourceOtherMax->setMaximum(30);
    spinBoxResourceOtherMax->setMinimum(3);
    spinBoxResourceOtherMax->setSingleStep(1);
    spinBoxResourceOtherMax->setValue(settings.value("MapOptions/defaultOtherMax", DEFAULT_RESOURCE_OTHER_MAX).toInt());

    labelResourceOther = new QLabel(this);
    labelResourceOther->setText(QApplication::translate("MapOptions", "Resource (other)", 0, QApplication::UnicodeUTF8));

    horizontalLayoutResourceOther = new QHBoxLayout();
    horizontalLayoutResourceOther->addWidget(labelResourceOther);
    horizontalLayoutResourceOther->addWidget(spinBoxResourceOtherMin);
    horizontalLayoutResourceOther->addWidget(spinBoxResourceOtherMax);

    verticalSpacerDefaults = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayoutDefaults->addLayout(horizontalLayoutResourceGold);
    verticalLayoutDefaults->addLayout(horizontalLayoutResourceWoodOre);
    verticalLayoutDefaults->addLayout(horizontalLayoutResourceOther);
    verticalLayoutDefaults->addItem(verticalSpacerDefaults);

    /* tab: authors/license */
    tabAuthorsLicense = new QWidget();
    verticalLayout9 = new QVBoxLayout(tabAuthorsLicense);

    labelAuthors = new QLabel(tabAuthorsLicense);
    labelAuthors->setAlignment(Qt::AlignCenter);
    labelAuthors->setText(QApplication::translate("MapOptions", "Authors:", 0, QApplication::UnicodeUTF8));

    plainTextEditAuthors = new QPlainTextEdit(tabAuthorsLicense);
    plainTextEditAuthors->setPlainText(map.authors());

    labelLicense = new QLabel(tabAuthorsLicense);
    labelLicense->setAlignment(Qt::AlignCenter);
    labelLicense->setText(QApplication::translate("MapOptions", "License:", 0, QApplication::UnicodeUTF8));

    plainTextEditLicense = new QPlainTextEdit(tabAuthorsLicense);
    plainTextEditLicense->setPlainText(map.license());

    verticalLayout9->addWidget(labelAuthors);
    verticalLayout9->addWidget(plainTextEditAuthors);
    verticalLayout9->addWidget(labelLicense);
    verticalLayout9->addWidget(plainTextEditLicense);

    /* end */
    tabWidget = new QTabWidget(this);
    tabWidget->addTab(tabInfo, QApplication::translate("MapOptions", "General Info", 0, QApplication::UnicodeUTF8));
    tabWidget->addTab(tabConditions, QApplication::translate("MapOptions", "Wins/Loss Condition", 0, QApplication::UnicodeUTF8));
    tabWidget->addTab(tabRumorsEvents, QApplication::translate("MapOptions", "Rumors and Events", 0, QApplication::UnicodeUTF8));
    tabWidget->addTab(tabDefaults, QApplication::translate("MapOptions", "Default Values", 0, QApplication::UnicodeUTF8));
    tabWidget->addTab(tabAuthorsLicense, QApplication::translate("MapOptions", "Authors and License", 0, QApplication::UnicodeUTF8));

    pushButtonSave = new QPushButton(this);
    pushButtonSave->setText(QApplication::translate("MapOptions", "Save", 0, QApplication::UnicodeUTF8));

    pushButtonCancel = new QPushButton(this);
    pushButtonCancel->setText(QApplication::translate("MapOptions", "Cancel", 0, QApplication::UnicodeUTF8));
    pushButtonSave->setEnabled(false);

    horizontalLayoutButton = new QHBoxLayout();
    horizontalLayoutButton->addWidget(pushButtonSave);
    horizontalSpacerButton = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayoutButton->addItem(horizontalSpacerButton);
    horizontalLayoutButton->addWidget(pushButtonCancel);

    verticalLayout2 = new QVBoxLayout(this);
    verticalLayout2->addWidget(tabWidget);
    verticalLayout2->addLayout(horizontalLayoutButton);

    // contents test
    winsCondHeroList << map.conditionHeroList(Conditions::Wins);
    winsCondTownList << map.conditionTownList(Conditions::Wins);
    winsCondArtifactList << map.conditionArtifactList();
    winsCondSideList << map.conditionSideList();
    lossCondHeroList << map.conditionHeroList(Conditions::Loss);
    lossCondTownList << map.conditionTownList(Conditions::Loss);

    listWidgetRumors->addItems(map.tavernRumorsList());

    for(DayEvents::const_iterator
        it = map.dayEvents().begin(); it != map.dayEvents().end(); ++it)
    {
	QListWidgetItem* item = new QListWidgetItem((*it).header());
	item->setData(Qt::UserRole, QVariant::fromValue(*it));
	static_cast<QListWidget*>(listWidgetEvents)->addItem(item);
    }
    listWidgetEvents->sortItems();

    setConditionsBoxesMapValues(map);

    // set size
    setMinimumSize(minimumSizeHint());
    resize(settings.value("MapOptions/size", minimumSize()).toSize());

    connect(lineEditName, SIGNAL(textChanged(const QString &)), this, SLOT(setEnableSaveButton(const QString &)));
    connect(plainTextEditDescription, SIGNAL(textChanged()), this, SLOT(setEnableSaveButton()));
    connect(comboBoxDifficulty, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setEnableSaveButton(const QString &)));

    connect(comboBoxWinsCond, SIGNAL(currentIndexChanged(int)), this, SLOT(winsConditionsSelected(int)));
    connect(comboBoxLossCond, SIGNAL(currentIndexChanged(int)), this, SLOT(lossConditionsSelected(int)));

    connect(comboBoxWinsCondExt, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setEnableSaveButton(const QString &)));
    connect(comboBoxLossCondExt, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setEnableSaveButton(const QString &)));

    connect(checkBoxAllowNormalVictory, SIGNAL(clicked()), this, SLOT(setEnableSaveButton()));
    connect(checkBoxCompAlsoWins, SIGNAL(clicked()), this, SLOT(setEnableSaveButton()));
    connect(checkBoxStartWithHero, SIGNAL(clicked()), this, SLOT(setEnableSaveButton()));

    connect(listWidgetRumors, SIGNAL(mousePressed()), listWidgetEvents, SLOT(clearSelection()));
    connect(listWidgetEvents, SIGNAL(mousePressed()), listWidgetRumors, SLOT(clearSelection()));

    connect(spinBoxResourceGoldMin, SIGNAL(valueChanged(int)), this, SLOT(setEnableSaveButton()));
    connect(spinBoxResourceGoldMax, SIGNAL(valueChanged(int)), this, SLOT(setEnableSaveButton()));
    connect(spinBoxResourceWoodOreMin, SIGNAL(valueChanged(int)), this, SLOT(setEnableSaveButton()));
    connect(spinBoxResourceWoodOreMax, SIGNAL(valueChanged(int)), this, SLOT(setEnableSaveButton()));
    connect(spinBoxResourceOtherMin, SIGNAL(valueChanged(int)), this, SLOT(setEnableSaveButton()));
    connect(spinBoxResourceOtherMax, SIGNAL(valueChanged(int)), this, SLOT(setEnableSaveButton()));

    connect(plainTextEditAuthors, SIGNAL(textChanged()), this, SLOT(setEnableSaveButton()));
    connect(plainTextEditLicense, SIGNAL(textChanged()), this, SLOT(setEnableSaveButton()));

    for(QVector<PlayerStatus*>::const_iterator
	it = labelPlayers.begin(); it != labelPlayers.end(); ++it)
	connect(*it, SIGNAL(mousePressed()), this, SLOT(setEnableSaveButton()));

    connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(pushButtonSave, SIGNAL(clicked()), this, SLOT(accept()));

    connect(this, SIGNAL(finished(int)), this, SLOT(saveSettings()));
}

void Form::MapOptions::saveSettings(void)
{
    QSettings & settings = Resource::localSettings();
    settings.setValue("MapOptions/size", size());
    settings.setValue("MapOptions/defaultGoldMin", spinBoxResourceGoldMin->value());
    settings.setValue("MapOptions/defaultGoldMax", spinBoxResourceGoldMax->value());
    settings.setValue("MapOptions/defaultWoodOreMin", spinBoxResourceWoodOreMin->value());
    settings.setValue("MapOptions/defaultWoodOreMax", spinBoxResourceWoodOreMax->value());
    settings.setValue("MapOptions/defaultOtherMin", spinBoxResourceOtherMin->value());
    settings.setValue("MapOptions/defaultOtherMax", spinBoxResourceOtherMax->value());
}

void Form::MapOptions::setConditionsBoxesMapValues(const MapData & map)
{
    const CondWins & condWins = map.conditionWins();
    const CondLoss & condLoss = map.conditionLoss();

    comboBoxWinsCond->setCurrentIndex(condWins.index());
    comboBoxLossCond->setCurrentIndex(condLoss.index());

    winsConditionsSelected(comboBoxWinsCond->currentIndex());

    switch(condWins.condition())
    {
	case Conditions::CaptureTown:
	    qDebug() << qvariant_cast<QPoint>(condWins.variant());
	    checkBoxAllowNormalVictory->setChecked(condWins.allowNormalVictory());
	    checkBoxCompAlsoWins->setChecked(condWins.compAlsoWins());
	    break;

	case Conditions::DefeatHero:
	    qDebug() << qvariant_cast<QPoint>(condWins.variant());
	    break;

	case Conditions::FindArtifact:
	    checkBoxAllowNormalVictory->setChecked(condWins.allowNormalVictory());
	    qDebug() << qvariant_cast<int>(condWins.variant());
	    break;

	case Conditions::SideWins:
	    qDebug() << qvariant_cast<int>(condWins.variant());
	    break;

	case Conditions::AccumulateGold:
	{
	    checkBoxAllowNormalVictory->setChecked(condWins.allowNormalVictory());
	    checkBoxCompAlsoWins->setChecked(condWins.compAlsoWins());
	    int find = comboBoxWinsCondExt->findData(condWins.variant());
	    if(0 > find)
		comboBoxWinsCondExt->clear();
	    else
		comboBoxWinsCondExt->setCurrentIndex(find);
	}
	    break;

	default: break;
    }

    lossConditionsSelected(comboBoxLossCond->currentIndex());

    switch(condLoss.condition())
    {
	case Conditions::LoseTown:
	case Conditions::LoseHero:
	    qDebug() << qvariant_cast<int>(condLoss.variant());
	    break;

	case Conditions::OutTime:
	{
	    int find = comboBoxLossCondExt->findData(condLoss.variant());
	    if(0 > find)
		comboBoxLossCondExt->clear();
	    else
		comboBoxLossCondExt->setCurrentIndex(find + 1);
	}
	break;

	default: break;
    }

    pushButtonSave->setEnabled(false);
}

void Form::MapOptions::winsConditionsSelected(int index)
{
    comboBoxWinsCondExt->clear();
    comboBoxWinsCondExt->setEnabled(false);
    checkBoxAllowNormalVictory->setEnabled(false);
    checkBoxCompAlsoWins->setEnabled(false);
    checkBoxAllowNormalVictory->setCheckState(Qt::Unchecked);
    checkBoxCompAlsoWins->setCheckState(Qt::Unchecked);

    switch(index)
    {
	// capture castle
	case 1:
	    comboBoxWinsCondExt->setEnabled(true);
	    fillComboBox(*comboBoxWinsCondExt, winsCondTownList);
	    checkBoxAllowNormalVictory->setEnabled(true);
	    checkBoxCompAlsoWins->setEnabled(true);
	    break;

	// defeat hero
	case 2:
	    comboBoxWinsCondExt->setEnabled(true);
	    fillComboBox(*comboBoxWinsCondExt, winsCondHeroList);
	    break;

	// find artifact
	case 3:
	    comboBoxWinsCondExt->setEnabled(true);
	    fillComboBox(*comboBoxWinsCondExt, winsCondArtifactList);
	    checkBoxAllowNormalVictory->setEnabled(true);
	    break;

	// defeat side
	case 4:
	    comboBoxWinsCondExt->setEnabled(true);
	    for(QList<QString>::const_iterator
		it = winsCondSideList.begin(); it != winsCondSideList.end(); ++it)
		comboBoxWinsCondExt->addItem(*it);
	    break;

	// accumulate gold
	case 5:
	    comboBoxWinsCondExt->setEnabled(true);
	    for(int ii = 50000; ii < 1005000; ii += 50000)
		comboBoxWinsCondExt->addItem(QApplication::translate("MapOptions", "%n golds", 0, QApplication::UnicodeUTF8, ii), ii);
	    checkBoxAllowNormalVictory->setEnabled(true);
	    checkBoxCompAlsoWins->setEnabled(true);
	    break;

	default: break;
    }

    setEnableSaveButton();
}

void Form::MapOptions::lossConditionsSelected(int index)
{
    comboBoxLossCondExt->clear();
    comboBoxLossCondExt->setEnabled(false);

    switch(index)
    {
	// lose castle
	case 1:
	    comboBoxLossCondExt->setEnabled(true);
	    fillComboBox(*comboBoxLossCondExt, lossCondTownList);
	    break;
	// lose chero
	case 2:
	    comboBoxLossCondExt->setEnabled(true);
	    fillComboBox(*comboBoxLossCondExt, lossCondHeroList);
	    break;
	// out of time
	case 3:
	    comboBoxLossCondExt->setEnabled(true);
	    for(int ii = 2; ii < 8; ++ii) // 2-7 days
		comboBoxLossCondExt->addItem(QApplication::translate("MapOptions", "%n days", 0, QApplication::UnicodeUTF8, ii), ii);
	    for(int ii = 2; ii < 9; ++ii) // 2-8 weeks
		comboBoxLossCondExt->addItem(QApplication::translate("MapOptions", "%n weeks", 0, QApplication::UnicodeUTF8, ii), ii * 7);
	    for(int ii = 3; ii < 13; ++ii) // 3-12 months
		comboBoxLossCondExt->addItem(QApplication::translate("MapOptions", "%n months", 0, QApplication::UnicodeUTF8, ii), ii * 7 * 4);
	    break;
	default: break;
    }

    setEnableSaveButton();
}

void Form::MapOptions::setEnableSaveButton(void)
{
    pushButtonSave->setEnabled(true);
}

void Form::MapOptions::setEnableSaveButton(const QString & val)
{
    Q_UNUSED(val);
    pushButtonSave->setEnabled(true);
}

Form::ItemsList::ItemsList(int colors, QWidget* parent) : QListWidget(parent), kingdomColors(colors)
{
    setViewMode(QListView::ListMode);

    addItemAct = new QAction(tr("Add"), this);
    addItemAct->setEnabled(true);

    editItemAct = new QAction(tr("Edit"), this);
    editItemAct->setEnabled(false);

    delItemAct = new QAction(tr("Delete"), this);
    delItemAct->setEnabled(false);

    connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editItem(QListWidgetItem*)));
    connect(addItemAct, SIGNAL(triggered()), this, SLOT(addItem()));
    connect(editItemAct, SIGNAL(triggered()), this, SLOT(editCurrentItem()));
    connect(delItemAct, SIGNAL(triggered()), this, SLOT(deleteCurrentItem()));

    MapOptions* ptr = qobject_cast<MapOptions*>(parent);

    if(ptr)
    {
	connect(addItemAct, SIGNAL(triggered()), ptr, SLOT(setEnableSaveButton()));
	connect(editItemAct, SIGNAL(triggered()), ptr, SLOT(setEnableSaveButton()));
	connect(delItemAct, SIGNAL(triggered()), ptr, SLOT(setEnableSaveButton()));
    }
}

void Form::ItemsList::editCurrentItem(void)
{
    editItem(currentItem());
}

void Form::ItemsList::deleteCurrentItem(void)
{
    takeItem(currentRow());
}

void Form::ItemsList::mousePressEvent(QMouseEvent* event)
{
    emit mousePressed();

    if(event->buttons() & Qt::RightButton)
    {
	bool selected = selectedItems().size();
    	editItemAct->setEnabled(selected);
	delItemAct->setEnabled(selected);

	QMenu menu(this);

	menu.addAction(addItemAct);
	menu.addAction(editItemAct);
        menu.addSeparator();
	menu.addAction(delItemAct);

	menu.exec(event->globalPos());
    }

    event->accept();

    QListWidget::mousePressEvent(event);
}

Form::RumorsList::RumorsList(int colors, QWidget* parent) : ItemsList(colors, parent)
{
    addItemAct->setStatusTip(tr("Add new rumor"));
    editItemAct->setStatusTip(tr("Edit rumor"));
    delItemAct->setStatusTip(tr("Delete rumor"));
}

void Form::RumorsList::addItem(void)
{
    RumorDialog dialog;

    if(QDialog::Accepted == dialog.exec())
    {
	QListWidget::addItem(dialog.plainText->toPlainText());
	setCurrentRow(count() - 1);
    }
}

void Form::RumorsList::editItem(QListWidgetItem* item)
{
    RumorDialog dialog(item->text());

    if(QDialog::Accepted == dialog.exec())
	item->setText(dialog.plainText->toPlainText());
}

Form::EventsList::EventsList(int colors, QWidget* parent) : ItemsList(colors, parent)
{
    addItemAct->setStatusTip(tr("Add new event"));
    editItemAct->setStatusTip(tr("Edit event"));
    delItemAct->setStatusTip(tr("Delete event"));
}

void Form::EventsList::addItem(void)
{
    DayEventDialog dialog(DayEvent(), kingdomColors);

    if(QDialog::Accepted == dialog.exec())
    {
	DayEvent event = dialog.result();
	QListWidgetItem* item = new QListWidgetItem(event.header());
	item->setData(Qt::UserRole, QVariant::fromValue(event));
	QListWidget::addItem(item);
	setCurrentRow(count() - 1);
    }
}

void Form::EventsList::editItem(QListWidgetItem* item)
{
    DayEvent event = qvariant_cast<DayEvent>(item->data(Qt::UserRole));
    DayEventDialog dialog(event, kingdomColors);

    if(QDialog::Accepted == dialog.exec())
    {
	event = dialog.result();
	item->setText(event.header());
	item->setData(Qt::UserRole, QVariant::fromValue(event));
    }
}

Form::RumorDialog::RumorDialog(const QString & msg)
{
    setWindowTitle(QApplication::translate("RumorDialog", "Rumor Detail", 0, QApplication::UnicodeUTF8));

    plainText = new QPlainTextEdit(this);
    plainText->setPlainText(msg);

    verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(plainText);

    pushButtonOk = new QPushButton(this);
    pushButtonOk->setText(QApplication::translate("RumorDialog", "Ok", 0, QApplication::UnicodeUTF8));
    pushButtonOk->setEnabled(false);

    horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    pushButtonCancel = new QPushButton(this);
    pushButtonCancel->setText(QApplication::translate("RumorDialog", "Cancel", 0, QApplication::UnicodeUTF8));

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(pushButtonOk);
    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(pushButtonCancel);
    verticalLayout->addLayout(horizontalLayout);

    resize(250, 160);

    connect(plainText, SIGNAL(textChanged()), this, SLOT(enableButtonOk()));
    connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(pushButtonOk, SIGNAL(clicked()), this, SLOT(accept()));
}

void Form::RumorDialog::enableButtonOk(void)
{
    pushButtonOk->setEnabled(! plainText->toPlainText().isEmpty());
}

Form::PlayerAllow::PlayerAllow(int c, bool v, QWidget* parent) : QLabel(parent), col(c), stat(v)
{
    updatePlayers();
}

void Form::PlayerAllow::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    stat = stat ? false : true;
    updatePlayers();
    emit mousePressed();
}

void Form::PlayerAllow::updatePlayers(void)
{
    QPixmap pix;

    switch(col)
    {
	case Color::Blue:	pix = EditorTheme::getImageICN("CELLWIN.ICN", 43).first; break;
    	case Color::Green:	pix = EditorTheme::getImageICN("CELLWIN.ICN", 44).first; break;
    	case Color::Red:	pix = EditorTheme::getImageICN("CELLWIN.ICN", 45).first; break;
    	case Color::Yellow:	pix = EditorTheme::getImageICN("CELLWIN.ICN", 46).first; break;
    	case Color::Orange:	pix = EditorTheme::getImageICN("CELLWIN.ICN", 47).first; break;
    	case Color::Purple:	pix = EditorTheme::getImageICN("CELLWIN.ICN", 48).first; break;
	default: break;
    }

    if(stat)
    {
	QPainter paint(& pix);
	paint.drawPixmap(QPoint(2, 2), EditorTheme::getImageICN("CELLWIN.ICN", 2).first);
    }

    setPixmap(pix);
}

Form::DayEventDialog::DayEventDialog(const DayEvent & event, int kingdomColors)
{
    setWindowTitle(QApplication::translate("DayEventDialog", "Event Detail", 0, QApplication::UnicodeUTF8));

    // tab 1
    tabDay = new QWidget();

    labelDayFirst = new QLabel(tabDay);
    labelDayFirst->setText(QApplication::translate("DayEventDialog", "Day of first occurent", 0, QApplication::UnicodeUTF8));

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(labelDayFirst);

    spinBoxDayFirst = new QSpinBox(tabDay);
    spinBoxDayFirst->setMinimum(1);
    spinBoxDayFirst->setMaximum(65535);
    spinBoxDayFirst->setValue(event.dayFirstOccurent);
    labelDayFirst->setBuddy(spinBoxDayFirst);
    horizontalLayout->addWidget(spinBoxDayFirst);

    labelSubsequent = new QLabel(tabDay);
    labelSubsequent->setText(QApplication::translate("DayEventDialog", "Subsequent occurrences", 0, QApplication::UnicodeUTF8));

    horizontalLayout2 = new QHBoxLayout();
    horizontalLayout2->addWidget(labelSubsequent);

    comboBoxSubsequent = new QComboBox(tabDay);
    comboBoxSubsequent->addItem(QApplication::translate("DayEventDialog", "Never", 0, QApplication::UnicodeUTF8), 0);
    comboBoxSubsequent->addItem(QApplication::translate("DayEventDialog", "Every Day", 0, QApplication::UnicodeUTF8), 1);
    comboBoxSubsequent->addItem(QApplication::translate("DayEventDialog", "Every 2 Days", 0, QApplication::UnicodeUTF8), 2);
    comboBoxSubsequent->addItem(QApplication::translate("DayEventDialog", "Every 3 Days", 0, QApplication::UnicodeUTF8), 3);
    comboBoxSubsequent->addItem(QApplication::translate("DayEventDialog", "Every 4 Days", 0, QApplication::UnicodeUTF8), 4);
    comboBoxSubsequent->addItem(QApplication::translate("DayEventDialog", "Every 5 Days", 0, QApplication::UnicodeUTF8), 5);
    comboBoxSubsequent->addItem(QApplication::translate("DayEventDialog", "Every 6 Days", 0, QApplication::UnicodeUTF8), 6);
    comboBoxSubsequent->addItem(QApplication::translate("DayEventDialog", "Every 7 Days", 0, QApplication::UnicodeUTF8), 7);
    comboBoxSubsequent->addItem(QApplication::translate("DayEventDialog", "Every 14 Days", 0, QApplication::UnicodeUTF8), 14);
    comboBoxSubsequent->addItem(QApplication::translate("DayEventDialog", "Every 21 Days", 0, QApplication::UnicodeUTF8), 21);
    comboBoxSubsequent->addItem(QApplication::translate("DayEventDialog", "Every 28 Days", 0, QApplication::UnicodeUTF8), 28);
    int find = comboBoxSubsequent->findData(event.daySubsequentOccurrences);
    if(0 <= find)
	comboBoxSubsequent->setCurrentIndex(find);
    horizontalLayout2->addWidget(comboBoxSubsequent);

    groupBoxAllowedColors = new QGroupBox(tabDay);
    groupBoxAllowedColors->setTitle(QApplication::translate("DayEventDialog", "Colors allowed to get event", 0, QApplication::UnicodeUTF8));

    horizontalLayout4 = new QHBoxLayout();
    horizontalSpacerPlayersLeft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalSpacerPlayersRight = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout4->addItem(horizontalSpacerPlayersLeft);

    // create allow players labels
    QVector<int> colors = Color::colors(Color::All);

    for(QVector<int>::const_iterator
	it = colors.begin(); it != colors.end(); ++it)
    if((*it) & kingdomColors)
    {
	labelPlayers.push_back(new PlayerAllow(*it, (*it) & event.colors, groupBoxAllowedColors));
	horizontalLayout4->addWidget(labelPlayers.back());
    }
    horizontalLayout4->addItem(horizontalSpacerPlayersRight);

    checkBoxAllowComp = new QCheckBox(groupBoxAllowedColors);
    checkBoxAllowComp->setLayoutDirection(Qt::LeftToRight);
    checkBoxAllowComp->setText(QApplication::translate("DayEventDialog", "Allow computer", 0, QApplication::UnicodeUTF8));
    checkBoxAllowComp->setChecked(event.allowComputer);

    verticalLayout3 = new QVBoxLayout(groupBoxAllowedColors);
    verticalLayout3->addLayout(horizontalLayout4);
    verticalLayout3->addWidget(checkBoxAllowComp);

    verticalLayout = new QVBoxLayout(tabDay);
    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addLayout(horizontalLayout2);
    verticalLayout->addWidget(groupBoxAllowedColors);

    // tab 2
    tabResource = new QWidget();

    const Resources & resources = event.resources;
    int resMin = -65535;
    int resMax = 65535;

    spinBoxResWood = new QSpinBox(tabResource);
    spinBoxResWood->setMinimum(resMin);
    spinBoxResWood->setMaximum(resMax);
    spinBoxResWood->setValue(resources.wood);

    labelResWood = new QLabel(tabResource);
    labelResWood->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 0).first);
    labelResWood->setBuddy(spinBoxResWood);

    horizontalSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    spinBoxResSulfur = new QSpinBox(tabResource);
    spinBoxResSulfur->setMinimum(resMin);
    spinBoxResSulfur->setMaximum(resMax);
    spinBoxResSulfur->setValue(resources.sulfur);

    labelResSulfur = new QLabel(tabResource);
    labelResSulfur->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 3).first);
    labelResSulfur->setBuddy(spinBoxResSulfur);

    horizontalLayout5 = new QHBoxLayout();
    horizontalLayout5->addWidget(labelResWood);
    horizontalLayout5->addWidget(spinBoxResWood);
    horizontalLayout5->addItem(horizontalSpacer2);
    horizontalLayout5->addWidget(labelResSulfur);
    horizontalLayout5->addWidget(spinBoxResSulfur);

    spinBoxResMercury = new QSpinBox(tabResource);
    spinBoxResMercury->setMinimum(resMin);
    spinBoxResMercury->setMaximum(resMax);
    spinBoxResMercury->setValue(resources.mercury);

    labelResMercury = new QLabel(tabResource);
    labelResMercury->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 1).first);
    labelResMercury->setBuddy(spinBoxResMercury);

    horizontalSpacer3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    spinBoxResCrystal = new QSpinBox(tabResource);
    spinBoxResCrystal->setMinimum(resMin);
    spinBoxResCrystal->setMaximum(resMax);
    spinBoxResCrystal->setValue(resources.crystal);

    labelResCrystal = new QLabel(tabResource);
    labelResCrystal->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 4).first);
    labelResCrystal->setBuddy(spinBoxResCrystal);

    horizontalLayout6 = new QHBoxLayout();
    horizontalLayout6->addWidget(labelResMercury);
    horizontalLayout6->addWidget(spinBoxResMercury);
    horizontalLayout6->addItem(horizontalSpacer3);
    horizontalLayout6->addWidget(labelResCrystal);
    horizontalLayout6->addWidget(spinBoxResCrystal);

    spinBoxResOre = new QSpinBox(tabResource);
    spinBoxResOre->setMinimum(resMin);
    spinBoxResOre->setMaximum(resMax);
    spinBoxResOre->setValue(resources.ore);

    labelResOre = new QLabel(tabResource);
    labelResOre->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 2).first);
    labelResOre->setBuddy(spinBoxResOre);

    horizontalSpacer4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    spinBoxResGems = new QSpinBox(tabResource);
    spinBoxResGems->setMinimum(resMin);
    spinBoxResGems->setMaximum(resMax);
    spinBoxResGems->setValue(resources.gems);

    labelResGems = new QLabel(tabResource);
    labelResGems->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 5).first);
    labelResGems->setBuddy(spinBoxResGems);

    horizontalLayout7 = new QHBoxLayout();
    horizontalLayout7->addWidget(labelResOre);
    horizontalLayout7->addWidget(spinBoxResOre);
    horizontalLayout7->addItem(horizontalSpacer4);
    horizontalLayout7->addWidget(labelResGems);
    horizontalLayout7->addWidget(spinBoxResGems);

    horizontalSpacer5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalSpacer6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    spinBoxResGold = new QSpinBox(tabResource);
    spinBoxResGold->setMinimum(resMin);
    spinBoxResGold->setMaximum(resMax);
    spinBoxResGold->setValue(resources.gold);

    labelResGold = new QLabel(tabResource);
    labelResGold->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 6).first);
    labelResGold->setBuddy(spinBoxResGold);

    horizontalLayout8 = new QHBoxLayout();
    horizontalLayout8->addItem(horizontalSpacer5);
    horizontalLayout8->addWidget(labelResGold);
    horizontalLayout8->addWidget(spinBoxResGold);
    horizontalLayout8->addItem(horizontalSpacer6);

    verticalLayout4 = new QVBoxLayout(tabResource);
    verticalLayout4->addLayout(horizontalLayout5);
    verticalLayout4->addLayout(horizontalLayout6);
    verticalLayout4->addLayout(horizontalLayout7);
    verticalLayout4->addLayout(horizontalLayout8);

#ifndef QT_NO_TOOLTIP
    labelResWood->setToolTip(QApplication::translate("DayEventDialog", "wood", 0, QApplication::UnicodeUTF8));
    spinBoxResWood->setToolTip(QApplication::translate("DayEventDialog", "wood", 0, QApplication::UnicodeUTF8));
    labelResSulfur->setToolTip(QApplication::translate("DayEventDialog", "sulfur", 0, QApplication::UnicodeUTF8));
    spinBoxResSulfur->setToolTip(QApplication::translate("DayEventDialog", "sulfur", 0, QApplication::UnicodeUTF8));
    labelResMercury->setToolTip(QApplication::translate("DayEventDialog", "mercury", 0, QApplication::UnicodeUTF8));
    spinBoxResMercury->setToolTip(QApplication::translate("DayEventDialog", "mercury", 0, QApplication::UnicodeUTF8));
    labelResCrystal->setToolTip(QApplication::translate("DayEventDialog", "crystal", 0, QApplication::UnicodeUTF8));
    spinBoxResCrystal->setToolTip(QApplication::translate("DayEventDialog", "crystal", 0, QApplication::UnicodeUTF8));
    labelResOre->setToolTip(QApplication::translate("DayEventDialog", "ore", 0, QApplication::UnicodeUTF8));
    spinBoxResOre->setToolTip(QApplication::translate("DayEventDialog", "ore", 0, QApplication::UnicodeUTF8));
    labelResGems->setToolTip(QApplication::translate("DayEventDialog", "gems", 0, QApplication::UnicodeUTF8));
    spinBoxResGems->setToolTip(QApplication::translate("DayEventDialog", "gems", 0, QApplication::UnicodeUTF8));
    labelResGold->setToolTip(QApplication::translate("DayEventDialog", "gold", 0, QApplication::UnicodeUTF8));
    spinBoxResGold->setToolTip(QApplication::translate("DayEventDialog", "gold", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    // tab 3
    tabMessage = new QWidget();

    plainTextMessage = new QPlainTextEdit(tabMessage);
    plainTextMessage->setPlainText(event.message);

    verticalLayout5 = new QVBoxLayout(tabMessage);
    verticalLayout5->addWidget(plainTextMessage);

    // end
    tabWidget = new QTabWidget(this);

    tabWidget->addTab(tabDay, "Days");
    tabWidget->addTab(tabResource, "Resources");
    tabWidget->addTab(tabMessage, "Message");

    pushButtonOk = new QPushButton(this);
    pushButtonOk->setText(QApplication::translate("DayEventDialog", "Ok", 0, QApplication::UnicodeUTF8));
    pushButtonOk->setEnabled(false);

    horizontalSpacer = new QSpacerItem(238, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    pushButtonCancel = new QPushButton(this);
    pushButtonCancel->setText(QApplication::translate("DayEventDialog", "Cancel", 0, QApplication::UnicodeUTF8));

    horizontalLayout3 = new QHBoxLayout();
    horizontalLayout3->addWidget(pushButtonOk);
    horizontalLayout3->addItem(horizontalSpacer);
    horizontalLayout3->addWidget(pushButtonCancel);

    verticalLayout2 = new QVBoxLayout(this);
    verticalLayout2->addWidget(tabWidget);
    verticalLayout2->addLayout(horizontalLayout3);

    tabWidget->setCurrentIndex(0);

    QSize minSize = minimumSizeHint();

    resize(minSize);
    setMinimumSize(minSize);

    connect(spinBoxDayFirst, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(comboBoxSubsequent , SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(checkBoxAllowComp, SIGNAL(clicked()), this, SLOT(setEnableOKButton()));

    connect(spinBoxResWood, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(spinBoxResMercury, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(spinBoxResOre, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(spinBoxResSulfur, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(spinBoxResCrystal, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(spinBoxResGems, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(spinBoxResGold, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));

    connect(plainTextMessage, SIGNAL(textChanged()), this, SLOT(setEnableOKButton()));

    for(QVector<PlayerAllow*>::const_iterator
	it = labelPlayers.begin(); it != labelPlayers.end(); ++it)
	connect(*it, SIGNAL(mousePressed()), this, SLOT(setEnableOKButton()));

    connect(pushButtonOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

void Form::DayEventDialog::setEnableOKButton(void)
{
    pushButtonOk->setEnabled(true);
}

void Form::DayEventDialog::setEnableOKButton(const QString & val)
{
    Q_UNUSED(val);
    pushButtonOk->setEnabled(true);
}

DayEvent Form::DayEventDialog::result(void) const
{
    DayEvent res;

    res.resources.wood = spinBoxResWood->value();
    res.resources.mercury = spinBoxResMercury->value();
    res.resources.ore = spinBoxResOre->value();
    res.resources.sulfur = spinBoxResSulfur->value();
    res.resources.crystal = spinBoxResCrystal->value();
    res.resources.gems = spinBoxResGems->value();
    res.resources.gold = spinBoxResGold->value();

    res.colors = 0;

    for(QVector<PlayerAllow*>::const_iterator
	it = labelPlayers.begin(); it != labelPlayers.end(); ++it)
    if((*it)->allow())
    {
	res.colors |= (*it)->color();
    }

    res.allowComputer = checkBoxAllowComp->isChecked();
    res.dayFirstOccurent = spinBoxDayFirst->value();
    res.daySubsequentOccurrences = qvariant_cast<int>(comboBoxCurrentData(comboBoxSubsequent));
    res.message = plainTextMessage->toPlainText();

    return res;
}

Form::MiniMap::MiniMap(QWidget* parent) : QFrame(parent)
{
    labelPixmap = new QLabel(this);
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(labelPixmap);
}

void Form::MiniMap::generateFromTiles(const MapTiles & tiles)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    const QSize & ms = tiles.mapSize();
    QImage image(ms, QImage::Format_RGB32);

    for(MapTiles::const_iterator
	it = tiles.begin(); it != tiles.end(); ++it)
    {
	const QPoint & pos = (*it)->mapPos();

	switch((*it)->groundType())
	{
	    case Ground::Desert:	image.setPixel(pos, qRgb(0xD0, 0xC0, 0x48)); break;
	    case Ground::Snow:		image.setPixel(pos, qRgb(0xE0, 0xE0, 0xE0)); break;
	    case Ground::Swamp:		image.setPixel(pos, qRgb(0x58, 0x94, 0xA0)); break;
	    case Ground::Wasteland:	image.setPixel(pos, qRgb(0xE0, 0x48, 0)); break;
	    case Ground::Beach:		image.setPixel(pos, qRgb(0xE0, 0xD0, 0x80)); break;
	    case Ground::Lava:		image.setPixel(pos, qRgb(0x58, 0x58, 0x58)); break;
	    case Ground::Dirt:		image.setPixel(pos, qRgb(0x80, 0x58, 0x28)); break;
	    case Ground::Grass:		image.setPixel(pos, qRgb(0x18, 0x68, 0x18)); break;
	    case Ground::Water:		image.setPixel(pos, qRgb(0, 0x48, 0xD0)); break;
	    default: break;
	}
    }

    const QSize sz(144, 144);
    QImage scaled;

    if(ms.width() > ms.height())
	scaled = image.scaledToWidth(sz.width());
    else
    if(ms.width() < ms.height())
        scaled = image.scaledToHeight(sz.height());
    else
        scaled = image.scaled(sz);

    QPixmap border(scaled.size() + QSize(2, 2));
    border.fill(QColor(0x10, 0x10, 0x10));
    QPainter paint(& border);
    paint.drawImage(1, 1, scaled);
    labelPixmap->setPixmap(border);

    QSize minSize = minimumSizeHint();
    resize(minSize);
    setFixedSize(minSize);

    QApplication::restoreOverrideCursor();
}

Form::EditResource::EditResource(int res)
{
    setWindowTitle(QApplication::translate("DialogEditResource", "Edit Resource", 0, QApplication::UnicodeUTF8));

    QSettings & settings = Resource::localSettings();
    int min = 0;
    int max = 0;

    switch(res)
    {
        case Resource::Gold:
	    min = settings.value("MapOptions/defaultGoldMin", DEFAULT_RESOURCE_GOLD_MIN).toInt();
	    max = settings.value("MapOptions/defaultGoldMax", DEFAULT_RESOURCE_GOLD_MAX).toInt();
	    break;

        case Resource::Wood:
        case Resource::Ore:
	    min = settings.value("MapOptions/defaultWoodOreMin", DEFAULT_RESOURCE_WOOD_ORE_MIN).toInt();
	    max = settings.value("MapOptions/defaultWoodOreMax", DEFAULT_RESOURCE_WOOD_ORE_MAX).toInt();
	    break;

        case Resource::Mercury:
        case Resource::Sulfur:
        case Resource::Crystal:
        case Resource::Gems:
	    min = settings.value("MapOptions/defaultOtherMin", DEFAULT_RESOURCE_OTHER_MIN).toInt();
	    max = settings.value("MapOptions/defaultOtherMax", DEFAULT_RESOURCE_OTHER_MAX).toInt();
	    break;

	default: break;
    }

    checkBoxDefault = new QCheckBox(this);
    checkBoxDefault->setChecked(true);
    checkBoxDefault->setText(tr("default (random: %1-%2)").arg(min).arg(max));

    spinBoxCount = new QSpinBox(this);
    spinBoxCount->setEnabled(false);
    spinBoxCount->setMinimum(min);
    spinBoxCount->setMaximum(max);
    spinBoxCount->setValue(min);

    labelCount = new QLabel(this);
    labelCount->setEnabled(false);
    labelCount->setText(QApplication::translate("DialogEditResource", "fixed value:", 0, QApplication::UnicodeUTF8));
    labelCount->setBuddy(spinBoxCount);

    horizontalLayout1 = new QHBoxLayout();
    horizontalLayout1->addWidget(labelCount);
    horizontalLayout1->addWidget(spinBoxCount);

    verticalSpacer = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);

    pushButtonOk = new QPushButton(this);
    pushButtonOk->setText(QApplication::translate("DialogEditResource", "Ok", 0, QApplication::UnicodeUTF8));

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    pushButtonCancel = new QPushButton(this);
    pushButtonCancel->setText(QApplication::translate("DialogEditResource", "Cancel", 0, QApplication::UnicodeUTF8));

    horizontalLayout2 = new QHBoxLayout();
    horizontalLayout2->addWidget(pushButtonOk);
    horizontalLayout2->addItem(horizontalSpacer);
    horizontalLayout2->addWidget(pushButtonCancel);

    verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(checkBoxDefault);
    verticalLayout->addLayout(horizontalLayout1);
    verticalLayout->addItem(verticalSpacer);
    verticalLayout->addLayout(horizontalLayout2);

    QSize minSize = minimumSizeHint();

    resize(minSize);
    setFixedSize(minSize);

    connect(checkBoxDefault, SIGNAL(toggled(bool)), this, SLOT(disableCustomCount(bool)));
    connect(pushButtonOk, SIGNAL(clicked()), this, SLOT(reject()));
    connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(accept()));
}

void Form::EditResource::disableCustomCount(bool f)
{
    labelCount->setEnabled(! f);
    spinBoxCount->setEnabled(! f);
}

Form::MapEventDialog::MapEventDialog(const MapEvent & event, int kingdomColors)
{
    setWindowTitle(QApplication::translate("MapEventDialog", "Event Detail", 0, QApplication::UnicodeUTF8));

    // tab 1
    tabAccess = new QWidget();

    groupBoxAllowedColors = new QGroupBox(tabAccess);
    groupBoxAllowedColors->setTitle(QApplication::translate("MapEventDialog", "Colors allowed to get event", 0, QApplication::UnicodeUTF8));

    horizontalLayoutPlayers = new QHBoxLayout();
    horizontalSpacerPlayersLeft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalSpacerPlayersRight = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayoutPlayers->addItem(horizontalSpacerPlayersLeft);

    // create allow players labels
    QVector<int> colors = Color::colors(Color::All);

    for(QVector<int>::const_iterator
	it = colors.begin(); it != colors.end(); ++it)
    if((*it) & kingdomColors)
    {
	labelPlayers.push_back(new PlayerAllow(*it, (*it) & event.colors, groupBoxAllowedColors));
	horizontalLayoutPlayers->addWidget(labelPlayers.back());
    }
    horizontalLayoutPlayers->addItem(horizontalSpacerPlayersRight);

    checkBoxAllowComp = new QCheckBox(groupBoxAllowedColors);
    checkBoxAllowComp->setLayoutDirection(Qt::LeftToRight);
    checkBoxAllowComp->setText(QApplication::translate("MapEventDialog", "Allow computer", 0, QApplication::UnicodeUTF8));
    checkBoxAllowComp->setChecked(event.allowComputer);

    checkBoxCancelAfterFirstVisit = new QCheckBox(groupBoxAllowedColors);
    checkBoxCancelAfterFirstVisit->setLayoutDirection(Qt::LeftToRight);
    checkBoxCancelAfterFirstVisit->setText(QApplication::translate("MapEventDialog", "Cancel after first visit", 0, QApplication::UnicodeUTF8));
    checkBoxCancelAfterFirstVisit->setChecked(event.cancelAfterFirstVisit);

    verticalLayoutAllowCols = new QVBoxLayout(groupBoxAllowedColors);
    verticalLayoutAllowCols->addLayout(horizontalLayoutPlayers);
    verticalLayoutAllowCols->addWidget(checkBoxAllowComp);
    verticalLayoutAllowCols->addWidget(checkBoxCancelAfterFirstVisit);

    spacerItemAccess = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayoutTabAcs = new QVBoxLayout(tabAccess);
    verticalLayoutTabAcs->addWidget(groupBoxAllowedColors);
    verticalLayoutTabAcs->addItem(spacerItemAccess);

    // tab 2
    tabGift = new QWidget();

    groupBoxResource = new QGroupBox(tabGift);
    groupBoxResource->setTitle(QApplication::translate("MapEventDialog", "Resources", 0, QApplication::UnicodeUTF8));

    const Resources & resources = event.resources;
    int resMin = -65535;
    int resMax = 65535;

    spinBoxResWood = new QSpinBox(groupBoxResource);
    spinBoxResWood->setMinimum(resMin);
    spinBoxResWood->setMaximum(resMax);
    spinBoxResWood->setValue(resources.wood);

    labelResWood = new QLabel(groupBoxResource);
    labelResWood->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 0).first);
    labelResWood->setBuddy(spinBoxResWood);

    horizontalSpacerWoodSulf = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    spinBoxResSulfur = new QSpinBox(groupBoxResource);
    spinBoxResSulfur->setMinimum(resMin);
    spinBoxResSulfur->setMaximum(resMax);
    spinBoxResSulfur->setValue(resources.sulfur);

    labelResSulfur = new QLabel(groupBoxResource);
    labelResSulfur->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 3).first);
    labelResSulfur->setBuddy(spinBoxResSulfur);

    horizontalLayoutWoodSulf = new QHBoxLayout();
    horizontalLayoutWoodSulf->addWidget(labelResWood);
    horizontalLayoutWoodSulf->addWidget(spinBoxResWood);
    horizontalLayoutWoodSulf->addItem(horizontalSpacerWoodSulf);
    horizontalLayoutWoodSulf->addWidget(labelResSulfur);
    horizontalLayoutWoodSulf->addWidget(spinBoxResSulfur);

    spinBoxResMercury = new QSpinBox(groupBoxResource);
    spinBoxResMercury->setMinimum(resMin);
    spinBoxResMercury->setMaximum(resMax);
    spinBoxResMercury->setValue(resources.mercury);

    labelResMercury = new QLabel(groupBoxResource);
    labelResMercury->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 1).first);
    labelResMercury->setBuddy(spinBoxResMercury);

    horizontalSpacerMercCryst = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    spinBoxResCrystal = new QSpinBox(tabGift);
    spinBoxResCrystal->setMinimum(resMin);
    spinBoxResCrystal->setMaximum(resMax);
    spinBoxResCrystal->setValue(resources.crystal);

    labelResCrystal = new QLabel(groupBoxResource);
    labelResCrystal->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 4).first);
    labelResCrystal->setBuddy(spinBoxResCrystal);

    horizontalLayoutMercCryst = new QHBoxLayout();
    horizontalLayoutMercCryst->addWidget(labelResMercury);
    horizontalLayoutMercCryst->addWidget(spinBoxResMercury);
    horizontalLayoutMercCryst->addItem(horizontalSpacerMercCryst);
    horizontalLayoutMercCryst->addWidget(labelResCrystal);
    horizontalLayoutMercCryst->addWidget(spinBoxResCrystal);

    spinBoxResOre = new QSpinBox(groupBoxResource);
    spinBoxResOre->setMinimum(resMin);
    spinBoxResOre->setMaximum(resMax);
    spinBoxResOre->setValue(resources.ore);

    labelResOre = new QLabel(groupBoxResource);
    labelResOre->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 2).first);
    labelResOre->setBuddy(spinBoxResOre);

    horizontalSpacerOreGems = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    spinBoxResGems = new QSpinBox(groupBoxResource);
    spinBoxResGems->setMinimum(resMin);
    spinBoxResGems->setMaximum(resMax);
    spinBoxResGems->setValue(resources.gems);

    labelResGems = new QLabel(groupBoxResource);
    labelResGems->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 5).first);
    labelResGems->setBuddy(spinBoxResGems);

    horizontalLayoutOreGems = new QHBoxLayout();
    horizontalLayoutOreGems->addWidget(labelResOre);
    horizontalLayoutOreGems->addWidget(spinBoxResOre);
    horizontalLayoutOreGems->addItem(horizontalSpacerOreGems);
    horizontalLayoutOreGems->addWidget(labelResGems);
    horizontalLayoutOreGems->addWidget(spinBoxResGems);

    horizontalSpacerGoldLeft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalSpacerGoldRight= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    spinBoxResGold = new QSpinBox(groupBoxResource);
    spinBoxResGold->setMinimum(resMin);
    spinBoxResGold->setMaximum(resMax);
    spinBoxResGold->setValue(resources.gold);

    labelResGold = new QLabel(groupBoxResource);
    labelResGold->setPixmap(EditorTheme::getImageICN("RESOURCE.ICN", 6).first);
    labelResGold->setBuddy(spinBoxResGold);

    horizontalLayoutGold = new QHBoxLayout();
    horizontalLayoutGold->addItem(horizontalSpacerGoldLeft);
    horizontalLayoutGold->addWidget(labelResGold);
    horizontalLayoutGold->addWidget(spinBoxResGold);
    horizontalLayoutGold->addItem(horizontalSpacerGoldRight);

    verticalLayoutResource = new QVBoxLayout(groupBoxResource);
    verticalLayoutResource->addLayout(horizontalLayoutWoodSulf);
    verticalLayoutResource->addLayout(horizontalLayoutMercCryst);
    verticalLayoutResource->addLayout(horizontalLayoutOreGems);
    verticalLayoutResource->addLayout(horizontalLayoutGold);

    groupBoxArtifact = new QGroupBox(tabGift);
    groupBoxArtifact->setTitle(QApplication::translate("MapEventDialog", "Artifact to give", 0, QApplication::UnicodeUTF8));

    labelArtifact = new QLabel(groupBoxArtifact);
    changeLabelArtifact(event.artifact);

    comboBoxArtifact = new QComboBox(groupBoxArtifact);
    for(int index = Artifact::None; index < Artifact::Unknown; ++index)
	comboBoxArtifact->addItem(Artifact::transcribe(index), index);
    comboBoxArtifact->setCurrentIndex(event.artifact);

    horizontalLayoutArtifact = new QHBoxLayout(groupBoxArtifact);
    horizontalLayoutArtifact->addWidget(labelArtifact);
    horizontalLayoutArtifact->addWidget(comboBoxArtifact);

    verticalLayoutGift = new QVBoxLayout(tabGift);
    verticalLayoutGift->addWidget(groupBoxResource);
    verticalLayoutGift->addWidget(groupBoxArtifact);

#ifndef QT_NO_TOOLTIP
    labelResWood->setToolTip(QApplication::translate("MapEventDialog", "wood", 0, QApplication::UnicodeUTF8));
    spinBoxResWood->setToolTip(QApplication::translate("MapEventDialog", "wood", 0, QApplication::UnicodeUTF8));
    labelResSulfur->setToolTip(QApplication::translate("MapEventDialog", "sulfur", 0, QApplication::UnicodeUTF8));
    spinBoxResSulfur->setToolTip(QApplication::translate("MapEventDialog", "sulfur", 0, QApplication::UnicodeUTF8));
    labelResMercury->setToolTip(QApplication::translate("MapEventDialog", "mercury", 0, QApplication::UnicodeUTF8));
    spinBoxResMercury->setToolTip(QApplication::translate("MapEventDialog", "mercury", 0, QApplication::UnicodeUTF8));
    labelResCrystal->setToolTip(QApplication::translate("MapEventDialog", "crystal", 0, QApplication::UnicodeUTF8));
    spinBoxResCrystal->setToolTip(QApplication::translate("MapEventDialog", "crystal", 0, QApplication::UnicodeUTF8));
    labelResOre->setToolTip(QApplication::translate("MapEventDialog", "ore", 0, QApplication::UnicodeUTF8));
    spinBoxResOre->setToolTip(QApplication::translate("MapEventDialog", "ore", 0, QApplication::UnicodeUTF8));
    labelResGems->setToolTip(QApplication::translate("MapEventDialog", "gems", 0, QApplication::UnicodeUTF8));
    spinBoxResGems->setToolTip(QApplication::translate("MapEventDialog", "gems", 0, QApplication::UnicodeUTF8));
    labelResGold->setToolTip(QApplication::translate("MapEventDialog", "gold", 0, QApplication::UnicodeUTF8));
    spinBoxResGold->setToolTip(QApplication::translate("MapEventDialog", "gold", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    // tab 3
    tabMessage = new QWidget();

    plainTextMessage = new QPlainTextEdit(tabMessage);
    plainTextMessage->setPlainText(event.message);

    verticalLayoutTabMsg = new QVBoxLayout(tabMessage);
    verticalLayoutTabMsg->addWidget(plainTextMessage);

    // end
    tabWidget = new QTabWidget(this);

    tabWidget->addTab(tabAccess, "Access");
    tabWidget->addTab(tabGift, "Gifts");
    tabWidget->addTab(tabMessage, "Message");

    pushButtonOk = new QPushButton(this);
    pushButtonOk->setText(QApplication::translate("MapEventDialog", "Ok", 0, QApplication::UnicodeUTF8));
    pushButtonOk->setEnabled(false);

    horizontalSpacerButtons = new QSpacerItem(238, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    pushButtonCancel = new QPushButton(this);
    pushButtonCancel->setText(QApplication::translate("MapEventDialog", "Cancel", 0, QApplication::UnicodeUTF8));

    horizontalLayoutButtons = new QHBoxLayout();
    horizontalLayoutButtons->addWidget(pushButtonOk);
    horizontalLayoutButtons->addItem(horizontalSpacerButtons);
    horizontalLayoutButtons->addWidget(pushButtonCancel);

    verticalLayoutForm = new QVBoxLayout(this);
    verticalLayoutForm->addWidget(tabWidget);
    verticalLayoutForm->addLayout(horizontalLayoutButtons);

    tabWidget->setCurrentIndex(0);

    QSize minSize = minimumSizeHint();

    resize(minSize);
    setMinimumSize(minSize);

    connect(checkBoxAllowComp, SIGNAL(clicked()), this, SLOT(setEnableOKButton()));
    connect(checkBoxCancelAfterFirstVisit, SIGNAL(clicked()), this, SLOT(setEnableOKButton()));

    connect(spinBoxResWood, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(spinBoxResMercury, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(spinBoxResOre, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(spinBoxResSulfur, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(spinBoxResCrystal, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(spinBoxResGems, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(spinBoxResGold, SIGNAL(valueChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(comboBoxArtifact , SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setEnableOKButton(const QString &)));
    connect(comboBoxArtifact , SIGNAL(currentIndexChanged(int)), this, SLOT(changeLabelArtifact(int)));

    connect(plainTextMessage, SIGNAL(textChanged()), this, SLOT(setEnableOKButton()));

    for(QVector<PlayerAllow*>::const_iterator
	it = labelPlayers.begin(); it != labelPlayers.end(); ++it)
	connect(*it, SIGNAL(mousePressed()), this, SLOT(setEnableOKButton()));

    connect(pushButtonOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

void Form::MapEventDialog::setEnableOKButton(void)
{
    pushButtonOk->setEnabled(true);
}

void Form::MapEventDialog::setEnableOKButton(const QString & val)
{
    Q_UNUSED(val);
    pushButtonOk->setEnabled(true);
}

void Form::MapEventDialog::changeLabelArtifact(int index)
{
    labelArtifact->setPixmap(EditorTheme::getImageICN("ARTIFACT.ICN", index).first.scaled(48, 48));
    labelArtifact->setToolTip(Artifact::transcribe(index));
}

MapEvent Form::MapEventDialog::result(const QPoint & pos, quint32 uid) const
{
    MapEvent res(pos, uid);

    res.resources.wood = spinBoxResWood->value();
    res.resources.mercury = spinBoxResMercury->value();
    res.resources.ore = spinBoxResOre->value();
    res.resources.sulfur = spinBoxResSulfur->value();
    res.resources.crystal = spinBoxResCrystal->value();
    res.resources.gems = spinBoxResGems->value();
    res.resources.gold = spinBoxResGold->value();

    res.colors = 0;

    for(QVector<PlayerAllow*>::const_iterator
	it = labelPlayers.begin(); it != labelPlayers.end(); ++it)
    if((*it)->allow())
    {
	res.colors |= (*it)->color();
    }

    res.artifact = comboBoxArtifact->currentIndex();

    res.allowComputer = checkBoxAllowComp->isChecked();
    res.cancelAfterFirstVisit = checkBoxCancelAfterFirstVisit->isChecked();
    res.message = plainTextMessage->toPlainText();

    return res;
}
