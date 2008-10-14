/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
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

#include <dirent.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include "error.h"
#include "gamedefs.h"
#include "config.h"
#include "tools.h"
#include "dir.h"

Dir::Dir()
{
}

void Dir::Read(const std::string &path, const std::string &filter, bool sensitive)
{
    // read directory
    DIR *dp;
    struct dirent *ep;
    struct stat fs;

    dp = opendir(path.c_str());

    if(H2Config::Debug()) Error::Verbose("Dir::open: " + (filter.size() ? path + " (" + filter + ")" : path));

    if(dp)
    {
	while(NULL != (ep = readdir(dp)))
	{
	    const std::string fullname(path + SEPARATOR + ep->d_name);

    	    // if not regular file
    	    if(stat(fullname.c_str(), &fs) || !S_ISREG(fs.st_mode)) continue;

	    if(filter.size())
	    {
    		std::string filename(ep->d_name);

		if(sensitive)
		{
		    if(std::string::npos == filename.find(filter)) continue;
    		}
    		else
    		{
    		    std::string filterlow(filter);
    		    String::Lower(filterlow);
    		    String::Lower(filename);

		    if(std::string::npos == filename.find(filterlow)) continue;
		}
    	    }

    	    push_back(fullname);
	}
	closedir(dp);
    }
}
