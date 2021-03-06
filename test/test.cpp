// Copyright (c) 2017-2020 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "gtest/gtest.h"
#include "Simulation.h"
#include "Utils.h"
#include "Version.h"

using namespace std;
using namespace KMC_Lattice;

// Simple derived Simulation class
class TestSim : public Simulation {
public:
	class CreationEvent : public Event {
	public:
		CreationEvent() : Event() {}
		CreationEvent(Simulation* simulation_ptr) : Event(simulation_ptr) {}
		string getEventType() const { return "Creation"; }
	};
	class MoveEvent : public Event {
	public:
		MoveEvent() : Event() {}
		MoveEvent(Simulation* simulation_ptr) : Event(simulation_ptr) {}
		string getEventType() const { return "Move"; }
	};

	// Data members
	list<Object> objects;
	list<Site> sites;
	CreationEvent event_creation;
	list<MoveEvent> events_move;
	Event event_previous;
	string event_previous_type;
	Coords coords_creation;
	int N_tests = 1;
	int N_steps = 1;
	double k_move = 1.0;
	int N_move_events = 0;
	int N_termination_events = 0;
	vector<int> move_counts;
	vector<double> displacement_data;

	bool init(const Parameters_Simulation& params) {
		// Initialize Simulation base class
		Simulation::init(params, 0);
		// Initialize Sites
		Site site;
		sites.assign(lattice.getNumSites(), site);
		vector<Site*> site_ptrs;
		for (auto& item : sites) {
			site_ptrs.push_back(&item);
		}
		lattice.setSitePointers(site_ptrs);
		// Perform test initialization
		coords_creation = { lattice.getLength() / 2, lattice.getWidth() / 2, lattice.getHeight() / 2 };
		CreationEvent event_creation_new(this);
		event_creation = event_creation_new;
		event_creation.calculateExecutionTime(0.01);
		addEvent(&event_creation);
		// Reserve memory
		displacement_data.reserve(N_tests);
		move_counts.reserve(N_tests);
		return true;
	}

	void calculateNextEvent(Object* object_ptr) {
		auto move_event_it = find_if(events_move.begin(), events_move.end(), [object_ptr](MoveEvent& a) { return a.getObjectPtr() == object_ptr; });
		move_event_it->calculateExecutionTime(k_move);
		Coords coords_i = object_ptr->getCoords();
		Coords coords_f = lattice.chooseRandomUnoccupiedNeighbor(coords_i);
		if (coords_f.x == -1) {
			setObjectEvent(object_ptr, nullptr);
		}
		else {
			move_event_it->setDestCoords(coords_f);
			setObjectEvent(object_ptr, &(*move_event_it));
		}
	}

	bool checkErrorMessageFunctions(string msg_in) {
		setErrorMessage(msg_in);
		string msg_out = getErrorMessage();
		return msg_in.compare(msg_out) == 0;
	}

	bool checkFinished() const {
		return (getN_objects_created() == N_tests);
	}

	Event* determinePathway(const vector<Event*>& possible_events) {
		return Simulation::determinePathway(possible_events);
	}

	Coords executeCreationEvent() {
		Coords coords_dest = coords_creation;
		if (lattice.isOccupied(coords_dest)) {
			coords_dest = lattice.chooseRandomUnoccupiedNeighbor(coords_dest);
		}
		if (coords_dest.x != -1) {
			Object object(getTime(), getN_objects_created(), coords_dest);
			objects.push_back(object);
			addObject(&objects.back());
			Simulation* sim_ptr = this;
			MoveEvent event1(sim_ptr);
			event1.setObjectPtr(&objects.back());
			events_move.push_back(event1);
			move_counts.push_back(0);
		}
		// Calculate next creation event
		event_creation.calculateExecutionTime(0.01);
		return coords_dest;
	}

	void executeMoveEvent(const std::list<Event*>::const_iterator event_it) {
		Event* event_ptr = *event_it;
		moveObject(event_ptr->getObjectPtr(), event_ptr->getDestCoords());
		move_counts[event_ptr->getObjectPtr()->getTag()]++;
		N_move_events++;
	}

	bool executeNextEvent() {
		// Check if any objects are finished
		for (auto& item : objects) {
			if (move_counts[item.getTag()] == N_steps) {
				displacement_data.push_back(item.calculateDisplacement());
				executeTerminationEvent(&item);
				break;
			}
		}
		auto event_it = chooseNextEvent();
		Event* event_ptr = *event_it;
		string event_type = event_ptr->getEventType();
		event_previous = *event_ptr;
		event_previous_type = event_ptr->getEventType();
		Coords coords_i;
		Coords coords_f = event_ptr->getDestCoords();
		setTime(event_ptr->getExecutionTime());
		if (event_type.compare("Creation") == 0) {
			coords_f = executeCreationEvent();
			coords_i = coords_f;
		}
		else {
			coords_i = event_ptr->getObjectPtr()->getCoords();
			coords_f = event_ptr->getDestCoords();
			if (event_type.compare("Move") == 0) {
				executeMoveEvent(event_it);
			}
			else {
				return false;
			}
		}
		if (coords_f.x != -1) {
			auto object_vec = findRecalcObjects(coords_i, coords_f);
			for (auto item : object_vec) {
				calculateNextEvent(item);
			}
		}
		return true;
	}

	void executeTerminationEvent(Object* object_ptr) {
		// Remove object from Simulation class
		removeObject(object_ptr);
		// Remove local move event
		auto move_event_it = find_if(events_move.begin(), events_move.end(), [object_ptr](MoveEvent& a) { return a.getObjectPtr() == object_ptr; });
		events_move.erase(move_event_it);
		// Remove local object
		auto object_it = find_if(objects.begin(), objects.end(), [object_ptr](Object& a) { return a.getTag() == object_ptr->getTag(); });
		objects.erase(object_it);
		N_termination_events++;
	}

	vector<Object*> getAllObjectPtrs() const {
		return Simulation::getAllObjectPtrs();
	}

	vector<Event*> getAllEventPtrs() const {
		return Simulation::getAllEventPtrs();
	}

	Coords getRandomCoords() {
		return lattice.generateRandomCoords();
	}

	void removeEvent(Event* event_ptr) {
		Simulation::removeEvent(event_ptr);
	}

	void removeObject(Object* object_ptr) {
		Simulation::removeObject(object_ptr);
	}

	void resetErrorStatus() {
		Error_found = false;
		error_msg = "";
	}
};

namespace SimulationTests {

	class SimulationTest : public ::testing::Test {
	protected:
		Parameters_Simulation params_base;
		TestSim sim;
		void SetUp() {
			{
				params_base.Enable_logging = false;
				params_base.Params_lattice.Enable_periodic_x = true;
				params_base.Params_lattice.Enable_periodic_y = true;
				params_base.Params_lattice.Enable_periodic_z = true;
				params_base.Params_lattice.Length = 200;
				params_base.Params_lattice.Width = 200;
				params_base.Params_lattice.Height = 200;
				params_base.Params_lattice.Unit_size = 1.0;
				params_base.Temperature = 300;
				params_base.Enable_FRM = false;
				params_base.Enable_selective_recalc = true;
				params_base.Recalc_cutoff = 3;
				params_base.Enable_full_recalc = false;
			}
			// Initialize TestSim object
			sim.init(params_base);
			sim.N_tests = 10000;
			sim.N_steps = 500;
			sim.k_move = 1000;
			sim.setGeneratorSeed(std::random_device{}());
		}
	};

	TEST_F(SimulationTest, CheckParameters) {
		// Check for invalid default parameters object
		Parameters_Simulation params;
		EXPECT_FALSE(params.checkParameters());
		// Check for invalid lattice params
		params = params_base;
		params.Params_lattice.Height = 0;
		EXPECT_FALSE(params.checkParameters());
		// Check for invalid temp
		params = params_base;
		params.Temperature = 0;
		EXPECT_FALSE(params.checkParameters());
		// Check for valid selection of KMC algorithm
		// Check for multiple KMC algorithms enabled
		params = params_base;
		params.Enable_FRM = true;
		params.Enable_selective_recalc = true;
		params.Enable_full_recalc = false;
		EXPECT_FALSE(params.checkParameters());
		params.Enable_FRM = true;
		params.Enable_selective_recalc = false;
		params.Enable_full_recalc = true;
		EXPECT_FALSE(params.checkParameters());
		params.Enable_FRM = false;
		params.Enable_selective_recalc = true;
		params.Enable_full_recalc = true;
		EXPECT_FALSE(params.checkParameters());
		// Check for no KMC algorithm specified
		params.Enable_FRM = false;
		params.Enable_selective_recalc = false;
		params.Enable_full_recalc = false;
		EXPECT_FALSE(params.checkParameters());
		// Check recalculation radius when using the selective recalc method
		params = params_base;
		params.Enable_selective_recalc = true;
		params.Recalc_cutoff = 0;
		EXPECT_FALSE(params.checkParameters());
		// Check for missing logfile when logging is enabled
		params = params_base;
		params.Enable_logging = true;
		EXPECT_FALSE(params.checkParameters());
		// Check for invalid logging file when logging is enabled
		params = params_base;
		params.Enable_logging = true;
		ofstream outfile;
		params.Logfile = &outfile;
		EXPECT_FALSE(params.checkParameters());
	}

