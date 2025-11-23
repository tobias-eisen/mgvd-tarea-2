#include <iostream>
#include "include/mrl_sketch.hpp"

int main()
{
    // Parameters
    const double epsilon = 0.8;
    const size_t n = 14;
    
    // Initialize MRL sketch
    mrl::MRLSketch sketch(epsilon, n);
    
    // Test stream
    int stream[] = {4, 1, 4, 5, 7, 4, 5, 7, 8, 2, 1, 1, 3, 2};
    
    std::cout << "Inserting stream: {4,1,4,5,7,4,5,7,8,2,1,1,3,2}" << std::endl;
    std::cout << "Parameters: n=" << n << ", epsilon=" << epsilon << std::endl << std::endl;
    
    // Insert elements from stream
    for (int value : stream) {
        sketch.insert(value, false); // set to true for printing sketch after each insertion
    }
    
    // Print the resulting sketch
    cout << "Final sketch:" << endl;
    sketch.print();

    // Test rank(7)
    cout << "rank(7) = " << sketch.rank(7) << endl;
    cout << "quantile(0.2) = " << sketch.quantile(0.2) << endl;
    cout << "select(2) = " << sketch.select(2) << endl;

    return 0;
}
