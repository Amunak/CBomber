CXX=g++
LD=g++
CFLAGS=-Wall -pedantic -Wno-long-long -std=c++11
LDLIB=-lmenu -lpanel -lncurses
SRC=src
OUT=bomber

all: compile doc

compile: Blocks.o CBomb.o CCoord.o CEntity.o CGame.o CNetwork.o CPickup.o CScreen.o CTile.o CWorld.o LivingEntities.o main.o
	$(LD) $(CFLAGS) $^ -o $(OUT) $(LDLIB)

#compile:
#	$(LD) $(CFLAGS) -o $(OUT) $(SRC)/*.cpp $(LDLIB)

doc:
	doxygen Doxyfile

run: compile
	./$(OUT)

clean:
	rm -f *.o $(OUT)
	rm -fr doc/

main.o:
	g++ -Wall -pedantic -Wno-long-long -O0 -ggdb -std=c++11 -c -o main.o src/main.cpp

%.o: $(SRC)/%.cpp $(SRC)/%.h
	$(CXX) $(CFLAGS) -c -o $@ $<

Blocks.o: $(SRC)/Blocks.cpp $(SRC)/Blocks.h $(SRC)/CTile.h
CBomb.o: $(SRC)/CBomb.cpp $(SRC)/CBomb.h $(SRC)/CEntity.h $(SRC)/CTile.h $(SRC)/CCoord.h $(SRC)/LivingEntities.h $(SRC)/CWorld.h $(SRC)/Blocks.h $(SRC)/CPickup.h $(SRC)/CGame.h
CCoord.o: $(SRC)/CCoord.cpp $(SRC)/CCoord.h $(SRC)/CTile.h $(SRC)/CGame.h $(SRC)/CWorld.h $(SRC)/Blocks.h $(SRC)/CEntity.h $(SRC)/LivingEntities.h $(SRC)/CPickup.h
CEntity.o: $(SRC)/CEntity.cpp $(SRC)/CEntity.h $(SRC)/CTile.h $(SRC)/CCoord.h $(SRC)/CGame.h $(SRC)/CWorld.h $(SRC)/Blocks.h $(SRC)/LivingEntities.h $(SRC)/CPickup.h
CGame.o: $(SRC)/CGame.cpp $(SRC)/CGame.h $(SRC)/CWorld.h $(SRC)/Blocks.h $(SRC)/CTile.h $(SRC)/CCoord.h $(SRC)/CEntity.h $(SRC)/LivingEntities.h $(SRC)/CPickup.h $(SRC)/CNetwork.h
CNetwork.o: $(SRC)/CNetwork.cpp $(SRC)/CNetwork.h $(SRC)/CGame.h $(SRC)/CWorld.h $(SRC)/Blocks.h $(SRC)/CTile.h $(SRC)/CCoord.h $(SRC)/CEntity.h $(SRC)/LivingEntities.h $(SRC)/CPickup.h
CPickup.o: $(SRC)/CPickup.cpp $(SRC)/CPickup.h $(SRC)/CEntity.h $(SRC)/CTile.h $(SRC)/CCoord.h
CScreen.o: $(SRC)/CScreen.cpp $(SRC)/CScreen.h $(SRC)/CCoord.h
CTile.o: $(SRC)/CTile.cpp $(SRC)/CTile.h
CWorld.o: $(SRC)/CWorld.cpp $(SRC)/CWorld.h $(SRC)/Blocks.h $(SRC)/CTile.h $(SRC)/CCoord.h $(SRC)/CEntity.h $(SRC)/LivingEntities.h $(SRC)/CPickup.h $(SRC)/CGame.h $(SRC)/CBomb.h
LivingEntities.o: $(SRC)/LivingEntities.cpp $(SRC)/LivingEntities.h $(SRC)/CEntity.h $(SRC)/CTile.h $(SRC)/CCoord.h $(SRC)/CWorld.h $(SRC)/Blocks.h $(SRC)/CPickup.h $(SRC)/CGame.h $(SRC)/CBomb.h
main.o: $(SRC)/main.cpp $(SRC)/CGame.h $(SRC)/CWorld.h $(SRC)/Blocks.h $(SRC)/CTile.h $(SRC)/CCoord.h $(SRC)/CEntity.h $(SRC)/LivingEntities.h $(SRC)/CPickup.h $(SRC)/CNetwork.h