	TEST_F(SimulationTest, SetupTests) {
		EXPECT_EQ(0, sim.getId());
		EXPECT_FALSE(sim.isLoggingEnabled());
		EXPECT_DOUBLE_EQ(0.0, sim.getTime());
		EXPECT_EQ(300, sim.getTemp());
		EXPECT_DOUBLE_EQ(params_base.Params_lattice.Length*params_base.Params_lattice.Width*params_base.Params_lattice.Height*1e-21, sim.getVolume());
		// Check that Simulation initialization with invalid parameters throws an exception
		auto params = params_base;
		params.Temperature = 0;
		EXPECT_THROW(sim.init(params), invalid_argument);
	}

	TEST_F(SimulationTest, RemoveEventObjectTests) {
		EXPECT_FALSE(sim.getErrorStatus());
		Event event1;
		sim.removeEvent(&event1);
		EXPECT_TRUE(sim.getErrorStatus());
		sim.resetErrorStatus();
		Object object1;
		sim.removeObject(&object1);
		EXPECT_TRUE(sim.getErrorStatus());
		sim.resetErrorStatus();
	}

	TEST_F(SimulationTest, BKL_determinePathwayTests) {
		// Construct list of events and event pointers that all have the same rate constant
		Event event1(&sim);
		vector<Event> events(10, event1);
		vector<Event*> event_ptrs;
		Coords coords_dest;
		for (int i = 0; i < (int)events.size(); i++) {
			coords_dest = { i,i,i };
			events[i].setDestCoords(coords_dest);
			events[i].setRateConstant(1e6);
			event_ptrs.push_back(&events[i]);
		}
		Event* event_ptr;
		// Generate collection of chosen event indices and wait times using BKL algorithm
		vector<int> indices((int)2e7);
		vector<double> times((int)2e7);
		for (int i = 0; i < (int)times.size(); i++) {
			event_ptr = sim.determinePathway(event_ptrs);
			indices[i] = event_ptr->getDestCoords().x;
			times[i] = event_ptr->getExecutionTime();
		}
		// Calculate probability hist of indices and times
		auto prob_hist = calculateProbabilityHist(times, 1000);
		// Calculate the discrete prob dist
		auto prob_dist = calculateDensityHist(prob_hist);
		// Test probability distribution of indices
		for (int i = 0; i < 10; i++) {
			int count = (int)count_if(indices.begin(), indices.end(), [i](int element) {return element == i; });
			EXPECT_NEAR(1.0 / 10.0, (double)count / (double)indices.size(), 2e-3);
		}
		// Test probability dist of times
		EXPECT_NEAR(1.0, integrateData(prob_dist), 2e-2);
		EXPECT_NEAR(1e7, prob_dist[0].second, 2e5);
		auto it = find_if(prob_dist.begin(), prob_dist.end(), [](pair<double, double>& x_y) {return x_y.first > 1e-7; });
		it--;
		EXPECT_NEAR(1e7 / 2.7182818284590, (*it).second, 2e5);
		// Test average wait time
		EXPECT_NEAR(vector_avg(times), 1e-7, 2e-9);
	}

	TEST_F(SimulationTest, EventExecutionTests) {
		EXPECT_EQ(0, (int)sim.objects.size());
		EXPECT_EQ(1, sim.getN_events());
		EXPECT_TRUE(sim.executeNextEvent());
		EXPECT_EQ(1, (int)sim.objects.size());
		EXPECT_EQ(0, sim.objects.front().getTag());
		EXPECT_EQ(2, sim.getN_events());
		EXPECT_EQ(1, sim.getN_events_executed());
		EXPECT_FALSE(sim.checkFinished());
		// Check objects and events
		auto object_ptrs = sim.getAllObjectPtrs();
		for (int i = 0; i < (int)sim.objects.size(); i++) {
			auto it = sim.objects.begin();
			advance(it, i);
			EXPECT_TRUE(&(*it) == object_ptrs[i]);
		}
		auto event_ptrs = sim.getAllEventPtrs();
		for (int i = 0; i < (int)event_ptrs.size(); i++) {
			if (i == 0) {
				EXPECT_TRUE(&sim.event_creation == event_ptrs[i]);
			}
			else {
				auto it = sim.events_move.begin();
				advance(it, i - 1);
				EXPECT_TRUE(&(*it) == event_ptrs[i]);
			}
		}
		// Check event destination coords
		vector<Coords> data(20000);
		for (int i = 0; i < 10; i++) {
			for (auto& item : data) {
				sim.calculateNextEvent(&sim.objects.front());
				item = sim.events_move.front().getDestCoords();
			}
			auto site_options(data);
			removeDuplicates(site_options);
			EXPECT_EQ(6, (int)site_options.size());
			for (auto item : site_options) {
				int count = (int)count_if(data.begin(), data.end(), [item](Coords element) {return element == item; });
				EXPECT_NEAR(1.0 / 6.0, (double)count / (double)data.size(), 2e-2);
			}
			EXPECT_TRUE(sim.executeNextEvent());
		}
	}

	TEST_F(SimulationTest, DisplacementTests) {
		params_base.Params_lattice.Enable_periodic_x = false;
		params_base.Params_lattice.Enable_periodic_y = false;
		params_base.Params_lattice.Enable_periodic_z = false;
		params_base.Params_lattice.Length = 100;
		params_base.Params_lattice.Width = 100;
		params_base.Params_lattice.Height = 100;
		TestSim sim2;
		sim2.init(params_base);
		sim2.coords_creation = sim2.getRandomCoords();
		sim2.N_tests = 1000;
		sim2.N_steps = 500;
		sim2.k_move = 1000;
		double displacement;
		vector<Coords> creation_coords;
		EXPECT_TRUE(sim2.executeNextEvent());
		while (!sim2.checkFinished()) {
			if (sim2.event_previous_type.compare("Creation") == 0) {
				creation_coords.push_back(sim2.objects.back().getCoords());
				EXPECT_TRUE(sim2.coords_creation == creation_coords.back());
				sim2.coords_creation = sim2.getRandomCoords();
			}
			Coords coords_i = creation_coords[sim2.objects.front().getTag()];
			Coords coords = sim2.objects.front().getCoords();
			displacement = sim2.objects.front().calculateDisplacement();
			EXPECT_DOUBLE_EQ(sqrt((coords.x - coords_i.x)*(coords.x - coords_i.x) + (coords.y - coords_i.y)*(coords.y - coords_i.y) + (coords.z - coords_i.z)*(coords.z - coords_i.z)), displacement);
			EXPECT_TRUE(sim2.executeNextEvent());
		}
	}

	TEST_F(SimulationTest, 3DRandomWalkTests) {
		// 3D
		while (!sim.checkFinished()) {
			EXPECT_TRUE(sim.executeNextEvent());
			if (sim.event_previous.getEventType().compare("Move") == 0) {
				EXPECT_EQ(sim.event_previous.getDestCoords(), sim.event_previous.getObjectPtr()->getCoords());
			}
		}
		EXPECT_EQ(10000, sim.getN_objects_created());
		double displacement = vector_avg(sim.displacement_data);
		double dim = 3.0;
		double expected_val = sqrt(2.0 * sim.N_steps / dim)*(tgamma((dim + 1.0) / 2.0) / tgamma(dim / 2.0));
		EXPECT_NEAR(expected_val, displacement, 3e-2*expected_val);
	}

	TEST_F(SimulationTest, 2DRandomWalkTests) {
		// 2D
		params_base.Params_lattice.Enable_periodic_x = true;
		params_base.Params_lattice.Enable_periodic_y = true;
		params_base.Params_lattice.Enable_periodic_z = false;
		params_base.Params_lattice.Length = 200;
		params_base.Params_lattice.Width = 200;
		params_base.Params_lattice.Height = 1;
		TestSim sim2D;
		sim2D.init(params_base);
		sim2D.N_tests = 10000;
		sim2D.N_steps = 500;
		sim2D.k_move = 1000;
		while (!sim2D.checkFinished()) {
			EXPECT_TRUE(sim2D.executeNextEvent());
		}
		double displacement2D = vector_avg(sim2D.displacement_data);
		double dim = 2.0;
		double expected_val = sqrt(2.0 * sim2D.N_steps / dim)*(tgamma((dim + 1.0) / 2.0) / tgamma(dim / 2.0));
		EXPECT_NEAR(expected_val, displacement2D, 3e-2*expected_val);
	}

	TEST_F(SimulationTest, 1DRandomWalkTests) {
		// 1D
		params_base.Params_lattice.Enable_periodic_x = true;
		params_base.Params_lattice.Enable_periodic_y = false;
		params_base.Params_lattice.Enable_periodic_z = false;
		params_base.Params_lattice.Length = 200;
		params_base.Params_lattice.Width = 1;
		params_base.Params_lattice.Height = 1;
		TestSim sim1D;
		sim1D.init(params_base);
		sim1D.N_tests = 10000;
		sim1D.N_steps = 500;
		sim1D.k_move = 1000;
		while (!sim1D.checkFinished()) {
			EXPECT_TRUE(sim1D.executeNextEvent());
		}
		double displacement1D = vector_avg(sim1D.displacement_data);
		double dim = 1.0;
		double expected_val = sqrt(2.0 * sim1D.N_steps / dim)*(tgamma((dim + 1.0) / 2.0) / tgamma(dim / 2.0));
		EXPECT_NEAR(expected_val, displacement1D, 3e-2*expected_val);
	}

