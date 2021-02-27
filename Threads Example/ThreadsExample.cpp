//============================================================================
// Name        : ThreadsExample.cpp
// Author      : ucastro
// Version     :
// Copyright   : GNU
// Description : Hello World in C++, Ansi-style
//============================================================================
//
// This program uses c++11 libraries, to accommodate this, the user have
// to define the flag  -std=c++11 in the compiler instructions in:
// Project Properties > Build Settings > C++ Build > Settings > Miscellaneous
//
// Also, for the preprocessor to function correctly, the user
// have to define the symbol __cplusplus with the value 201103L
// in the Project Properties > C++ General > Paths & Symbols
//
// This settings have to be made for All configurations (Debug & Release)

#include <iostream>		//Standard console output
#include <thread>		//Thread managing
#include <vector>		//vector library for threads
#include <atomic>		//atomic access to global variable (semaphore)
#include <chrono>		//milliseconds halting
#include <cstdlib>		//halting randomness

//macro halts execution of the thread for a milliseconds
#define halt(a) std::this_thread::sleep_for(std::chrono::milliseconds(a))

/* Global variable for lock semaphore */
std::atomic_flag lock = ATOMIC_FLAG_INIT;

void sellTicket(int agent, int *numTicketsp) {
	int count = 0;
	while (true) {
		while (lock.test_and_set()) {}		// -->  Lock semaphore so no other thread can execute
		if (*numTicketsp == 0) break;
		halt(10 + rand() % 41);				// -->  Halts execution for 10 to 50 milliseconds
		(*numTicketsp)--;					// -->  Decrease variable numTickets share by all threads
											//		and protected by semaphore so only one thread decrease
											//		it at the time
		std::cout << "Agent " << agent << " sells a ticket." << std::endl;
		count++;
		lock.clear();						// -->  Release semaphore
	}
	std::cout << "Agent " << agent << " -> sold " << count <<" tickets.\n";
	lock.clear();							// --> Release semaphore if it's locked. (when exiting for loop with break statement)
}

int main() {
	int numAgents = 5;					//number
	int numTickets = 150;				//number of tickets to sell
	std::vector<std::thread> threads;	//vector to hold the execution threads

	srand (time(NULL));					// -->  Initialize the random generator

	for (int agent=1; agent<=numAgents; agent++) {
		//Generates a thread with the sellTicket function and
		//stores it in the vector defined for.
		threads.push_back(std::thread(sellTicket, agent, &numTickets));
	}

	//Wait until all threads have been done
	for (auto& th : threads) th.join();

	return (0);
}
