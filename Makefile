OBJ = ijgdec.o \
      ijgenc.o \
      metric.o \
      api.o \
      main.o

LIBOBJ = ijgdec.lo \
         ijgenc.lo \
         metric.lo \
         api.lo

PREFIX=/usr/local
CFLAGS=-Wall
LDFLAGS=-s
LIBS=-ljpeg -lm

all: smallfry lib

smallfry: $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

$(LIBOBJ):
	$(CC) -fPIC $(CFLAGS) -c $(@:.lo=.c) -o $@

libsmallfry.so: $(LIBOBJ)
	$(CC) -Wl,--version-script,smallfry.ver -shared -o $@ $^ $(LIBS)

lib: libsmallfry.so

clean:
	@rm -vf ./*.o
	@rm -vf ./smallfry
	@rm -vf ./*.lo
	@rm -vf ./libsmallfry.so

install: smallfry
	@cp -vf ./smallfry $(PREFIX)/bin/smallfry

uninstall:
	@rm -vf $(PREFIX)/bin/smallfry