	TEST_F(SimulationTest, AlgorithmTests) {
		// Selective recalculation method
		sim = TestSim();
		params_base.Enable_FRM = false;
		params_base.Enable_selective_recalc = true;
		params_base.Recalc_cutoff = 3;
		params_base.Enable_full_recalc = false;
		params_base.Params_lattice.Enable_periodic_x = true;
		params_base.Params_lattice.Enable_periodic_y = true;
		params_base.Params_lattice.Enable_periodic_z = true;
		params_base.Params_lattice.Length = 200;
		params_base.Params_lattice.Width = 200;
		params_base.Params_lattice.Height = 200;
		sim.init(params_base);
		sim.N_tests = 2000;
		sim.N_steps = 1000;
		sim.k_move = 1000;
		while (!sim.checkFinished()) {
			EXPECT_TRUE(sim.executeNextEvent());
		}
		double displacement1 = vector_avg(sim.displacement_data);
		// First reaction method
		sim = TestSim();
		params_base.Enable_FRM = true;
		params_base.Enable_selective_recalc = false;
		params_base.Enable_full_recalc = false;
		sim.init(params_base);
		sim.N_tests = 2000;
		sim.N_steps = 1000;
		sim.k_move = 1000;
		while (!sim.checkFinished()) {
			EXPECT_TRUE(sim.executeNextEvent());
		}
		double displacement2 = vector_avg(sim.displacement_data);
		// Full recalculation method
		sim = TestSim();
		params_base.Enable_FRM = false;
		params_base.Enable_selective_recalc = false;
		params_base.Enable_full_recalc = true;
		sim.init(params_base);
		sim.N_tests = 2000;
		sim.N_steps = 1000;
		sim.k_move = 1000;
		while (!sim.checkFinished()) {
			EXPECT_TRUE(sim.executeNextEvent());
		}
		double displacement3 = vector_avg(sim.displacement_data);
		// Check different displacement values
		EXPECT_NEAR(displacement1, displacement2, 5e-2*displacement1);
		EXPECT_NEAR(displacement1, displacement3, 5e-2*displacement1);
		EXPECT_NEAR(displacement2, displacement3, 5e-2*displacement2);
	}

	TEST_F(SimulationTest, ErrorMessageTests) {
		EXPECT_TRUE(sim.checkErrorMessageFunctions("Error!"));
		EXPECT_TRUE(sim.checkErrorMessageFunctions("Error! There is a problem."));
		EXPECT_TRUE(sim.checkErrorMessageFunctions(" "));
		EXPECT_TRUE(sim.checkErrorMessageFunctions(""));
	}
}

namespace UtilsTests {

	TEST(UtilsTests, CoordsTests) {
		Coords coords{ 1, 1, 1 };
		EXPECT_EQ(1, coords.x);
		EXPECT_EQ(1, coords.y);
		EXPECT_EQ(1, coords.z);
		Coords coords2{ 0, 5, 10 };
		EXPECT_TRUE(coords != coords2);
		coords2.setXYZ(1, 1, 1);
		EXPECT_TRUE(coords == coords2);
	}

	TEST(UtilsTests, CalculateHistTests) {
		// Check behavior for an empty data set
		vector<int> data;
		EXPECT_THROW(calculateHist(data, 1), invalid_argument);
		// Create sample data
		data = { 0,0,2,1,2,4,3,1,0,4,2 };
		// Calculate the histogram
		auto hist = calculateHist(data, 1);
		// Check hist size
		EXPECT_EQ(5, (int)hist.size());
		// Check hist bins
		EXPECT_DOUBLE_EQ(0.0, hist[0].first);
		EXPECT_DOUBLE_EQ(1.0, hist[1].first);
		EXPECT_DOUBLE_EQ(2.0, hist[2].first);
		EXPECT_DOUBLE_EQ(3.0, hist[3].first);
		EXPECT_DOUBLE_EQ(4.0, hist[4].first);
		// Check hist values
		EXPECT_EQ(3, hist[0].second);
		EXPECT_EQ(2, hist[1].second);
		EXPECT_EQ(3, hist[2].second);
		EXPECT_EQ(1, hist[3].second);
		EXPECT_EQ(2, hist[4].second);
		// Calculate the probability hist using the hist
		auto prob_hist = calculateProbabilityHist(hist);
		// Check behavior if the hist is empty
		hist.clear();
		EXPECT_THROW(calculateProbabilityHist(hist), invalid_argument);
		// Check prob_hist size
		EXPECT_EQ(5, (int)prob_hist.size());
		// Check prob values
		EXPECT_DOUBLE_EQ(3.0 / 11.0, prob_hist[0].second);
		EXPECT_DOUBLE_EQ(2.0 / 11.0, prob_hist[1].second);
		EXPECT_DOUBLE_EQ(3.0 / 11.0, prob_hist[2].second);
		EXPECT_DOUBLE_EQ(1.0 / 11.0, prob_hist[3].second);
		EXPECT_DOUBLE_EQ(2.0 / 11.0, prob_hist[4].second);
		// Check that the prob hist sums to 1
		auto cum_hist = calculateCumulativeHist(prob_hist);
		EXPECT_DOUBLE_EQ(1.0, cum_hist.back().second);
		// Calculate the prob hist directly from the data vector
		prob_hist = calculateProbabilityHist(data, 1);
		// Check prob size
		EXPECT_EQ(5, (int)prob_hist.size());
		// Check prob values
		EXPECT_DOUBLE_EQ(3.0 / 11.0, prob_hist[0].second);
		EXPECT_DOUBLE_EQ(2.0 / 11.0, prob_hist[1].second);
		EXPECT_DOUBLE_EQ(3.0 / 11.0, prob_hist[2].second);
		EXPECT_DOUBLE_EQ(1.0 / 11.0, prob_hist[3].second);
		EXPECT_DOUBLE_EQ(2.0 / 11.0, prob_hist[4].second);
		// Check that the prob hist sums to 1
		cum_hist = calculateCumulativeHist(prob_hist);
		EXPECT_DOUBLE_EQ(1.0, cum_hist.back().second);
		// Check behavior how larger bin size
		// Create sample data
		data = { 0,0,2,1,2,4,3,1,0,4,2,5 };
		// Calculate the histogram
		hist = calculateHist(data, 2);
		// Check hist size
		EXPECT_EQ(3, (int)hist.size());
		// Check hist bins
		EXPECT_DOUBLE_EQ(0.5, hist[0].first);
		EXPECT_DOUBLE_EQ(2.5, hist[1].first);
		EXPECT_DOUBLE_EQ(4.5, hist[2].first);
		// Check hist values
		EXPECT_EQ(5, hist[0].second);
		EXPECT_EQ(4, hist[1].second);
		EXPECT_EQ(3, hist[2].second);
		// Check behavior with invalid bin size
		EXPECT_THROW(calculateHist(data, 0), invalid_argument);
		EXPECT_THROW(calculateHist(data, -1), invalid_argument);
	}

