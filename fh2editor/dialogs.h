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

#ifndef _EDITOR_DIALOGS_H_
#define _EDITOR_DIALOGS_H_

#include <QDialog>
#include <QSize>
#include "engine.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QComboBox;
class QSpacerItem;
class QHBoxLayout;
class QLabel;
class QSpinBox;
class QPushButton;
class QTabWidget;
class QWidget;
class QLineEdit;
class QPlainTextEdit;
class QGroupBox;
class QSpacerItem;
class QTabWidget;
class QListWidget;
class QListWidgetItem;
class QDomElement;
QT_END_NAMESPACE

class MapData;
class EditorTheme;
class CompositeObject;

namespace Dialog
{
    void	MapOptions(MapData &);
}

namespace Form
{
    class SelectMapSize : public QDialog
    {
	Q_OBJECT

    public slots:
	void			clickExpert(void);
	void			clickOk(void);

    public:
	SelectMapSize();

	QVBoxLayout*		vboxLayout;
	QComboBox*		comboBoxSize;
	QSpacerItem*		spacerItem;
	QHBoxLayout*		hboxLayout;
	QSpacerItem*		spacerItem1;
	QLabel*			labelWidth;
	QSpinBox*		spinBoxWidth;
	QSpacerItem*		spacerItem2;
	QHBoxLayout*		hboxLayout1;
	QSpacerItem*		spacerItem3;
	QLabel*			labelHeight;
	QSpinBox*		spinBoxHeight;
	QSpacerItem*		spacerItem4;
	QSpacerItem*		spacerItem5;
	QHBoxLayout*		hboxLayout2;
	QSpacerItem*		spacerItem6;
	QPushButton*		pushButtonOk;
	QSpacerItem*		spacerItem7;
	QPushButton*		pushButtonExpert;
	QSpacerItem*		spacerItem8;

	QSize			result;
    };    

    class SelectDataFile : public QDialog
    {
	Q_OBJECT

    public slots:
	void			clickSelect(void);

    public:
	SelectDataFile(const QString &, const QStringList &);

	QVBoxLayout*		verticalLayout;
	QLabel*			labelHeader;
	QHBoxLayout*		horizontalLayout2;
	QLabel*			labelImage;
	QLabel*			labelBody;
	QSpacerItem*		verticalSpacer;
	QHBoxLayout*		horizontalLayout1;
	QPushButton*		pushButtonSelect;
	QPushButton*		pushButtonSave;
	QSpacerItem*		horizontalSpacer;
	QPushButton*		pushButtonExit;

	QString			result;
    };

    class MapOptions : public QDialog
    {
	Q_OBJECT

    public:
	MapOptions(MapData &);

	QVBoxLayout*		verticalLayout2;
	QTabWidget*		tabWidget;
	QWidget*		tabInfo;
	QVBoxLayout*		verticalLayout;
	QLabel*			labelName;
	QLineEdit*		lineEditName;
	QLabel*			labelDifficulty;
	QComboBox*		comboBoxDifficulty;
	QLabel*			labelDescription;
	QPlainTextEdit*		plainTextEditDescription;
	QWidget*		tabCondition;
	QGroupBox*		groupBoxWinsCond;
	QComboBox*		comboBoxWinsCond;
        QGroupBox*		groupBox;
	QComboBox*		comboBoxLossCond;
	QHBoxLayout*		horizontalLayout;
	QPushButton*		pushButtonOk;
	QSpacerItem*		horizontalSpacer;
	QPushButton*		pushButtonCancel;
    };

    class SelectImage : public QDialog
    {
        Q_OBJECT

    public:
        SelectImage(EditorTheme &);

        QVBoxLayout*		verticalLayout;
        QTabWidget*		tabWidget;
        QHBoxLayout*		horizontalLayout;
        QPushButton*		pushButtonSelect;
        QSpacerItem*		horizontalSpacer;
        QPushButton*		pushButtonClose;

	CompositeObject		result;

    protected slots:
	void			tabSwitched(int);
	void			clickSelect(void);
	void			accept(QListWidgetItem*);
	void			selectionChanged(void);
    };

    class SelectImageTab : public QWidget
    {
        Q_OBJECT

    public:
	SelectImageTab(const QDomElement &, const QString &, EditorTheme &);

	QVBoxLayout*        	verticalLayout;
	QListWidget*        	listWidget;

	bool 			isSelected(void) const;
    };
}

#endif
