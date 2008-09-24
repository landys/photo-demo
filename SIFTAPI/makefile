CXX=g++
CXXFLAGS=-fPIC -O3
INCS=-I/usr/local/include/opencv
LDFLAGS=-fPIC -shared
#-L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

BINDIR=bin
OBJDIR=obj
TARGET=SIFTAPI.so

OBJS=imgfeatures.o sift.o siftfeat.o utils.o

all:init $(TARGET)

.PHONY:init
init:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
$(TARGET):$(OBJS)
#	$(CXX) $(LDFLAGS) -o $(BINDIR)/$(TARGET) $(patsubst %, $(OBJDIR)/%, $(OBJS))

utils.o:utils.cpp utils.h
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

imgfeatures.o:imgfeatures.cpp imgfeatures.h utils.h
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

sift.o:sift.cpp sift.h imgfeatures.h utils.h
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

siftfeat.o:siftfeat.cpp sift.cpp sift.h imgfeatures.h utils.h
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

.PHONY:clean
clean:
	-rm -rf $(OBJDIR)
	-rm -rf $(BINDIR)
