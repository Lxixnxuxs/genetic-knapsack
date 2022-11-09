//
// Created by Linus on 03.11.2022.
//

#ifndef GENETIC_GENETIC_H
#define GENETIC_GENETIC_H

#include <iostream>
#include <bitset>
#include <search.h>
#include "random_ex.h"

template <size_t n>
class GeneticKnapsackSolution {
public:
    std::bitset<n> genes;

    GeneticKnapsackSolution() {
        genes.reset();

        RandomGenerator R;

        for (int i = 0; i < n; i++) {
            if (R.random_bool(0.5))
                genes.set(i, true);
        }
    }

    GeneticKnapsackSolution(const GeneticKnapsackSolution& m, const GeneticKnapsackSolution& f) {
        for (int i = 0; i < n; i++) {
            if (i < n/2) {
                genes.set(i, m.genes.test(i));
            } else {
                genes.set(i, f.genes.test(i));
            }
        }
    }

    int fitness_score(const int values[n], const int weights[n], int max_weight) {
        int value=0, weight=0;
        for (int i = 0; i < n; i++) {

            if (genes.test(i)) {
                value += values[i];
                weight += weights[i];
            }
        }

        return weight <= max_weight ? value : 0;
    }

    bool fitter_than(GeneticKnapsackSolution<n>& b, const int values[n], const int weights[n], int max_weight) {
        return fitness_score(values, weights, max_weight) > b.fitness_score(values, weights, max_weight);
    }

    void mutate(double mutation_factor) {
        RandomGenerator R;

        for (int i = 0; i < n; i++) {
            if (R.random_bool(mutation_factor)) {
                genes.flip(i);
            }
        }
    }

    friend std::ostream& operator << (std::ostream& os, const GeneticKnapsackSolution& s) {
        os << "[" << s.genes << "]";
        return os;
    }
};

template <size_t n, size_t k>
class GeneticKnapsackPopulation {
    int *values{}, *weights{}, max_weight{};
    GeneticKnapsackSolution<k> *P;

public:
    GeneticKnapsackPopulation(int v[], int w[], int m): values(v), weights(w), max_weight(m){
        P = new GeneticKnapsackSolution<k>[n];
    };

    // Uses a tournament based selection algorithm
    void evolve_tournament(double mutation_factor) {
        int u, v;
        GeneticKnapsackSolution<k> m, f;
        GeneticKnapsackSolution<k> evolved[n];

        RandomGenerator R;

        for (int i = 0 ; i < n; i++) {
            u = R.random_integer(0, n);
            v = R.random_integer(0, n);
            if (P[u].fitter_than(P[v], values, weights, max_weight)) {
                m = P[u];
            } else {
                m = P[v];
            }
            u = R.random_integer(0, n);
            v = R.random_integer(0, n);
            if (P[u].fitter_than(P[v], values, weights, max_weight)) {
                f = P[u];
            } else {
                f = P[v];
            }

            evolved[i] = GeneticKnapsackSolution<k>(m, f);
            evolved[i].mutate(mutation_factor);
        }

        P = evolved;
    }

    void evolve_roulette(double mutation_factor) {
        RandomGenerator R;

        GeneticKnapsackSolution<k> m, f;
        GeneticKnapsackSolution<k> evolved[n];

        double proportionate_probs[n];

        double total_fitness = 0;
        for (int i = 0; i < n; i++) {
            total_fitness += P[i].fitness_score(values, weights, max_weight);
        }

        for (int i = 0; i < n; i++) {
            proportionate_probs[i] = P[i].fitness_score(values, weights, max_weight) / total_fitness;
        }

        for (int i = 0; i < n; i++) {
            m = P[roulette_select(proportionate_probs, total_fitness, R.random_double_unit())];
            f = P[roulette_select(proportionate_probs, total_fitness, R.random_double_unit())];

            evolved[i] = GeneticKnapsackSolution<k>(m, f);
            evolved[i].mutate(mutation_factor);
        }

        P = evolved;
    }

    int roulette_select(double proportionate_probs[n], double total_fitness, double p) {

        double running_prob = 0.0;

        for (int i = 0; i < n; i++) {
            running_prob += proportionate_probs[i];

            if (p <= running_prob) {
                return i;
            }
        }

        return n-1;
    }

    double average_fitness() {
        double running_sum = 0.;

        for (int i = 0; i < n; i++) {
            running_sum += P[i].fitness_score(values, weights, max_weight);
        }

        return running_sum / n;
    }

    double maximum_fitness() {
        int running_max = 0;

        for (int i = 0; i < n; i++) {
            running_max = std::max(running_max, P[i].fitness_score(values, weights, max_weight));
        }

        return running_max;
    }

    friend std::ostream& operator << (std::ostream& os, GeneticKnapsackPopulation p) {

        if (n == 0) {
            os << "{}";
        } else {
            os << "{" << p.P[0];
            for (int i = 1; i < n; i++) {
                os << "," << p.P[i];
            }
            os << "}";
        }

        return os;
    }
};

#endif //GENETIC_GENETIC_H