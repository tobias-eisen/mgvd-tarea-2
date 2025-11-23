#include <iostream>
#include <fstream>
#include <string>
#include "include/mrl_sketch.hpp"
#include "include/argparse.hpp"

using namespace std;

struct Params : public argparse::Args {
    int& n = kwarg("n", "Number of elements n in the input stream");
    double& epsilon = kwarg("e,epsilon", "Tolerated error epsilon");
    string& input_path  = kwarg("i,input-path", "Path to the txt file of the input stream");
    bool& print_output = flag("print-output", "Activate print output during sketch insertion");
    bool& interactive = flag("interactive", "Activate interactive MRL sketch evaluation");
};

int main(int argc, char *argv[])
{
    auto args = argparse::parse<Params>(argc, argv);
    mrl::MRLSketch sketch(args.epsilon, args.n);
    
    ifstream input_stream(args.input_path);

    if (!input_stream.is_open()) {
        cerr << "Error: Unable to open file \"" << args.input_path << "\"" << endl;
        return 2;
    }
    
    string line;
    while (getline(input_stream, line)) {
        try {
            int new_elem = stoi(line);
            sketch.insert(new_elem, args.print_output);
        } catch (const invalid_argument& e) {
            cerr << "Invalid argument: " << e.what() << " for line: " << line << endl;
        } catch (const out_of_range& e) {
            cerr << "Out of range: " << e.what() << " for line: " << line << endl;
        }
    }
    cout << "Final sketch:" << endl;
    sketch.print(true);
    input_stream.close();

    // Interactive command line interface for MRL sketch evaluation
    if (args.interactive){ 
        string command;
        cout << "\nInteractive MRL sketch evaluation. Available commands:" << endl;
        cout << "  rank(x)       - Get rank of element x" << endl;
        cout << "  select(r)     - Get element at position r (1-indexed)" << endl;
        cout << "  quantile(phi) - Get element at quantile phi (phi in [0,1])" << endl;
        cout << "  exit          - Exit interactive MRL sketch evaluation" << endl;
        
        while (true) {
            cout << "\n> ";
            if (!getline(cin, command)) break;
            
            // Trim whitespace
            command.erase(0, command.find_first_not_of(" \t\n\r"));
            command.erase(command.find_last_not_of(" \t\n\r") + 1);
            
            if (command == "exit") break;
            
            // Parse rank(x)
            if (command.substr(0, 5) == "rank(") {
                size_t close = command.find(')');
                if (close != string::npos) {
                    try {
                        int x = stoi(command.substr(5, close - 5));
                        int result = sketch.rank(x);
                        cout << "rank(" << x << ") = " << result << endl;
                    } catch (...) {
                        cerr << "Error: Invalid integer for rank" << endl;
                    }
                } else {
                    cerr << "Error: Invalid syntax. Use rank(x)" << endl;
                }
            }
            // Parse select(r)
            else if (command.substr(0, 7) == "select(") {
                size_t close = command.find(')');
                if (close != string::npos) {
                    try {
                        int r = stoi(command.substr(7, close - 7));
                        if (r >= 1 && r <= args.n) {
                            int result = sketch.select(r);
                            cout << "select(" << r << ") = " << result << endl;
                        } else {
                            cerr << "Error: r must be in [1, " << args.n << "]" << endl;
                        }
                    } catch (...) {
                        cerr << "Error: Invalid integer for select" << endl;
                    }
                } else {
                    cerr << "Error: Invalid syntax. Use select(r)" << endl;
                }
            }
            // Parse quantile(phi)
            else if (command.substr(0, 9) == "quantile(") {
                size_t close = command.find(')');
                if (close != string::npos) {
                    try {
                        double phi = stod(command.substr(9, close - 9));
                        if (phi >= 0.0 && phi <= 1.0) {
                            int result = sketch.quantile(phi);
                            cout << "quantile(" << phi << ") = " << result << endl;
                        } else {
                            cerr << "Error: phi must be in [0,1]" << endl;
                        }
                    } catch (...) {
                        cerr << "Error: Invalid number for quantile" << endl;
                    }
                } else {
                    cerr << "Error: Invalid syntax. Use quantile(phi)" << endl;
                }
            }
            else {
                cerr << "Error: Unknown command. Use rank(x), select(r), quantile(phi), or exit" << endl;
            }
        }
    }
    
    return 0;
}