#ifndef RAND_H
#define RAND_H

#include <random>

class RNG
{

public:

    float nextFloat(){ return floatU(engine); }
    float nextNormal() { return floatN(engine); }

private:

    static std::uniform_real_distribution<float> floatU;
    static std::normal_distribution<float> floatN;
    static std::random_device device;
    static std::mt19937 engine;

};

#endif /* RAND_H */

