# Project: MatrixSS
# Makefile created by Dev-C++ 5.9.2

CPP      = g++.exe
WINDRES  = windres.exe
RES      = ./res/MatrixSS.res
OBJ      = matrix.o $(RES)
LINKOBJ  = matrix.o $(RES)
LIBS     = -L"C:/Dev-Cpp/MinGW64/lib32" -L"C:/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib32" -mwindows -lgdi32 -m32 -s
INCS     = -I"C:/Dev-Cpp/MinGW64/include" -I"C:/Dev-Cpp/MinGW64/x86_64-w64-mingw32/include" -I"C:/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.8.1/include"
CXXINCS  = -I"C:/Dev-Cpp/MinGW64/include" -I"C:/Dev-Cpp/MinGW64/x86_64-w64-mingw32/include" -I"C:/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.8.1/include" -I"C:/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.8.1/include/c++"
BIN      = MatrixSS.scr
CXXFLAGS = $(CXXINCS) -Os -m32 -Wall -Wextra -pipe
RM       = rm.exe -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

matrix.o: matrix.cpp
	$(CPP) -c matrix.cpp -o matrix.o $(CXXFLAGS)

./res/MatrixSS.res: ./res/ScriptMatrix.rc
	$(WINDRES) -i ./res/ScriptMatrix.rc -F pe-i386 --input-format=rc -o ./res/MatrixSS.res -O coff  --include-dir ./res
