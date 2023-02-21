#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <set>

using namespace std;

class LCSFinder {
    private:
    int s1sz, s2sz;
    vector<int> s, suff_arr;
    vector<vector<int>> prank;

    void BuildSuffixArray() {
        suff_arr.resize(s.size());
        iota(suff_arr.begin(), suff_arr.end(), 0);

        // Remap to a 0 to n-1 alphabet
        auto alphabet = s;
        sort(alphabet.begin(), alphabet.end());
        alphabet.erase(unique(alphabet.begin(), alphabet.end()), alphabet.end());
        for (int& e : s)
            e = lower_bound(alphabet.begin(), alphabet.end(), e) - alphabet.begin();

        vector<int> rank(s.size()), tmp_rank(s.size());
        prank.clear();

        // Sort by power of 2 length prefixes
        for (int i = 0; i < s.size(); ++i) rank[i] = s[i];

        // print rank
        // std::cout << "Rank is: ";
        // for (int i: rank)
        //     std::cout << i << ' ';
        // std::cout << '\n';

        // std::cout << "Rank is: " << rank << '\n';
        for (int p2 = 1; p2 <= s.size(); p2 *= 2) {
            // std::cout << "p is: " << p2 << '\n';
            
            prank.push_back(rank);

            // std::cout << "Rank is: ";
            // for (int i: rank)
            //     std::cout << i << ' ';
            // std::cout << '\n';

            // std::cout << "Sorted aray is: ";
            // for (int i: suff_arr)
            //     std::cout << i << ' ';
            // std::cout << '\n';

            sort(begin(suff_arr), end(suff_arr), [&](int i, int j) {
                int r2i = i+p2 < s.size() ? rank[i+p2] : -1;
                int r2j = j+p2 < s.size() ? rank[j+p2] : -1;
                return rank[i] == rank[j] ? r2i < r2j : rank[i] < rank[j];
            });

            // std::cout << "Post Sorted aray is: ";
            // for (int i: suff_arr)
            //     std::cout << i << ' ';
            // std::cout << '\n';

            int ridx = 0;
            tmp_rank[suff_arr[0]] = 0;
            for (int i = 1; i < s.size(); ++i) {
                int r2 = suff_arr[i]+p2 < s.size() ? rank[suff_arr[i]+p2] : -1;
                int r2prev = suff_arr[i-1]+p2 < s.size() ? rank[suff_arr[i-1]+p2] : -1;
                if (rank[suff_arr[i]] != rank[suff_arr[i-1]] || r2 != r2prev)
                    ridx++;
                tmp_rank[suff_arr[i]] = ridx;
            }
            rank = tmp_rank;
        }


        // std::cout << "prank is: ";
        // for (int i: prank[0])
        //     std::cout << i << ' ';
        // std::cout << '\n';
        // std::cout << "prank is: ";
        // for (int i: prank[1])
        //     std::cout << i << ' ';
        // std::cout << '\n';
        // std::cout << "prank is: ";
        // for (int i: prank[2])
        //     std::cout << i << ' ';
        // std::cout << '\n';
        // std::cout << "prank is: ";
        // for (int i: prank[3])
        //     std::cout << i << ' ';
        // std::cout << '\n';
        // std::cout << "prank is: ";
        // for (int i: prank[4])
        //     std::cout << i << ' ';
        // std::cout << '\n';
    }

    public:
    LCSFinder(const vector<int>& s1, const vector<int>& s2) {
        s1sz = s1.size();
        s2sz = s2.size();
        s.insert(s.end(), s1.begin(), s1.end());
        // Assumes int min is never used in s1 and s2 so it can be used a special character
        // which is < all other characters
        s.push_back(numeric_limits<int>::min());
        s.insert(s.end(), s2.begin(), s2.end());

        BuildSuffixArray();
    }

    int FindLCP(int i, int j) {
        // Catches cases when precomp wasn't done or empty strings
        if (prank.size() < 1) return -1;
        int pid = prank.size()-1, p2 = 1<<pid, ans = 0;
        // Essentially do binary counting on matching prefix size using prank
        // std::cout << "pid : " << pid <<'\n';
        while (pid >= 0 && i < s.size() && j < s.size()) {
            // std::cout << "FindLCP " << prank[pid][i] << ' '<< prank[pid][j] << ' ' << p2 << '\n';
            if (i+p2 <= s.size() && j+p2 <= s.size() && prank[pid][i] == prank[pid][j]) {
                i = i+p2;
                j = j+p2;
                ans += p2;
                // std::cout << "FindLCP " << ans << ' ' << i << ' ' << j << '\n';
            }
            --pid;
            p2 /= 2;
        }
        return ans;
    }

