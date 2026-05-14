#ifndef OPTIMIZATIONPARAMS_HPP
#define OPTIMIZATIONPARAMS_HPP

struct OptimizationParams
{
    int pMedians{2};
    double costPerKm{1.0};
    double tMax{1000.0};
    double tMin{0.1};
    double coolingFactor{0.95};
    int iterationsPerStep{100};
};

#endif // OPTIMIZATIONPARAMS_HPP
