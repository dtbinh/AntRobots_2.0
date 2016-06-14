CXX=g++
LIB=bcm2835
CCFLAGS=-c -g
LDFLAGS=-g
SRC=src/
BLD=build/
EXE=run
INC= -I /usr/include/libusb-1.0/
LIBS=-L/usr/lib/ -lboost_system -lboost_thread -lboost_chrono -lusb-1.0 -lpixyusb
OBJS=$(BLD)encoders.o $(BLD)arduino_comm.o $(BLD)motor_control.o $(BLD)vision.o $(BLD)state_machine.o $(BLD)main.o

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(LDFLAGS) $(INC) $(BLD)*.o $(LIBS) -o $(EXE) -l $(LIB)

$(BLD)encoders.o: $(SRC)encoders.cpp $(SRC)encoders.h $(SRC)defs.h
	$(CXX) $(CCFLAGS) $(SRC)encoders.cpp
	@-mv *.o $(BLD)

$(BLD)arduino_comm.o: $(SRC)arduino_comm.cpp $(SRC)arduino_comm.h $(SRC)defs.h
	$(CXX) $(CCFLAGS) $(SRC)arduino_comm.cpp
	@-mv *.o $(BLD)

$(BLD)motor_control.o: $(SRC)motor_control.cpp $(SRC)motor_control.h $(SRC)defs.h
	$(CXX) $(CCFLAGS) $(SRC)motor_control.cpp
	@-mv *.o $(BLD)

$(BLD)vision.o: $(SRC)vision.cpp $(SRC)vision.h $(SRC)defs.h
	$(CXX) $(CCFLAGS) $(INC) $(SRC)vision.cpp
	@-mv *.o $(BLD)

$(BLD)state_machine.o: $(SRC)state_machine.cpp $(SRC)state_machine.h $(SRC)defs.h
	$(CXX) $(CCFLAGS) $(INC) $(SRC)state_machine.cpp
	@-mv *.o $(BLD)

$(BLD)main.o: $(SRC)main.cpp $(SRC)*.h
	$(CXX) $(CCFLAGS) $(INC) $(SRC)main.cpp
	@-mv *.o $(BLD)

rebuild: clean all

.PHONY: clean
clean:
	@-rm -rf $(BLD)*.o $(EXE)
