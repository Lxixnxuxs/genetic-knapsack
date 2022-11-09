//
// Created by Linus on 09.11.2022.
//

#include "genetic.h"

void print_generation_stats(int i, GeneticKnapsackPopulation<256, 10> P) {
    std::cout << "generation " << i << ": "  << " (MAX: " <<
    P.maximum_fitness() << ") (AVG: " << P.average_fitness() << ") \n";
}

int main() {

    int values[] = {92,57,49,68,60,43,67,84,87,72};
    int weights[] = {23,31,29,44,53,38,63,85,89,82};
    int max_weight = 165;

    GeneticKnapsackPopulation<256, 10> P(values, weights, max_weight);

    double mutation_factor = 0.01;

    for (int i = 0; i < 100; i++) {
        print_generation_stats(i, P);
        //P.evolve_tournament(mutation_factor);
        P.evolve_roulette(mutation_factor);
    }

    return 0;
}