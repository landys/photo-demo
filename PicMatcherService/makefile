CXX=g++
CXXFLAGS=-fPIC -O3

JDK_INCLUDE=/usr/lib/jvm/java-1.5.0-sun-1.5.0.10/include/
INCS=-I../SIFTAPI -I../E2LSH2 -I$(JDK_INCLUDE) -I$(JDK_INCLUDE)/linux
LDFLAGS=-fPIC -shared -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

BINDIR=bin
OBJDIR=obj
SIFTAPI_OBJDIR=../SIFTAPI/obj
E2LSH_OBJDIR=../E2LSH2/obj
TARGET=libPicMatcherService.so

SUBDIRS=../SIFTAPI ../E2LSH2

OBJS=PicMatcherService.o
SIFTAPI_OBJS=imgfeatures.o sift.o siftfeat.o utils.o
E2LSH_OBJS=BucketHashing.o \
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
$(TARGET):$(OBJS) subdirs
	$(CXX) $(LDFLAGS) -o $(BINDIR)/$(TARGET) $(patsubst %, $(OBJDIR)/%, $(OBJS)) \
$(patsubst %, $(SIFTAPI_OBJDIR)/%, $(SIFTAPI_OBJS)) \
$(patsubst %, $(E2LSH_OBJDIR)/%, $(E2LSH_OBJS))

PicMatcherService.o:PicMatcherService.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJDIR)/$@

.PHONY:subdirs $(SUBDIRS)
subdirs:$(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY:clean
clean:
	-rm -rf $(OBJDIR)
	-rm -rf $(BINDIR)
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done
