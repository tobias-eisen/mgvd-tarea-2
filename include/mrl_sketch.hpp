#ifndef MRL_SKETCH_H
#define MRL_SKETCH_H

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include <utility>
#include <limits>

using namespace std;

namespace mrl {
    class MRLSketch {
    private:
        const double epsilon;
        const size_t n;
        unsigned int k;
        unsigned int L;
        vector<vector<int> > A;

        int get_first_free_idx(vector<int>& arr){
        for (int i=0; i<k; i++) {
            if (arr[i] != numeric_limits<int>::min())
                return i;
        }
        return -1;
    }
    public:
        MRLSketch(const double epsilon, const size_t n)
            : epsilon(epsilon),
              n(n)
        {
            k = (1 / epsilon) * ceil(log2(epsilon * n)) + 1;
            L = ceil(log2((double) n / k));
            A = vector<vector<int> >(L+1, vector<int>(k, numeric_limits<int>::min()));
        }

        // Insert (recursive implementation)
        void insert(int new_elem, bool print_output = false){insert(new_elem, 0, print_output);}
        void insert(int new_elem, int j, bool print_output = false)
        {
            // Check if MRL sketch overflows with given epsilon
            if (j > L) {
                cerr << "Error: Epsilon value (" << epsilon << ") is too high for input stream size n=" << n << "." << endl;
                cerr << "This causes overflow beyond maximum level L=" << L << "." << endl;
                cerr << "Please use a smaller epsilon value." << endl;
                exit(1);
            }
            
            vector<int>& A_j = A[j];
            for (int i=0; i<k; i++) {
                if (A_j[i] == numeric_limits<int>::min()){
                    A_j[i] = new_elem;
                    break;
                }
            }
            
            if (A_j.back() != numeric_limits<int>::min()){
                sort(A_j.begin(), A_j.end());
                for (int i=0; i<k; i+=2) {
                    int odd_idx_elem = A_j[i];
                    insert(odd_idx_elem, j+1);
                }
                fill(A_j.begin(), A_j.end(), numeric_limits<int>::min());
            }
            
            if (print_output){
                cout << "Inserting: " << new_elem << endl;
                print();
            }
        }

        // Rank
        int rank(int x)
        {
            int ans = 0;
            for (int j=0; j<L+1; j++){
                for (int& z : A[j]){
                    if (z == numeric_limits<int>::min()) break;
                    if (z < x) ans += pow(2, j);
                }
            }
            return ans;
        }

        // Select
        int select(int r)
        {
            // Construct B
            vector<pair<int, int> > B;
            for (int j=0; j<L+1; j++){
                for (int& z : A[j]) {
                    if (z == numeric_limits<int>::min()) break;
                    pair<int, int> b_i(z, pow(2, j));
                    B.push_back(b_i);
                }
            }
            // Sort B by first component
            sort(B.begin(), B.end(), [](const auto& left, const auto& right) {
                return left.first < right.first;
            });

            // Get sum of second components until >= r and return first component
            int second_component_sum = 0;
            if (second_component_sum >= r) return B[0].first;
            for (pair<int, int> b_i : B){
                second_component_sum += b_i.second;
                if (second_component_sum >= r) return b_i.first;
            }
            return -1;
        }

        // Quantile
        int quantile(double phi){return select(floor(phi * n));}

        // Print MRL sketch scheme
        void print(bool compressed_output = false){
            cout << "--- MRL sketch scheme (k = " << k << ", L = " << L << ") ---" << endl;
            for (int j = L; j >= 0; j--) {
                if (compressed_output){
                    if (j == L-3) for (int a=0;a<3;a++) cout << " ." << endl;
                    if (j <= L-3 && j >= 3) continue;
                }
                cout << "A_" << j << ": [";
                for (int i = 0; i < k; i++) {
                    if (A[j][i] != numeric_limits<int>::min())
                        cout << A[j][i];
                    else
                        cout << "-";
                    if (i < k - 1)
                        cout << ", ";
                }
                cout << "]" << endl;
            }
            cout << endl;
        }
    };
}
#endif
