# Project: scrnsave
# Makefile created by Dev-C++ 5.9.2

CC       = gcc.exe
OBJ      = scrnsave.o
LINKOBJ  = scrnsave.o
LIBS     = -L"C:/Dev-Cpp/MinGW64/lib32" -L"C:/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib32" -m32
INCS     = -I"C:/Dev-Cpp/MinGW64/include" -I"C:/Dev-Cpp/MinGW64/x86_64-w64-mingw32/include" -I"C:/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.8.1/include"
BIN      = libscrnsave.a
CFLAGS   = $(INCS) -Os -m32 -Wall -Wextra -pipe
RM       = rm.exe -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(LINKOBJ)
	ar r $(BIN) $(LINKOBJ)
	ranlib $(BIN)

scrnsave.o: scrnsave.c
	$(CC) -c scrnsave.c -o scrnsave.o $(CFLAGS)
