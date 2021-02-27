# Threads
This repository includes a couple of projects explaning the basic functionality of the usage of threads 
in programs. 

The first "ThreadsExample" corresponds to a simple selling of tickets process where a set of sellers have
a unique sorce of tickets to sell from. If any seller has the hability to sell without checking if the 
source has been emptied a logical problem would arise. Thus in order to adequatelly model the behaviour of 
the selling tickets process, a threaded program has been written.

The second example "IceCream" corresponds to the proccess of selling IceCream to a group of customers. The 
problem ilustrates the modelling of the proccess of IceCream sellong as a customer might take some time to 
choice a specific flavor, so in order to avoid a queue, the next available customer with a flavor selected 
may place its order. In this cases the model represents the interaction between different procceses rather 
than the problem of a unique source of tickets as the previous case.
