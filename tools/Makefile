
#ifndef WOTDIR
    # Define where is installed WoT project if not in a environment variable
    WTDIR="."
#endif

OBJS=csv/csv.o csv/utils.o xml/xml.o html/html.o
PROG=portraits_view

all: $(PROG)

csv/utils.o:
	make -C csv

csv/csv.o:
	make -C csv

html/html.o:
	make -C html

xml/xml.o:
	make -C xml

portraits_view: portraits_view.c $(OBJS)
	gcc -DWOTDIR=\"$(WOTDIR)\" -o $(PROG) $< $(OBJS)

clean:

mrproper: clean
	rm -f $(PROG)
