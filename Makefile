all: liblistio.a db display
	chmod 0755 *.c
	chmod 0755 *.php       
	chmod 0755 writer.py
	gcc -Wall -pedantic -g -std=c99 -l mysqlclient -L /usr/lib/x86_64-linux-gnu/ main.c \
	    liblistio.a -o a4

liblistio.a: listio.o
	ar cr liblistio.a listio.o 

listio.o:
	gcc -c -Wall -pedantic -g -std=c99 listio.c -o listio.o

db:
	gcc -Wall -pedantic -g -std=c99 -l mysqlclient -L \
	    /usr/lib/x86_64-linux-gnu/ db.c -o db

display:
	gcc -Wall -pedantic -g -std=c99 -l mysqlclient -L \
            /usr/lib/x86_64-linux-gnu/ display_files.c -o db

clean:
	rm *.a *.o *.bin a4 db
