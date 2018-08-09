BUILD_DIR ?= ./build
SRC_DIRS ?= ../
GTEST_DIR = ../../../../../../extsrc/googletest

SRCS := $(shell find $(SRC_DIRS) -name "*.cpp" -or -name "*.c")
TMP := $(notdir $(SRCS:%=%.o))
OBJS := $(TMP:%=$(BUILD_DIR)/%)
DEPS := $(OBJS:.o=.d)

TEST_SRC := $(shell find $(SRC_DIRS) -name "*.cpp" -path "*/test/*")
TEST_NAME := $(notdir $(basename $(TEST_SRC)))
TEST_INC := -I$(GTEST_DIR)/googletest/include -I$(GTEST_DIR)/googlemock/include

INC_DIRS := $(shell find $(SRC_DIRS)/.. -type d)
INC_FLAGS := $(addprefix -I, $(INC_DIRS))

all : $(BUILD_DIR)/$(TEST_NAME)
	./$<

$(BUILD_DIR)/$(TEST_NAME): $(OBJS)
	@echo "$(OBJS)"
	g++ -o $@ $(OBJS) -L$(GTEST_DIR)/googlemock/gtest -lgtest -lgtest_main -L$(GTEST_DIR)/googlemock -lgmock -lgmock_main -lpthread

$(BUILD_DIR)/%.cpp.o: %.cpp 
	$(MKDIR_P) $(dir $@)
	g++ $(INC_FLAGS) $(TEST_INC) -o $@ -c $< -D DEBUG_GTEST

$(BUILD_DIR)/%.cpp.o: $(SRC_DIRS)/%.cpp 
	$(MKDIR_P) $(dir $@)
	g++ $(INC_FLAGS) $(TEST_INC) -o $@ -c $< -D DEBUG_GTEST

.PHONY: clean

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
