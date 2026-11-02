

CUR_DIR=$(shell pwd)
INC_DIR=include
SRC_DIR=src
OBJ_DIR=obj

BUILD_PYTHON ?= 1

PYTHONLIB := _core$(shell python3-config --extension-suffix)


SRC_FILES := $(filter-out $(SRC_DIR)/pybind11_wrapper.cpp,$(wildcard $(SRC_DIR)/*.cpp))
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
DEP_FILES := $(OBJ_FILES:.o=.d) $(OBJ_DIR)/pybind11_wrapper.d


PYTHON_TARGET :=
HAS_PYBIND11 := $(shell python3 -m pybind11 --includes 2>/dev/null)
ifeq ($(BUILD_PYTHON),1)
ifneq ($(HAS_PYBIND11),)
PYTHON_TARGET := $(PYTHONLIB)
endif
endif

all: create_objdir libverif_msi_pp.a libverif_msi_pp.so $(PYTHON_TARGET)

#CFLAGS=-g -std=c++20 -Wall -O0 -fPIC -Wno-vla-cxx-extension -MMD -MP
CFLAGS=-std=c++20 -Wall -O3 -fPIC -Wno-vla-cxx-extension -MMD -MP
INCLUDES=-I$(INC_DIR)


create_objdir:
	mkdir -p $(OBJ_DIR)

libverif_msi_pp.a: $(OBJ_FILES)
	ar rcs $@ $^

libverif_msi_pp.so: $(OBJ_FILES)
	$(CXX) $^ -shared -o $@

$(PYTHONLIB): libverif_msi_pp.so $(SRC_DIR)/pybind11_wrapper.cpp | create_objdir
	$(CXX) -O3 -Wall -Werror -shared -std=c++20 -fPIC -MMD -MP -MF $(OBJ_DIR)/pybind11_wrapper.d `python3 -m pybind11 --includes` $(INCLUDES) $(SRC_DIR)/pybind11_wrapper.cpp -o $@ -L. -lverif_msi_pp -Wl,-rpath,$(CUR_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | create_objdir
	$(CXX) -c $(CFLAGS) $(INCLUDES) -o $@ $<

-include $(DEP_FILES)

test: all
	python3 run_tests.py

clean:
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d

realclean: clean
	rm -rf $(OBJ_DIR) libverif_msi_pp.a libverif_msi_pp.so $(PYTHONLIB)