    // Computes all the Longest Common Substring (LCS) values for each pair of indexes into s1 and s2.
    // The first part of a pair is an index into s1 and the second is into s2.
    // It is assumed inds is sorted.
    // It is assumed that both the first and second element of each pair are monotonically increasing.
    // Since each is a "time point", the indexes into s1 and s2 should increase monotonically.
    //
    // Returns a list of all the LCS values.
    // For a given pair, say that the first index into s1 is called i and the second into s2 is called j.
    // The corresponding entry in the return list is the length of the longest string that starts in the range s2[0..j)
    // and that matches a prefix of the string s1[i..n).
    // Note that the right end point of the matching substring in s2 may go up to or past j.
    vector<int> ComputeAllLCSs(vector<pair<int, int>>& inds) {
        vector<int> ans, sa_loc(s.size());
        for (int i = 0; i < s.size(); ++i) sa_loc[suff_arr[i]] = i;
        set<int> active;
        int idx = 0;
        for (auto& ind : inds) {
            int s1idx = ind.first;
            int s2idx = s1sz+1+idx;
            while (idx < ind.second) {
                active.insert(sa_loc[s2idx]);
                ++idx;
                s2idx = s1sz+1+idx;
            }
            int lcs = 0;
            auto it = active.lower_bound(sa_loc[s1idx]);
            if (it != active.end()) {
                lcs = max(lcs, FindLCP(suff_arr[*it], s1idx));
            }
            it = active.lower_bound(sa_loc[s1idx]);
            if (it != active.begin()) {
                it = prev(it);
                lcs = max(lcs, FindLCP(suff_arr[*it], s1idx));
            }
            ans.push_back(lcs);
        }
        return ans;
    }

    vector<int> GetS() {
        return s;
    }
    vector<int> GetSA() {
        return suff_arr;
    }

};

vector<int> BruteForceLCSs(const vector<pair<int,int>>& inds, const vector<int>& s1, const vector<int>& s2) {
    vector<int> brute_lcss;
    for (auto& ind : inds) {
        int mx = 0;
        for (int i = 0; i < ind.second; ++i) {
            int k = 0;
            for (; ind.first+k < s1.size() && i+k < s2.size() && s1[ind.first+k] == s2[i+k]; ++k);
            mx = max(mx, k);
        }
        brute_lcss.push_back(mx);
    }
    return brute_lcss;
}


int main() {

    vector<int> s1 = {1, 1, 3, 0, 4, 1, 4, 4, 2, 1};
    vector<int> s2 = {0, 1, 0, 1, 3, 2, 1, 4, 0, 0};
    LCSFinder lcs(s2, s1);

    // Test suffix array construction
    auto s = lcs.GetS();
    auto sa = lcs.GetSA();

    std::cout << "Final suffix aray is: ";
    for (int i: sa)
        std::cout << i << ' ';
    std::cout << '\n';

    std::cout << "Result: " << '\n';

    
    // std::cout << lcs.FindLCP(11,0);
    // std::cout << lcs.FindLCP(12,1);
    // std::cout << lcs.FindLCP(12,2);
    // std::cout << lcs.FindLCP(12,3);
    // std::cout << lcs.FindLCP(13,3);
    // std::cout << lcs.FindLCP(15,4);
    // std::cout << lcs.FindLCP(16,5);
    // std::cout << lcs.FindLCP(14,5);
    // std::cout << lcs.FindLCP(15,6);
    // std::cout << lcs.FindLCP(18,7);
    // std::cout << lcs.FindLCP(16,8);
    // std::cout << lcs.FindLCP(17,8);
    // std::cout << lcs.FindLCP(12,9);
    // std::cout << lcs.FindLCP(13,9);

    vector<pair<int, int>> inds;
    int s2idx = 0;
    for (int i = 0; i < s1.size(); ++i) {
            inds.emplace_back(i, i);
    }

    auto la = lcs.ComputeAllLCSs(inds);

    for (int i: la)
        std::cout << i << ", ";
    std::cout << '\n';

    }

