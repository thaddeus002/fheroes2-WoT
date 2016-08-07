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


int main(int argc, char **argv) {

    // the spec files
    xmlNode *specMini, *specMedi;

    // the html output
    htmlDocument *page = create_html_document("Portraits view");

    htmlTable *table;

    xmlNode *spriteNode;

    char **headers = malloc(4*sizeof(char *));

    // returned error code
    int err;

    headers[0]="Heroe";
    headers[1]="mini";
    headers[2]="medi";
    headers[3]="port";

    table = create_html_table(4, 60, headers);
    html_add_table(page, table);



    specMini = read_xml_file(MINIPORTSPEC);

    if(specMini == NULL) {
        fprintf(stderr, "Spec file not read : exit\n");
        exit(1);
    }

    spriteNode = specMini->children;

    while(spriteNode!=NULL) {

        xmlNode *item;
        char heroeName[50];
        char *index = xml_get_attribute(spriteNode, "index");
        int line = atoi(index);
        // sprite filename
        char *name = xml_get_attribute(spriteNode, "name");
        char *completeFileName;

        completeFileName = malloc(sizeof(char) * (strlen(MINIPORTDIR) + strlen(name) + 2));

        sprintf(heroeName, "Heroe %s", index);
        free(index);

        if(completeFileName != NULL && line <= 60) {

            sprintf(completeFileName, "%s/%s", MINIPORTDIR, name);

            html_add_image_in_table(table, completeFileName, 1, line);

            free(completeFileName);
        }

        free(name);
        spriteNode = spriteNode->next;
    }


    destroy_xmlNode(specMini);


    err = html_write_to_file(page, "portraits_view.html");

    destroy_html_document(page);

    return err;

}
