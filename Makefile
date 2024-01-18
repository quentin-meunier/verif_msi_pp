

CUR_DIR=$(shell pwd)
PYTHONLIB:=verif_msi_pp$(shell python3.9-config --extension-suffix)
all: libverif_msi_pp.a libverif_msi_pp.so $(PYTHONLIB)

#CFLAGS=-g -std=c++20 -Wall -O0 -fPIC
CFLAGS=-std=c++20 -Wall -O3 -fPIC
INCLUDES=



libverif_msi_pp.a: node.o arrayexp.o SHA256.o utils.o simplify.o check_leakage.o tps.o concrev.o hw.o
	ar rcs $@ $^

libverif_msi_pp.so: node.o arrayexp.o SHA256.o utils.o simplify.o check_leakage.o tps.o concrev.o hw.o
	g++ $^ -shared -o $@

$(PYTHONLIB): libverif_msi_pp.so pybind11_wrapper.cpp
	g++ -O3 -Wall -Werror -shared -std=c++20 -fPIC `python3 -m pybind11 --includes` -I. pybind11_wrapper.cpp -o $@ -L. -lverif_msi_pp -Wl,-rpath,$(CUR_DIR)


SHA256.o: SHA256.cpp SHA256.hpp
	g++ -c $(CFLAGS) $(INCLUDES) -o $@ $<

arrayexp.o: arrayexp.cpp arrayexp.hpp node.hpp
	g++ -c $(CFLAGS) $(INCLUDES) -o $@ $<

tps.o: tps.cpp tps.hpp node.hpp simplify.hpp
	g++ -c $(CFLAGS) $(INCLUDES) -o $@ $<

node.o: node.cpp node.hpp config.hpp arrayexp.hpp SHA256.hpp
	g++ -c $(CFLAGS) $(INCLUDES) -o $@ $<

concrev.o: concrev.cpp concrev.hpp node.hpp utils.hpp arrayexp.hpp
	g++ -c $(CFLAGS) $(INCLUDES) -o $@ $<

simplify.o: simplify.cpp simplify.hpp node.hpp SHA256.hpp arrayexp.hpp
	g++ -c $(CFLAGS) $(INCLUDES) -o $@ $<

hw.o: hw.cpp hw.hpp node.hpp simplify.hpp check_leakage.hpp tps.hpp config.hpp
	g++ -c $(CFLAGS) $(INCLUDES) -o $@ $<

check_leakage.o: check_leakage.cpp check_leakage.hpp node.hpp utils.hpp tps.hpp simplify.hpp
	g++ -c $(CFLAGS) $(INCLUDES) -o $@ $<

utils.o: utils.cpp utils.hpp node.hpp arrayexp.hpp simplify.hpp check_leakage.hpp config.hpp hw.hpp
	g++ -c $(CFLAGS) $(INCLUDES) -o $@ $<


clean:
	rm -f *.o


