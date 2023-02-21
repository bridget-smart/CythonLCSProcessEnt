
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <set>

vector<int> s;
s = (1, 2, 2, 2, 2, 1, 1, 1, 0, 1, 2, 2, 1, 1, 2, 1, 1);
suff_arr.resize(s.size()); 
iota(suff_arr.begin(), suff_arr.end(), 0); // Create an array (suff_arr) which contains 0 - N-1 for n is the length of s (N_s+N_t+1)

// Remap to a 0 to n-1 alphabet
auto alphabet = s; 
sort(alphabet.begin(), alphabet.end());
alphabet.erase(unique(alphabet.begin(), alphabet.end()), alphabet.end());
for (int& e : s)
    e = lower_bound(alphabet.begin(), alphabet.end(), e) - alphabet.begin();

vector<int> rank(s.size()), tmp_rank(s.size());
// Create two vectors both with size s

prank.clear(); // turns prank into an empty vector

// Sort by power of 2 length prefixes
for (int i = 0; i < s.size(); ++i) rank[i] = s[i];
// for i from 0 to N-1 in increments of 1, set rank[i] = s[i]

for (int p2 = 1; p2 <= s.size(); p2 *= 2) {
    // for p2 from 1 to <= N, doubling each time

    prank.push_back(rank);
    // append rank to prank

    sort(begin(suff_arr), end(suff_arr), [&](int i, int j) { // sort the array suff_array
    // the code begin(vector) returns a bidirectional iterator starting at the start of suffix array
    // The comparison function is a lambda function that takes two suffix indices i and j, and 
    // compares their ranks. Specifically, it first computes the ranks of the suffixes based on the 
    // current power-of-2 prefixes, which are rank[i] and rank[j]. It then compares these ranks to 
    // determine their relative order. If the ranks are equal, it compares the next power-of-2 
    // prefixes, which are r2i and r2j.

        int r2i = i+p2 < s.size() ? rank[i+p2] : -1;
        int r2j = j+p2 < s.size() ? rank[j+p2] : -1;
        return rank[i] == rank[j] ? r2i < r2j : rank[i] < rank[j];
    });


    int ridx = 0;

    tmp_rank[suff_arr[0]] = 0; 

    // For i from 1 to smaller than len(s) incrementing by 1 each time
    for (int i = 1; i < s.size(); ++i) {
        int r2 = suff_arr[i]+p2 < s.size() ? rank[suff_arr[i]+p2] : -1; // condition ? (if) true : (or) else false
        int r2prev = suff_arr[i-1]+p2 < s.size() ? rank[suff_arr[i-1]+p2] : -1;
        if (rank[suff_arr[i]] != rank[suff_arr[i-1]] || r2 != r2prev)
            ridx++;
        tmp_rank[suff_arr[i]] = ridx;
    }
    rank = tmp_rank;
}