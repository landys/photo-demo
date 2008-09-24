CXX=g++
CXXFLAGS=-fPIC -O3
INCS=
LDFLAGS=-fPIC -shared

BINDIR=bin
OBJDIR=obj
TARGET=E2LSH.so

OBJS=BucketHashing.o \
Geometry.o \
GlobalVars.o \
LocalitySensitiveHashing.o \
LSHMain.o \
NearNeighbors.o \
Random.o \
SelfTuning.o \
Util.o

all:init $(TARGET)

.PHONY:init
init:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
$(TARGET):$(OBJS)
#	$(CXX) $(LDFLAGS) -o $(BINDIR)/$(TARGET) $(patsubst %, $(OBJDIR)/%, $(OBJS))

BucketHashing.o:BucketHashing.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

erf.o:erf.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

genDS.o:genDS.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

genPlantedDS.o:genPlantedDS.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

Geometry.o:Geometry.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

GlobalVars.o:GlobalVars.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

LocalitySensitiveHashing.o:LocalitySensitiveHashing.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

LSHMain.o:LSHMain.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

NearNeighbors.o:NearNeighbors.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

Random.o:Random.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

SelfTuning.o:SelfTuning.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

Util.o:Util.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

.PHONY:clean
clean:
	-rm -rf $(OBJDIR)
	-rm -rf $(BINDIR)
