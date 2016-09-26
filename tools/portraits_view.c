/**
 * \file portraits_view.c
 * \brief Create a page to view the heroes portraits.
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xml/xml.h"
#include "html/html.h"


#ifndef WOTDIR
    /** \brief where is WoT project installed */
    #define WOTDIR "."
#endif


/** Where are the mini portraits */
#define MINIPORTDIR WOTDIR"/files/images/miniport.icn"
/** Where are the medium portraits */
#define MEDIPORTDIR WOTDIR"/files/images/portmedi.icn"


/** spec file for mini portraits */
#define MINIPORTSPEC MINIPORTDIR"/spec.xml"
/** spec file for medium portraits */
#define MEDIPORTSPEC MEDIPORTDIR"/spec.xml"




const char* names[] = {
    // knight
    "Lord Kilburn", "Sir Gallanth", "Ector", "Gwenneth", "Tyro", "Ambrose", "Ruby", "Maximus", "Dimitry",
    // barbarian
    "Thundax", "Fineous", "Jojosh", "Crag Hack", "Jezebel", "Jaclyn", "Ergon", "Tsabu", "Atlas",
    // sorceress
    "Astra", "Natasha", "Troyan", "Vatawna", "Rebecca", "Gem", "Ariel", "Carlawn", "Luna",
    // warlock
    "Arie", "Alamar", "Vesper", "Crodo", "Barok", "Kastore", "Agar", "Falagar", "Wrathmont",
    // wizard
    "Myra", "Flint", "Dawn", "Halon", "Myrini", "Wilfrey", "Sarakin", "Kalindra", "Mandigal",
    // necromant
    "Zom", "Darlana", "Zam", "Ranloo", "Charity", "Rialdo", "Roxana", "Sandro", "Celia",
    // campains
    "Roland", "Lord Corlagon", "Sister Eliza", "Archibald", "Lord Halton", "Brother Bax",
    // loyalty version
    "Solmyr", "Dainwin", "Mog", "Uncle Ivan", "Joseph", "Gallavant", "Elderian", "Ceallach", "Drakonia", "Martine", "Jarkonas"
};




/**
 * @param table
 * @param spec
 * @param col the index of the column of the table where put the portraits
 * @param directory
 */
static void add_portraits(htmlTable *table, xmlNode *spec, int col, char *directory) {

    xmlNode *spriteNode;

    spriteNode = spec->children;

    while(spriteNode!=NULL) {

        xmlNode *item;
        char heroeName[50];
        char *index = xml_get_attribute(spriteNode, "index");
        // index values begin at 1 in spec.xml files, but we want begin at 0
        int line = atoi(index) - 1;
        // sprite filename
        char *name = xml_get_attribute(spriteNode, "name");
        char *completeFileName;

        completeFileName = malloc(sizeof(char) * (strlen(directory) + strlen(name) + 2));

        sprintf(heroeName, "Heroe %s", index);
        free(index);

        if(col==2) {
            // the medium portraits have at first index aleatory (unknown) heroe
            line--;
        }

        if(completeFileName != NULL && line >= 0) {

            sprintf(completeFileName, "%s/%s", directory, name);

            html_add_image_in_table(table, completeFileName, col, line);

            free(completeFileName);
        }

        free(name);
        spriteNode = spriteNode->next;
    }
}







int main(int argc, char **argv) {

    // the spec files
    xmlNode *specMini, *specMedi;

    // the html output
    htmlDocument *page = html_create_document("Portraits view");

    htmlTable *table;

    char **headers = malloc(4*sizeof(char *));
    int i;

    // returned error code
    int err;

    headers[0]="Heroe";
    headers[1]="mini";
    headers[2]="medi";
    headers[3]="port";

    table = html_create_table(4, 60, headers);
    html_add_table(page, table);

    for (i=0; i<60; i++) {
        char number[3];
        sprintf(number, "%d", i+1);
        html_set_text_in_table(table, names[i], 0, i);
    }



    specMini = xml_read_file(MINIPORTSPEC);

    if(specMini == NULL) {
        fprintf(stderr, "Spec file not read : exit\n");
    } else {
        add_portraits(table, specMini, 1, MINIPORTDIR);
        xml_destroy_node(specMini);
    }


    specMedi = xml_read_file(MEDIPORTSPEC);

    if(specMedi == NULL) {
        fprintf(stderr, "Spec file not read : exit\n");
    } else {
        add_portraits(table, specMedi, 2, MEDIPORTDIR);
        xml_destroy_node(specMedi);
    }


    for (i=0; i<60; i++) {
        char icndir[500];
        char specfile[500];
        xmlNode *specPort;
        xmlNode *spriteNode;
        // sprite filename
        char *name;
        char *completeFileName;


        sprintf(icndir, "%s/files/images/port00%02d.icn", WOTDIR, i);
        sprintf(specfile, "%s/spec.xml", icndir);

        specPort = xml_read_file(specfile);
        if(specPort != NULL) {
            spriteNode = specPort->children;
            name = xml_get_attribute(spriteNode, "name");
            xml_destroy_node(specPort);

            completeFileName = malloc(sizeof(char) * (strlen(icndir) + strlen(name) + 2));
            sprintf(completeFileName, "%s/%s", icndir, name);
            free(name);

            html_add_image_in_table(table, completeFileName, 3, i);

            free(completeFileName);
        }
    }


    err = html_write_to_file(page, "portraits_view.html");

    html_destroy_document(page);

    return err;
}

