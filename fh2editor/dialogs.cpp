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
    labelHeader->setObjectName(QString::fromUtf8("labelHeader"));
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

Form::MapOptions::MapOptions(MapData & map)
{
    setWindowTitle(QApplication::translate("DialogMapOptions", "Dialog", 0, QApplication::UnicodeUTF8));

    verticalLayout2 = new QVBoxLayout(this);
    tabWidget = new QTabWidget(this);
    tabInfo = new QWidget();
    verticalLayout = new QVBoxLayout(tabInfo);

    labelName = new QLabel(tabInfo);
    labelName->setAlignment(Qt::AlignCenter);
    labelName->setText(QApplication::translate("DialogMapOptions", "Map Name:", 0, QApplication::UnicodeUTF8));
    verticalLayout->addWidget(labelName);

    lineEditName = new QLineEdit(tabInfo);
    verticalLayout->addWidget(lineEditName);

    labelDifficulty = new QLabel(tabInfo);
    labelDifficulty->setAlignment(Qt::AlignCenter);
    labelDifficulty->setText(QApplication::translate("DialogMapOptions", "Map Difficulty:", 0, QApplication::UnicodeUTF8));
    verticalLayout->addWidget(labelDifficulty);

    comboBoxDifficulty = new QComboBox(tabInfo);
    comboBoxDifficulty->insertItems(0, QStringList()
         << QApplication::translate("DialogMapOptions", "Easy", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Normal", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Tough", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Expert", 0, QApplication::UnicodeUTF8)
    );
    comboBoxDifficulty->setCurrentIndex(1);
    verticalLayout->addWidget(comboBoxDifficulty);

    labelDescription = new QLabel(tabInfo);
    labelDescription->setAlignment(Qt::AlignCenter);
    labelDescription->setText(QApplication::translate("DialogMapOptions", "Map Description:", 0, QApplication::UnicodeUTF8));
    verticalLayout->addWidget(labelDescription);

    plainTextEditDescription = new QPlainTextEdit(tabInfo);
    verticalLayout->addWidget(plainTextEditDescription);

    tabWidget->addTab(tabInfo, "Info");

    tabCondition = new QWidget();

    groupBoxWinsCond = new QGroupBox(tabCondition);
    groupBoxWinsCond->setGeometry(QRect(10, 10, 421, 141));
    groupBoxWinsCond->setTitle(QApplication::translate("DialogMapOptions", "Victory Condition", 0, QApplication::UnicodeUTF8));

    comboBoxWinsCond = new QComboBox(groupBoxWinsCond);
    comboBoxWinsCond->setGeometry(QRect(10, 30, 141, 27));
    comboBoxWinsCond->insertItems(0, QStringList()
         << QApplication::translate("DialogMapOptions", "None", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Capture a particular castle", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Defeat a particular hero", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Find a particular artifact", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "One side defeats another", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Accumulate gold", 0, QApplication::UnicodeUTF8)
    );

    groupBox = new QGroupBox(tabCondition);
    groupBox->setGeometry(QRect(10, 190, 421, 81));
    groupBox->setTitle(QApplication::translate("DialogMapOptions", "Loss Condition", 0, QApplication::UnicodeUTF8));

    comboBoxLossCond = new QComboBox(groupBox);
    comboBoxLossCond->setGeometry(QRect(10, 30, 141, 27));
    comboBoxLossCond->insertItems(0, QStringList()
         << QApplication::translate("DialogMapOptions", "None", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Lose a particuclar castle", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Lose a particular hero", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Run out of time", 0, QApplication::UnicodeUTF8)
    );

    tabWidget->addTab(tabCondition, "Conditions");
    verticalLayout2->addWidget(tabWidget);

    horizontalLayout = new QHBoxLayout();

    pushButtonOk = new QPushButton(this);
    pushButtonOk->setText(QApplication::translate("DialogMapOptions", "Ok", 0, QApplication::UnicodeUTF8));
    horizontalLayout->addWidget(pushButtonOk);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    pushButtonCancel = new QPushButton(this);
    pushButtonCancel->setText(QApplication::translate("DialogMapOptions", "Cancel", 0, QApplication::UnicodeUTF8));
    horizontalLayout->addWidget(pushButtonCancel);
    verticalLayout2->addLayout(horizontalLayout);

    resize(470, 394);
    setMinimumSize(QSize(470, 394));
    setMaximumSize(QSize(470, 394));

    tabWidget->setCurrentIndex(0);
    tabWidget->setTabText(tabWidget->indexOf(tabInfo), QApplication::translate("DialogMapOptions", "Page", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tabCondition), QApplication::translate("DialogMapOptions", "Page", 0, QApplication::UnicodeUTF8));

    QObject::connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(pushButtonOk, SIGNAL(clicked()), this, SLOT(accept()));
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
    }
};

Form::SelectImageTab::SelectImageTab(const QDomElement & groupElem, const QString & dataFolder, EditorTheme & theme)
{
    verticalLayout = new QVBoxLayout(this);
    listWidget = new QListWidget(this);
    verticalLayout->addWidget(listWidget);
    listWidget->setIconSize(QSize(48, 48));

    Editor::MyXML templateObjects(Resource::FindFile(dataFolder, "template.xml"), "template");
    Editor::MyXML objectsElem(Resource::FindFile(dataFolder, groupElem.attribute("file")), "objects");
    QString icn = objectsElem.attribute("icn");

    if(! objectsElem.isNull())
    {
        // parse element: object
        QDomNodeList objectsList = objectsElem.elementsByTagName("object");

        for(int pos2 = 0; pos2 < objectsList.size(); ++pos2)
        {
            CompositeObject obj(icn, objectsList.item(pos2).toElement());

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
                CompositeObject obj(icn, objElem, startIndex);

                // override tags
                if(tmplElem.hasAttribute("name"))
                    obj.name = tmplElem.attribute("name");

            	if(obj.isValid())
		    listWidget->addItem(new SelectImageItem(obj, theme));
            }
        }
    }

    QObject::connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
}

