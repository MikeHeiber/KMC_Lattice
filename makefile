# Copyright (c) 2018 Michael C. Heiber
# This source file is part of the KMC_Lattice project, which is subject to the MIT License.
# For more information, see the LICENSE file that accompanies this software.
# The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

ifeq ($(lastword $(subst /, ,$(CXX))),g++)
	FLAGS += -Wall -Wextra -O3 -std=c++11 -I. -Isrc
endif
ifeq ($(lastword $(subst /, ,$(CXX))),pgc++)
	FLAGS += -O2 -fastsse -Mvect -std=c++11 -Mdalign -Munroll -Mipa=fast -Kieee -m64 -I. -Isrc
endif

OBJS = src/Event.o src/Lattice.o src/Object.o src/Simulation.o src/Site.o src/Utils.o

all : libKMC.a
ifndef FLAGS
	$(error Valid compiler not detected.)
endif

libKMC.a : $(OBJS)
	ar rcs $@ $^

src/Event.o : src/Event.cpp src/Event.h src/Lattice.h src/Object.h src/Simulation.h src/Site.h src/Utils.h
	mpicxx $(FLAGS) -c $< -o $@

src/Lattice.o : src/Lattice.cpp src/Lattice.h src/Site.h src/Utils.h
	mpicxx $(FLAGS) -c $< -o $@

src/Object.o : src/Object.cpp src/Object.h src/Utils.h
	mpicxx $(FLAGS) -c $< -o $@

src/Simulation.o : src/Simulation.cpp src/Simulation.h src/Event.h src/Lattice.h src/Object.h src/Site.h src/Utils.h
	mpicxx $(FLAGS) -c $< -o $@
	
src/Site.o : src/Site.cpp KMC_Lattice/Site.h
	mpicxx $(FLAGS) -c $< -o $@
	
src/Utils.o : KMC_Lattice/Utils.cpp KMC_Lattice/Utils.h
	mpicxx $(FLAGS) -c $< -o $@

#
# Testing Section using googletest
#

ifndef FLAGS
	$(error Valid compiler not detected.)
endif
GTEST_DIR = googletest/googletest
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)
ifeq ($(lastword $(subst /, ,$(CXX))),g++)
	GTEST_FLAGS = -isystem $(GTEST_DIR)/include -pthread
endif
ifeq ($(lastword $(subst /, ,$(CXX))),pgc++)
	GTEST_FLAGS = -I$(GTEST_DIR)/include
endif

test : FLAGS = -fprofile-arcs -ftest-coverage -std=c++11 -Wall -Wextra -I. -Isrc
test : test/KMC_Lattice_tests.exe	
	
test/KMC_Lattice_tests.exe : test/test.o test/gtest-all.o $(OBJS)
	mpicxx $(GTEST_FLAGS) $(FLAGS) -lpthread $^ -o $@

test/gtest-all.o : $(GTEST_SRCS_)
	mpicxx $(GTEST_FLAGS) -I$(GTEST_DIR) $(FLAGS) -c $(GTEST_DIR)/src/gtest-all.cc -o $@
			
test/test.o : test/test.cpp $(GTEST_HEADERS) $(OBJS)
	mpicxx $(GTEST_FLAGS) $(FLAGS) -c $< -o $@

test_mpi : FLAGS = -fprofile-arcs -ftest-coverage -std=c++11 -Wall -Wextra -I. -Isrc
test_mpi : test/KMC_Lattice_MPI_tests.exe

test/Excimontec_MPI_tests.exe : test/test_mpi.o test/gtest-all.o $(OBJS)
	mpicxx $(GTEST_FLAGS) $(FLAGS) -lpthread $^ -o $@

test/test_mpi.o : test/test_mpi.cpp $(GTEST_HEADERS) $(OBJS)
	mpicxx $(GTEST_FLAGS) $(FLAGS) -c $< -o $@
	
clean:
	-rm src/*.o src/*.gcno* src/*.gcda KMC_Lattice/*.o KMC_Lattice/*.gcno* KMC_Lattice/*.gcda testing/*.o testing/*.gcno* testing/*.gcda *~ Excimontec.exe testing/*.o testing/Excimontec_tests.exe testing/Excimontec_mpi_tests.exe
