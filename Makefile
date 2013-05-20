CC=cc

CFLAGS=-O
#CFLAGS=-O -DMAX_WBITS=14 -DMAX_MEM_LEVEL=7
#CFLAGS=-g -DDEBUG
#CFLAGS=-O3 -Wall -Wwrite-strings -Wpointer-arith -Wconversion \
#           -Wstrict-prototypes -Wmissing-prototypes

LIBOBJS=univaruints.o
BASENAME=univaruints
VER=0.1.0
MAJOR=0

STATICLIB=lib$(BASENAME).a
SHAREDLIB=lib$(BASENAME).so
SHAREDLIBV=lib$(BASENAME).so.$(VER)
SHAREDLIBM=lib$(BASENAME).so.$(MAJOR)
LIBS=$(SHAREDLIB)

all: $(LIBS) test

#libz.a: $(OBJS)
#	$(AR) $(ARFLAGS) $@ $(OBJS)
#	-@ ($(RANLIB) $@ || true) >/dev/null 2>&1

$(SHAREDLIB): $(SHAREDLIBM)
	rm $(SHAREDLIB) 2>/dev/null || :
	ln -s $(SHAREDLIBM) $(SHAREDLIB)

$(SHAREDLIBM): $(SHAREDLIBV)
	rm $(SHAREDLIBM) 2>/dev/null || :
	ln -s $(SHAREDLIBV) $(SHAREDLIBM)

$(SHAREDLIBV): $(LIBOBJS)
	$(CC) $(CFLAGS) -shared -Wl,-soname,$(SHAREDLIBM) -o $(SHAREDLIBV) $(LIBOBJS) -lc
	ls -lh *.so*

%.o: %.c %.h
	# do we need to add -g
	$(CC) $(CFLAGS) -fPIC -c -Wall $*.c


.PHONY: clean test
test: $(LIBS)
	$(CC) $(CFLAGS) -I. -L. -l$(BASENAME) -o test test.c && LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. ./test

clean:
	-rm test *.so* *.o *.a 2>/dev/null || :
