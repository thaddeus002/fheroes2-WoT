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

#include "program.h"
#include "mapdata.h"
#include "dialogs.h"

void Dialog::MapOptions(MapData & map)
{
    Form::MapOptions form(map);
    form.exec();
}

Form::SelectMapSize::SelectMapSize()
{
    setObjectName(QString::fromUtf8("SelectMapSize"));

    vboxLayout = new QVBoxLayout(this);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));

    comboBoxSize = new QComboBox(this);
    comboBoxSize->setObjectName(QString::fromUtf8("comboBoxSize"));
    vboxLayout->addWidget(comboBoxSize);

    spacerItem = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vboxLayout->addItem(spacerItem);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem1);

    labelWidth = new QLabel(this);
    labelWidth->setObjectName(QString::fromUtf8("labelWidth"));
    labelWidth->setEnabled(true);
    hboxLayout->addWidget(labelWidth);

    spinBoxWidth = new QSpinBox(this);
    spinBoxWidth->setObjectName(QString::fromUtf8("spinBoxWidth"));
    spinBoxWidth->setMaximum(1024);
    spinBoxWidth->setMinimum(36);
    spinBoxWidth->setSingleStep(2);
    hboxLayout->addWidget(spinBoxWidth);

    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem2);
    vboxLayout->addLayout(hboxLayout);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));

    spacerItem3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout1->addItem(spacerItem3);

    labelHeight = new QLabel(this);
    labelHeight->setObjectName(QString::fromUtf8("labelHeight"));
    hboxLayout1->addWidget(labelHeight);

    spinBoxHeight = new QSpinBox(this);
    spinBoxHeight->setObjectName(QString::fromUtf8("spinBoxHeight"));
    spinBoxHeight->setMaximum(1024);
    spinBoxHeight->setMinimum(36);
    spinBoxHeight->setSingleStep(2);
    hboxLayout1->addWidget(spinBoxHeight);

    spacerItem4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout1->addItem(spacerItem4);
    vboxLayout->addLayout(hboxLayout1);

    spacerItem5 = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vboxLayout->addItem(spacerItem5);

    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(0);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));

    spacerItem6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout2->addItem(spacerItem6);

    pushButtonOk = new QPushButton(this);
    pushButtonOk->setObjectName(QString::fromUtf8("pushButtonOk"));
    hboxLayout2->addWidget(pushButtonOk);

    spacerItem7 = new QSpacerItem(61, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout2->addItem(spacerItem7);

    pushButtonExpert = new QPushButton(this);
    pushButtonExpert->setObjectName(QString::fromUtf8("pushButtonExpert"));
    hboxLayout2->addWidget(pushButtonExpert);

    spacerItem8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout2->addItem(spacerItem8);
    vboxLayout->addLayout(hboxLayout2);

    labelWidth->setBuddy(spinBoxWidth);
    labelHeight->setBuddy(spinBoxHeight);

    setWindowTitle(QApplication::translate("SelectMapSize", "Select Size", 0, QApplication::UnicodeUTF8));
    comboBoxSize->clear();
    comboBoxSize->addItem(QApplication::translate("SelectMapSize", "Small (36x36)", 0, QApplication::UnicodeUTF8));
    comboBoxSize->addItem(QApplication::translate("SelectMapSize", "Medium (72x72)", 0, QApplication::UnicodeUTF8));
    comboBoxSize->addItem(QApplication::translate("SelectMapSize", "Large (108x108)", 0, QApplication::UnicodeUTF8));
    comboBoxSize->addItem(QApplication::translate("SelectMapSize", "Extra Large (144x144)", 0, QApplication::UnicodeUTF8));
    labelWidth->setText(QApplication::translate("SelectMapSize", "width", 0, QApplication::UnicodeUTF8));
    labelHeight->setText(QApplication::translate("SelectMapSize", "height", 0, QApplication::UnicodeUTF8));
    pushButtonOk->setText(QApplication::translate("SelectMapSize", "Ok", 0, QApplication::UnicodeUTF8));
    pushButtonExpert->setText(QApplication::translate("SelectMapSize", "Expert", 0, QApplication::UnicodeUTF8));
    labelWidth->setVisible(false);
    labelHeight->setVisible(false);
    spinBoxWidth->setVisible(false);
    spinBoxHeight->setVisible(false);
    comboBoxSize->setCurrentIndex(1);

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
    setObjectName(QString::fromUtf8("DialogSelectDataFile"));

    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

    labelHeader = new QLabel(this);
    labelHeader->setObjectName(QString::fromUtf8("labelHeader"));
    labelHeader->setAlignment(Qt::AlignCenter);
    verticalLayout->addWidget(labelHeader);

    horizontalLayout2 = new QHBoxLayout();
    horizontalLayout2->setObjectName(QString::fromUtf8("horizontalLayout2"));

    labelImage = new QLabel(this);
    labelImage->setObjectName(QString::fromUtf8("labelImage"));
    labelImage->setPixmap(QPixmap(QString::fromUtf8(":/images/cancel.png")));
    labelImage->setScaledContents(false);
    horizontalLayout2->addWidget(labelImage);

    labelBody = new QLabel(this);
    labelBody->setObjectName(QString::fromUtf8("label"));
    labelBody->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    horizontalLayout2->addWidget(labelBody);
    verticalLayout->addLayout(horizontalLayout2);

    verticalSpacer = new QSpacerItem(288, 6, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    horizontalLayout1 = new QHBoxLayout();
    horizontalLayout1->setObjectName(QString::fromUtf8("horizontalLayout1"));

    pushButtonSelect = new QPushButton(this);
    pushButtonSelect->setObjectName(QString::fromUtf8("pushButtonSelect"));
    horizontalLayout1->addWidget(pushButtonSelect);

    pushButtonSave = new QPushButton(this);
    pushButtonSave->setObjectName(QString::fromUtf8("pushButtonSave"));
    pushButtonSave->setEnabled(false);
    horizontalLayout1->addWidget(pushButtonSave);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout1->addItem(horizontalSpacer);

    pushButtonExit = new QPushButton(this);
    pushButtonExit->setObjectName(QString::fromUtf8("pushButtonExit"));
    horizontalLayout1->addWidget(pushButtonExit);
    verticalLayout->addLayout(horizontalLayout1);

    setWindowTitle(QApplication::translate("DialogSelectDataFile", "Warning", 0, QApplication::UnicodeUTF8));

    pushButtonSelect->setText(QApplication::translate("DialogSelectDataFile", "Select", 0, QApplication::UnicodeUTF8));
    pushButtonSave->setText(QApplication::translate("DialogSelectDataFile", "Save", 0, QApplication::UnicodeUTF8));
    pushButtonExit->setText(QApplication::translate("DialogSelectDataFile", "Exit", 0, QApplication::UnicodeUTF8));
    labelHeader->setText(QApplication::translate("DialogSelectDataFile", "Cannot find resource file: ", 0, QApplication::UnicodeUTF8) + dataFile);
    labelBody->setText(QApplication::translate("DialogSelectDataFile", "Scan directories: ", 0, QApplication::UnicodeUTF8) + "\n" + dirList.join("\n"));

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
    setObjectName(QString::fromUtf8("DialogMapOptions"));

    verticalLayout2 = new QVBoxLayout(this);
    verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));

    tabWidget = new QTabWidget(this);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));

    tabInfo = new QWidget();
    tabInfo->setObjectName(QString::fromUtf8("tabInfo"));

    verticalLayout = new QVBoxLayout(tabInfo);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

    labelName = new QLabel(tabInfo);
    labelName->setObjectName(QString::fromUtf8("labelName"));
    labelName->setAlignment(Qt::AlignCenter);
    verticalLayout->addWidget(labelName);

    lineEditName = new QLineEdit(tabInfo);
    lineEditName->setObjectName(QString::fromUtf8("lineEditName"));
    verticalLayout->addWidget(lineEditName);

    labelDifficulty = new QLabel(tabInfo);
    labelDifficulty->setObjectName(QString::fromUtf8("labelDifficulty"));
    labelDifficulty->setAlignment(Qt::AlignCenter);
    verticalLayout->addWidget(labelDifficulty);

    comboBoxDifficulty = new QComboBox(tabInfo);
    comboBoxDifficulty->setObjectName(QString::fromUtf8("comboBoxDifficulty"));
    verticalLayout->addWidget(comboBoxDifficulty);

    labelDescription = new QLabel(tabInfo);
    labelDescription->setObjectName(QString::fromUtf8("labelDescription"));
    labelDescription->setAlignment(Qt::AlignCenter);
    verticalLayout->addWidget(labelDescription);

    plainTextEditDescription = new QPlainTextEdit(tabInfo);
    plainTextEditDescription->setObjectName(QString::fromUtf8("plainTextEditDescription"));
    verticalLayout->addWidget(plainTextEditDescription);

    tabWidget->addTab(tabInfo, "Info");

    tabCondition = new QWidget();
    tabCondition->setObjectName(QString::fromUtf8("tabCondition"));

    groupBoxWinsCond = new QGroupBox(tabCondition);
    groupBoxWinsCond->setObjectName(QString::fromUtf8("groupBoxWinsCond"));
    groupBoxWinsCond->setGeometry(QRect(10, 10, 421, 141));

    comboBoxWinsCond = new QComboBox(groupBoxWinsCond);
    comboBoxWinsCond->setObjectName(QString::fromUtf8("comboBoxWinsCond"));
    comboBoxWinsCond->setGeometry(QRect(10, 30, 141, 27));

    groupBox = new QGroupBox(tabCondition);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(10, 190, 421, 81));

    comboBoxLossCond = new QComboBox(groupBox);
    comboBoxLossCond->setObjectName(QString::fromUtf8("comboBoxLossCond"));
    comboBoxLossCond->setGeometry(QRect(10, 30, 141, 27));

    tabWidget->addTab(tabCondition, "Conditions");
    verticalLayout2->addWidget(tabWidget);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

    pushButtonOk = new QPushButton(this);
    pushButtonOk->setObjectName(QString::fromUtf8("pushButtonOk"));
    horizontalLayout->addWidget(pushButtonOk);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    pushButtonCancel = new QPushButton(this);
    pushButtonCancel->setObjectName(QString::fromUtf8("pushButtonCancel"));
    horizontalLayout->addWidget(pushButtonCancel);
    verticalLayout2->addLayout(horizontalLayout);

    resize(470, 394);
    setMinimumSize(QSize(470, 394));
    setMaximumSize(QSize(470, 394));

    setWindowTitle(QApplication::translate("DialogMapOptions", "Dialog", 0, QApplication::UnicodeUTF8));
    labelName->setText(QApplication::translate("DialogMapOptions", "Map Name:", 0, QApplication::UnicodeUTF8));
    labelDifficulty->setText(QApplication::translate("DialogMapOptions", "Map Difficulty:", 0, QApplication::UnicodeUTF8));
    comboBoxDifficulty->clear();
    comboBoxDifficulty->insertItems(0, QStringList()
         << QApplication::translate("DialogMapOptions", "Easy", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Normal", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Tough", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Expert", 0, QApplication::UnicodeUTF8)
    );
    labelDescription->setText(QApplication::translate("DialogMapOptions", "Map Description:", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tabInfo), QApplication::translate("DialogMapOptions", "Page", 0, QApplication::UnicodeUTF8));
    groupBoxWinsCond->setTitle(QApplication::translate("DialogMapOptions", "Victory Condition", 0, QApplication::UnicodeUTF8));
    comboBoxWinsCond->clear();
    comboBoxWinsCond->insertItems(0, QStringList()
         << QApplication::translate("DialogMapOptions", "None", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Capture a particular castle", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Defeat a particular hero", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Find a particular artifact", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "One side defeats another", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Accumulate gold", 0, QApplication::UnicodeUTF8)
    );
    groupBox->setTitle(QApplication::translate("DialogMapOptions", "Loss Condition", 0, QApplication::UnicodeUTF8));
    comboBoxLossCond->clear();
    comboBoxLossCond->insertItems(0, QStringList()
         << QApplication::translate("DialogMapOptions", "None", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Lose a particuclar castle", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Lose a particular hero", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMapOptions", "Run out of time", 0, QApplication::UnicodeUTF8)
    );
    tabWidget->setTabText(tabWidget->indexOf(tabCondition), QApplication::translate("DialogMapOptions", "Page", 0, QApplication::UnicodeUTF8));
    pushButtonOk->setText(QApplication::translate("DialogMapOptions", "Ok", 0, QApplication::UnicodeUTF8));
    pushButtonCancel->setText(QApplication::translate("DialogMapOptions", "Cancel", 0, QApplication::UnicodeUTF8));

    tabWidget->setCurrentIndex(0);
    comboBoxDifficulty->setCurrentIndex(1);

    QObject::connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(pushButtonOk, SIGNAL(clicked()), this, SLOT(accept()));
}
