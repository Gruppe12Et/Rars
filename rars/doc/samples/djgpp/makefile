AR	 = ar
CC	 = gcc
#WARNING = -Wall 
OPTIMIZE = -fomit-frame-pointer -O2 -s -m486 -D__DJGPP__
#DEBUG   = -g 
CFLAGS	 = $(DEBUG) $(WARNING) $(OPTIMIZE)
LFLAGS   = -N
INCL 	 = -I. -I/usr/include

PROGS_O  = g_view.o g_view2d.o g_viewbd.o g_viewcl.o burns.o lisa.o carz.o misc.o \
	   g_bitmap.o g_divers.o g_lowgra.o g_vmanag.o track.o g_palett.o \
	   bingo1.o bulle.o blender.o indretti.o report.o draw.o \
	   os.o 
#gr_obj.o

LIBS     = -lm -lgrx -lpc -lgpp

all:	rars320 rars640 rars641

exe:
	coff2exe -s go32.exe rars2

progs : $(PROGS)

.cc.o:
	$(CC) $(CFLAGS) $(INCL) -c -o $*.o $< 

.cc.s:
	$(CC) $(CFLAGS) $(INCL) -S -o $*.s $<

.s.o:
	$(CC) $(CFLAGS) $(INCL) -c -o $*.o $*.S 

.o:
	$(CC) $(CFLAGS) $(INCL) $(LFLAGS) -c -o $*.o $*.cc -g

asm:
	$(CC) $(CFLAGS) $(INCL) -S dj_graph.cc

level.o: level.cc level.h global.h types.h routine.h
	$(CC) $(DEBUG) $(INCL) -c -o $*.o $< 

control.o: control.cc control.h dj_divrs.h
	$(CC) $(DEBUG) $(INCL) -c -o $*.o $< 

rars320: $(PROGS_O) dj_gr320.o
	$(CC) $(CFLAGS) $(INCL) -o rars320 $(PROGS_O) dj_gr320.o $(LIBS)

rars640: $(PROGS_O) dj_gr640.o
	$(CC) $(CFLAGS) $(INCL) -o rars640 $(PROGS_O) dj_gr640.o $(LIBS)

rars641: $(PROGS_O) dj_gr641.o
	$(CC) $(CFLAGS) $(INCL) -o rars641 $(PROGS_O) dj_gr641.o $(LIBS)
