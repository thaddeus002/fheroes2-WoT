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

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem1);

    labelWidth = new QLabel(this);
    labelWidth->setEnabled(true);
    labelWidth->setVisible(false);
    labelWidth->setText(QApplication::translate("SelectMapSize", "width", 0, QApplication::UnicodeUTF8));
    hboxLayout->addWidget(labelWidth);

    spinBoxWidth = new QSpinBox(this);
    spinBoxWidth->setMaximum(1024);
    spinBoxWidth->setMinimum(36);
    spinBoxWidth->setSingleStep(2);
    spinBoxWidth->setVisible(false);
    hboxLayout->addWidget(spinBoxWidth);

    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem2);
    vboxLayout->addLayout(hboxLayout);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);

    spacerItem3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout1->addItem(spacerItem3);

    labelHeight = new QLabel(this);
    labelHeight->setText(QApplication::translate("SelectMapSize", "height", 0, QApplication::UnicodeUTF8));
    labelHeight->setVisible(false);
    hboxLayout1->addWidget(labelHeight);

    spinBoxHeight = new QSpinBox(this);
    spinBoxHeight->setMaximum(1024);
    spinBoxHeight->setMinimum(36);
    spinBoxHeight->setSingleStep(2);
    spinBoxHeight->setVisible(false);
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

    labelWidth->setBuddy(spinBoxWidth);
    labelHeight->setBuddy(spinBoxHeight);

    QSize minSize = minimumSizeHint();

    resize(minSize);
    setMinimumSize(minSize);
    setMaximumSize(minSize);

    QObject::connect(pushButtonExpert, SIGNAL(clicked()), this, SLOT(clickExpert()));
    QObject::connect(pushButtonOk, SIGNAL(clicked()), this, SLOT(clickOk()));
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
    setMinimumSize(minSize);
    setMaximumSize(minSize);

    QObject::connect(pushButtonExit, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(pushButtonSave, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(pushButtonSelect, SIGNAL(clicked()), this, SLOT(clickSelect()));
}

void Form::SelectDataFile::clickSelect(void)
{
    result = QFileDialog::getOpenFileName(this, tr("Open data file"), "", "heroes2.agg");
    pushButtonSave->setEnabled(true);
}

class SelectImageItem : public QListWidgetItem
{
public:
    SelectImageItem(const CompositeObject & obj, EditorTheme & theme)
    {
	setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	setData(Qt::UserRole, QVariant::fromValue(obj));
	setIcon(theme.getImage(obj));
	setText(obj.name);
	setSizeHint(QSize(132, 80));
    }
};

Form::SelectImageTab::SelectImageTab(const QDomElement & groupElem, const QString & dataFolder, EditorTheme & theme)
{
    verticalLayout = new QVBoxLayout(this);
    listWidget = new QListWidget(this);
    verticalLayout->addWidget(listWidget);
    listWidget->setIconSize(QSize(64, 64));
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setWrapping(true);
    listWidget->setResizeMode(QListView::Adjust);

    Editor::MyXML templateObjects(theme.resourceFile(dataFolder, "template.xml"), "template");
    Editor::MyXML objectsElem(theme.resourceFile(dataFolder, groupElem.attribute("file")), "objects");
    QString icn = objectsElem.attribute("icn");

    if(! objectsElem.isNull())
    {
        // parse element: object
        QDomNodeList objectsList = objectsElem.elementsByTagName("object");

        for(int pos2 = 0; pos2 < objectsList.size(); ++pos2)
        {
            CompositeObject obj(icn, objectsList.item(pos2).toElement(), 0);

            if(obj.isValid())
		listWidget->addItem(new SelectImageItem(obj, theme));
        }

        // parse element: template
        objectsList = objectsElem.elementsByTagName("template");

        for(int pos2 = 0; pos2 < objectsList.size(); ++pos2)
        {
            QDomElement tmplElem = objectsList.item(pos2).toElement();

            if(! templateObjects.isNull() && tmplElem.hasAttribute("section"))
            {
                QDomElement objElem = templateObjects.firstChildElement(tmplElem.attribute("section"));
                int startIndex = tmplElem.attribute("index").toInt();

                // override tags: icn
                if(tmplElem.hasAttribute("icn"))
                    icn = tmplElem.attribute("icn");

                CompositeObject obj(icn, objElem, startIndex);

                // override tags: name
                if(tmplElem.hasAttribute("name"))
                    obj.name = tmplElem.attribute("name");


            	if(obj.isValid())
		    listWidget->addItem(new SelectImageItem(obj, theme));
            }
        }
    }
}

bool Form::SelectImageTab::isSelected(void) const
{
    return listWidget->selectedItems().size();
}

namespace Form
{
    int SelectImage::lastNumTab = 0;
};

Form::SelectImage::SelectImage(EditorTheme & theme)
{
    setWindowTitle(QApplication::translate("SelectImage", "Select Object", 0, QApplication::UnicodeUTF8));

    tabWidget = new QTabWidget(this);

    const QString dataFolder("objects");
    Editor::MyXML groupsElem(theme.resourceFile(dataFolder, "groups.xml"), "groups");

    if(! groupsElem.isNull())
    {
        QDomNodeList groupsList = groupsElem.elementsByTagName("group");

        for(int pos1 = 0; pos1 < groupsList.size(); ++pos1)
        {
            QDomElement groupElem = groupsList.item(pos1).toElement();
            QString name = groupElem.attribute("name");

            if(! name.isEmpty())
		tabWidget->addTab(new SelectImageTab(groupElem, dataFolder, theme), name);
	}

	tabWidget->setCurrentIndex(lastNumTab);
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

    resize(540, 410);
    tabSwitched(lastNumTab);

    QObject::connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSwitched(int)));
    QObject::connect(pushButtonClose, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(pushButtonSelect, SIGNAL(clicked()), this, SLOT(clickSelect()));
}

void Form::SelectImage::tabSwitched(int num)
{
    SelectImageTab* tab = qobject_cast<SelectImageTab*>(tabWidget->widget(num));

    disconnect(this, SLOT(accept(QListWidgetItem*)));
    disconnect(this, SLOT(selectionChanged()));

    if(tab)
    {
	pushButtonSelect->setEnabled(tab->isSelected());

	QObject::connect(tab->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept(QListWidgetItem*)));
	QObject::connect(tab->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

	lastNumTab = num;
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
	ts << "(" << (*it).second.x() << ", " << (*it).second.x() << ")" << " - " << (*it).first;
	box.addItem(str, (*it).second);
    }
}

Form::PlayerStatus::PlayerStatus(int c, int v, EditorTheme & t, QWidget* parent) : QLabel(parent), col(c), stat(v), theme(t)
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
        case Color::Blue:	setPixmap(theme.getImageICN("CELLWIN.ICN", 19 + status()).first); break;
        case Color::Green:	setPixmap(theme.getImageICN("CELLWIN.ICN", 23 + status()).first); break;
        case Color::Red:	setPixmap(theme.getImageICN("CELLWIN.ICN", 27 + status()).first); break;
        case Color::Yellow:	setPixmap(theme.getImageICN("CELLWIN.ICN", 31 + status()).first); break;
        case Color::Orange:	setPixmap(theme.getImageICN("CELLWIN.ICN", 35 + status()).first); break;
        case Color::Purple:	setPixmap(theme.getImageICN("CELLWIN.ICN", 39 + status()).first); break;
        default: break;
    }
}

