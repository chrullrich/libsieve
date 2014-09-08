FLEX=flex
BISON=bison
AR=lib

FLFLAGS=--nounistd --bison-bridge
YFLAGS=

!IFDEF DEBUG
CFLAGS=/Isrc /Zi /MDd /Fdbuild\libsieve.pdb
!ELSE
CFLAGS=/Isrc /O2 /MD
!ENDIF

all: build\libsieve.lib

clean:
	del /s *.lib
	del /s *.obj
	del /s *.pdb
	del src\sv_parser\addr.c src\sv_parser\addr.h
	del src\sv_parser\header.c src\sv_parser\header.h
	del src\sv_parser\sieve.c src\sv_parser\sieve.h
	del src\sv_parser\*-lex.c src\sv_parser\*-lex.h

src\sv_parser\addr-lex.c: src\sv_parser\addr-lex.l src\sv_parser\addr.h
	$(FLEX) $(FLFLAGS) --header-file=$*.h -o $*.c $**

src\sv_parser\header-lex.c: src\sv_parser\header-lex.l src\sv_parser\header.h
	$(FLEX) $(FLFLAGS) --header-file=$*.h -o $*.c $**

src\sv_parser\sieve-lex.c: src\sv_parser\sieve-lex.l src\sv_parser\sieve.h
	$(FLEX) $(FLFLAGS) --header-file=$*.h -o $*.c $**

src\sv_parser\addr.c src\sv_parser\addr.h: src\sv_parser\addr.y
	$(BISON) $(YFLAGS) -o $*.c $**

src\sv_parser\header.c src\sv_parser\header.h: src\sv_parser\header.y
	$(BISON) $(YFLAGS) -o $*.c $**

src\sv_parser\sieve.c src\sv_parser\sieve.h: src\sv_parser\sieve.y
	$(BISON) $(YFLAGS) -o $*.c $**

build\libsieve.lib: src\sv_interface\callbacks2.obj src\sv_interface\context2.obj src\sv_interface\message.obj src\sv_interface\message2.obj src\sv_interface\script.obj src\sv_interface\script2.obj src\sv_interface\tree.obj src\sv_parser\addr.obj src\sv_parser\addr-lex.obj src\sv_parser\comparator.obj src\sv_parser\header.obj src\sv_parser\header-lex.obj src\sv_parser\sieve.obj src\sv_parser\sieve-lex.obj src\sv_regex\regex.obj src\sv_util\exception.obj src\sv_util\md5.obj src\sv_util\util.obj
	$(AR) /nologo /out:$@ $(**F)

