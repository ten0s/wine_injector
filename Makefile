CFLAGS := -g -O0
MINGWCC := x86_64-w64-mingw32-gcc

all: main.c
	$(MINGWCC) $(CFLAGS) main.c -o wine_injector.exe

clean:
	rm -f *.exe
