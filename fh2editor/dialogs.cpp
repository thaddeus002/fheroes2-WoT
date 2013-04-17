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

void Dialog::MapOptions(MapData & map)
{
    Form::MapOptions form(map);
    form.exec();
}

Form::SelectMapSize::SelectMapSize()
{
    setWindowTitle(QApplication::translate("SelectMapSize", "Select Size", 0, QApplication::UnicodeUTF8));

    vboxLayout = new QVBoxLayout(this);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);

    comboBoxSize = new QComboBox(this);
    comboBoxSize->clear();
    comboBoxSize->addItem(QApplication::translate("SelectMapSize", "Small (36x36)", 0, QApplication::UnicodeUTF8));
    comboBoxSize->addItem(QApplication::translate("SelectMapSize", "Medium (72x72)", 0, QApplication::UnicodeUTF8));
    comboBoxSize->addItem(QApplication::translate("SelectMapSize", "Large (108x108)", 0, QApplication::UnicodeUTF8));
    comboBoxSize->addItem(QApplication::translate("SelectMapSize", "Extra Large (144x144)", 0, QApplication::UnicodeUTF8));
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
	switch(comboBoxSize->currentIndex())
	{
	    case 0:	result = QSize(36, 36); break;
	    case 1:	result = QSize(72, 72); break;
	    case 2:	result = QSize(108, 108); break;
	    case 3:	result = QSize(144, 144); break;
	    default: break;
	}
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
    setWindowTitle(QApplication::translate("SelectImage", "Dialog", 0, QApplication::UnicodeUTF8));

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

