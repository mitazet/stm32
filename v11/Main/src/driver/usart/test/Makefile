BUILD_DIR ?= ./build
SRC_DIRS ?= ../
GTEST_DIR = ../../../../../../extsrc/googletest

SRCS := $(shell find $(SRC_DIRS) -name "*.cpp" -not -path "*/test/*" -or -name "*.c")
OBJS := $(notdir $(SRCS:%=%.o))
DEPS := $(OBJS:.o=.d)

TEST_SRC := $(shell find $(SRC_DIRS) -name "*.cpp" -path "*/test/*")
TEST_NAME := $(notdir $(basename $(TEST_SRC)))
TEST_INC := -I$(GTEST_DIR)/googletest/include -I$(GTEST_DIR)/googlemock/include

INC_DIRS := $(shell find $(SRC_DIRS)/.. -type d)
INC_FLAGS := $(addprefix -I, $(INC_DIRS))

all : $(BUILD_DIR)/$(TEST_NAME)
	./$<

$(BUILD_DIR)/$(TEST_NAME): $(TEST_SRC) $(BUILD_DIR)/$(OBJS)
	g++ -o $@ $< $(INC_FLAGS) $(TEST_INC) -L$(GTEST_DIR)/googlemock/gtest -lgtest -lgtest_main -L$(GTEST_DIR)/googlemock -lgmock -lgmock_main -lpthread $(BUILD_DIR)/$(OBJS) -D DEBUG_GTEST

$(BUILD_DIR)/$(OBJS): $(SRCS)
	$(MKDIR_P) $(dir $@)
	g++ $(INC_FLAGS) -o $@ -c $< -D DEBUG_GTEST

.PHONY: clean

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
