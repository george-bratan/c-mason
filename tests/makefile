MAIN_NAME := main
MAIN_SRCS := $(filter-out test.cpp, $(wildcard *.cpp))
MAIN_OBJS := ${MAIN_SRCS:.cpp=.o}

TEST_NAME := test
TEST_SRCS := $(filter-out main.cpp, $(wildcard *.cpp))
TEST_OBJS := ${TEST_SRCS:.cpp=.o}

INCLUDE_DIRS := /usr/local/include
LIBRARY_DIRS := /usr/local/lib
LIBRARIES := curl mysqlcppconn

CXXFLAGS += $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir))
CXXFLAGS += -g -std=c++14
LDFLAGS += $(foreach librarydir,$(LIBRARY_DIRS),-L$(librarydir))
LDLIBS += $(foreach library,$(LIBRARIES),-l$(library))

.PHONY: all clean

all: ${MAIN_NAME} ${TEST_NAME}

$(MAIN_NAME): $(MAIN_OBJS)
	$(LINK.cc) $(MAIN_OBJS) -o $(MAIN_NAME) $(LDLIBS)

$(TEST_NAME): $(TEST_OBJS)
	$(LINK.cc) $(TEST_OBJS) -o $(TEST_NAME) $(LDLIBS)

clean:
	@- rm -rf $(MAIN_OBJS) $(TEST_OBJS) $(MAIN_NAME) $(TEST_NAME)