/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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

#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "sdlnet.h"

#define DEFAULT_PORT	5154
#define MAX_CLIENTS	6

extern char *optarg;

int stringToInt(const std::string & str)
{
    std::istringstream stream(str);
    int res;
    stream >> res;
    return res;
}

int printHelp(const char *basename)
{
    std::cout << "Usage: " << basename << " [OPTIONS]\n" \
    << "  -s\tserver\n" \
    << "  -p\tport number (" << DEFAULT_PORT << " default)\n" \
    << "  -d\tdebug mode\n" \
    << "  -h\tprint this help and exit" << std::endl;

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    bool debug = false;
    int opt;
    Uint16 port = DEFAULT_PORT;
    std::string server("localhost");

    while(-1 != (opt = getopt(argc, argv, "hdp:s:")))
    {
	switch(opt)
        {
            case 'd': debug = true; break;

            case 'p': port = stringToInt(optarg); break;
            case 's': server = optarg; break;

            case '?':
            case 'h': return printHelp(argv[0]);

    	    default: break;
        }
    }

    if(SDL_Init(SDL_INIT_TIMER) < 0)
    {
    	std::cerr << "SDL_Init: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    std::atexit(SDL_Quit);

    if(! Network::Init()) return EXIT_FAILURE;
    std::atexit(Network::Quit);

    IPaddress ip;
    if(! Network::ResolveHost(ip, server.c_str(), port)) return EXIT_FAILURE;

    Network::Socket client;
    if(! client.Open(ip)) return EXIT_FAILURE;

    Network::Message packet;
    Uint16 test16;
    Uint32 test32;
    std::string test;

    packet.SetID(Network::MSG_CONNECT);
    packet.Send(client);

    // skip banner and wait connect
    while(1) if(packet.Recv(client) && Network::MSG_CONNECT == packet.GetID()) break;
    std::cout << "Connected.. " << std::endl;

    if(packet.Recv(client)) packet.Dump();
    if(Network::MSG_DATA == packet.GetID())
    {
	// unpack data
	packet.Pop(test16);
	packet.Pop(test32);
	packet.Pop(test);

	std::cout << "Uint16: 0x" << std::hex << test16 << std::endl;
	std::cout << "Uint32: 0x" << std::hex << test32 << std::endl;
	std::cout << "string: " << test << std::endl;
    }

    packet.Reset();
    packet.SetID(Network::MSG_SHUTDOWN);
    packet.Send(client);

    client.Close();
    std::cout << "bye!" << std::endl;
    
    return EXIT_SUCCESS;
}
