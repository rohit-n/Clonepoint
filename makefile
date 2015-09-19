SYS := $(shell gcc -dumpmachine)

CXX=g++
CFLAGS= -Wall -pedantic
OBJECTS = statemanager.o audio.o locator.o draw.o static_sprites.o animations.o vec.o matrix.o map.o texture.o button.o scene.o scene_guards.o scene_trace.o bindings.o \
scene_physics.o scene_saved_game.o intersect.o state.o file.o entity.o enemy.o linkableentity.o player.o elevators.o fieldofview.o config.o font.o \
livingentity.o stairs.o menustate.o mainmenustate.o creditsstate.o gamestate.o levelendstate.o pausestate.o loadmapstate.o optionsstate.o upgradesstate.o sprite.o \
tinyxml/tinyxml.o tinyxml/tinystr.o tinyxml/tinyxmlparser.o tinyxml/tinyxmlerror.o

ifneq (, $(findstring mingw, $(SYS)))
LIBS = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lopenal32
DELCMD = del *.o clonepoint.exe && cd tinyxml/ && del *.o
MAKECMD = mingw32-make
else
LIBS = `sdl2-config --libs` -lGL -lopenal
DELCMD = rm -f *.o clonepoint && cd tinyxml/ && make clean
MAKECMD = make
endif

all: tinyxmldir game

tinyxmldir:
	cd tinyxml/ && $(MAKECMD)

game: main.cpp $(OBJECTS)
	$(CXX) main.cpp $(OBJECTS) $(LIBS) $(CFLAGS) -o clonepoint

vec.o: vec.cpp vec.h
	$(CXX) vec.cpp $(CFLAGS) -c

matrix.o: matrix.cpp matrix.h vec.o
	$(CXX) matrix.cpp $(CFLAGS) -c

file.o: file.cpp file.h
	$(CXX) file.cpp $(CFLAGS) -c

audio.o: audio.cpp audio.h
	$(CXX) audio.cpp $(CFLAGS) -c

locator.o: locator.cpp locator.h audio.o
	$(CXX) locator.cpp $(CFLAGS) -c

button.o: button.cpp button.h
	$(CXX) button.cpp $(CFLAGS) -c

state.o: state.cpp state.h locator.o
	$(CXX) state.cpp $(CFLAGS) -c

statemanager.o: statemanager.cpp statemanager.h gamestate.o mainmenustate.o creditsstate.o levelendstate.o pausestate.o loadmapstate.o upgradesstate.o
	$(CXX) statemanager.cpp $(CFLAGS) -c

entity.o: entity.cpp entity.h vec.o sprite.o
	$(CXX) entity.cpp $(CFLAGS) -c

enemy.o: enemy.cpp entity.o livingentity.o
	$(CXX) enemy.cpp $(CFLAGS) -c

linkableentity.o: linkableentity.cpp entity.o
	$(CXX) linkableentity.cpp $(CFLAGS) -c

player.o: player.cpp entity.o livingentity.o
	$(CXX) player.cpp $(CFLAGS) -c

elevators.o: elevators.cpp entity.o
	$(CXX) elevators.cpp $(CFLAGS) -c

fieldofview.o: fieldofview.cpp entity.o
	$(CXX) fieldofview.cpp $(CFLAGS) -c

stairs.o: stairs.cpp entity.o
	$(CXX) stairs.cpp $(CFLAGS) -c

livingentity.o: livingentity.cpp entity.o
	$(CXX) livingentity.cpp $(CFLAGS) -c

scene.o: scene.cpp scene.h locator.o entity.o map.o
	$(CXX) scene.cpp $(CFLAGS) -c

scene_guards.o: scene_guards.cpp scene.o
	$(CXX) scene_guards.cpp $(CFLAGS) -c

scene_physics.o: scene_physics.cpp scene.o
	$(CXX) scene_physics.cpp $(CFLAGS) -c

scene_saved_game.o: scene_saved_game.cpp scene.o
	$(CXX) scene_saved_game.cpp $(CFLAGS) -c

scene_trace.o: scene_trace.cpp scene.o
	$(CXX) scene_trace.cpp $(CFLAGS) -c

intersect.o: intersect.cpp intersect.h vec.o
	$(CXX) intersect.cpp $(CFLAGS) -c

gamestate.o: gamestate.cpp gamestate.h state.o scene.o bindings.o
	$(CXX) gamestate.cpp $(CFLAGS) -c

menustate.o: menustate.cpp menustate.h state.o button.o
	$(CXX) menustate.cpp $(CFLAGS) -c

mainmenustate.o: mainmenustate.cpp mainmenustate.h menustate.o
	$(CXX) mainmenustate.cpp $(CFLAGS) -c

levelendstate.o: levelendstate.cpp levelendstate.h menustate.o
	$(CXX) levelendstate.cpp $(CFLAGS) -c

pausestate.o: pausestate.cpp pausestate.h menustate.o
	$(CXX) pausestate.cpp $(CFLAGS) -c

creditsstate.o: creditsstate.cpp creditsstate.h menustate.o
	$(CXX) creditsstate.cpp $(CFLAGS) -c

loadmapstate.o: loadmapstate.cpp loadmapstate.h menustate.o
	$(CXX) loadmapstate.cpp $(CFLAGS) -c

optionsstate.o: optionsstate.cpp optionsstate.h menustate.o
	$(CXX) optionsstate.cpp $(CFLAGS) -c

upgradesstate.o: upgradesstate.cpp upgradesstate.h menustate.o statemanager.h
	$(CXX) upgradesstate.cpp $(CFLAGS) -c

texture.o: texture.cpp texture.h
	$(CXX) texture.cpp $(CFLAGS) -c

draw.o: draw.cpp draw.h texture.o gamestate.o menustate.o
	$(CXX) draw.cpp $(CFLAGS) -c

static_sprites.o: static_sprites.cpp static_sprites.h
	$(CXX) static_sprites.cpp $(CFLAGS) -c

map.o: map.cpp map.h entity.o
	$(CXX) map.cpp $(CFLAGS) -c

sprite.o: sprite.cpp sprite.h texture.o
	$(CXX) sprite.cpp $(CFLAGS) -c

animations.o: animations.cpp animations.h
	$(CXX) animations.cpp $(CFLAGS) -c

bindings.o: bindings.cpp bindings.h
	$(CXX) bindings.cpp $(CFLAGS) -c

config.o: config.cpp config.h
	$(CXX) config.cpp $(CFLAGS) -c

font.o: font.cpp font.h
	$(CXX) font.cpp $(CFLAGS) -c

clean:
	$(DELCMD)