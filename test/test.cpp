// Copyright (c) 2018 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "gtest/gtest.h"
#include "Simulation.h"
#include "Utils.h"


using namespace std;
using namespace Utils;

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
	class TerminationEvent : public Event {
	public:
		TerminationEvent() : Event() {}
		TerminationEvent(Simulation* simulation_ptr) : Event(simulation_ptr) {}
		string getEventType() const { return "Termination"; }
	};

	// Data members
	list<Object> objects;
	list<Site> sites;
	CreationEvent event_creation;
	list<MoveEvent> events_move;
	list<TerminationEvent> events_termination;
	Event event_previous;
	string event_previous_type;
	int N_move_events = 0;
	int N_termination_events = 0;
	vector<double> displacement_data;

	bool init(const Parameters_Simulation& params, const int id) {
		// Initialize Simulation base class
		Simulation::init(params, id);
		// Initialize Sites
		Site site;
		sites.assign(lattice.getNumSites(), site);
		vector<Site*> site_ptrs;
		for (auto& item : sites) {
			site_ptrs.push_back(&item);
		}
		lattice.setSitePointers(site_ptrs);
		// Perform test initialization
		CreationEvent event_creation_new(this);
		event_creation = event_creation_new;
		event_creation.setDestCoords(lattice.generateRandomCoords());
		event_creation.calculateExecutionTime(1.0);
		addEvent(&event_creation);
		return true;
	}

	void calculateNextEvent(Object* object_ptr) {
		auto move_event_it = find_if(events_move.begin(), events_move.end(), [object_ptr](MoveEvent& a) { return a.getObjectPtr() == object_ptr; });
		auto termination_event_it = find_if(events_termination.begin(), events_termination.end(), [object_ptr](TerminationEvent& a) { return a.getObjectPtr() == object_ptr; });
		// Calculate move events
		move_event_it->calculateExecutionTime(100);
		Coords coords_i = object_ptr->getCoords();
		Coords coords_f;
		int i = 0, j = 0, k = 0;
		uniform_int_distribution<> dist(0, 5);
		int rand_num = dist(generator);
		switch (rand_num) {
		case 0:
			i = -1;
			break;
		case 1:
			i = 1;
			break;
		case 2:
			j = -1;
			break;
		case 3:
			j = 1;
			break;
		case 4:
			k = -1;
			break;
		case 5:
			k = 1;
			break;
		default:
			i = 1;
			break;
		}
		lattice.calculateDestinationCoords(coords_i, i, j, k, coords_f);
		move_event_it->setDestCoords(coords_f);
		// Calculate termination event
		termination_event_it->setDestCoords(coords_i);
		termination_event_it->calculateExecutionTime(10);
		// Select fastest event
		if (move_event_it->getExecutionTime() < termination_event_it->getExecutionTime()) {
			setObjectEvent(object_ptr, &(*move_event_it));
		}
		else {
			setObjectEvent(object_ptr, &(*termination_event_it));
		}
	}

	bool checkFinished() const {
		return (getN_objects_created() == 200000);
	}

	void executeCreationEvent(const std::list<Event*>::const_iterator event_it) {
		Event* event_ptr = *event_it;
		Coords coords_dest = event_ptr->getDestCoords();
		Object object(event_ptr->getExecutionTime(),getN_objects_created()+1,coords_dest);
		objects.push_back(object);
		addObject(&objects.back());
		Simulation* sim_ptr = this;
		MoveEvent event1(sim_ptr);
		event1.setObjectPtr(&objects.back());
		events_move.push_back(event1);
		TerminationEvent event2(sim_ptr);
		event2.setObjectPtr(&objects.back());
		events_termination.push_back(event2);
		// Calculate next creation event
		event_creation.setDestCoords(lattice.generateRandomCoords());
		event_creation.calculateExecutionTime(1.0);
	}

	void executeMoveEvent(const std::list<Event*>::const_iterator event_it) {
		Event* event_ptr = *event_it;
		moveObject(event_ptr->getObjectPtr(), event_ptr->getDestCoords());
		N_move_events++;
	}

	bool executeNextEvent() {
		auto event_it = chooseNextEvent();
		Event* event_ptr = *event_it;
		string event_type = event_ptr->getEventType();
		event_previous = *event_ptr;
		event_previous_type = event_type;
		Coords coords_i;
		Coords coords_f = event_ptr->getDestCoords();
		setTime(event_ptr->getExecutionTime());
		if (event_type.compare("Creation")==0) {
			executeCreationEvent(event_it);
			coords_i = coords_f;
		}
		else {
			coords_i = event_ptr->getObjectPtr()->getCoords();
			if (event_type.compare("Move")==0) {
				executeMoveEvent(event_it);
			}
			else if (event_type.compare("Termination")==0) {
				Object* object_ptr = (*event_it)->getObjectPtr();
				displacement_data.push_back(object_ptr->calculateDisplacement());
				executeTerminationEvent(event_it);
			}
			else {
				return false;
			}
		}
		auto object_vec = findRecalcObjects(coords_i, coords_f);
		for (auto item : object_vec) {
			calculateNextEvent(item);
		}
		return true;
	}

	void executeTerminationEvent(const std::list<Event*>::const_iterator event_it) {
		Object* object_ptr = (*event_it)->getObjectPtr();
		// Remove object from Simulation class
		removeObject(object_ptr);
		// Remove local events
		auto move_event_it = find_if(events_move.begin(), events_move.end(), [object_ptr](MoveEvent& a) { return a.getObjectPtr() == object_ptr; });
		events_move.erase(move_event_it);
		auto termination_event_it = find_if(events_termination.begin(), events_termination.end(), [object_ptr](TerminationEvent& a) { return a.getObjectPtr() == object_ptr; });
		events_termination.erase(termination_event_it);
		N_termination_events++;
	}

	vector<Object*> getAllObjectPtrs() const {
		return getAllObjectPtrs();
	}

	vector<Event*> getAllEventPtrs() const {
		return getAllEventPtrs();
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
				params_base.Enable_periodic_x = true;
				params_base.Enable_periodic_y = true;
				params_base.Enable_periodic_z = true;
				params_base.Length = 50;
				params_base.Width = 50;
				params_base.Height = 50;
				params_base.Unit_size = 1.0;
				params_base.Temperature = 300;
				params_base.Enable_FRM = false;
				params_base.Enable_selective_recalc = true;
				params_base.Recalc_cutoff = 3;
				params_base.Enable_full_recalc = false;
			}
			// Initialize TestSim object
			sim.init(params_base, 0);
			sim.setGeneratorSeed(0);
		}
	};

	TEST_F(SimulationTest, SetupTests) {
		EXPECT_EQ(0, sim.getId());
		EXPECT_FALSE(sim.isLoggingEnabled());
		EXPECT_DOUBLE_EQ(0.0, sim.getTime());
		EXPECT_EQ(300, sim.getTemp());
		EXPECT_DOUBLE_EQ(125000e-21, sim.getVolume());
	}

	TEST_F(SimulationTest, EventExecutionTests) {
		EXPECT_EQ(0, (int)sim.objects.size());
		EXPECT_EQ(1, sim.getN_events());
		EXPECT_TRUE(sim.executeNextEvent());
		EXPECT_EQ(1, (int)sim.objects.size());
		EXPECT_EQ(1, sim.objects.front().getTag());
		EXPECT_EQ(2, sim.getN_events());
		EXPECT_EQ(1, sim.getN_events_executed());
		EXPECT_FALSE(sim.checkFinished());
	}

	TEST_F(SimulationTest, KMCAlgorithmTests) {
		while (!sim.checkFinished()) {
			EXPECT_TRUE(sim.executeNextEvent());
		}
		EXPECT_EQ(200000, sim.getN_objects_created());
		double displacement = vector_avg(sim.displacement_data);
		TestSim sim2;
		params_base.Enable_FRM = true;
		params_base.Enable_selective_recalc = false;
		params_base.Enable_full_recalc = false;
		sim2.init(params_base, 0);
		while (!sim2.checkFinished()) {
			EXPECT_TRUE(sim2.executeNextEvent());
		}
		EXPECT_EQ(200000, sim2.getN_objects_created());
		double displacement2 = vector_avg(sim2.displacement_data);
		EXPECT_NEAR(displacement, displacement2, 5e-2);
		TestSim sim3;
		params_base.Enable_FRM = false;
		params_base.Enable_selective_recalc = false;
		params_base.Enable_full_recalc = true;
		sim3.init(params_base, 0);
		while (!sim3.checkFinished()) {
			EXPECT_TRUE(sim3.executeNextEvent());
		}
		EXPECT_EQ(200000, sim3.getN_objects_created());
		double displacement3 = vector_avg(sim3.displacement_data);
		EXPECT_NEAR(displacement, displacement3, 5e-2);
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

	TEST(UtilsTests, CalculateProbabilityHistTests) {
		mt19937 gen(std::random_device{}());
		uniform_real_distribution<> dist(0, 100);
		vector<double> data((int)1e7);
		for (int i = 0; i < (int)data.size(); i++) {
			data[i] = dist(gen);
		}
		auto hist = calculateProbabilityHist(data, 10);
		uniform_int_distribution<> dist2(0, 9);
		EXPECT_EQ(10, (int)hist.size());
		EXPECT_NEAR(1.0 / 100.0, hist[dist2(gen)].second,1e-4);
		EXPECT_NEAR(1.0 / 100.0, hist[dist2(gen)].second, 1e-4);
		EXPECT_NEAR(1.0 / 100.0, hist[dist2(gen)].second, 1e-4);
		hist = calculateProbabilityHist(data, 10.0);
		EXPECT_EQ(10, (int)hist.size());
		EXPECT_NEAR(1.0 / 100.0, hist[dist2(gen)].second, 1e-4);
		EXPECT_NEAR(1.0 / 100.0, hist[dist2(gen)].second, 1e-4);
		EXPECT_NEAR(1.0 / 100.0, hist[dist2(gen)].second, 1e-4);
		data.clear();
		hist = calculateProbabilityHist(data, 10.0);
		EXPECT_DOUBLE_EQ(0.0, hist[0].first);
		EXPECT_DOUBLE_EQ(0.0, hist[0].second);
		hist = calculateProbabilityHist(data, 5);
		EXPECT_DOUBLE_EQ(0.0, hist[0].first);
		EXPECT_DOUBLE_EQ(0.0, hist[0].second);
		hist = calculateProbabilityHist(data, 1.0, 5);
		EXPECT_DOUBLE_EQ(0.0, hist[0].first);
		EXPECT_DOUBLE_EQ(0.0, hist[0].second);
		data = {0.0, 1.0, 2.0, 3.0, 4.0};
		hist = calculateProbabilityHist(data, 10);
		EXPECT_EQ(5, (int)hist.size());
		hist = calculateProbabilityHist(data, 0.1);
		EXPECT_EQ(5, (int)hist.size());
	}

	TEST(UtilsTests, ExponentialDOSTests) {
		mt19937 gen(std::random_device{}());
		vector<double> data((int)2e7, 0.0);
		createExponentialDOSVector(data, 0.0, 0.1, gen);
		auto hist = calculateProbabilityHist(data, 1000);
		EXPECT_NEAR(1.0, integrateData(hist), 1e-4);
		vector<double> prob;
		for_each(hist.begin(), hist.end(), [&prob](pair<double, double>& x_y) {prob.push_back(x_y.second); });
		double peak = *max_element(prob.begin(), prob.end());
		EXPECT_NEAR(0.5*(1.0 / 0.1), peak, 1e-2*peak);
		createExponentialDOSVector(data, 0.0, 0.05, gen);
		hist = calculateProbabilityHist(data, 1000);
		EXPECT_NEAR(1.0, integrateData(hist), 1e-4);
		prob.clear();
		for_each(hist.begin(), hist.end(), [&prob](pair<double, double>& x_y) {prob.push_back(x_y.second); });
		peak = *max_element(prob.begin(), prob.end());
		EXPECT_NEAR(0.5*(1.0 / 0.05), peak, 1e-2*peak);
	}

	TEST(UtilsTests, GaussianDOSTests) {
		mt19937 gen(std::random_device{}());
		vector<double> data((int)3e7, 0.0);
		createGaussianDOSVector(data, 0.0, 0.15, gen);
		EXPECT_NEAR(0.0, vector_avg(data), 1e-4);
		EXPECT_NEAR(0.15, vector_stdev(data), 1e-4);
		auto hist = calculateProbabilityHist(data, 1000);
		EXPECT_NEAR(1.0, integrateData(hist), 1e-4);
		double peak = hist[499].second;
		EXPECT_NEAR(1.0 / sqrt(2.0*Pi*intpow(0.15, 2)), peak, 1e-1*peak);
		createGaussianDOSVector(data, 0.0, 0.05, gen);
		EXPECT_NEAR(0.0, vector_avg(data), 1e-4);
		EXPECT_NEAR(0.05, vector_stdev(data), 1e-4);
		hist = calculateProbabilityHist(data, 0.005);
		EXPECT_NEAR(1.0, integrateData(hist), 1e-4);
		vector<double> prob;
		for_each(hist.begin(), hist.end(), [&prob](pair<double, double>& x_y) {prob.push_back(x_y.second); });
		peak = *max_element(prob.begin(), prob.end());
		EXPECT_NEAR(1.0 / sqrt(2.0*Pi*intpow(0.05, 2)), peak, 1e-1*peak);
	}

	TEST(UtilsTests, ImportBooleanTests) {
		bool error_status;
		EXPECT_TRUE(importBooleanParam("true", error_status));
		EXPECT_TRUE(importBooleanParam(" true  ", error_status));
		EXPECT_FALSE(importBooleanParam("false	", error_status));
		EXPECT_FALSE(importBooleanParam("   false", error_status));
		EXPECT_FALSE(importBooleanParam("blah", error_status));
		EXPECT_FALSE(importBooleanParam("	blah  ", error_status));
		EXPECT_TRUE(error_status);
	}

	TEST(UtilsTests, IntegrateDataTests) {
		vector<pair<double, double>> data_vec = { {0.0,0.0},{1.0,1.0},{2.0,2.0},{3.0,3.0} };
		auto area = integrateData(data_vec);
		EXPECT_DOUBLE_EQ(area, 9.0 / 2.0);
	}

	TEST(UtilsTests, InterpolateDataTests) {
		vector<pair<double, double>> data_vec(100);
		for (int i = 0; i < (int)data_vec.size();i++) {
			data_vec[i].first = 0.1*i;
			data_vec[i].second = exp(-data_vec[i].first/2.85);
		}
		EXPECT_NEAR(1 / exp(1), interpolateData(data_vec, 2.85), 1e-4);
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
		Object object1(0, 1, { 0,0,0 });
		Object object2(0, 2, { 1,1,1 });
		vector<Object*> object_ptrs{ &object1, &object1, &object2, &object1, &object2 };
		removeDuplicates(object_ptrs);
		EXPECT_EQ(2, (int)object_ptrs.size());
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
		// negative ints
		for (int i = 0; i < 10; i++) {
			int_data[i] = -(i + 1);
		}
		EXPECT_DOUBLE_EQ(-5.5, vector_avg(int_data));
		EXPECT_NEAR(3.02765035409749, vector_stdev(int_data), 1e-14);
		// positive doubles
		vector<double> double_data;
		double_data.assign(10, 0);
		for (int i = 0; i < 10; i++) {
			double_data[i] = 0.5*(i + 1);
		}
		EXPECT_DOUBLE_EQ(2.75, vector_avg(double_data));
		EXPECT_NEAR(1.51382517704875, vector_stdev(double_data), 1e-14);
		// negative doubles
		double_data.assign(10, 0);
		for (int i = 0; i < 10; i++) {
			double_data[i] = -0.5*(i + 1);
		}
		EXPECT_DOUBLE_EQ(-2.75, vector_avg(double_data));
		EXPECT_NEAR(1.51382517704875, vector_stdev(double_data), 1e-14);
	}
}

