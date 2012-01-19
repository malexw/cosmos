#include <iostream>
#include <stdlib.h>

#include "CosmosSimulation.hpp"

int main(int argc, char* argv[]) {

  srand(31337);

  CosmosSimulation sim;
  std::cout << "Simulation initialized" << std::endl;

  sim.run();

  return 0;
}