Form::MapOptions::MapOptions(MapData & map)
{
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
        labelPlayers.push_back(new PlayerStatus(*it, stat, map.theme(), groupBoxPlayers));
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

    listWidgetRumors = new RumorsList(this);
    listWidgetEvents = new EventsList(this);

    verticalLayout7 = new QVBoxLayout(groupBoxRumors);
    verticalLayout7->addWidget(listWidgetRumors);

    verticalLayout8 = new QVBoxLayout(groupBoxEvents);
    verticalLayout8->addWidget(listWidgetEvents);

    horizontalLayout6 = new QHBoxLayout(tabRumorsEvents);
    horizontalLayout6->addWidget(groupBoxRumors);
    horizontalLayout6->addWidget(groupBoxEvents);

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
	static_cast<QListWidget*>(listWidgetEvents)->addItem((*it).header());
    }
    listWidgetEvents->sortItems();

    setConditionsBoxesMapValues(map);


    QSize minSize = minimumSizeHint();

    resize(minSize);
    setMinimumSize(minSize);

    QObject::connect(lineEditName, SIGNAL(textChanged(const QString &)), this, SLOT(setEnableSaveButton(const QString &)));
    QObject::connect(plainTextEditDescription, SIGNAL(textChanged()), this, SLOT(setEnableSaveButton()));
    QObject::connect(comboBoxDifficulty, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setEnableSaveButton(const QString &)));

    QObject::connect(comboBoxWinsCond, SIGNAL(currentIndexChanged(int)), this, SLOT(winsConditionsSelected(int)));
    QObject::connect(comboBoxLossCond, SIGNAL(currentIndexChanged(int)), this, SLOT(lossConditionsSelected(int)));

    QObject::connect(comboBoxWinsCondExt, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setEnableSaveButton(const QString &)));
    QObject::connect(comboBoxLossCondExt, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setEnableSaveButton(const QString &)));

    QObject::connect(checkBoxAllowNormalVictory, SIGNAL(clicked()), this, SLOT(setEnableSaveButton()));
    QObject::connect(checkBoxCompAlsoWins, SIGNAL(clicked()), this, SLOT(setEnableSaveButton()));
    QObject::connect(checkBoxStartWithHero, SIGNAL(clicked()), this, SLOT(setEnableSaveButton()));

    QObject::connect(listWidgetRumors, SIGNAL(mousePressed()), listWidgetEvents, SLOT(clearSelection()));
    QObject::connect(listWidgetEvents, SIGNAL(mousePressed()), listWidgetRumors, SLOT(clearSelection()));

    QObject::connect(plainTextEditAuthors, SIGNAL(textChanged()), this, SLOT(setEnableSaveButton()));
    QObject::connect(plainTextEditLicense, SIGNAL(textChanged()), this, SLOT(setEnableSaveButton()));

    for(QVector<PlayerStatus*>::const_iterator
	it = labelPlayers.begin(); it != labelPlayers.end(); ++it)
	QObject::connect(*it, SIGNAL(mousePressed()), this, SLOT(setEnableSaveButton()));

    QObject::connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(pushButtonSave, SIGNAL(clicked()), this, SLOT(accept()));
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

