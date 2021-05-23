PROJ_NAME := client

CXX := g++
CXXFLAGS := -pthread -I/usr/local/include $(CXX_STD_FLAGS)
LDFLAGS := -L/usr/local/lib -lprotobuf -lpthread

SRC_DIR := ./src
OBJ_DIR := $(if $(G_OBJ_DIR),$(G_OBJ_DIR)/$(PROJ_NAME),./obj)
BIN_DIR := $(if $(G_BIN_DIR),$(G_BIN_DIR),./bin)

SRC_FILES := $(shell find $(SRC_DIR) -name *.cpp -or -name *.cc)
OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%.o,$(SRC_FILES))

DEP_OBJ_FILES := $(G_OBJ_DIR)/protobuf/network.pb.cc.o

build_bin: $(BIN_DIR)/$(PROJ_NAME)

# client -> build_obj
$(BIN_DIR)/$(PROJ_NAME): $(OBJ_FILES) | bin_dir_check
	$(CXX) $^ $(DEP_OBJ_FILES) -o $@ $(LDFLAGS)

build_obj: $(OBJ_FILES)

# *.cpp.o -> *.cpp
$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp | obj_dir_check
	$(CXX) $(CXXFLAGS) -c $^ -o $@

# *.cc.o -> *.cc
$(OBJ_DIR)/%.cc.o: $(SRC_DIR)/%.cc | obj_dir_check
	$(CXX) $(CXXFLAGS) -c $^ -o $@

bin_dir_check:
	@mkdir -p $(BIN_DIR)

obj_dir_check:
	@mkdir -p $(OBJ_DIR)

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR)