bool Form::SelectImageTab::isSelected(void) const
{
    return listWidget->selectedItems().size();
}

void Form::SelectImageTab::selectionChanged(void)
{
    QStackedWidget* stackWidget = qobject_cast<QStackedWidget*>(parent());

    if(stackWidget)
    {
	QTabWidget* tabWidget = qobject_cast<QTabWidget*>(stackWidget->parent());

	if(tabWidget)
	{
	    Form::SelectImage* form = qobject_cast<Form::SelectImage*>(tabWidget->parent());

	    if(form)
		form->pushButtonSelect->setEnabled(isSelected());
	}
    }
}

Form::SelectImage::SelectImage(EditorTheme & theme)
{
    setObjectName(QString::fromUtf8("SelectImage"));
    setWindowTitle(QApplication::translate("SelectImage", "Dialog", 0, QApplication::UnicodeUTF8));

    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

    tabWidget = new QTabWidget(this);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));

    const QString dataFolder("objects");
    Editor::MyXML groupsElem(Resource::FindFile(dataFolder, "groups.xml"), "groups");

    if(! groupsElem.isNull())
    {
        QDomNodeList groupsList = groupsElem.elementsByTagName("group");

        for(int pos1 = 0; pos1 < groupsList.size(); ++pos1)
        {
            QDomElement groupElem = groupsList.item(pos1).toElement();
            QString name = groupElem.attribute("name");

            if(! name.isEmpty())
	    {
		tabWidget->addTab(new SelectImageTab(groupElem, dataFolder, theme), name);
	    }
	}

	tabWidget->setCurrentIndex(0);
    }

    verticalLayout->addWidget(tabWidget);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

    pushButtonSelect = new QPushButton(this);
    pushButtonSelect->setObjectName(QString::fromUtf8("pushButtonSelect"));
    pushButtonSelect->setText(QApplication::translate("SelectImage", "Select", 0, QApplication::UnicodeUTF8));
    pushButtonSelect->setEnabled(false);
    horizontalLayout->addWidget(pushButtonSelect);

    horizontalSpacer = new QSpacerItem(268, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    pushButtonClose = new QPushButton(this);
    pushButtonClose->setObjectName(QString::fromUtf8("pushButtonClose"));
    pushButtonClose->setText(QApplication::translate("SelectImage", "Close", 0, QApplication::UnicodeUTF8));
    horizontalLayout->addWidget(pushButtonClose);
    verticalLayout->addLayout(horizontalLayout);

    resize(540, 410);

    QObject::connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSwitched(int)));
    QObject::connect(pushButtonClose, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(pushButtonSelect, SIGNAL(clicked()), this, SLOT(accept()));
}

void Form::SelectImage::tabSwitched(int num)
{
    SelectImageTab* tab = qobject_cast<SelectImageTab*>(tabWidget->widget(num));
    pushButtonSelect->setEnabled(tab && tab->isSelected());
}