	TEST(UtilsTests, CalculateProbabilityHistTests) {
		vector<int> int_data;
		// Test with empty int data set that exception is thrown
		EXPECT_THROW(calculateProbabilityHist(int_data, 5), invalid_argument);
		// Generate a set of data from a uniform real distribution
		mt19937_64 gen(std::random_device{}());
		uniform_real_distribution<double> dist(0, 100);
		vector<double> data((int)3e7);
		generate(data.begin(), data.end(), [&]() { return dist(gen); });
		// Calculate histogram with 10 bins
		auto prob_hist = calculateProbabilityHist(data, 10);
		// Check for the correct number of bins
		EXPECT_EQ(10, (int)prob_hist.size());
		// Check several random values from the uniform probability hist
		uniform_int_distribution<> dist2(0, 9);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		// Check that the prob hist sums to 1
		auto cum_hist = calculateCumulativeHist(prob_hist);
		EXPECT_DOUBLE_EQ(1.0, cum_hist.back().second);
		// Calculate histogram with 1000 bins
		prob_hist = calculateProbabilityHist(data, 1000);
		// Check for the correct number of bins
		EXPECT_EQ(1000, (int)prob_hist.size());
		// Calculate the discrete prob dist
		auto prob_dist = calculateDensityHist(prob_hist);
		// Check that the prob dist integrates to 1
		EXPECT_NEAR(1.0, integrateData(prob_dist), 2e-3);
		// Calculate histogram with a bin size of 10.0
		prob_hist = calculateProbabilityHist(data, 10.0);
		// Check for the correct number of bins
		EXPECT_EQ(10, (int)prob_hist.size());
		// Check several random values from the uniform probability hist
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		// Check that the prob hist sums to 1
		cum_hist = calculateCumulativeHist(prob_hist);
		EXPECT_DOUBLE_EQ(1.0, cum_hist.back().second);
		// Calculate histogram with a bin size of 0.1
		prob_hist = calculateProbabilityHist(data, 0.1);
		// Check for the correct number of bins
		EXPECT_EQ(1000, (int)prob_hist.size());
		// Calculate the discrete prob dist
		prob_dist = calculateDensityHist(prob_hist);
		// Check that the prob dist integrates to 1
		EXPECT_NEAR(1.0, integrateData(prob_dist), 2e-3);
		//
		// Check that the double versions still work when there is invalid data in the vector
		//
		data.push_back(NAN);
		data.push_back(INFINITY);
		// Calculate histogram with 10 bins
		prob_hist = calculateProbabilityHist(data, 10);
		// Check for the correct number of bins
		EXPECT_EQ(10, (int)prob_hist.size());
		// Check several random values from the uniform probability hist
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		// Check that the prob hist sums to 1
		cum_hist = calculateCumulativeHist(prob_hist);
		EXPECT_DOUBLE_EQ(1.0, cum_hist.back().second);
		// Calculate histogram with 1000 bins
		prob_hist = calculateProbabilityHist(data, 1000);
		// Check for the correct number of bins
		EXPECT_EQ(1000, (int)prob_hist.size());
		// Calculate the discrete prob dist
		prob_dist = calculateDensityHist(prob_hist);
		// Check that the prob dist integrates to 1
		EXPECT_NEAR(1.0, integrateData(prob_dist), 2e-3);
		// Calculate histogram with a bin size of 10.0
		prob_hist = calculateProbabilityHist(data, 10.0);
		// Check for the correct number of bins
		EXPECT_EQ(10, (int)prob_hist.size());
		// Check several random values from the uniform probability hist
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		// Check that the prob hist sums to 1
		cum_hist = calculateCumulativeHist(prob_hist);
		EXPECT_DOUBLE_EQ(1.0, cum_hist.back().second);
		// Calculate histogram with a bin size of 0.1
		prob_hist = calculateProbabilityHist(data, 0.1);
		// Check for the correct number of bins
		EXPECT_EQ(1000, (int)prob_hist.size());
		// Calculate the discrete prob dist
		prob_dist = calculateDensityHist(prob_hist);
		// Check that the prob dist integrates to 1
		EXPECT_NEAR(1.0, integrateData(prob_dist), 2e-3);
		//
		// Check that empty double data vectors throw an exception
		//
		// Clear data vector
		data.clear();
		// Check that empty double data vectors throw an exception
		EXPECT_THROW(calculateProbabilityHist(data, 10.0), invalid_argument);
		EXPECT_THROW(calculateProbabilityHist(data, 5), invalid_argument);
		EXPECT_THROW(calculateProbabilityHist(data, 1.0, 5), invalid_argument);
		// Deallocate data vector
		vector<double>().swap(data);
		//
		// Check behavior of float data
		//
		vector<float> data_float((int)3e7);
		generate(data_float.begin(), data_float.end(), [&]() { return (float)dist(gen); });
		// Calculate histogram with 10 bins
		prob_hist = calculateProbabilityHist(data_float, 10);
		// Check for the correct number of bins
		EXPECT_EQ(10, (int)prob_hist.size());
		// Check several random values from the uniform probability hist
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		// Check that the prob hist sums to 1
		cum_hist = calculateCumulativeHist(prob_hist);
		EXPECT_DOUBLE_EQ(1.0, cum_hist.back().second);
		// Calculate histogram with 1000 bins
		prob_hist = calculateProbabilityHist(data_float, 1000);
		// Check for the correct number of bins
		EXPECT_EQ(1000, (int)prob_hist.size());
		// Calculate the discrete prob dist
		prob_dist = calculateDensityHist(prob_hist);
		// Check that the prob dist integrates to 1
		EXPECT_NEAR(1.0, integrateData(prob_dist), 2e-3);
		// Calculate histogram with a bin size of 10.0
		prob_hist = calculateProbabilityHist(data_float, 10.0);
		// Check for the correct number of bins
		EXPECT_EQ(10, (int)prob_hist.size());
		// Check several random values from the uniform probability hist
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		// Check that the prob hist sums to 1
		cum_hist = calculateCumulativeHist(prob_hist);
		EXPECT_DOUBLE_EQ(1.0, cum_hist.back().second);
		// Calculate histogram with a bin size of 0.1
		prob_hist = calculateProbabilityHist(data_float, 0.1);
		// Check for the correct number of bins
		EXPECT_EQ(1000, (int)prob_hist.size());
		// Calculate the discrete prob dist
		prob_dist = calculateDensityHist(prob_hist);
		// Check that the prob dist integrates to 1
		EXPECT_NEAR(1.0, integrateData(prob_dist), 2e-3);
		//
		// Check that the float verions still work when there is invalid data in the vector
		//
		data_float.push_back(NAN);
		data.push_back(INFINITY);
		// Calculate histogram with 10 bins
		prob_hist = calculateProbabilityHist(data_float, 10);
		// Check for the correct number of bins
		EXPECT_EQ(10, (int)prob_hist.size());
		// Check several random values from the uniform probability hist
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		// Check that the prob hist sums to 1
		cum_hist = calculateCumulativeHist(prob_hist);
		EXPECT_DOUBLE_EQ(1.0, cum_hist.back().second);
		// Calculate histogram with 1000 bins
		prob_hist = calculateProbabilityHist(data_float, 1000);
		// Check for the correct number of bins
		EXPECT_EQ(1000, (int)prob_hist.size());
		// Calculate the discrete prob dist
		prob_dist = calculateDensityHist(prob_hist);
		// Check that the prob dist integrates to 1
		EXPECT_NEAR(1.0, integrateData(prob_dist), 2e-3);
		// Calculate histogram with a bin size of 10.0
		prob_hist = calculateProbabilityHist(data_float, 10.0);
		// Check for the correct number of bins
		EXPECT_EQ(10, (int)prob_hist.size());
		// Check several random values from the uniform probability hist
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		EXPECT_NEAR(10.0 / 100.0, prob_hist[dist2(gen)].second, 2.5e-4);
		// Check that the prob hist sums to 1
		cum_hist = calculateCumulativeHist(prob_hist);
		EXPECT_DOUBLE_EQ(1.0, cum_hist.back().second);
		// Calculate histogram with a bin size of 0.1
		prob_hist = calculateProbabilityHist(data_float, 0.1);
		// Check for the correct number of bins
		EXPECT_EQ(1000, (int)prob_hist.size());
		// Calculate the discrete prob dist
		prob_dist = calculateDensityHist(prob_hist);
		// Check that the prob dist integrates to 1
		EXPECT_NEAR(1.0, integrateData(prob_dist), 2e-3);
		//
		// Check that empty float data vectors throw an exception
		//
		// Clear float data vector
		data_float.clear();
		EXPECT_THROW(calculateProbabilityHist(data_float, 10.0), invalid_argument);
		EXPECT_THROW(calculateProbabilityHist(data_float, 5), invalid_argument);
		EXPECT_THROW(calculateProbabilityHist(data_float, 1.0, 5), invalid_argument);
		// Check that calculation on empty histogram vectors throw an exception
		vector<pair<double, double>> hist;
		EXPECT_THROW(calculateDensityHist(hist), invalid_argument);
		EXPECT_THROW(calculateCumulativeHist(hist), invalid_argument);
		// Check that calculation on single entry histogram throws an exception
		hist.push_back(make_pair(0.0, 1.0));
		EXPECT_THROW(calculateDensityHist(hist), invalid_argument);
		// Check when there are less double data entries than bins
		data = { 0.0, 1.0, 2.0, 3.0, 4.0 };
		prob_hist = calculateProbabilityHist(data, 10);
		EXPECT_EQ(5, (int)prob_hist.size());
		prob_hist = calculateProbabilityHist(data, 0.1);
		EXPECT_EQ(5, (int)prob_hist.size());
		// Check when there are less float data entries than bins
		data_float = { 0.0, 1.0, 2.0, 3.0, 4.0 };
		prob_hist = calculateProbabilityHist(data_float, 10);
		EXPECT_EQ(5, (int)prob_hist.size());
		prob_hist = calculateProbabilityHist(data_float, 0.1);
		EXPECT_EQ(5, (int)prob_hist.size());
	}

	TEST(UtilsTests, ExponentialDOSTests) {
		mt19937_64 gen(std::random_device{}());
		// Check double version
		vector<double> data((int)2e7, 0.0);
		createExponentialDOSVector(data, 0.0, 0.1, gen);
		// Calculate probability density histogram
		auto hist = calculateProbabilityHist(data, 1000);
		auto prob_dist = calculateDensityHist(hist);
		// Check that the probability density histogram integrates to 1
		EXPECT_NEAR(1.0, integrateData(prob_dist), 1e-4);
		// Check the probablity density histogram peak height
		double peak_height = max_element(prob_dist.begin(), prob_dist.end(), [](pair<double,double> a, pair<double, double> b) {
			return a.second < b.second;
		})->second;
		double expected_height = 0.5*(1.0 / 0.1);
		EXPECT_NEAR(expected_height, peak_height, 1e-2*expected_height);
		// Check float version
		vector<float> data_float((int)2e7, 0.0);
		createExponentialDOSVector(data_float, 0.0, 0.1, gen);
		// Calculate probability density histogram
		hist = calculateProbabilityHist(data_float, 1000);
		prob_dist = calculateDensityHist(hist);
		// Check that the probability density histogram integrates to 1
		EXPECT_NEAR(1.0, integrateData(prob_dist), 1e-4);
		// Check the Gaussian probablity density histogram peak height
		peak_height = max_element(prob_dist.begin(), prob_dist.end(), [](pair<double, double> a, pair<double, double> b) {
			return a.second < b.second;
		})->second;
		expected_height = 0.5*(1.0 / 0.1);
		EXPECT_NEAR(expected_height, peak_height, 1e-2*expected_height);
	}

