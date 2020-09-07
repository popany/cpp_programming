#include <iostream>
#include <random>

class RandNumGenerator
{
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> distrib;

public:
    RandNumGenerator(int min, int max):
        gen(rd()),
        distrib(min, max)
    {}

    void SetSeed(int seed)
    {
        gen.seed(seed);
    }

    int Get()
    {
        return distrib(gen);
    }
};


int main()
{
    RandNumGenerator rd(1, 100);
    for (int i = 0; i < 10; ++i) {
        std::cout << rd.Get() << std::endl;
    }
    int seed = 42;
    rd.SetSeed(seed);
    std::cout << "seed: " << seed << std::endl;
    for (int i = 0; i < 10; ++i) {
        std::cout << rd.Get() << std::endl;
    }

    return 0;
}

