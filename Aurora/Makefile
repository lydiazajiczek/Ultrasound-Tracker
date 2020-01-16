TARGET_EXEC ?= capisample

OS=$(shell uname -s)
ifndef PLATFORM
  ifeq ($(OS),Darwin)
	  PLATFORM=macosx
	  TARGET_LIB ?= libndicapi.dylib
  else 
	  ifeq ($(OS),Linux)
      PLATFORM=linux
      TARGET_LIB ?= libndicapi.so
	  else
      PLATFORM=x86-win64
      TARGET_LIB ?= ndicapi.dll
	  endif
  endif
endif

BUILD_DIR ?= ./build/$(PLATFORM)
OBJ_DIR ?= ./build/$(PLATFORM)/obj
SRC_DIRS ?= ./library/src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

APP_SRCS := $(shell find ./sample/src -name *.cpp -or -name *.c -or -name *.s)
APP_OBJS := $(APP_SRCS:%=$(OBJ_DIR)/%.o)
APP_DEPS := $(APP_OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d) ./library/include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

# statically linked sample app
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS) $(APP_OBJS) $(BUILD_DIR)/$(TARGET_EXEC)-dyn
#$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS) $(APP_OBJS) $(BUILD_DIR)/$(TARGET_LIB)
	$(MKDIR_P) $(dir $@)
	$(CXX) $(OBJS) $(APP_OBJS) -o $@ $(LDFLAGS)  

# dynamically linked sample app
$(BUILD_DIR)/$(TARGET_EXEC)-dyn: $(BUILD_DIR)/$(TARGET_LIB)  $(APP_OBJS)
	$(MKDIR_P) $(dir $@)
	$(CXX) $(APP_OBJS) -L$(BUILD_DIR) -lndicapi -o $@ $(LDFLAGS)  

# shared library
$(BUILD_DIR)/$(TARGET_LIB): $(OBJS)
	$(MKDIR_P) $(dir $@)
	$(CXX) -L$(BUILD_DIR) $(OBJS) -shared -o $@ $(LDFLAGS) 

# assembly
$(OBJ_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(OBJ_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c $< -o $@

# c++ source
$(OBJ_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC -c $< -o $@


.PHONY: clean all

all: $(BUILD_DIR)/$(TARGET_EXEC)

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