	TEST(UtilsTests, GaussianDOSTests) {
		mt19937_64 gen(std::random_device{}());
		// Check double version
		vector<double> data((int)2e7, 0.0);
		createGaussianDOSVector(data, 0.0, 0.15, gen);
		// Check avg of data
		EXPECT_NEAR(0.0, vector_avg(data), 1e-4);
		// Check stdev of data
		EXPECT_NEAR(0.15, vector_stdev(data), 1e-4);
		// Calculate probability density histogram
		auto hist = calculateProbabilityHist(data, 1000);
		auto prob_dist = calculateDensityHist(hist);
		// Check that the probability density histogram integrates to 1
		EXPECT_NEAR(1.0, integrateData(prob_dist), 1e-4);
		// Check the Gaussian probablity density histogram peak height
		double peak_height = max_element(prob_dist.begin(), prob_dist.end(), [](pair<double, double> a, pair<double, double> b) {
			return a.second < b.second;
		})->second;
		double expected_height = 1.0 / sqrt(2.0*Pi*intpow(0.15, 2));
		EXPECT_NEAR(expected_height, peak_height, 2e-2*expected_height);
		// Check float version
		vector<float> data_float((int)2e7, 0.0);
		createGaussianDOSVector(data_float, 0.0, 0.15, gen);
		// Check avg of data
		EXPECT_NEAR(0.0, vector_avg(data_float), 1e-4);
		// Check stdev of data
		EXPECT_NEAR(0.15, vector_stdev(data_float), 1e-4);
		// Calculate probability density histogram
		hist = calculateProbabilityHist(data_float, 1000);
		prob_dist = calculateDensityHist(hist);
		// Check that the probability density histogram integrates to 1
		EXPECT_NEAR(1.0, integrateData(prob_dist), 1e-4);
		// Check the Gaussian probablity density histogram peak height
		peak_height = max_element(prob_dist.begin(), prob_dist.end(), [](pair<double, double> a, pair<double, double> b) {
			return a.second < b.second;
		})->second;
		expected_height = 1.0 / sqrt(2.0*Pi*intpow(0.15, 2));
		EXPECT_NEAR(expected_height, peak_height, 2e-2*expected_height);
	}

	TEST(UtilsTests, Str2boolTests) {
		EXPECT_TRUE(str2bool("true"));
		EXPECT_TRUE(str2bool(" true  "));
		EXPECT_FALSE(str2bool("false	"));
		EXPECT_FALSE(str2bool("   false"));
		// Check that invalid input strings throw an exception as designed
		EXPECT_THROW(str2bool("blah"), invalid_argument);
		EXPECT_THROW(str2bool("	blah  "), invalid_argument);
	}

	TEST(UtilsTests, IntegrateDataTests) {
		vector<pair<double, double>> data_vec = { { 0.0,0.0 },{ 1.0,1.0 },{ 2.0,2.0 },{ 3.0,3.0 } };
		auto area = integrateData(data_vec);
		EXPECT_DOUBLE_EQ(area, 9.0 / 2.0);
	}

	TEST(UtilsTests, InterpolateDataTests) {
		vector<pair<double, double>> data_vec(100);
		for (int i = 0; i < (int)data_vec.size(); i++) {
			data_vec[i].first = 0.1*i;
			data_vec[i].second = exp(-data_vec[i].first / 2.85);
		}
		EXPECT_NEAR(1 / exp(1), interpolateData(data_vec, 2.85), 1e-4);
		// Test with a small lifetime and nonlinear x-vals
		for (int i = 0; i < (int)data_vec.size(); i++) {
			data_vec[i].first = pow(10, log10(1e-12) + i * 0.1);
			data_vec[i].second = exp(-data_vec[i].first / 1e-10);
		}
		EXPECT_NEAR(1 / exp(1), interpolateData(data_vec, 1e-10), 1e-4);
		for (int i = 0; i < (int)data_vec.size(); i++) {
			data_vec[i].first = 0.2*i;
			data_vec[i].second = 2.5*data_vec[i].first - 5.0;
		}
		EXPECT_NEAR(3.25, interpolateData(data_vec, 3.3), 1e-4);
		EXPECT_DOUBLE_EQ(7.5, interpolateData(data_vec, 5));
		// Attempt to interpolate beyond the data range
		EXPECT_TRUE(std::isnan(interpolateData(data_vec, -1.0)));
		EXPECT_TRUE(std::isnan(interpolateData(data_vec, 21.0)));

	}

	TEST(UtilsTests, RemoveWhitespaceTests) {
		string str = " text          ";
		EXPECT_EQ(removeWhitespace(str), "text");
		str = " text	";
		EXPECT_EQ(removeWhitespace(str), "text");
		str = "			text ";
		EXPECT_EQ(removeWhitespace(str), "text");
		str = "t e	xt";
		EXPECT_EQ(removeWhitespace(str), "text");
		str = "text";
		EXPECT_EQ(removeWhitespace(str), "text");
	}

	TEST(UtilsTests, ArrayStatsTests) {
		// positive ints
		int int_data[10];
		for (int i = 0; i < 10; i++) {
			int_data[i] = i + 1;
		}
		EXPECT_DOUBLE_EQ(5.5, array_avg(int_data, 10));
		EXPECT_NEAR(3.02765035409749, array_stdev(int_data, 10), 1e-14);
		// negative ints
		for (int i = 0; i < 10; i++) {
			int_data[i] = -(i + 1);
		}
		EXPECT_DOUBLE_EQ(-5.5, array_avg(int_data, 10));
		EXPECT_NEAR(3.02765035409749, array_stdev(int_data, 10), 1e-14);
		// positive doubles
		double double_data[10];
		for (int i = 0; i < 10; i++) {
			double_data[i] = 0.5*(i + 1);
		}
		EXPECT_DOUBLE_EQ(2.75, array_avg(double_data, 10));
		EXPECT_NEAR(1.51382517704875, array_stdev(double_data, 10), 1e-14);
		// negative doubles
		for (int i = 0; i < 10; i++) {
			double_data[i] = -0.5*(i + 1);
		}
		EXPECT_DOUBLE_EQ(-2.75, array_avg(double_data, 10));
		EXPECT_NEAR(1.51382517704875, array_stdev(double_data, 10), 1e-14);
	}

	TEST(UtilsTests, RoundTests) {
		// Check round down positive val
		EXPECT_EQ(1, round_int(1.1));
		// Check round up positive val
		EXPECT_EQ(2, round_int(1.5));
		// Check round down to zero
		EXPECT_EQ(0, round_int(0.4));
		// Check round up to zero
		EXPECT_EQ(0, round_int(-0.4));
		// Check round down negative val
		EXPECT_EQ(-2, round_int(-1.5));
		// Check round up negative val
		EXPECT_EQ(-1, round_int(-1.1));
	}


	TEST(UtilsTests, IntPowTests) {
		EXPECT_DOUBLE_EQ(1.0, intpow(2.5, 0));
		EXPECT_DOUBLE_EQ(2.5, intpow(2.5, 1));
		EXPECT_DOUBLE_EQ(6.25, intpow(2.5, 2));
		EXPECT_DOUBLE_EQ(9536.7431640625, intpow(2.5, 10));
		EXPECT_DOUBLE_EQ(0.4, intpow(2.5, -1));
		EXPECT_DOUBLE_EQ(0.16, intpow(2.5, -2));
		EXPECT_DOUBLE_EQ(1.048576e-4, intpow(2.5, -10));
		EXPECT_DOUBLE_EQ(1.0, intpow(15.04564, 0));
		EXPECT_DOUBLE_EQ(1e-21, intpow(1e-7, 3));
		// Check integer base to negative power
		EXPECT_DOUBLE_EQ(0.5, intpow(2, -1));
	}

	TEST(UtilsTests, RemoveDuplicatesTests) {
		vector<int> vec{ 0, 1, 1, 2, 3, 1, 4, 2 };
		removeDuplicates(vec);
		EXPECT_EQ(2, vec[2]);
		EXPECT_EQ(5, (int)vec.size());
		vec = { 0, 1, 2, 1 };
		removeDuplicates(vec);
		EXPECT_EQ(2, vec[2]);
		EXPECT_EQ(3, (int)vec.size());
		vector<double> vec2{ 0.0, 1.0, 1.0, 2.0, 3.0, 1.0, 4.0, 2.0 };
		removeDuplicates(vec2);
		EXPECT_DOUBLE_EQ(2.0, vec2[2]);
		EXPECT_EQ(5, (int)vec2.size());
		Coords coords1{ 1,2,3 };
		Coords coords2{ 1,2,3 };
		Coords coords3{ 4,5,6 };
		vector<Coords> vec3{ coords1, coords1, coords2, coords3, coords1, coords3, coords2 };
		removeDuplicates(vec3);
		EXPECT_EQ(4, vec3[1].x);
		EXPECT_EQ(2, (int)vec3.size());
		vector<int> vec4 = {};
		removeDuplicates(vec4);
		EXPECT_EQ(0, (int)vec4.size());
		vector<int> vec5 = { 0 };
		removeDuplicates(vec5);
		EXPECT_EQ(1, (int)vec5.size());
		vector<int> vec6 = { 0,0 };
		removeDuplicates(vec6);
		EXPECT_EQ(1, (int)vec6.size());
	}

