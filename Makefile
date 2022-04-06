EXEC=ee

CFLAGS=-Wall -O2

CFILES=	basic.c buffer.c display.c file.c fileio.c line.c main.c \
	random.c region.c search.c tcap.c termio.c window.c word.c \
	utils.c

OFILES=	basic.o buffer.o display.o file.o fileio.o line.o main.o \
	random.o region.o search.o tcap.o termio.o window.o word.o \
	utils.o

all:	$(OFILES)
	$(CC) $(CFLAGS) $(OFILES) -o $(EXEC) $(LFLAGS)

clean:
	rm -f *.o *~ $(EXEC)

basic.o: basic.c estruct.h edef.h
buffer.o: buffer.c estruct.h edef.h
display.o: display.c estruct.h edef.h
file.o: file.c estruct.h edef.h
fileio.o: fileio.c estruct.h
line.o: line.c estruct.h edef.h
main.o: main.c estruct.h edef.h efunc.h ebind.h
random.o: random.c estruct.h edef.h
region.o: region.c estruct.h edef.h
search.o: search.c estruct.h edef.h
tcap.o: tcap.c estruct.h edef.h
termio.o: termio.c
window.o: window.c estruct.h edef.h
word.o: word.c estruct.h edef.h
utils.o: utils.c estruct.h edef.h
