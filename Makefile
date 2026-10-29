CXX := g++
CXXWARNINGS := -Wall -Wextra -Werror
CXXOPT := -O3
CXXSTD := -std=c++17
INCLUDES := -I include -I external/include -I $(shell root-config --incdir)

CXXFLAGS := $(CXXWARNINGS) $(CXXSTD) $(CXXOPT) $(INCLUDES)
LDFLAGS := $(shell root-config --libs)

# Variables used to generate/compile a root dictonary 
HEADERS := include/Observation.h
SOURCES := src/Observation.cxx
ROOTSYS := $(shell root-config --prefix)

.PHONY: all clean

all: main

# Generate and compile root dictonary, see <https://root.cern/manual/io_custom_classes/#selecting-dictionary-entries-linkdefh>
# Do NOT have space after -I flag
ObservationDict.cxx: $(HEADERS) include/Linkdef.h
	rootcling -f $@ -c -Iinclude -Iexternal/include -I$(shell root-config --incdir) -p $^

libObservation.so: ObservationDict.cxx $(SOURCES)
	$(CXX) -fPIC -shared -o$@ $(CXXFLAGS) -I$(ROOTSYS)/include $^ $(shell root-config --ldflags --libs)

main: main.cxx src/DataExtraction.o src/Observation.o src/analysis1.o src/analysis2.o src/analysis3.o src/analysis4.o ObservationDict.cxx
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.cxx
	$(CXX) $(CXXFLAGS) $^ -c -o $@

clean:
	rm -v src/*.o datasets/*.csv datasets/*.dat datasets/*.txt libObservation.so ObservationDict.cxx ObservationDict_rdict.pcm temperature_tree.root main
	