namespace LatticeTests{
		
	class LatticeTest : public ::testing::Test {
		protected:
			mt19937 gen;
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

	TEST_F(LatticeTest, InitializationTests) {
		EXPECT_EQ(50, lattice.getLength());
		EXPECT_EQ(50, lattice.getWidth());
		EXPECT_EQ(50, lattice.getHeight());
		EXPECT_DOUBLE_EQ(1.0, lattice.getUnitSize());
		EXPECT_EQ((long int)50 * 50 * 50, lattice.getNumSites());
		EXPECT_DOUBLE_EQ(125000e-21,lattice.getVolume());
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
		coords_i = {1, 49, 0};
		coords_f = {0, 0, 49};
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
		vector<Site*> site_ptrs(10, &site);
		EXPECT_FALSE(lattice.setSitePointers(site_ptrs));
		site_ptrs.assign(50 * 50 * 50, &site);
		EXPECT_TRUE(lattice.setSitePointers(site_ptrs));
		Coords coords{5,6,7};
		Site* site1 = *lattice.getSiteIt(coords);
		Site* site2 = site_ptrs[lattice.getSiteIndex(coords)];
		EXPECT_EQ(site1,site2);
		// Check for sites and indices outside the lattice
		coords = {0, 60, 0};
		EXPECT_THROW(lattice.getSiteIndex(coords),out_of_range);
		EXPECT_THROW(lattice.getSiteCoords(150000),out_of_range);
		// Check site object assignment and clearing
		Object object;
		site.setObjectPtr(&object);
		EXPECT_TRUE(site.isOccupied());
		EXPECT_EQ(&object,site.getObjectPtr());
		site.clearOccupancy();
		EXPECT_FALSE(site.isOccupied());
	}
}

namespace EventTests {

