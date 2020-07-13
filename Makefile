OBJS = PendulumProject.cpp

OBJ_NAME = SDLMain

all : $(OBJS)
		g++ $(OBJS) -IC:\SDL2-devel-2.0.12-mingw\SDL2-2.0.12\i686-w64-mingw32\include -LC:\SDL2-devel-2.0.12-mingw\SDL2-2.0.12\i686-w64-mingw32\lib -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -o $(OBJ_NAME)
