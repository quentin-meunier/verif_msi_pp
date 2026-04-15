

CUR_DIR=$(shell pwd)
INC_DIR=include
SRC_DIR=src
OBJ_DIR=obj

BUILD_PYTHON ?= 1

PYTHONLIB := _core$(shell python3-config --extension-suffix)


SRC_FILES=$(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))


PYTHON_TARGET :=
SRC_FILES := $(filter-out $(SRC_DIR)/pybind11_wrapper.cpp,$(SRC_FILES))
ifeq ($(BUILD_PYTHON),1)
PYTHON_TARGET := $(PYTHONLIB)
endif

all: create_objdir libverif_msi_pp.a libverif_msi_pp.so $(PYTHON_TARGET)

CC=g++
#CFLAGS=-g -std=c++20 -Wall -O0 -fPIC -Wno-vla-cxx-extension
CFLAGS=-std=c++20 -Wall -O3 -fPIC -Wno-vla-cxx-extension
INCLUDES=-I$(INC_DIR)


create_objdir:
	mkdir -p $(OBJ_DIR)

libverif_msi_pp.a: $(OBJ_FILES)
	ar rcs $@ $^

libverif_msi_pp.so: $(OBJ_FILES)
	$(CC) $^ -shared -o $@

$(PYTHONLIB): libverif_msi_pp.so $(SRC_DIR)/pybind11_wrapper.cpp
	$(CC) -O3 -Wall -Werror -shared -std=c++20 -fPIC `python3 -m pybind11 --includes` $(INCLUDES) $(SRC_DIR)/pybind11_wrapper.cpp -o $@ -L. -lverif_msi_pp -Wl,-rpath,$(CUR_DIR)


$(OBJ_DIR)/SHA256.o: $(SRC_DIR)/SHA256.cpp $(INC_DIR)/SHA256.hpp
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/arrayexp.o: $(SRC_DIR)/arrayexp.cpp $(INC_DIR)/arrayexp.hpp $(INC_DIR)/node.hpp $(INC_DIR)/config.hpp
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/tps.o: $(SRC_DIR)/tps.cpp $(INC_DIR)/tps.hpp $(INC_DIR)/node.hpp $(INC_DIR)/simplify.hpp $(INC_DIR)/config.hpp $(INC_DIR)/utils_private.hpp
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/node.o: $(SRC_DIR)/node.cpp $(INC_DIR)/node.hpp $(INC_DIR)/config.hpp $(INC_DIR)/arrayexp.hpp $(INC_DIR)/SHA256.hpp
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/concrev.o: $(SRC_DIR)/concrev.cpp $(INC_DIR)/concrev.hpp $(INC_DIR)/node.hpp $(INC_DIR)/utils.hpp $(INC_DIR)/utils_private.hpp $(INC_DIR)/arrayexp.hpp $(INC_DIR)/config.hpp
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/simplify.o: $(SRC_DIR)/simplify.cpp $(INC_DIR)/simplify.hpp $(INC_DIR)/simp_conc.hpp $(INC_DIR)/node.hpp $(INC_DIR)/SHA256.hpp $(INC_DIR)/arrayexp.hpp $(INC_DIR)/config.hpp
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/simp_conc.o: $(SRC_DIR)/simp_conc.cpp $(INC_DIR)/simp_conc.hpp $(INC_DIR)/node.hpp $(INC_DIR)/config.hpp
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/hw.o: $(SRC_DIR)/hw.cpp $(INC_DIR)/hw.hpp $(INC_DIR)/node.hpp $(INC_DIR)/simplify.hpp $(INC_DIR)/check_leakage.hpp $(INC_DIR)/tps.hpp $(INC_DIR)/config.hpp $(INC_DIR)/utils.hpp
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/check_leakage.o: $(SRC_DIR)/check_leakage.cpp $(INC_DIR)/check_leakage.hpp $(INC_DIR)/node.hpp $(INC_DIR)/utils.hpp $(INC_DIR)/tps.hpp $(INC_DIR)/simplify.hpp $(INC_DIR)/config.hpp
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/utils.o: $(SRC_DIR)/utils.cpp $(INC_DIR)/utils.hpp $(INC_DIR)/node.hpp $(INC_DIR)/arrayexp.hpp $(INC_DIR)/simplify.hpp $(INC_DIR)/check_leakage.hpp $(INC_DIR)/config.hpp $(INC_DIR)/hw.hpp
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/simp_rules.o: $(SRC_DIR)/simp_rules.cpp $(INC_DIR)/simp_rules.hpp $(INC_DIR)/node.hpp $(INC_DIR)/config.hpp
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/utils_private.o: $(SRC_DIR)/utils_private.cpp $(INC_DIR)/utils_private.hpp $(INC_DIR)/node.hpp $(INC_DIR)/config.hpp
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<



clean:
	rm -f $(OBJ_DIR)/*.o

realclean: clean
	rm -rf $(OBJ_DIR) libverif_msi_pp.a libverif_msi_pp.so 