	class EventTest : public ::testing::Test {
	protected:
		Parameters_Simulation params_base;
		TestSim test_sim;
		void setUp(){
			{
			params_base.Enable_logging = false;
			params_base.Enable_periodic_x = true;
			params_base.Enable_periodic_y = true;
			params_base.Enable_periodic_z = true;
			params_base.Length = 50;
			params_base.Width = 50;
			params_base.Height = 50;
			params_base.Unit_size = 1.0;
			params_base.Temperature = 300;
			params_base.Enable_FRM = false;
			params_base.Enable_selective_recalc = true;
			params_base.Recalc_cutoff = 3;
			params_base.Enable_full_recalc = false;
			}
			// Initialize TestSim object
			test_sim.init(params_base, 0);
		}
	};
	
	TEST_F(EventTest, GeneralEventTests){
		Event event(&test_sim);
		EXPECT_EQ("Event",event.getEventType());
		Coords coords1(0,0,0);
		Object object1(0.0, 1, coords1);
		event.setObjectPtr(&object1);
		EXPECT_EQ(&object1,event.getObjectPtr());
		Coords coords2(1,0,0);
		Object object2(0.0, 2, coords2);
		event.setObjectTargetPtr(&object2);
		EXPECT_EQ(&object2,event.getObjectTargetPtr());
		event.setDestCoords(coords2);
		EXPECT_EQ(coords2,event.getDestCoords());
		EXPECT_FALSE(event.setExecutionTime(-1.0));
		EXPECT_TRUE(event.setExecutionTime(1.0));
		EXPECT_DOUBLE_EQ(1.0,event.getExecutionTime());
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
		EXPECT_NEAR(1.0, integrateData(hist), 2e-2);
		EXPECT_NEAR(1e9, hist[0].second, 2e7);
		auto it = find_if(hist.begin(), hist.end(), [](pair<double, double>& x_y) {return x_y.first > 1e-9; });
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
		EXPECT_NEAR(1.0, integrateData(hist), 2e-2);
		EXPECT_NEAR(1e12, hist[0].second, 2e10);
		it = find_if(hist.begin(), hist.end(), [](pair<double, double>& x_y) {return x_y.first > 1e-12; });
		it--;
		EXPECT_NEAR(1e12 / 2.7182818284590, (*it).second, 2e10);
		// Test average wait time
		EXPECT_NEAR(vector_avg(times), 1e-12, 2e-15);
	}
}

namespace ObjectTests {
	
