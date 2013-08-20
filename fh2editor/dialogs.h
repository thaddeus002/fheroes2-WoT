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
#include <QGroupBox>
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
class QGraphicsScene;
class QDialogButtonBox;
QT_END_NAMESPACE

class MapData;
class MapTiles;
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

    class TabWidgetWest : public QTabWidget
    {
        Q_OBJECT

    public:
	TabWidgetWest(QWidget*);
    };

    class SelectImageObject : public QDialog
    {
        Q_OBJECT

    public:
        SelectImageObject();

        QVBoxLayout*		verticalLayout;
        TabWidgetWest*		tabWidget;
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
	void			saveSettings(void);
    };

    class SelectImageTab : public QDialog
    {
        Q_OBJECT

    public:
	SelectImageTab(const QDomElement &, const QString &);

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

	ItemsList(QWidget*);
	QStringList		results(void) const;

	virtual void		addItem(void) = 0;
	virtual void		editItem(QListWidgetItem*) = 0;
	virtual void		checkLimit(void) {}
	void			setCurrentItem(int);

    signals:
	void			mousePressed(void);
	void			listChanged(void);

    protected slots:
	void			addNewItem(void);
	void			editCurrentItem(void);
	void			deleteCurrentItem(void);
	void			slotCheckLimit(void);

    protected:
	void			mousePressEvent(QMouseEvent*);
	virtual void		createMenuItems(QMenu*);
    };

    class RumorsList : public ItemsList
    {
	Q_OBJECT

    public:
	RumorsList(QWidget*);

	TavernRumors		results(void) const;

	void			addItem(void);
	void			editItem(QListWidgetItem*);
    };

    class DayEventsList : public ItemsList
    {
	Q_OBJECT

	int                     kingdomColors;

     public:
	DayEventsList(int, QWidget*);

	DayEvents		results(void) const;

	void			addItem(void);
	void			editItem(QListWidgetItem*);
    };

    class PlayerStatus : public QLabel
    {
	Q_OBJECT

	int			col;
	int			stat;

    public:
	PlayerStatus(int, int, QWidget*);

	int			color(void) const { return col; }
	int			status(void) const { return stat % 4; }

    signals:
        void                    mousePressed(void);

    protected:
	void			updatePlayers(void);
	void			mousePressEvent(QMouseEvent*);
    };

    class PlayerAllow : public QLabel
    {
	Q_OBJECT

	int			col;
	bool			stat;

    public:
	PlayerAllow(int, bool, QWidget*);

	int			color(void) const { return col; }
	bool			allow(void) const { return stat; }

    signals:
        void                    mousePressed(void);

    protected:
	void			updatePlayers(void);
	void			mousePressEvent(QMouseEvent*);
    };

    class AccessGroup : public QGroupBox
    {
        Q_OBJECT

    public:
	AccessGroup(QWidget*, int, int);

	QVBoxLayout*		verticalLayoutAllowCols;
	QHBoxLayout*		horizontalLayoutPlayers;
	QSpacerItem*		horizontalSpacerPlayersLeft;
	QSpacerItem*		horizontalSpacerPlayersRight;
	QVector<PlayerAllow*>	labelPlayers;
	QCheckBox*		checkBoxAllowComp;
	QCheckBox*		checkBoxCancelAfterFirstVisit;

	void			setCancelAfterFirstVisit(bool);
	void			setAllowComputer(bool);

	int			colors(void) const;
	bool			allowComputer(void) const;
	bool			cancelAfterFirstVisit(void) const;

    signals:
        void                    formChanged(void);

    protected slots:
        void                    setFormChanged(void);
    };

    class AccessDialog : public QDialog
    {
        Q_OBJECT

    public:
        AccessDialog(int, int, bool, bool);

        AccessGroup*		accessGroup;
        QVBoxLayout*		formLayout;
        QHBoxLayout*		buttonsLayout;
        QPushButton*		pushButtonOk;
        QSpacerItem*		horizontalSpacer;
        QPushButton*		pushButtonCancel;

        AccessResult           	result(void) const;

    protected slots:
	void			enableButtonOk(void);
    };

    class ArtifactGroup : public QGroupBox
    {
        Q_OBJECT

    public:
	ArtifactGroup(QWidget*, int = Artifact::None);

        QLabel*                 labelArtifact;
        QComboBox*              comboBoxArtifact;
        QHBoxLayout*		horizontalLayout;

        int                     result(void) const;

    signals:
        void                    formChanged(void);

    protected slots:
        void                    changeLabelArtifact(int);
        void                    setFormChanged(void);
    };

    class ArtifactDialog : public QDialog
    {
        Q_OBJECT

    public:
        ArtifactDialog(int = Artifact::None);

        ArtifactGroup*		artifactGroup;
        QVBoxLayout*		formLayout;
        QHBoxLayout*		buttonsLayout;
        QPushButton*		pushButtonOk;
        QSpacerItem*		horizontalSpacer;
        QPushButton*		pushButtonCancel;

        int               	artifact(void) const;

    protected slots:
	void			enableButtonOk(void);
    };

    class ResourcesGroup : public QGroupBox
    {
        Q_OBJECT

    public:
	ResourcesGroup(QWidget*, const Resources &);

        QHBoxLayout*            horizontalLayoutWoodSulfur;
        QLabel*                 labelResWood;
        QSpinBox*               spinBoxResWood;
        QSpacerItem*            horizontalSpacerWoodSulfur;
        QLabel*                 labelResSulfur;
        QSpinBox*               spinBoxResSulfur;
        QHBoxLayout*            horizontalLayoutMercuryCristal;
        QLabel*                 labelResMercury;
        QSpinBox*               spinBoxResMercury;
        QSpacerItem*            horizontalSpacerMercuryCristal;
        QLabel*                 labelResCrystal;
        QSpinBox*               spinBoxResCrystal;
        QHBoxLayout*            horizontalLayoutOreGems;
        QLabel*                 labelResOre;
        QSpinBox*               spinBoxResOre;
        QSpacerItem*            horizontalSpacerOreGems;
        QLabel*                 labelResGems;
        QSpinBox*               spinBoxResGems;
        QHBoxLayout*            horizontalLayoutGold;
        QSpacerItem*            horizontalSpacerGoldLeft;
        QLabel*                 labelResGold;
        QSpinBox*               spinBoxResGold;
        QSpacerItem*            horizontalSpacerGoldRight;
	QVBoxLayout*		verticalLayoutBox;

        Resources               result(void) const;

    signals:
        void                    formChanged(void);

    protected slots:
        void                    setFormChanged(void);
    };

    class ResourcesDialog : public QDialog
    {
        Q_OBJECT

    public:
        ResourcesDialog(const Resources & = Resources());

        ResourcesGroup*	resourcesGroup;
        QVBoxLayout*		formLayout;
        QHBoxLayout*		buttonsLayout;
        QPushButton*		pushButtonOk;
        QSpacerItem*		horizontalSpacer;
        QPushButton*		pushButtonCancel;

        Resources               resources(void) const;

    protected slots:
	void			enableButtonOk(void);
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
        DayEventsList*		listWidgetEvents;
        QHBoxLayout*		horizontalLayoutButton;
	QWidget*		tabDefaults;
	QLabel*			labelResourceGold;
	QLabel*			labelResourceWoodOre;
	QLabel*			labelResourceOther;
	QSpinBox*		spinBoxResourceGoldMin;
	QSpinBox*		spinBoxResourceGoldMax;
	QSpinBox*		spinBoxResourceWoodOreMin;
	QSpinBox*		spinBoxResourceWoodOreMax;
	QSpinBox*		spinBoxResourceOtherMin;
	QSpinBox*		spinBoxResourceOtherMax;
	QHBoxLayout*		horizontalLayoutResourceGold;
	QHBoxLayout*		horizontalLayoutResourceWoodOre;
	QHBoxLayout*		horizontalLayoutResourceOther;
	QVBoxLayout*		verticalLayoutDefaults;
	QSpacerItem*		verticalSpacerDefaults;
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

    protected slots:
	void			winsConditionsSelected(int);
	void			lossConditionsSelected(int);
	void			setEnableSaveButton(void);
	void			setEnableSaveButton(const QString &);
	void			setConditionsBoxesMapValues(const MapData &);
	void			saveSettings(void);
    };

    class MessageDialog : public QDialog
    {
        Q_OBJECT

    public:
        MessageDialog(const QString & = QString());

        QVBoxLayout*		verticalLayout;
        QHBoxLayout*		horizontalLayout;
        QPushButton*		pushButtonOk;
        QSpacerItem*		horizontalSpacer;
        QPushButton*		pushButtonCancel;
	QPlainTextEdit*		plainText;

	QString			message(void) const;

    protected slots:
	void			enableButtonOk(void);
    };


    class DayEventDialog : public QDialog
    {
	Q_OBJECT

    public:
	DayEventDialog(const DayEvent &, int);

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
	ResourcesGroup*		resourcesGroup;
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

    class MiniMap : public QLabel
    {
	Q_OBJECT

    public:
	MiniMap(QWidget*);

	void			generateFromTiles(const MapTiles &);
	void			setWindowPos(int, int, int, int);

	const QSize & 		mapSize(void) const { return miniMapSize; }

    signals:
	void			windowPositionNeedChange(const QPoint &);

    protected:
	void			mouseMoveEvent(QMouseEvent*);
	void			mousePressEvent(QMouseEvent*);
	void			paintEvent(QPaintEvent*);

	QSize			miniMapSize;
	QRect			windowPos;
	QPixmap			windowPixmap;
    };

    class MapEventDialog : public QDialog
    {
	Q_OBJECT

    public:
	MapEventDialog(const MapEvent &, int);

	QVBoxLayout*		verticalLayoutForm;
	QTabWidget*		tabWidget;
	QWidget*		tabAccess;
	QSpacerItem*		spacerItemAccess;
	QVBoxLayout*		verticalLayoutTabAcs;
	AccessGroup*		accessGroup;
	QWidget*		tabGift;
	ResourcesGroup*		resourcesGroup;
	QVBoxLayout*		verticalLayoutGift;
	ArtifactGroup*		artifactGroup;
	QWidget*		tabMessage;
	QVBoxLayout*		verticalLayoutTabMsg;
	QPlainTextEdit*		plainTextMessage;
	QHBoxLayout*		horizontalLayoutButtons;
	QPushButton*		pushButtonOk;
	QSpacerItem*		horizontalSpacerButtons;
	QPushButton*		pushButtonCancel;

	MapEvent		result(const QPoint &, quint32 uid) const;

    protected slots:
	void			setEnableOKButton(void);
    };


    class TownDialog : public QDialog
    {
	Q_OBJECT

    public:
	TownDialog(const MapTown &);

	QVBoxLayout*		verticalLayoutWidget;
	QTabWidget*		tabWidget;
	QWidget*		tabInfo;
	QVBoxLayout*		verticalLayoutInfo;
	QHBoxLayout*		horizontalLayoutName;
	QLabel*			labelName;
	QComboBox*		comboBoxName;
	QHBoxLayout*		horizontalLayoutColor;
	QLabel*			labelColor;
	QComboBox*		comboBoxColor;
	QCheckBox*		checkBoxCaptain;
	QCheckBox*		checkBoxAllowCastle;
	QSpacerItem*		verticalSpacerInfo;
	QWidget*		tabTroops;
	QVBoxLayout*		verticalLayoutTroops;
	QCheckBox*		checkBoxTroopsDefault;
	QHBoxLayout*		horizontalLayoutT1;
	QLabel*			labelSlot1;
	QComboBox*		comboBoxTroop1;
	QSpinBox*		spinBoxCount1;
	QHBoxLayout*		horizontalLayoutT2;
	QLabel*			labelSlot2;
	QComboBox*		comboBoxTroop2;
	QSpinBox*		spinBoxCount2;
	QHBoxLayout*		horizontalLayoutT3;
	QLabel*			labelSlot3;
	QComboBox*		comboBoxTroop3;
	QSpinBox*		spinBoxCount3;
	QHBoxLayout*		horizontalLayoutT4;
	QLabel*			labelSlot4;
	QComboBox*		comboBoxTroop4;
	QSpinBox*		spinBoxCount4;
	QHBoxLayout*		horizontalLayoutT5;
	QLabel*			labelSlot5;
	QComboBox*		comboBoxTroop5;
	QSpinBox*		spinBoxCount5;
	QSpacerItem*		verticalSpacerTroops;
	QWidget*		tabBuildings;
	QVBoxLayout*		verticalLayoutBuildings;
	QCheckBox*		checkBoxBuildingsDefault;
	QLabel*			labelMageGuild;
	QComboBox*		comboBoxMageGuild;
	QHBoxLayout*		horizontalLayoutMageGuild;
	QHBoxLayout*		horizontalLayoutB1;
	QCheckBox*		checkBoxMarket;
	QCheckBox*		checkBoxLeftTurret;
	QHBoxLayout*		horizontalLayoutB2;
	QCheckBox*		checkBoxTavern;
	QCheckBox*		checkBoxRightTurret;
        QHBoxLayout*		horizontalLayoutB3;
        QCheckBox*		checkBoxShipyard;
        QCheckBox*		checkBoxMoat;
	QHBoxLayout*		horizontalLayoutB4;
	QCheckBox*		checkBoxWell;
	QCheckBox*		checkBoxExt;
	QHBoxLayout*		horizontalLayoutB5;
	QCheckBox*		checkBoxStatue;
	QCheckBox*		checkBoxSpec;
	QCheckBox*		checkBoxThievesGuild;
	QSpacerItem*		verticalSpacerBuildings;
	QWidget*		tabDwellings;
	QVBoxLayout*		verticalLayoutDwellings;
	QCheckBox*		checkBoxDwellingsDefault;
	QCheckBox*		checkBoxDwelling1;
	QHBoxLayout*		horizontalLayoutD2;
	QCheckBox*		checkBoxDwelling2;
	QCheckBox*		checkBoxUpgrade2;
	QHBoxLayout*		horizontalLayoutD3;
	QCheckBox*		checkBoxDwelling3;
	QCheckBox*		checkBoxUpgrade3;
	QHBoxLayout*		horizontalLayoutD4;
	QCheckBox*		checkBoxDwelling4;
	QCheckBox*		checkBoxUpgrade4;
	QHBoxLayout*		horizontalLayoutD5;
	QCheckBox*		checkBoxDwelling5;
	QCheckBox*		checkBoxUpgrade5;
	QHBoxLayout*		horizontalLayoutD6;
	QCheckBox*		checkBoxDwelling6;
	QCheckBox*		checkBoxUpgrade6;
	QSpacerItem*		verticalSpacerDwellings;
	QHBoxLayout*		horizontalLayoutButtons;
	QPushButton*		pushButtonOk;
	QSpacerItem*		horizontalSpacerButton;
	QPushButton*		pushButtonCancel;

	int			buildings(void) const;
	int			dwellings(void) const;
	Troops			troops(void) const;

    protected slots:
	void			setEnableOKButton(void);
	void			setDefaultTroops(bool);
	void			setDefaultBuildings(bool);
	void			setDefaultDwellings(bool);
    };

    class SignDialog : public QDialog
    {
	Q_OBJECT

    public:
	SignDialog(const QString &);

	QVBoxLayout*		verticalLayout;
	QPlainTextEdit*		plainTextEdit;
	QHBoxLayout*		horizontalLayout;
	QPushButton*		pushButtonOk;
	QSpacerItem*		horizontalSpacerButtons;
	QPushButton*		pushButtonCancel;

    protected slots:
	void			setEnableOKButton(void);
    };

    class ArtifactsList : public ItemsList
    {
	Q_OBJECT

    public:
	ArtifactsList(QWidget*);

	bool			limit(void) const;

	void			addItem(void);
	void			editItem(QListWidgetItem*);
	void			checkLimit(void);
    };

    class SkillsList : public ItemsList
    {
	Q_OBJECT

    public:
	SkillsList(QWidget*);

	bool			limit(void) const;

	void			addItem(void);
	void			editItem(QListWidgetItem*);
	void			checkLimit(void);
    };

    class HeroDialog : public QDialog
    {
	Q_OBJECT

	int			oldport;

    public:
	HeroDialog(const MapHero &);

	QVBoxLayout* 		verticalLayoutWidget;
	QTabWidget* 		tabWidget;
	QWidget* 		tabInfo;
	QVBoxLayout* 		verticalLayoutInfo;
	QHBoxLayout* 		horizontalLayout;
	QVBoxLayout* 		verticalLayoutNameExp;
	QHBoxLayout* 		horizontalLayoutName;
	QLabel* 		labelName;
	QLineEdit* 		lineEditName;
	QHBoxLayout*		horizontalLayoutColor;
	QLabel*			labelColor;
	QComboBox*		comboBoxColor;
	QHBoxLayout*		horizontalLayoutRace;
	QLabel*			labelRace;
	QComboBox*		comboBoxRace;
	QHBoxLayout* 		horizontalLayoutExp;
	QLabel* 		labelExperience;
	QLineEdit* 		lineEditExperience;
	QSpacerItem* 		horizontalSpacerCenter;
	QLabel* 		labelPortrait;
	QScrollBar* 		verticalScrollBarPort;
	QSpacerItem* 		verticalSpacerInfo;
	QWidget* 		tabTroops;
	QVBoxLayout* 		verticalLayoutTroops;
	QCheckBox* 		checkBoxTroopsDefault;
	QHBoxLayout* 		horizontalLayoutT1;
	QLabel* 		labelSlot1;
	QComboBox* 		comboBoxTroop1;
	QSpinBox* 		spinBoxCount1;
	QHBoxLayout* 		horizontalLayoutT2;
	QLabel* 		labelSlot2;
	QComboBox* 		comboBoxTroop2;
	QSpinBox* 		spinBoxCount2;
	QHBoxLayout* 		horizontalLayoutT3;
	QLabel* 		labelSlot3;
	QComboBox* 		comboBoxTroop3;
	QSpinBox* 		spinBoxCount3;
	QHBoxLayout* 		horizontalLayoutT4;
	QLabel* 		labelSlot4;
	QComboBox* 		comboBoxTroop4;
        QSpinBox* 		spinBoxCount4;
	QHBoxLayout* 		horizontalLayoutT5;
        QLabel* 		labelSlot5;
        QComboBox* 		comboBoxTroop5;
        QSpinBox* 		spinBoxCount5;
        QSpacerItem* 		verticalSpacerTroops;
        QWidget* 		tabArtifacts;
	QVBoxLayout* 		verticalLayoutArtifacts;
        ArtifactsList* 		listWidgetArtifacts;
	QSpacerItem* 		verticalSpacerArtifacts;
        QWidget* 		tabSkills;
	QVBoxLayout* 		verticalLayoutSkills;
	QCheckBox* 		checkBoxDefaultSkills;
	SkillsList* 		listWidgetSkills;
	QSpacerItem* 		verticalSpacerSkills;
        QWidget* 		tabOther;
	QVBoxLayout* 		verticalLayoutOther;
	QGroupBox* 		groupBoxPatrol;
	QVBoxLayout* 		verticalLayoutPatrol;
	QCheckBox* 		checkBoxEnablePatrol;
	QComboBox* 		comboBoxPatrol;
	QSpacerItem* 		verticalSpacerOther;
	QHBoxLayout* 		horizontalLayoutButtons;
	QPushButton* 		pushButtonOk;
	QSpacerItem* 		horizontalSpacerButtons;
	QPushButton* 		pushButtonCancel;

	Troops			troops(void) const;
	QVector<int>		artifacts(void) const;
	Skills			skills(void) const;

    protected slots:
	void			setEnableOKButton(void);
	void			setPortrait(int);
	void			setDefaultTroops(bool);
	void			widgetSkillsVisible(bool);
    };

    class ListDialog : public QDialog
    {
	Q_OBJECT

    public:
	ListDialog();

	QVBoxLayout*		verticalLayout;
	QListWidget*		listWidget;
	QHBoxLayout*		horizontalLayout;
	QPushButton*		pushButtonOk;
	QSpacerItem*		horizontalSpacer;
	QPushButton*		pushButtonCancel;

    protected slots:
	void			setEnableOKButton(void);
    };

    class SelectArtifactDialog : public ListDialog
    {
	Q_OBJECT

    public:
	SelectArtifactDialog(int = 0);
    };

    class SelectSkillDialog : public ListDialog
    {
	Q_OBJECT

    public:
	SelectSkillDialog(const Skill & = Skill());
    };

    class RiddlesList : public ItemsList
    {
	Q_OBJECT

    public:
	RiddlesList(QWidget*);

	void			addItem(void);
	void			editItem(QListWidgetItem*);
    };

    class MapSphinxDialog : public QDialog
    {
	Q_OBJECT

    public:
	MapSphinxDialog(const MapSphinx &);

	QVBoxLayout*		verticalLayoutForm;
	QTabWidget*		tabWidget;
	QWidget*		tabGift;
	ResourcesGroup*		resourcesGroup;
	QVBoxLayout*		verticalLayoutGift;
	ArtifactGroup*		artifactGroup;
	QWidget*		tabMessage;
	QVBoxLayout*		verticalLayoutTabMsg;
	QPlainTextEdit*		plainTextMessage;
	QWidget*		tabAnswers;
	RiddlesList*		listWidgetAnswers;
	QVBoxLayout*		verticalLayoutAnswers;
	QHBoxLayout*		horizontalLayoutButtons;
	QPushButton*		pushButtonOk;
	QSpacerItem*		horizontalSpacerButtons;
	QPushButton*		pushButtonCancel;

	MapSphinx		result(const QPoint &, quint32 uid) const;

    protected slots:
	void			setEnableOKButton(void);
    };

    class ObjectEventsList : public ItemsList
    {
	Q_OBJECT

    public:
	ObjectEventsList(QWidget*);

	bool			limit(void) const;

	void			addItem(void){}
	void			editItem(QListWidgetItem*);
	void			checkLimit(void);

    protected slots:
	void			addEventsAction(QAction*);

    protected:
	void			createMenuItems(QMenu*);

	QActionGroup*		eventsGroupAct;
    };

    class ObjectEventsDialog : public QDialog
    {
	Q_OBJECT

    public:
	ObjectEventsDialog();

	QVBoxLayout*		verticalLayoutForm;
	QHBoxLayout*		horizontalLayoutList;
	ObjectEventsList*	listWidgetEvents;
        QVBoxLayout*		verticalLayoutButtons;
	QPushButton*		pushButtonUp;
	QPushButton*		pushButtonDown;
	QSpacerItem*		verticalSpacerButtons;
	QSpacerItem*		verticalSpacerForm;
	QHBoxLayout*		horizontalLayoutButtons;
	QPushButton*		pushButtonOk;
	QSpacerItem*		horizontalSpacerButtons;
	QPushButton*		pushButtonCancel;

    protected slots:
	void			checkUpDownButtons(void);
	void			setEnableOKButton(void);
	void			moveCurrentItemUp(void);
	void			moveCurrentItemDown(void);
    };

    class EditPassableDialog : public QDialog
    {
	Q_OBJECT

    public:
	EditPassableDialog(const MapTile &);

	int			result(void) const;

	QVBoxLayout*		verticalLayout;
        QHBoxLayout*		horizontalLayout5;
	QSpacerItem*		horizontalSpacer2;
        QLabel*			labelText;
        QSpacerItem*		horizontalSpacer3;
        QSpacerItem*		verticalSpacer2;
        QHBoxLayout*		horizontalLayout2;
        QSpacerItem*		horizontalSpacer;
        QLabel*			labelTopLeft;
        QLabel*			labelTop;
        QLabel*			labelTopRight;
        QSpacerItem*		horizontalSpacer4;
        QHBoxLayout*		horizontalLayout3;
        QSpacerItem*		horizontalSpacer5;
        QLabel*			labelLeft;
        QLabel*			labelCenter;
        QLabel*			labelRight;
        QSpacerItem*		horizontalSpacer7;
        QHBoxLayout*		horizontalLayout4;
        QSpacerItem*		horizontalSpacer8;
        QLabel*			labelBottomLeft;
        QLabel*			labelBottom;
        QLabel*			labelBottomRight;
        QSpacerItem*		horizontalSpacer11;
        QSpacerItem*		verticalSpacer;
        QHBoxLayout*		horizontalLayout;
        QPushButton*		pushButtonOk;
        QSpacerItem*		horizontalSpacerButtons;
        QPushButton*		pushButtonCancel;
    };
}

#endif
