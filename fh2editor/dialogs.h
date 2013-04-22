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
#include <QPair>
#include <QLabel>
#include <QVariant>
#include <QListWidget>
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
class QCheckBox;
class QListWidget;
class QListWidgetItem;
class QDomElement;
QT_END_NAMESPACE

class MapData;
class EditorTheme;
class CompositeObject;

QVariant comboBoxCurrentData(const QComboBox*);

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
	static int		lastNumTab;

    protected slots:
	void			tabSwitched(int);
	void			clickSelect(void);
	void			accept(QListWidgetItem*);
	void			selectionChanged(void);
    };

    class SelectImageTab : public QDialog
    {
        Q_OBJECT

    public:
	SelectImageTab(const QDomElement &, const QString &, EditorTheme &);

	QVBoxLayout*        	verticalLayout;
	QListWidget*        	listWidget;

	bool 			isSelected(void) const;
    };

    class MapOptions;

    class ItemsList : public QListWidget
    {
        Q_OBJECT

    public:
	QAction*		addItemAct;
	QAction*		editItemAct;
	QAction*		delItemAct;
	MapOptions*		parentObj;

	ItemsList(MapOptions*);

    signals:
	void			mousePressed(void);

    protected slots:
	virtual void		addItem(void) = 0;
	virtual void		editItem(QListWidgetItem*) = 0;
	void			editCurrentItem(void);
	void			deleteCurrentItem(void);

    protected:
	void			mousePressEvent(QMouseEvent*);
    };

    class RumorsList : public ItemsList
    {
	Q_OBJECT

    public:
	RumorsList(MapOptions*);

    protected slots:
	void			addItem(void);
	void			editItem(QListWidgetItem*);
    };

    class EventsList : public ItemsList
    {
	Q_OBJECT

    public:
	EventsList(MapOptions*);

    protected slots:
	void			addItem(void);
	void			editItem(QListWidgetItem*);
    };

    class PlayerStatus : public QLabel
    {
	Q_OBJECT

	int			col;
	int			stat;
	EditorTheme &		theme;

    public:
	PlayerStatus(int, int, EditorTheme &, QWidget*);

	int			color(void) const { return col; }
	int			status(void) const { return stat % 4; }

    signals:
        void                    mousePressed(void);

    protected:
	void			updatePlayers(void);
	void			mousePressEvent(QMouseEvent*);
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
	QWidget*		tabConditions;
	QVBoxLayout*		verticalLayout6;
	QGroupBox*		groupBoxWinsCond;
	QVBoxLayout*		verticalLayout3;
	QHBoxLayout*		horizontalLayoutVictorySlct;
	QComboBox*		comboBoxWinsCond;
	QComboBox*		comboBoxWinsCondExt;
	QHBoxLayout*		horizontalLayoutVictoryCheck;
	QCheckBox*		checkBoxAllowNormalVictory;
	QCheckBox*		checkBoxCompAlsoWins;
	QGroupBox*		groupBoxLossCond;
	QVBoxLayout*		verticalLayout4;
	QHBoxLayout*		horizontalLayoutLossCond;
	QComboBox*		comboBoxLossCond;
	QComboBox*		comboBoxLossCondExt;
	QGroupBox*		groupBoxPlayers;
	QVBoxLayout*		verticalLayout5;
	QHBoxLayout*		horizontalLayoutPlayers;
	QSpacerItem*		horizontalSpacerPlayersLeft;
	QVector<PlayerStatus*>	labelPlayers;
	QSpacerItem*		horizontalSpacerPlayersRight;
	QCheckBox*		checkBoxStartWithHero;
	QSpacerItem*		verticalSpacerPage2;
	QWidget*		tabRumorsEvents;
	QHBoxLayout*		horizontalLayout6;
	QGroupBox*		groupBoxRumors;
	QVBoxLayout*		verticalLayout7;
	RumorsList*		listWidgetRumors;
	QGroupBox*		groupBoxEvents;
        QVBoxLayout*		verticalLayout8;
        EventsList*		listWidgetEvents;
        QHBoxLayout*		horizontalLayoutButton;
	QWidget*		tabAuthorsLicense;
	QVBoxLayout*		verticalLayout9;
	QLabel*			labelAuthors;
	QLabel*			labelLicense;
	QPlainTextEdit*		plainTextEditAuthors;
	QPlainTextEdit*		plainTextEditLicense;
	QPushButton*		pushButtonSave;
        QSpacerItem*		horizontalSpacerButton;
	QPushButton*		pushButtonCancel;

	ListStringPos		winsCondHeroList;
	ListStringPos		winsCondTownList;
	QList<QString>		winsCondSideList;
	ListStringPos		winsCondArtifactList;
	ListStringPos		lossCondHeroList;
	ListStringPos		lossCondTownList;

	MapData &		data;

    protected slots:
	void			winsConditionsSelected(int);
	void			lossConditionsSelected(int);
	void			setEnableSaveButton(void);
	void			setEnableSaveButton(const QString &);
	void			setConditionsBoxesMapValues(const MapData &);
    };

    class RumorDialog : public QDialog
    {
        Q_OBJECT

    public:
        RumorDialog(const QString & = QString());

        QVBoxLayout*		verticalLayout;
        QHBoxLayout*		horizontalLayout;
        QPushButton*		pushButtonOk;
        QSpacerItem*		horizontalSpacer;
        QPushButton*		pushButtonCancel;
	QPlainTextEdit*		plainText;

    protected slots:
	void			enableButtonOk(void);
    };

    class PlayerAllow : public QLabel
    {
	Q_OBJECT

	int			col;
	bool			stat;
	EditorTheme &		theme;

    public:
	PlayerAllow(int, bool, EditorTheme &, QWidget*);

	int			color(void) const { return col; }
	bool			allow(void) const { return stat; }

    signals:
        void                    mousePressed(void);

    protected:
	void			updatePlayers(void);
	void			mousePressEvent(QMouseEvent*);
    };

    class DayEventDialog : public QDialog
    {
	Q_OBJECT

    public:
	DayEventDialog(const DayEvent &, int, EditorTheme &);

	QVBoxLayout*		verticalLayout2;
	QTabWidget*		tabWidget;
	QWidget*		tabDay;
	QVBoxLayout*		verticalLayout;
	QHBoxLayout*		horizontalLayout;
	QLabel*			labelDayFirst;
	QSpinBox*		spinBoxDayFirst;
	QHBoxLayout*		horizontalLayout2;
	QLabel*			labelSubsequent;
	QComboBox*		comboBoxSubsequent;
	QGroupBox*		groupBoxAllowedColors;
	QVBoxLayout*		verticalLayout3;
	QHBoxLayout*		horizontalLayout4;
	QSpacerItem*		horizontalSpacerPlayersLeft;
	QSpacerItem*		horizontalSpacerPlayersRight;
	QVector<PlayerAllow*>	labelPlayers;
	QCheckBox*		checkBoxAllowComp;
	QWidget*		tabResource;
	QVBoxLayout*		verticalLayout4;
	QHBoxLayout*		horizontalLayout5;
	QLabel*			labelResWood;
	QSpinBox*		spinBoxResWood;
	QSpacerItem*		horizontalSpacer2;
	QLabel*			labelResSulfur;
	QSpinBox*		spinBoxResSulfur;
        QHBoxLayout*		horizontalLayout6;
	QLabel*			labelResMercury;
	QSpinBox*		spinBoxResMercury;
	QSpacerItem*		horizontalSpacer3;
	QLabel*			labelResCrystal;
	QSpinBox*		spinBoxResCrystal;
	QHBoxLayout*		horizontalLayout7;
	QLabel*			labelResOre;
	QSpinBox*		spinBoxResOre;
	QSpacerItem*		horizontalSpacer4;
	QLabel*			labelResGems;
	QSpinBox*		spinBoxResGems;
	QHBoxLayout*		horizontalLayout8;
	QSpacerItem*		horizontalSpacer5;
	QLabel*			labelResGold;
	QSpinBox*		spinBoxResGold;
	QSpacerItem*		horizontalSpacer6;
	QWidget*		tabMessage;
	QVBoxLayout*		verticalLayout5;
	QPlainTextEdit*		plainTextMessage;
	QHBoxLayout*		horizontalLayout3;
	QPushButton*		pushButtonOk;
	QSpacerItem*		horizontalSpacer;
	QPushButton*		pushButtonCancel;

	DayEvent		result(void) const;

    protected slots:
	void			setEnableOKButton(void);
	void			setEnableOKButton(const QString &);
    };
}

#endif