	TEST(ObjectTests, GeneralObjectTests) {
		// Assume object is created in a 50 x 50 x 50 lattice
		Coords coords = {0,0,0};
		Object object1(0.0, 1, coords);
		EXPECT_EQ("Object",object1.getObjectType());
		EXPECT_EQ(1,object1.getTag());
		EXPECT_EQ(coords,object1.getCoords());
		EXPECT_DOUBLE_EQ(0.0,object1.getCreationTime());
		// Object-Event tests
		Event event;
		list<Event*> event_ptrs;
		event_ptrs.push_back(&event);
		object1.setEventIt(event_ptrs.begin());
		EXPECT_EQ(&event,*(object1.getEventIt()));
	}
	
	TEST(ObjectTests, CalculateDisplacementTests) {
		// Assume object is created in a 50 x 50 x 50 lattice
		Object object1(0.0, 1, { 0,0,0 });
		object1.setCoords({ 0,0,5 });
		EXPECT_DOUBLE_EQ(5.0, object1.calculateDisplacement());
		object1.setCoords({ 0,0,49 });
		object1.incrementDZ(-50);
		EXPECT_DOUBLE_EQ(1.0, object1.calculateDisplacement());
		object1.setCoords({ 49,49,49 });
		object1.incrementDX(-50);
		object1.incrementDY(-50);
		EXPECT_DOUBLE_EQ(sqrt(3), object1.calculateDisplacement());
		object1.setCoords({ 49,0,49 });
		object1.incrementDY(50);
		EXPECT_DOUBLE_EQ(sqrt(2), object1.calculateDisplacement());
		object1.resetInitialCoords({5,5,5});
		object1.setCoords({ 4,4,4 });
		EXPECT_DOUBLE_EQ(sqrt(3), object1.calculateDisplacement());
	}
	
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
