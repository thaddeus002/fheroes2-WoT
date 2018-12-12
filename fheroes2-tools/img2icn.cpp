/**
 * \file img2inc.cpp - build with : g++ img2icn.cpp -o img2icn -IyImage yImage.o yColor.o yImage_io.o palette.o h2icn.o -lpng -Ltinyxml -ltinyxml
 */



#include <iostream>

#include "h2icn.h"



static void usage(std::string prog) {
    std::cerr << "Usage: " << prog << " <icndir>" << std::endl;
    exit(1);
}



int main(int argc, char **argv) {

    if(argc != 2) {
        usage(argv[0]);
    }

    icnfile *object = new icnfile(argv[1]);

    object->create_icn_file("out.icn");

    delete object;

    return 0;
}


