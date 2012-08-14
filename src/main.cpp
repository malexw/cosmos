#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "CosmosSimulation.hpp"

int main(int argc, char* argv[]) {

  srand(time(0));

  CosmosSimulation sim;
  std::cout << "Simulation initialized" << std::endl;

  sim.run();

  return 0;
}
