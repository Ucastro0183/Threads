/*
 * IceCream.cpp
 *
 *  Created on: Sep 26, 2014
 *      Author: ucastro
 */

#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <queue>
#include <vector>
#include <atomic>
#include <thread>

using namespace std;

const int num_customers = 10;

struct inspectionT {
	bool pass;					// Init false
	atomic_flag request;		// Init closed until a cone is made
	atomic_flag finished;		// Init closed until manager finish inspection
	atomic_flag managerLock; 	// Init open to enter to manager
};

struct lineT {
	int number;
	atomic_flag requested;
	atomic_flag customers[num_customers];
	atomic_flag lock;
};

inspectionT inspection;
lineT line;
vector<thread> threads;

void init();
void manager(int conesNeeded);
void clerk(atomic_flag *instruction);
void customer(int cones);
void goToCashier();
void cashier();

int main() {
	int totalCones;

	init();
	srand (time(NULL));
	for (int i=0; i<num_customers; i++) {			//creating customers
		int cones = rand() % 4 + 1;
		threads.push_back(thread(customer,cones));	//start asking for cones
		totalCones += cones;
	}
	threads.push_back(thread(manager,totalCones));	//start inspection of cones that will be made
	threads.push_back(thread(cashier));
	for (auto& th : threads) th.join();
	return (0);
}

void init() {
	inspection.pass = false;				// Initialized false
	inspection.request.test_and_set();		// Initialized closed until a cone is made
	inspection.finished.test_and_set();		// Initialized closed until manager finish inspection
	inspection.managerLock.clear(); 		// Initialized open to enter to manager
	line.lock.clear();
	line.number = 0;
	line.requested.test_and_set();
	for (int i=0; i<num_customers; i++)
		line.customers[i].test_and_set();
}


void customer(int cones) {
	this_thread::sleep_for(chrono::milliseconds(rand() % 50));	//Deciding which cone to order
	vector<atomic_flag *> instructions;					//vector of pointers to instructions
	for (int i=0; i<cones; i++) {
		atomic_flag *instruction = new atomic_flag;
		while (instruction->test_and_set()) {}			//lock flag to wait cone to be done
		instructions.push_back(instruction);			//store flag in vector
		threads.push_back(thread(clerk,instruction));	//launch instructions to clerk to start making cones in background
	}
	for (auto &inst : instructions) {
		while (inst->test_and_set()) {}					//Wait until all the cones have been made
	}													//A vector for the threads could have been defined and join all those threads
	goToCashier();

}

void manager(int conesNeeded) {
	int numAproved = 0;
	int numInpected = 0;

	while (numAproved < conesNeeded) {
		while (inspection.request.test_and_set()) {} //wait until an inspection is demanded
		numInpected++;
		inspection.pass = rand() % 2;
		if (inspection.pass)
			numAproved++;
		inspection.finished.clear();	// indicates the clerk that the inspection has finished
	}
}

void clerk(atomic_flag *instruction) {
	bool passed = false;
	//MakeCone
	while (!passed) {
		while(inspection.managerLock.test_and_set()) {}		// lock manager office
		inspection.request.clear();							// request inspection by allowing manager to execute inspection
		while(inspection.finished.test_and_set()) {}		// wait inspection has been done
		passed = inspection.pass;
		inspection.managerLock.clear();						// unlock manager office
	}
	instruction->clear();
}

void goToCashier () {
	line.lock.test_and_set();		//lock for ticket number generation
	int place = line.number++;
	line.lock.clear();
	line.customers[place].clear();
}

void cashier() {
	int cust = 0;
	while ( cust <= num_customers) {
		while (line.customers[cust].test_and_set())	{} //Wait next customer
		line.requested.test_and_set();
		printf("customer %d satisfied.",cust);
		line.requested.clear();
		cust++;
	}
}
