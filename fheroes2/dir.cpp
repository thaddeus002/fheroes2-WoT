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
#include "error.h"
#include "dir.h"

#if defined(_WINDOWS) || defined(_WIN32) || defined(__WIN32__) 
#define SEPARATOR       "\\" 
#else 
#define SEPARATOR       "/" 
#endif 

Dir::Dir(const std::string &path, const std::string &filter) : std::vector<std::string>()
{
    // read directory
    DIR *dp;
    struct dirent *ep;

    dp = opendir(path.c_str());

    if(dp == NULL) Error::Except("error open directory: " + path);

    while(NULL != (ep = readdir(dp))){

        std::string filename(ep->d_name);
        if( '.' == filename[0] ) continue;
	if( !filter.empty() && std::string::npos == filename.find(filter) ) continue;

        push_back(std::string(path + SEPARATOR + filename));
    }

    closedir(dp);
}
