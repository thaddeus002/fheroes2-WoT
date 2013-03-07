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

#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QRegExp>
#include <QDebug>
#include <QDir>
#include <QtGui>
#include <QTextStream>
#include <ctime>

#include "engine.h"
#include "mainwindow.h"
#include "dialogs.h"
#include "program.h"

#define PROGRAM_SHARE "fh2editor"

struct ProgramShareEnv : QRegExp
{
    ProgramShareEnv() : QRegExp(QString(PROGRAM_SHARE).toUpper() + "=(.+)"){}

    bool operator() (const QString & str) const { return exactMatch(str); }
};

namespace Resource
{
    QStringList shares;

    const QStringList & ShareDirs(void)
    {
	return shares;
    }

    QString FindFile(const QString & dir, const QString & file)
    {
	for(QStringList::const_iterator
	    it = shares.begin(); it != shares.end(); ++it)
	{
	    const QString path = QDir::toNativeSeparators(*it + QDir::separator() + dir + QDir::separator() + file);
	    if(QFile(path).exists()) return path;
	}

	return NULL;
    }

    QString Path(const char* str, ...)
    {
	QStringList list;

	for(const char** ptr = &str; *ptr != NULL; ++ptr)
	    list.push_back(*ptr);

	QString shortres = list.join(QDir::separator());

	for(QStringList::const_iterator
	    it = shares.begin(); it != shares.end(); ++it)
	{
	    QString fullres = QDir::toNativeSeparators(*it + QDir::separator() + shortres);
	    if(QFile(fullres).exists()) return fullres;
	}

	return NULL;
    }

    void InitShares(void)
    {
	QStringList list;

#ifdef BUILD_PROGRAM_SHARE
	list.push_back(QString(BUILD_PROGRAM_SHARE));
#endif

	const QStringList & envs = QProcess::systemEnvironment();
	struct ProgramShareEnv regExp;

	for(QStringList::const_iterator
	    it = envs.begin(); it != envs.end(); ++it)
	if(regExp.exactMatch(*it)){ list.push_back(regExp.cap(1)); break; }

	list.push_back(QCoreApplication::applicationDirPath());
	list.push_back(QDir::toNativeSeparators(QDir::homePath() + QDir::separator() + "." + QString(PROGRAM_SHARE).toLower()));

	Resource::shares.clear();

	for(QStringList::const_iterator
	    it = list.begin(); it != list.end(); ++it)
	{
	    qDebug() << "registry sharedir:" << *it;
    	    Resource::shares.push_front(*it);
	}
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Resource::InitShares();
    qsrand(std::time(0));

    QSettings settings("fheroes2", "editor");
    QString dataFile = settings.value("dataFile", "").toString();

    if(! QFile(dataFile).exists())
    {
	const QString resourceAgg = "HEROES2.AGG";
	const QString subFolder = "data";

	dataFile = Resource::FindFile(subFolder, resourceAgg);

	if(dataFile.isEmpty())
	    dataFile = Resource::FindFile(subFolder, resourceAgg.toLower());

	if(dataFile.isEmpty())
	{
	    dataFile = Dialog::SelectDataFile(resourceAgg);
	    if(dataFile.isEmpty()) return 0;
	}

	settings.setValue("dataFile", dataFile);
    }

    MainWindow mainWin(dataFile);
    mainWin.show();

    return app.exec();
}