	TEST(UtilsTests, VectorStatsTests) {
		// positive ints
		vector<int> int_data;
		int_data.assign(10, 0);
		for (int i = 0; i < 10; i++) {
			int_data[i] = i + 1;
		}
		EXPECT_DOUBLE_EQ(5.5, vector_avg(int_data));
		EXPECT_NEAR(3.02765035409749, vector_stdev(int_data), 1e-14);
		EXPECT_DOUBLE_EQ(5.5, vector_median(int_data));
		EXPECT_EQ(4, vector_which_median(int_data));
		// negative ints
		for (int i = 0; i < 10; i++) {
			int_data[i] = -(i + 1);
		}
		EXPECT_DOUBLE_EQ(-5.5, vector_avg(int_data));
		EXPECT_NEAR(3.02765035409749, vector_stdev(int_data), 1e-14);
		EXPECT_DOUBLE_EQ(-5.5, vector_median(int_data));
		EXPECT_EQ(4, vector_which_median(int_data));
		// positive doubles
		vector<double> double_data;
		double_data.assign(10, 0);
		for (int i = 0; i < 10; i++) {
			double_data[i] = 0.5*(i + 1);
		}
		EXPECT_DOUBLE_EQ(2.75, vector_avg(double_data));
		EXPECT_NEAR(1.51382517704875, vector_stdev(double_data), 1e-14);
		EXPECT_DOUBLE_EQ(2.75, vector_median(double_data));
		EXPECT_EQ(4, vector_which_median(double_data));
		// negative doubles
		double_data.assign(10, 0);
		for (int i = 0; i < 10; i++) {
			double_data[i] = -0.5*(i + 1);
		}
		EXPECT_DOUBLE_EQ(-2.75, vector_avg(double_data));
		EXPECT_NEAR(1.51382517704875, vector_stdev(double_data), 1e-14);
		EXPECT_DOUBLE_EQ(-2.75, vector_median(double_data));
		EXPECT_EQ(4, vector_which_median(double_data));
		// Check simple odd size vector median
		vector<double> data = { 3.0, 0.0, 1.0, 5.0, 10.0 };
		EXPECT_DOUBLE_EQ(3.0, vector_median(data));
		EXPECT_EQ(0, vector_which_median(data));
	}
}

namespace LatticeTests {

	class LatticeTest : public ::testing::Test {
	protected:
		mt19937_64 gen;
		Parameters_Lattice params_lattice;
		vector<Site> sites;
		Lattice lattice;

		void SetUp() {
			gen.seed(std::random_device{}());
			// Setup params
			params_lattice.Enable_periodic_x = true;
			params_lattice.Enable_periodic_y = true;
			params_lattice.Enable_periodic_z = true;
			params_lattice.Length = 50;
			params_lattice.Width = 50;
			params_lattice.Height = 50;
			params_lattice.Unit_size = 1.0;
			// Initialize Lattice object
			lattice.init(params_lattice, &gen);
			Site site;
			sites.assign(lattice.getNumSites(), site);
			vector<Site*> site_ptrs((int)sites.size());
			for (int i = 0; i < (int)sites.size(); i++) {
				site_ptrs[i] = &sites[i];
			}
			lattice.setSitePointers(site_ptrs);
		}
	};

	TEST_F(LatticeTest, CheckParameters) {
		// Check for invalid default params object
		Parameters_Lattice params;
		EXPECT_FALSE(params.checkParameters());
		// Check for invalid lattice dimensions
		params = params_lattice;
		params.Height = 0;
		EXPECT_FALSE(params.checkParameters());
		// Check for invalid unit size
		params = params_lattice;
		params.Unit_size = 0;
		EXPECT_FALSE(params.checkParameters());
	}

	TEST_F(LatticeTest, InitializationTests) {
		// Check that SetUp creates a lattice with the proper dimensions and unit size
		EXPECT_EQ(50, lattice.getLength());
		EXPECT_EQ(50, lattice.getWidth());
		EXPECT_EQ(50, lattice.getHeight());
		EXPECT_DOUBLE_EQ(1.0, lattice.getUnitSize());
		EXPECT_EQ((long int)50 * 50 * 50, lattice.getNumSites());
		EXPECT_DOUBLE_EQ(125000e-21, lattice.getVolume());
		// Check that initialization with invalid parameters throws an exception
		auto params = params_lattice;
		params.Height = 0;
		EXPECT_THROW(lattice.init(params, &gen), invalid_argument);
	}

	TEST_F(LatticeTest, CalculateDestCoordsTests) {
		Coords coords_i{ 49, 49, 49 };
		Coords coords_f;
		lattice.calculateDestinationCoords(coords_i, 2, -2, 0, coords_f);
		EXPECT_EQ(1, coords_f.x);
		EXPECT_EQ(47, coords_f.y);
		EXPECT_EQ(49, coords_f.z);
		coords_i.setXYZ(0, 0, 49);
		lattice.calculateDestinationCoords(coords_i, 1, -2, 2, coords_f);
		EXPECT_EQ(1, coords_f.x);
		EXPECT_EQ(48, coords_f.y);
		EXPECT_EQ(1, coords_f.z);
		coords_i.setXYZ(1, 48, 1);
		lattice.calculateDestinationCoords(coords_i, -2, 2, -3, coords_f);
		EXPECT_EQ(49, coords_f.x);
		EXPECT_EQ(0, coords_f.y);
		EXPECT_EQ(48, coords_f.z);
	}

	TEST_F(LatticeTest, PeriodicCrossingTests) {
		Coords coords_i{ 49, 49, 49 };
		Coords coords_f{ 0, 49, 48 };
		EXPECT_EQ(lattice.calculateDX(coords_i, coords_f), -50);
		EXPECT_EQ(lattice.calculateDY(coords_i, coords_f), 0);
		EXPECT_EQ(lattice.calculateDZ(coords_i, coords_f), 0);
		coords_i = { 0, 49, 48 };
		coords_f = { 49, 49, 49 };
		EXPECT_EQ(lattice.calculateDX(coords_i, coords_f), 50);
		EXPECT_EQ(lattice.calculateDY(coords_i, coords_f), 0);
		EXPECT_EQ(lattice.calculateDZ(coords_i, coords_f), 0);
		coords_i.setXYZ(0, 0, 49);
		coords_f.setXYZ(1, 49, 0);
		EXPECT_EQ(lattice.calculateDX(coords_i, coords_f), 0);
		EXPECT_EQ(lattice.calculateDY(coords_i, coords_f), 50);
		EXPECT_EQ(lattice.calculateDZ(coords_i, coords_f), -50);
		coords_i = { 1, 49, 0 };
		coords_f = { 0, 0, 49 };
		EXPECT_EQ(lattice.calculateDX(coords_i, coords_f), 0);
		EXPECT_EQ(lattice.calculateDY(coords_i, coords_f), -50);
		EXPECT_EQ(lattice.calculateDZ(coords_i, coords_f), 50);
		coords_i.setXYZ(4, 5, 6);
		coords_f.setXYZ(3, 6, 5);
		EXPECT_EQ(lattice.calculateDX(coords_i, coords_f), 0);
		EXPECT_EQ(lattice.calculateDY(coords_i, coords_f), 0);
		EXPECT_EQ(lattice.calculateDZ(coords_i, coords_f), 0);
	}

	TEST_F(LatticeTest, CheckMoveValidityTests) {
		params_lattice.Enable_periodic_x = false;
		params_lattice.Enable_periodic_y = false;
		params_lattice.Enable_periodic_z = true;
		Lattice lattice2;
		lattice2.init(params_lattice, &gen);
		EXPECT_FALSE(lattice2.isXPeriodic());
		EXPECT_FALSE(lattice2.isYPeriodic());
		EXPECT_TRUE(lattice2.isZPeriodic());
		Coords coords1{ 49, 0, 49 };
		EXPECT_FALSE(lattice2.checkMoveValidity(coords1, 0, 0, 0));
		EXPECT_FALSE(lattice2.checkMoveValidity(coords1, 1, 0, 0));
		EXPECT_FALSE(lattice2.checkMoveValidity(coords1, 0, -1, 0));
		EXPECT_TRUE(lattice2.checkMoveValidity(coords1, 0, 0, 1));
		EXPECT_TRUE(lattice2.checkMoveValidity(coords1, -1, 1, -1));
		params_lattice.Enable_periodic_z = false;
		lattice2.init(params_lattice, &gen);
		EXPECT_FALSE(lattice2.isZPeriodic());
		EXPECT_FALSE(lattice2.checkMoveValidity(coords1, 0, 0, 1));
	}

	TEST_F(LatticeTest, RandomSiteGenTests) {
		Coords coords;
		int N_test = 40000000;
		vector<int> xcoords(N_test);
		vector<int> ycoords(N_test);
		vector<int> zcoords(N_test);
		for (int i = 0; i < N_test; i++) {
			coords = lattice.generateRandomCoords();
			EXPECT_TRUE(coords.x >= 0 && coords.x < lattice.getLength());
			EXPECT_TRUE(coords.y >= 0 && coords.y < lattice.getWidth());
			EXPECT_TRUE(coords.z >= 0 && coords.z < lattice.getHeight());
			xcoords[i] = coords.x;
			ycoords[i] = coords.y;
			zcoords[i] = coords.z;
		}
		EXPECT_NEAR(24.5, vector_avg(xcoords), 2e-2);
		EXPECT_NEAR(lattice.getLength() / sqrt(12.0), vector_stdev(xcoords), 1e-2);
		EXPECT_NEAR(24.5, vector_avg(ycoords), 2e-2);
		EXPECT_NEAR(lattice.getWidth() / sqrt(12.0), vector_stdev(ycoords), 1e-2);
		EXPECT_NEAR(24.5, vector_avg(zcoords), 2e-2);
		EXPECT_NEAR(lattice.getHeight() / sqrt(12.0), vector_stdev(zcoords), 1e-2);
	}