Form::MapOptions::MapOptions(const MapData & map)
{
    setWindowTitle(QApplication::translate("MapOptions", "Dialog", 0, QApplication::UnicodeUTF8));

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
    comboBoxDifficulty->insertItems(0, QStringList()
         << QApplication::translate("MapOptions", "Easy", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MapOptions", "Normal", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MapOptions", "Tough", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MapOptions", "Expert", 0, QApplication::UnicodeUTF8)
    );
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
    comboBoxWinsCond->insertItems(0, QStringList()
         << QApplication::translate("MapOptions", "None", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MapOptions", "Capture a particular castle", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MapOptions", "Defeat a particular hero", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MapOptions", "Find a particular artifact", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MapOptions", "One side defeats another", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MapOptions", "Accumulate gold", 0, QApplication::UnicodeUTF8)
    );

    comboBoxWinsCondExt = new QComboBox(groupBoxWinsCond);
    comboBoxWinsCondExt->insertItems(0, QStringList()
         << QApplication::translate("MapOptions", "None", 0, QApplication::UnicodeUTF8)
    );
    comboBoxWinsCondExt->setEnabled(false);

    horizontalLayoutVictorySlct = new QHBoxLayout();
    horizontalLayoutVictorySlct->addWidget(comboBoxWinsCond);
    horizontalLayoutVictorySlct->addWidget(comboBoxWinsCondExt);

    checkBoxAllowNormalVicory = new QCheckBox(groupBoxWinsCond);
    checkBoxAllowNormalVicory->setEnabled(false);
    checkBoxAllowNormalVicory->setText(QApplication::translate("MapOptions", "Allow normal victory condition", 0, QApplication::UnicodeUTF8));

    checkBoxCompAlsoWins = new QCheckBox(groupBoxWinsCond);
    checkBoxCompAlsoWins->setEnabled(false);
    checkBoxCompAlsoWins->setText(QApplication::translate("MapOptions", "Comp also wins via Special VC", 0, QApplication::UnicodeUTF8));

    horizontalLayoutVictoryCheck = new QHBoxLayout();
    horizontalLayoutVictoryCheck->addWidget(checkBoxAllowNormalVicory);
    horizontalLayoutVictoryCheck->addWidget(checkBoxCompAlsoWins);

    verticalLayout3 = new QVBoxLayout(groupBoxWinsCond);
    verticalLayout3->addLayout(horizontalLayoutVictorySlct);
    verticalLayout3->addLayout(horizontalLayoutVictoryCheck);

    groupBoxLossCond = new QGroupBox(tabConditions);
    groupBoxLossCond->setTitle(QApplication::translate("MapOptions", "Loss Condition", 0, QApplication::UnicodeUTF8));

    comboBoxLossCond = new QComboBox(groupBoxLossCond);
    comboBoxLossCond->insertItems(0, QStringList()
         << QApplication::translate("MapOptions", "None", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MapOptions", "Lose a particuclar castle", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MapOptions", "Lose a particular hero", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MapOptions", "Run out of time", 0, QApplication::UnicodeUTF8)
    );

    horizontalLayoutLossCond = new QHBoxLayout();
    horizontalLayoutLossCond->addWidget(comboBoxLossCond);

    comboBoxLossCondExt = new QComboBox(groupBoxLossCond);
    comboBoxLossCondExt->insertItems(0, QStringList()
         << QApplication::translate("MapOptions", "None", 0, QApplication::UnicodeUTF8)
    );
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

    labelPlayer1 = new QLabel(groupBoxPlayers);
    labelPlayer2 = new QLabel(groupBoxPlayers);
    labelPlayer3 = new QLabel(groupBoxPlayers);
    labelPlayer1->setText(QApplication::translate("MapOptions", "player 1", 0, QApplication::UnicodeUTF8));
    labelPlayer2->setText(QApplication::translate("MapOptions", "player 2", 0, QApplication::UnicodeUTF8));
    labelPlayer3->setText(QApplication::translate("MapOptions", "player 3", 0, QApplication::UnicodeUTF8));

    horizontalLayoutPlayers = new QHBoxLayout();
    horizontalLayoutPlayers->addItem(horizontalSpacerPlayersLeft);
    horizontalLayoutPlayers->addWidget(labelPlayer1);
    horizontalLayoutPlayers->addWidget(labelPlayer2);
    horizontalLayoutPlayers->addWidget(labelPlayer3);
    horizontalLayoutPlayers->addItem(horizontalSpacerPlayersRight);

    checkBoxStartWithHero = new QCheckBox(groupBoxPlayers);
    checkBoxStartWithHero->setText(QApplication::translate("MapOptions", "Start with hero in each player's main castle", 0, QApplication::UnicodeUTF8));

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

    listWidgetRumors = new QListWidget(groupBoxRumors);
    listWidgetEvents = new QListWidget(groupBoxEvents);

    verticalLayout7 = new QVBoxLayout(groupBoxRumors);
    verticalLayout7->addWidget(listWidgetRumors);

    verticalLayout8 = new QVBoxLayout(groupBoxEvents);
    verticalLayout8->addWidget(listWidgetEvents);

    horizontalLayout6 = new QHBoxLayout(tabRumorsEvents);
    horizontalLayout6->addWidget(groupBoxRumors);
    horizontalLayout6->addWidget(groupBoxEvents);

    /* end */
    tabWidget = new QTabWidget(this);
    tabWidget->addTab(tabInfo, QApplication::translate("MapOptions", "General Info", 0, QApplication::UnicodeUTF8));
    tabWidget->addTab(tabConditions, QApplication::translate("MapOptions", "Wins/Loss Condition", 0, QApplication::UnicodeUTF8));
    tabWidget->addTab(tabRumorsEvents, QApplication::translate("MapOptions", "Rumors and Events", 0, QApplication::UnicodeUTF8));

    pushButtonOk = new QPushButton(this);
    pushButtonOk->setText(QApplication::translate("MapOptions", "Save", 0, QApplication::UnicodeUTF8));

    pushButtonCancel = new QPushButton(this);
    pushButtonCancel->setText(QApplication::translate("MapOptions", "Cancel", 0, QApplication::UnicodeUTF8));

    horizontalLayoutButton = new QHBoxLayout();
    horizontalLayoutButton->addWidget(pushButtonOk);
    horizontalSpacerButton = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayoutButton->addItem(horizontalSpacerButton);
    horizontalLayoutButton->addWidget(pushButtonCancel);

    verticalLayout2 = new QVBoxLayout(this);
    verticalLayout2->addWidget(tabWidget);
    verticalLayout2->addLayout(horizontalLayoutButton);

    QSize minSize = minimumSizeHint();

    resize(minSize);
    setMinimumSize(minSize);

    QObject::connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(pushButtonOk, SIGNAL(clicked()), this, SLOT(clickSave()));
}

void Form::MapOptions::clickSave(void)
{
    accept();
}
