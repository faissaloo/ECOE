SHELL = /bin/sh
INSTALL_PATH?=/usr/local
CC = gcc
all:
	$(CC) -o ecoe `xml2-config --cflags` ./src/main.c ./src/gm_obj_lib.c ./src/lgl_lib.c `xml2-config --libs` -lncursesw -O3
install:
	$(CC) -o ecoe `xml2-config --cflags` ./src/main.c ./src/gm_obj_lib.c ./src/lgl_lib.c `xml2-config --libs` -lncursesw -O3
	mv ecoe $(INSTALL_PATH)/bin/ecoe
	cp -r lib $(INSTALL_PATH)/bin/lib
debug:
	$(CC) -o ecoe -Wall `xml2-config --cflags` ./src/main.c ./src/gm_obj_lib.c ./src/lgl_lib.c `xml2-config --libs` -lncursesw -g
	gdb -tui ecoe
memcheck:
	$(CC) -o ecoe -Wall `xml2-config --cflags` ./src/main.c ./src/gm_obj_lib.c ./src/lgl_lib.c `xml2-config --libs` -lncursesw -g
	valgrind --tool=memcheck --leak-check=full ./ecoe 2> log.txt
	less log.txt
	rm log.txt
clean:
	rm ecoe
