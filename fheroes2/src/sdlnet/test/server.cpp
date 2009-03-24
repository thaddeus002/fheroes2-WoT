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
    << "  -d\tdebug mode\n" \
    << "  -p\tport number (" << DEFAULT_PORT << " default)\n" \
    << "  -m\tmax clients (" << MAX_CLIENTS << " default)\n" \
    << "  -h\tprint this help and exit" << std::endl;

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    bool debug = false;
    int opt;
    Uint16 port = DEFAULT_PORT;
    Uint8 maxclients = MAX_CLIENTS;

    while(-1 != (opt = getopt(argc, argv, "hdp:m:")))
    {
	switch(opt)
        {
            case 'd': debug = true; break;

            case 'p': port = stringToInt(optarg); break;
            case 'm': maxclients = stringToInt(optarg); break;

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
    if(! Network::ResolveHost(ip, NULL, port)) return EXIT_FAILURE;

    Network::Server server;
    if(! server.Open(ip)) return EXIT_FAILURE;

    Network::Message banner;
    banner.Push("fheroes2 server, version 0.1\n");
    banner.SetID(Network::MSG_RAW);


    bool exit = false;
    
    Network::Message packet;
    const Uint16 test16 = 0xABCD;
    const Uint32 test32 = 0x01235678;
    const std::string test("test string");

    while(! exit)
    {
	TCPsocket csd = server.Accept();

	if(csd)
	{
	    Network::Socket client(csd);
	    banner.Send(client);

	    if(packet.Recv(client) && Network::MSG_CONNECT == packet.GetID())
	    {
		packet.Dump();
		packet.Send(client);

		// new packet
		packet.Reset();
		packet.SetID(Network::MSG_DATA);
		// pack data
		packet.Push(test16);
		packet.Push(test32);
		packet.Push(test);
		packet.Send(client);

		if(packet.Recv(client)) packet.Dump();
		if(Network::MSG_SHUTDOWN == packet.GetID()) exit = true;
	    }
	    else
		client.Close();
	}

	SDL_Delay(1);
    }

    server.Close();
    std::cout << "bye!" << std::endl;
    
    return EXIT_SUCCESS;
}
