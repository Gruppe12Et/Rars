# The robot drivers!
#DRIVERS:=bulle.o felix12.o gryma1.o burns.o h1.o apex8.o stocker.o
#DRIVERS:=$(DRIVERS) rach221b.o turtle5.o magic.o rusty.o vector.o
#DRIVERS:=$(DRIVERS) oscarstr.o ralph2a.o jas.o wappucar.o
#DRIVERS:=$(DRIVERS) pokey02b.o douge1.o
.AUTODEPEND
   
#               *Translator Definitions*   
CC = c:\prog\bc45\bin\bcc +rars.CFG   
TASM = c:\prog\bc45\bin\TASM   
TLIB = c:\prog\bc45\bin\tlib   
TLINK = c:\prog\bc45\bin\tlink   
LIBPATH = c:\prog\bc45\lib   
INCLUDEPATH = c:\prog\bc45\include
   
   
#               *Implicit Rules*   
.c.obj:   
	bcc +rars.cfg -c -ml $*.cpp   
   
.cpp.obj:   
	bcc +rars.cfg -c -ml $*.cpp   
   
#               *List Macros*   
   
   
EXE_dependencies =  \   
	carz.obj\
	draw.obj\
	drivers.obj\
	gi.obj\
	misc.obj\
	movie.obj\
	os.obj\
	report.obj\
	track.obj\
	trackgen.obj\ 
	apex8.obj\   
	burns.obj\   
	gryma1.obj\   
	oscar1.obj\
	felix12.obj\   
	jas.obj\   
	h1.obj\   
	magic.obj\   
	mat1.obj\   
	djoefe.obj\
	oscarstr.obj\   
	rach221b.obj\   
	stocker.obj\   
	turtle5.obj\   
	vector.obj\   
	wappucar.obj   
   
#               *Explicit Rules*   
rars.exe: rars.cfg $(EXE_dependencies)   
  $(TLINK) /v/s/c/P-/L$(LIBPATH) @&&|   
c0l.obj+   
carz.obj+   
draw.obj+   
drivers.obj+
gi.obj+   
misc.obj+   
movie.obj+   
os.obj+   
report.obj+   
track.obj+   
trackgen.obj+ 
apex8.obj+   
burns.obj+   
gryma1.obj+   
oscar1.obj+
felix12.obj+   
jas.obj+   
h1.obj+   
djoefe.obj+   
oscarstr.obj+
magic.obj+   
mat1.obj+   
rach221b.obj+   
stocker.obj+   
turtle5.obj+   
vector.obj+   
wappucar.obj+   
c:\prog\bc45\bgi\egavga.obj   
rars   
	#       no map file   
emu.lib+   
mathl.lib+   
graphics.lib+   
cl.lib   
|   
   
   
	#               *Individual File Dependencies*   
carz.obj:       carz.cpp   
   
draw.obj:       draw.cpp   
   
drivers.obj:    drivers.cpp

gi.obj:         gi.cpp   
   
misc.obj:       misc.cpp   
   
movie.obj:      movie.cpp   
   
os.obj:         os.cpp   
   
report.obj:     report.cpp   
   
track.obj:      track.cpp   
   
trackgen.obj:   trackgen.cpp 
	
apex8.obj:      apex8.cpp   
   
burns.obj:      burns.cpp   
   
gryma1.obj:     gryma1.cpp   
   
oscar1.obj:   oscar1.cpp   
   
felix12.obj:    felix12.cpp   
   
jas.obj:        jas.cpp   
    
h1.obj:     h1.cpp   
   
magic.obj:      magic.cpp   
   
mat1.obj:       mat1.cpp   
   
djoefe.obj:     djoefe.cpp   
   
oscarstr.obj:   oscarstr.cpp   
   
rach221b.obj:   rach221b.cpp   
   
stocker.obj:    stocker.cpp   
   
turtle5.obj:    turtle5.cpp   
   
vector.obj:     vector.cpp   
   
wappucar.obj:   wappucar.cpp   
   
#               *Compiler Configuration File*   
rars.cfg: makefile   
  copy &&|   
-ml   
-v   
-vi-   
-wpro   
-weas   
-wpre   
-I$(INCLUDEPATH)   
-L$(LIBPATH)   
| rars.cfg   

