/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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


#include <iostream>

#include "h2icn.h"




void usage(char *prog) {
    std::cout << "Extract sprites from icn file" << std::endl;
    std::cout << "Usage : " << prog << " <file.icn>" << std::endl;
    exit(1);
}



/**
 * Main program.
 */
int main(int argc, char **argv) {

    char *filename = NULL;

    if(argc >= 2) {
        filename = argv[1];
    } else {
        usage(argv[0]);
    }

    icnfile *icn = new icnfile(filename, 2);

    std::string dirname(filename);

    icn->create_files(dirname + ".extract");

    delete icn;

    return 0;
}
