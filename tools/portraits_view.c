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



/** spec file for mini portraits */
#define MINIPORTSPEC MINIPORTDIR"/spec.xml"



int main(int argc, char **argv) {

    // the spec file
    xmlNode *spec;

    // the html output
    htmlDocument *page = create_html_document("Portraits view");
    htmlList *list = html_add_list(page);
    xmlNode *spriteNode;

    // returned error code
    int err;




    spec = read_xml_file(MINIPORTSPEC);

    if(spec == NULL) {
        fprintf(stderr, "Spec file not read : exit\n");
        exit(1);
    }

    spriteNode = spec->children;

    while(spriteNode!=NULL) {

        xmlNode *item;
        char heroeName[50];
        char *index = xml_get_attribute(spriteNode, "index");
        // sprite filename
        char *name = xml_get_attribute(spriteNode, "name");
        char *completeFileName;

        completeFileName = malloc(sizeof(char) * (strlen(MINIPORTDIR) + strlen(name) + 2));

        sprintf(heroeName, "Heroe %s", index);
        free(index);

        if(completeFileName != NULL) {

            sprintf(completeFileName, "%s/%s", MINIPORTDIR, name);
            item = html_add_list_item(list, heroeName);
            html_add_image_in_node(item, completeFileName);

            free(completeFileName);
        }

        free(name);
        spriteNode = spriteNode->next;
    }


    destroy_xmlNode(spec);


    err = html_write_to_file(page, "portraits_view.html");

    destroy_html_document(page);

    return err;

}