	TEST_F(LatticeTest, RandomUnoccupiedNeighborTests) {
		vector<int> data(30000);
		for (int i = 0; i < 6; i++) {
			Coords coords = lattice.generateRandomCoords();
			for (auto& item : data) {
				Coords coords_new = lattice.chooseRandomUnoccupiedNeighbor(coords);
				EXPECT_FALSE(coords_new == coords);
				item = lattice.getSiteIndex(coords_new);
			}
			auto site_options(data);
			removeDuplicates(site_options);
			EXPECT_EQ(6, (int)site_options.size());
			for (auto item : site_options) {
				int count = (int)count_if(data.begin(), data.end(), [item](int element) {return element == item; });
				EXPECT_NEAR(1.0 / 6.0, (double)count / (double)data.size(), 1e-2);
			}
		}
		// Check for correct output when all neighbor sites are occupied
		Coords coords(5, 5, 5);
		lattice.setOccupied(Coords(4, 5, 5));
		lattice.setOccupied(Coords(6, 5, 5));
		lattice.setOccupied(Coords(5, 4, 5));
		lattice.setOccupied(Coords(5, 6, 5));
		lattice.setOccupied(Coords(5, 5, 4));
		lattice.setOccupied(Coords(5, 5, 6));
		Coords coords_new = lattice.chooseRandomUnoccupiedNeighbor(coords);
		EXPECT_TRUE(coords_new == Coords(-1, -1, -1));
	}

	TEST_F(LatticeTest, 2DTests) {
		// Initialize 2D Lattice object
		params_lattice.Enable_periodic_x = true;
		params_lattice.Enable_periodic_y = true;
		params_lattice.Enable_periodic_z = false;
		params_lattice.Length = 50;
		params_lattice.Width = 50;
		params_lattice.Height = 1;
		params_lattice.Unit_size = 1.0;
		Lattice lattice2D;
		lattice2D.init(params_lattice, &gen);
		Site site;
		sites.assign(lattice2D.getNumSites(), site);
		vector<Site*> site_ptrs((int)sites.size());
		for (int i = 0; i < (int)sites.size(); i++) {
			site_ptrs[i] = &sites[i];
		}
		lattice2D.setSitePointers(site_ptrs);
		// Check random neighbor site generation in 2D
		vector<int> data(30000);
		for (int i = 0; i < 6; i++) {
			Coords coords = lattice2D.generateRandomCoords();
			for (auto& item : data) {
				item = lattice2D.getSiteIndex(lattice2D.chooseRandomUnoccupiedNeighbor(coords));
			}
			auto site_options(data);
			removeDuplicates(site_options);
			EXPECT_EQ(4, (int)site_options.size());
			for (auto item : site_options) {
				int count = (int)count_if(data.begin(), data.end(), [item](int element) {return element == item; });
				EXPECT_NEAR(1.0 / 4.0, (double)count / (double)data.size(), 1e-2);
			}
		}
	}

	TEST_F(LatticeTest, 1DTests) {
		// Initialize 1D Lattice object
		params_lattice.Enable_periodic_x = true;
		params_lattice.Enable_periodic_y = false;
		params_lattice.Enable_periodic_z = false;
		params_lattice.Length = 50;
		params_lattice.Width = 1;
		params_lattice.Height = 1;
		params_lattice.Unit_size = 1.0;
		Lattice lattice1D;
		lattice1D.init(params_lattice, &gen);
		Site site;
		sites.assign(lattice1D.getNumSites(), site);
		vector<Site*> site_ptrs((int)sites.size());
		for (int i = 0; i < (int)sites.size(); i++) {
			site_ptrs[i] = &sites[i];
		}
		lattice1D.setSitePointers(site_ptrs);
		// Check random neighbor site generation in 1D
		vector<int> data(30000);
		for (int i = 0; i < 6; i++) {
			Coords coords = lattice1D.generateRandomCoords();
			for (auto& item : data) {
				item = lattice1D.getSiteIndex(lattice1D.chooseRandomUnoccupiedNeighbor(coords));
			}
			auto site_options(data);
			removeDuplicates(site_options);
			EXPECT_EQ(2, (int)site_options.size());
			for (auto item : site_options) {
				int count = (int)count_if(data.begin(), data.end(), [item](int element) {return element == item; });
				EXPECT_NEAR(1.0 / 2.0, (double)count / (double)data.size(), 1e-2);
			}
		}
	}

	TEST_F(LatticeTest, LatticeDistanceTests) {
		Coords coords_i{ 49, 49, 49 };
		Coords coords_f{ 1, 49, 47 };
		EXPECT_EQ(8, lattice.calculateLatticeDistanceSquared(coords_i, coords_f));
		coords_i.setXYZ(4, 5, 6);
		coords_f.setXYZ(3, 6, 5);
		EXPECT_EQ(3, lattice.calculateLatticeDistanceSquared(coords_i, coords_f));
		coords_i.setXYZ(0, 49, 1);
		coords_f.setXYZ(48, 1, 49);
		EXPECT_EQ(12, lattice.calculateLatticeDistanceSquared(coords_i, coords_f));
	}

	TEST_F(LatticeTest, GetSiteCoordsTests) {
		Coords coords1, coords2;
		int index;
		for (int i = 0; i < 100; i++) {
			coords1 = lattice.generateRandomCoords();
			index = lattice.getSiteIndex(coords1);
			coords2 = lattice.getSiteCoords(index);
			EXPECT_EQ(coords1.x, coords2.x);
			EXPECT_EQ(coords1.y, coords2.y);
			EXPECT_EQ(coords1.z, coords2.z);
		}
	}

	TEST_F(LatticeTest, OccupancyTests) {
		Coords coords;
		for (int x = 0; x < lattice.getLength(); x++) {
			for (int y = 0; y < lattice.getWidth(); y++) {
				for (int z = 0; z < lattice.getHeight(); z++) {
					coords.setXYZ(x, y, z);
					EXPECT_FALSE(lattice.isOccupied(coords));
				}
			}
		}
		coords.setXYZ(5, 5, 5);
		lattice.setOccupied(coords);
		EXPECT_TRUE(lattice.isOccupied(coords));
		stringstream ss1, ss2;
		auto old_buffer = cout.rdbuf(ss1.rdbuf());
		lattice.outputLatticeOccupancy();
		cout.rdbuf(old_buffer);
		ss2 << "Site " << lattice.getSiteIndex(coords) << " is occupied.\n";
		EXPECT_EQ(ss1.str(), ss2.str());
		for (int x = 0; x < lattice.getLength(); x++) {
			for (int y = 0; y < lattice.getWidth(); y++) {
				for (int z = 0; z < lattice.getHeight(); z++) {
					if (lattice.isOccupied(coords)) {
						EXPECT_EQ(5, coords.x);
						EXPECT_EQ(5, coords.y);
						EXPECT_EQ(5, coords.z);
					}
				}
			}
		}
		lattice.clearOccupancy(coords);
		EXPECT_FALSE(lattice.isOccupied(coords));
	}

	TEST_F(LatticeTest, SiteTests) {
		Site site;
		EXPECT_FALSE(site.isOccupied());
		vector<Site*> site_ptrs(10, &site);
		EXPECT_FALSE(lattice.setSitePointers(site_ptrs));
		site_ptrs.assign(50 * 50 * 50, &site);
		EXPECT_TRUE(lattice.setSitePointers(site_ptrs));
		Coords coords{ 5,6,7 };
		Site* site1 = *lattice.getSiteIt(coords);
		Site* site2 = site_ptrs[lattice.getSiteIndex(coords)];
		EXPECT_EQ(site1, site2);
		// Check for sites and indices outside the lattice
		coords = { 0, 60, 0 };
		EXPECT_THROW(lattice.getSiteIndex(coords), out_of_range);
		EXPECT_THROW(lattice.getSiteCoords(150000), out_of_range);
		// Check site object assignment and clearing
		Object object;
		site.setObjectPtr(&object);
		EXPECT_TRUE(site.isOccupied());
		EXPECT_EQ(&object, site.getObjectPtr());
		site.clearOccupancy();
		EXPECT_FALSE(site.isOccupied());
	}
}

namespace EventTests {

	class EventTest : public ::testing::Test {
	protected:
		Parameters_Simulation params_base;
		TestSim test_sim;
		void setUp() {
			{
				params_base.Enable_logging = false;
				params_base.Params_lattice.Enable_periodic_x = true;
				params_base.Params_lattice.Enable_periodic_y = true;
				params_base.Params_lattice.Enable_periodic_z = true;
				params_base.Params_lattice.Length = 50;
				params_base.Params_lattice.Width = 50;
				params_base.Params_lattice.Height = 50;
				params_base.Params_lattice.Unit_size = 1.0;
				params_base.Temperature = 300;
				params_base.Enable_FRM = false;
				params_base.Enable_selective_recalc = true;
				params_base.Recalc_cutoff = 3;
				params_base.Enable_full_recalc = false;
			}
			// Initialize TestSim object
			test_sim.init(params_base);
		}
	};

	TEST_F(EventTest, GeneralEventTests) {
		Event event(&test_sim);
		EXPECT_EQ("Event", event.getEventType());
		Coords coords1(0, 0, 0);
		Object object1(0.0, 1, coords1);
		event.setObjectPtr(&object1);
		EXPECT_EQ(&object1, event.getObjectPtr());
		Coords coords2(1, 0, 0);
		Object object2(0.0, 2, coords2);
		event.setObjectTargetPtr(&object2);
		EXPECT_EQ(&object2, event.getObjectTargetPtr());
		event.setDestCoords(coords2);
		EXPECT_EQ(coords2, event.getDestCoords());
		EXPECT_FALSE(event.setExecutionTime(-1.0));
		EXPECT_TRUE(event.setExecutionTime(1.0));
		EXPECT_DOUBLE_EQ(1.0, event.getExecutionTime());
		// Check initial default value of the rate constant.
		EXPECT_DOUBLE_EQ(-1.0, event.getRateConstant());
		// Execute default rate constant function.
		event.calculateRateConstant(1.0);
		// Check that the rate constant is correctly assigned.
		EXPECT_DOUBLE_EQ(1.0, event.getRateConstant());
	}

