GBENCH_PREFIX := /opt/google-benchmark
GBENCH_LDFLAGS := -L$(GBENCH_PREFIX)/lib
GBENCH_CPPFLAGS := -I$(GBENCH_PREFIX)/include

OPT := -O2
CXXFLAGS := $(OPT) -std=c++11 -Wall -pthread -g
CPPFLAGS := $(GBENCH_CPPFLAGS)
LDFLAGS := $(GBENCH_LDFLAGS)
LDLIBS := -lbenchmark

Link_Benchmark = $(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

tls: tls.o
	$(Link_Benchmark)

.PHONY: clean
clean:
	rm -f *.o tls