Form::ItemsList::ItemsList(QWidget* parent) : QListWidget(parent)
{
    setViewMode(QListView::ListMode);

    addItemAct = new QAction(tr("Add"), this);
    addItemAct->setEnabled(true);

    editItemAct = new QAction(tr("Edit"), this);
    editItemAct->setEnabled(false);

    delItemAct = new QAction(tr("Delete"), this);
    delItemAct->setEnabled(false);

    QObject::connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editItem(QListWidgetItem*)));
    QObject::connect(addItemAct, SIGNAL(triggered()), this, SLOT(addItem()));
    QObject::connect(editItemAct, SIGNAL(triggered()), this, SLOT(editCurrentItem()));
    QObject::connect(delItemAct, SIGNAL(triggered()), this, SLOT(deleteCurrentItem()));

    MapOptions* ptr = qobject_cast<MapOptions*>(parent);

    if(ptr)
    {
	QObject::connect(addItemAct, SIGNAL(triggered()), ptr, SLOT(setEnableSaveButton()));
	QObject::connect(editItemAct, SIGNAL(triggered()), ptr, SLOT(setEnableSaveButton()));
	QObject::connect(delItemAct, SIGNAL(triggered()), ptr, SLOT(setEnableSaveButton()));
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

Form::RumorsList::RumorsList(QWidget* parent) : ItemsList(parent)
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

Form::EventsList::EventsList(QWidget* parent) : ItemsList(parent)
{
    addItemAct->setStatusTip(tr("Add new event"));
    editItemAct->setStatusTip(tr("Edit event"));
    delItemAct->setStatusTip(tr("Delete event"));
}

void Form::EventsList::addItem(void)
{
    QListWidget::addItem(QString("event event event"));
    setCurrentRow(count() - 1);
}

void Form::EventsList::editItem(QListWidgetItem*)
{
    qDebug() << "edit event";
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

    QObject::connect(plainText, SIGNAL(textChanged()), this, SLOT(enableButtonOk()));
    QObject::connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(pushButtonOk, SIGNAL(clicked()), this, SLOT(accept()));
}

void Form::RumorDialog::enableButtonOk(void)
{
    pushButtonOk->setEnabled(! plainText->toPlainText().isEmpty());
}
