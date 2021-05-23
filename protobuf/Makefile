PROJ_NAME := protobuf

CXX := g++
CXXFLAGS := -pthread -I/usr/local/include $(CXX_STD_FLAGS)

PROTOC := protoc

DEF_DIR := ./def
SRC_DIR := ./src
OBJ_DIR := $(if $(G_OBJ_DIR),$(G_OBJ_DIR)/$(PROJ_NAME),./obj)

DEF_FILES := $(shell find $(DEF_DIR) -name *.proto)
SRC_FILES := $(patsubst $(DEF_DIR)/%.proto,$(SRC_DIR)/%.pb.cc,$(DEF_FILES))
OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%.o,$(SRC_FILES))

build_obj: $(OBJ_FILES)


# *.cc.o -> *.cc
$(OBJ_DIR)/%.cc.o: $(SRC_DIR)/%.cc | obj_dir_check
	$(CXX) $(CXXFLAGS) -c $^ -o $@


build_proto: $(SRC_FILES)

# *.pb.cc -> *.proto
$(SRC_DIR)/%.pb.cc: $(DEF_DIR)/%.proto | src_dir_check
	$(PROTOC) -I$(DEF_DIR) --cpp_out=$(SRC_DIR) $^


obj_dir_check:
	@mkdir -p $(OBJ_DIR)

src_dir_check:
	@mkdir -p $(SRC_DIR)


.PHONY: clean

clean:
	rm -rf $(SRC_DIR)
	rm -rf $(OBJ_DIR)
