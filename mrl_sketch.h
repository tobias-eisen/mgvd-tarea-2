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

        int getFirstFreeIdx(vector<int>& arr){
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

        // Insert (recursive implementation, should also work for odd k values)
        void insertar(int newElement, bool printOutput = false) {insertar(newElement, 0, printOutput);}

        void insertar(int newElement, int j, bool printOutput = false)
        {
            vector<int>& A_j = A[j];
            for (int i=0; i<k; i++) {
                if (A_j[i] == numeric_limits<int>::min()){
                    A_j[i] = newElement;
                    break;
                }
            }
            
            if (A_j.back() != numeric_limits<int>::min()){
                sort(A_j.begin(), A_j.end());
                for (int i=0; i<k; i+=2) {
                    int oddIndexElement = A_j[i];
                    insertar(oddIndexElement, j+1);
                }
                fill(A_j.begin(), A_j.end(), numeric_limits<int>::min());
            }
            
            if (printOutput){
                cout << "Inserting: " << newElement << endl;
                print();
            }
        }

        // Insert (implementation exactly like pseudo code, depends on condition that k is an even number)
        void insertEfficient(int newElement)
        { 
            // assert that k is even number
            if (k % 2 != 0) {
                throw std::invalid_argument("k must be an even number for insertEfficient");
            }
            // insert element in A_0
            vector<int>& A_0 = A[0];
            for (int i = 0; i < k; i++) {
                if (A_0[i] != numeric_limits<int>::min()){
                    A_0[i] = newElement;
                    break;
                }
            }
            
            int j = 0;
            while (j < L+1 && A[j].back() != numeric_limits<int>::min()){ // Check if A_j is full (when last element is not INT_MIN)
                vector<int>& A_j = A[j];
                sort(A_j.begin(), A_j.end()); // Sort A_j
                int firstFreeIdx = getFirstFreeIdx(A[j+1]); // get the first free index (to avoid relooping every time)
                for (int a = 0; a < k; a+=2) { // loop over odd indexes of A_j (here even, because start at 0)
                    for (int b = 0; a+b < k; b++)
                        A[j+1][firstFreeIdx+b] = A_j[a]; // insert values from A_j in A_j+1 starting at first free position
                }
                fill(A_j.begin(), A_j.end(), numeric_limits<int>::min()); // clear A_j (reset values to INT_MIN)
                j++;
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
            std::sort(B.begin(), B.end(), [](const auto& left, const auto& right) {
                return left.first < right.first;
            });

            // Get sum of second components until >= r and return first component
            int secondComponentSum = 0;
            if (secondComponentSum >= r) return B[0].first;
            for (pair<int, int> b_i : B){
                secondComponentSum += b_i.second;
                if (secondComponentSum >= r) return b_i.first;
            }
            return -1;
        }

        // Quantile
        int quantile(double phi){return select(floor(phi * n));}

        // Print MRL sketch scheme
        void print(){
            cout << "--- MRL sketch scheme (k = " << k << ", L = " << L << ") ---" << endl;
            for (int j = L; j >= 0; j--) {
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
