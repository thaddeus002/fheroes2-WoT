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

    class SelectImageObject : public QDialog
    {
        Q_OBJECT

    public:
        SelectImageObject();

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

    signals:
	void			mousePressed(void);
	void			listChanged(void);

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
	RumorsList(QWidget*);

	TavernRumors		results(void) const;

    protected slots:
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

    protected slots:
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

    protected slots:
	void			enableButtonOk(void);
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

    class MiniMap : public QFrame
    {
	Q_OBJECT

    public:
	MiniMap(QWidget*);

	QVBoxLayout*		verticalLayout;
	QLabel*			labelPixmap;

	void			generateFromTiles(const MapTiles &);
    };

    class EditResourceDialog : public QDialog
    {
	Q_OBJECT

    public:
	QVBoxLayout*		verticalLayout;
	QCheckBox*		checkBoxDefault;
	QHBoxLayout*		horizontalLayout1;
	QLabel*			labelCount;
	QSpinBox*		spinBoxCount;
	QHBoxLayout*		horizontalLayout2;
	QPushButton*		pushButtonOk;
	QSpacerItem*		horizontalSpacer;
	QPushButton*		pushButtonCancel;
        QSpacerItem*		verticalSpacer;

	EditResourceDialog(int res, int count);

    protected slots:
	void			 disableCustomCount(bool);
	void			 enableButtonOk(void);
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
	QGroupBox*		groupBoxAllowedColors;
	QVBoxLayout*		verticalLayoutAllowCols;
	QHBoxLayout*		horizontalLayoutPlayers;
	QSpacerItem*		horizontalSpacerPlayersLeft;
	QSpacerItem*		horizontalSpacerPlayersRight;
	QVector<PlayerAllow*>	labelPlayers;
	QCheckBox*		checkBoxAllowComp;
	QCheckBox*		checkBoxCancelAfterFirstVisit;
	QWidget*		tabGift;
	QVBoxLayout*		verticalLayoutGift;
	QVBoxLayout*		verticalLayoutResource;
	QHBoxLayout*		horizontalLayoutWoodSulf;
	QLabel*			labelResWood;
	QSpinBox*		spinBoxResWood;
	QSpacerItem*		horizontalSpacerWoodSulf;
	QLabel*			labelResSulfur;
	QSpinBox*		spinBoxResSulfur;
        QHBoxLayout*		horizontalLayoutMercCryst;
	QLabel*			labelResMercury;
	QSpinBox*		spinBoxResMercury;
	QSpacerItem*		horizontalSpacerMercCryst;
	QLabel*			labelResCrystal;
	QSpinBox*		spinBoxResCrystal;
	QHBoxLayout*		horizontalLayoutOreGems;
	QLabel*			labelResOre;
	QSpinBox*		spinBoxResOre;
	QSpacerItem*		horizontalSpacerOreGems;
	QLabel*			labelResGems;
	QSpinBox*		spinBoxResGems;
	QHBoxLayout*		horizontalLayoutGold;
	QSpacerItem*		horizontalSpacerGoldLeft;
	QLabel*			labelResGold;
	QSpinBox*		spinBoxResGold;
	QSpacerItem*		horizontalSpacerGoldRight;
	QHBoxLayout*		horizontalLayoutArtifact;
	QLabel*			labelArtifact;
	QComboBox*		comboBoxArtifact;
	QGroupBox*              groupBoxResource;
	QGroupBox*              groupBoxArtifact;
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
	void			setEnableOKButton(const QString &);
	void			changeLabelArtifact(int);
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
        QLineEdit*		lineEditName;
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

    protected slots:
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

    protected slots:
	void			addItem(void);
	void			editItem(QListWidgetItem*);
	void			checkLimit(void);
    };

    class HeroDialog : public QDialog
    {
	Q_OBJECT

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

    protected slots:
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
	QVBoxLayout*		verticalLayoutGift;
	QVBoxLayout*		verticalLayoutResource;
	QHBoxLayout*		horizontalLayoutWoodSulf;
	QLabel*			labelResWood;
	QSpinBox*		spinBoxResWood;
	QSpacerItem*		horizontalSpacerWoodSulf;
	QLabel*			labelResSulfur;
	QSpinBox*		spinBoxResSulfur;
        QHBoxLayout*		horizontalLayoutMercCryst;
	QLabel*			labelResMercury;
	QSpinBox*		spinBoxResMercury;
	QSpacerItem*		horizontalSpacerMercCryst;
	QLabel*			labelResCrystal;
	QSpinBox*		spinBoxResCrystal;
	QHBoxLayout*		horizontalLayoutOreGems;
	QLabel*			labelResOre;
	QSpinBox*		spinBoxResOre;
	QSpacerItem*		horizontalSpacerOreGems;
	QLabel*			labelResGems;
	QSpinBox*		spinBoxResGems;
	QHBoxLayout*		horizontalLayoutGold;
	QSpacerItem*		horizontalSpacerGoldLeft;
	QLabel*			labelResGold;
	QSpinBox*		spinBoxResGold;
	QSpacerItem*		horizontalSpacerGoldRight;
	QHBoxLayout*		horizontalLayoutArtifact;
	QLabel*			labelArtifact;
	QComboBox*		comboBoxArtifact;
	QGroupBox*              groupBoxResource;
	QGroupBox*              groupBoxArtifact;
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
	void			setEnableOKButton(const QString &);
	void			changeLabelArtifact(int);
    };

    class ObjectEventsList : public ItemsList
    {
	Q_OBJECT

    public:
	ObjectEventsList(QWidget*);

	bool			limit(void) const;

    protected slots:
	void			addItem(void);
	void			editItem(QListWidgetItem*);
	void			checkLimit(void);
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
    };
}

#endif
