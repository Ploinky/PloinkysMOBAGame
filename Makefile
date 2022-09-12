SRC_DIR := code/src
OBJ_DIR := obj
PS_SRC := code/shaders/pixel
VS_SRC := code/shaders/vertex
PS_VS_DEST := build/shaders
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
PS_IN := $(wildcard $(PS_SRC)/*.hlsl)
PS_OUT := $(patsubst $(PS_SRC)/%.hlsl,$(PS_VS_DEST)/%.cso,$(PS_IN))
VS_IN := $(wildcard $(VS_SRC)/*.hlsl)
VS_OUT := $(patsubst $(VS_SRC)/%.hlsl,$(PS_VS_DEST)/%.cso,$(VS_IN))
EXE_FILE := build/PloinkysMOBAGame.exe
FOLDERS = build build/data build/shaders build/obj
all: $(FOLDERS) $(PS_OUT) $(VS_OUT) $(EXE_FILE)

$(FOLDERS):
	if not exist .\build mkdir .\build
	if not exist .\build\shaders mkdir .\build\shaders
	if not exist .\obj mkdir .\obj
	if not exist .build\data xcopy data .\build\data /I /S /Y

$(EXE_FILE): $(OBJ_FILES)
	g++ -Wall -std=c++17 $^ -I code/include -o $@ -ld3d11 -lws2_32 -municode

obj/%.o: code/src/%.cpp code/include/%.hpp
	g++ -Wall -std=c++17 $< -c -I code/include -o $@

$(PS_OUT): $(PS_IN)
	fxc -T ps_5_0 /Fo $@ $<

$(VS_OUT): $(VS_IN)
	fxc -T vs_5_0 /Fo $@ $<

run:
	.\build\PloinkysMOBAGame.exe

clean:
	if exist .\build rmdir /S /Q build
	if exist .\obj rmdir /S /Q obj