	TEST_F(EventTest, CalculateExecutionTimeTests) {
		Event event(&test_sim);
		// Generate collection of wait times
		vector<double> times((int)3e7);
		for (int i = 0; i < (int)times.size(); i++) {
			event.calculateExecutionTime(1e9);
			times[i] = event.getExecutionTime();
		}
		// Calculate probability distribution of calculated times
		auto hist = calculateProbabilityHist(times, 1000);
		// Test probability distribution
		auto prob_dist = calculateDensityHist(hist);
		EXPECT_NEAR(1.0, integrateData(prob_dist), 2e-2);
		EXPECT_NEAR(1e9, prob_dist[0].second, 2e7);
		auto it = find_if(prob_dist.begin(), prob_dist.end(), [](pair<double, double>& x_y) {return x_y.first > 1e-9; });
		it--;
		EXPECT_NEAR(1e9 / 2.7182818284590, (*it).second, 2e7);
		// Test average wait time
		EXPECT_NEAR(vector_avg(times), 1e-9, 2e-12);
		// Generate collection of wait times
		for (int i = 0; i < (int)times.size(); i++) {
			event.calculateExecutionTime(1e12);
			times[i] = event.getExecutionTime();
		}
		// Calculate probability distribution of calculated times
		hist = calculateProbabilityHist(times, 1000);
		// Test probability distribution
		prob_dist = calculateDensityHist(hist);
		EXPECT_NEAR(1.0, integrateData(prob_dist), 2e-2);
		EXPECT_NEAR(1e12, prob_dist[0].second, 2e10);
		it = find_if(prob_dist.begin(), prob_dist.end(), [](pair<double, double>& x_y) {return x_y.first > 1e-12; });
		it--;
		EXPECT_NEAR(1e12 / 2.7182818284590, (*it).second, 2e10);
		// Test average wait time
		EXPECT_NEAR(vector_avg(times), 1e-12, 2e-15);
	}
}

namespace ObjectTests {

	TEST(ObjectTests, GeneralObjectTests) {
		// Assume object is created in a 50 x 50 x 50 lattice
		Coords coords = { 0,0,0 };
		Object object1(0.0, 1, coords);
		EXPECT_EQ("Object", object1.getObjectType());
		EXPECT_EQ(1, object1.getTag());
		EXPECT_EQ(coords, object1.getCoords());
		EXPECT_DOUBLE_EQ(0.0, object1.getCreationTime());
		// Object-Event tests
		Event event;
		list<Event*> event_ptrs;
		event_ptrs.push_back(&event);
		object1.setEventIt(event_ptrs.begin());
		EXPECT_EQ(&event, *(object1.getEventIt()));
	}

	TEST(ObjectTests, CalculateDisplacementTests) {
		// Assume object is created in a 50 x 50 x 50 lattice
		Object object1(0.0, 1, { 0,0,0 });
		object1.setCoords({ 0,0,5 });
		EXPECT_DOUBLE_EQ(5.0, object1.calculateDisplacement());
		EXPECT_DOUBLE_EQ(5.0, object1.calculateDisplacement(0));
		EXPECT_DOUBLE_EQ(0.0, object1.calculateDisplacement(1));
		EXPECT_DOUBLE_EQ(0.0, object1.calculateDisplacement(2));
		EXPECT_DOUBLE_EQ(5.0, object1.calculateDisplacement(3));
		object1.setCoords({ 0,0,49 });
		object1.incrementDZ(-50);
		EXPECT_DOUBLE_EQ(1.0, object1.calculateDisplacement());
		EXPECT_DOUBLE_EQ(1.0, object1.calculateDisplacement(0));
		EXPECT_DOUBLE_EQ(0.0, object1.calculateDisplacement(1));
		EXPECT_DOUBLE_EQ(0.0, object1.calculateDisplacement(2));
		EXPECT_DOUBLE_EQ(-1.0, object1.calculateDisplacement(3));
		object1.setCoords({ 49,49,49 });
		object1.incrementDX(-50);
		object1.incrementDY(-50);
		EXPECT_DOUBLE_EQ(sqrt(3), object1.calculateDisplacement());
		EXPECT_DOUBLE_EQ(sqrt(3), object1.calculateDisplacement(0));
		EXPECT_DOUBLE_EQ(-1.0, object1.calculateDisplacement(1));
		EXPECT_DOUBLE_EQ(-1.0, object1.calculateDisplacement(2));
		EXPECT_DOUBLE_EQ(-1.0, object1.calculateDisplacement(3));
		object1.setCoords({ 49,0,49 });
		object1.incrementDY(50);
		EXPECT_DOUBLE_EQ(sqrt(2), object1.calculateDisplacement());
		EXPECT_DOUBLE_EQ(sqrt(2), object1.calculateDisplacement(0));
		EXPECT_DOUBLE_EQ(-1.0, object1.calculateDisplacement(1));
		EXPECT_DOUBLE_EQ(0.0, object1.calculateDisplacement(2));
		EXPECT_DOUBLE_EQ(-1.0, object1.calculateDisplacement(3));
		object1.resetInitialCoords({ 5,5,5 });
		object1.setCoords({ 4,4,4 });
		EXPECT_DOUBLE_EQ(sqrt(3), object1.calculateDisplacement());
		EXPECT_DOUBLE_EQ(sqrt(3), object1.calculateDisplacement(0));
		EXPECT_DOUBLE_EQ(-1.0, object1.calculateDisplacement(1));
		EXPECT_DOUBLE_EQ(-1.0, object1.calculateDisplacement(2));
		EXPECT_DOUBLE_EQ(-1.0, object1.calculateDisplacement(3));
		// Check invalid direction parameter usage
		EXPECT_THROW(object1.calculateDisplacement(4), invalid_argument);
	}

}

namespace VersionTests {

	TEST(VersionTests, ConstructorTests) {
		// Check behavior of incorrect formats
		EXPECT_THROW(Version ver1("blah"), invalid_argument);
		EXPECT_THROW(Version ver1("v1.0"), invalid_argument);
		EXPECT_THROW(Version ver1("0.0.0"), invalid_argument);
		EXPECT_THROW(Version ver1("0.0.1-gamma"), invalid_argument);
		EXPECT_THROW(Version ver1("0.0.1-alpha"), invalid_argument);
		EXPECT_THROW(Version ver1("0.0.1-alpha.0"), invalid_argument);
		EXPECT_THROW(Version ver1("0.0.0-alpha.1"), invalid_argument);
		EXPECT_THROW(Version ver1("1.0.0-alpha"), invalid_argument);
		EXPECT_THROW(Version ver1("-1.0.0-alpha"), invalid_argument);
	}

	TEST(VersionTests, ComparisonTests) {
		Version ver1("1.0.0-alpha.1");
		Version ver2("1.0.0-alpha.1");
		EXPECT_EQ(ver1, ver2);
		EXPECT_GE(ver1, ver2);
		EXPECT_LE(ver1, ver2);
		// Check different prerelease numbers
		Version ver3("1.0.0-alpha.2");
		EXPECT_NE(ver1, ver3);
		EXPECT_LT(ver1, ver3);
		EXPECT_GT(ver3, ver2);
		// Check different prerelease names
		// compare beta and alpha
		Version ver4("1.0.0-beta.1");
		EXPECT_NE(ver4, ver1);
		EXPECT_GT(ver4, ver1);
		EXPECT_LT(ver1, ver4);
		// compare beta and rc
		Version ver5("1.0.0-rc.1");
		EXPECT_NE(ver4, ver5);
		EXPECT_LT(ver4, ver5);
		EXPECT_GT(ver5, ver4);
		// compare alpha and rc
		EXPECT_NE(ver1, ver5);
		EXPECT_GT(ver5, ver1);
		EXPECT_LT(ver1, ver5);
		// Check different major numbers
		Version ver6("2.0.0-alpha.1");
		EXPECT_NE(ver6, ver1);
		EXPECT_GT(ver6, ver1);
		EXPECT_LT(ver1, ver6);
		// Check different minor numbers
		Version ver7("1.1.0-alpha.1");
		EXPECT_NE(ver7, ver1);
		EXPECT_GT(ver7, ver1);
		EXPECT_LT(ver1, ver7);
		// Check different rev numbers
		Version ver8("1.0.1-alpha.1");
		EXPECT_NE(ver8, ver1);
		EXPECT_GT(ver8, ver1);
		EXPECT_LT(ver1, ver8);
		// Check abbreviated version
		Version ver9("1.0-alpha.1");
		EXPECT_EQ(ver9, ver1);
		Version ver10("1.0-beta.1");
		EXPECT_EQ(ver10, ver4);
	}

	TEST(VersionTests, GetVersionStrTests) {
		string version_str = "1.0.0-beta.1";
		Version ver1(version_str);
		EXPECT_EQ(version_str, ver1.getVersionStr());
		stringstream ss1;
		ss1 << ver1;
		EXPECT_EQ(version_str, ss1.str());
		version_str = "2.0.1";
		Version ver2(version_str);
		EXPECT_EQ(version_str, ver2.getVersionStr());
		stringstream ss2;
		ss2 << ver2;
		EXPECT_EQ(version_str, ss2.str());
	}
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	// Redirect cout to NULL to suppress command line output during the tests
	//cout.rdbuf(NULL);
	return RUN_ALL_TESTS();
}
