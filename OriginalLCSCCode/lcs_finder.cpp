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
        for (int p2 = 1; p2 <= s.size(); p2 *= 2) {
            prank.push_back(rank);
            sort(begin(suff_arr), end(suff_arr), [&](int i, int j) {
                int r2i = i+p2 < s.size() ? rank[i+p2] : -1;
                int r2j = j+p2 < s.size() ? rank[j+p2] : -1;
                return rank[i] == rank[j] ? r2i < r2j : rank[i] < rank[j];
            });
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
        while (pid >= 0 && i < s.size() && j < s.size()) {
            if (i+p2 <= s.size() && j+p2 <= s.size() && prank[pid][i] == prank[pid][j]) {
                i = i+p2;
                j = j+p2;
                ans += p2;
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
    // The corresponding entry in the return list is the LCS in prefix s2[0..j) that matches a prefix of the string s1[i..n).
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
            while (it != active.end()) {
                int match = FindLCP(suff_arr[*it], s1idx);
                if (suff_arr[*it]+match > s2idx) {
                    match = s2idx-suff_arr[*it];
                } else {
                    lcs = max(lcs, match);
                    break;
                }
                lcs = max(lcs, match);
                it = next(it);
            }
            it = active.lower_bound(sa_loc[s1idx]);
            while (it != active.begin()) {
                it = prev(it);
                int match = FindLCP(suff_arr[*it], s1idx);
                if (suff_arr[*it]+match > s2idx) {
                    match = s2idx-suff_arr[*it];
                } else {
                    lcs = max(lcs, match);
                    break;
                }
                lcs = max(lcs, match);
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
            for (; ind.first+k < s1.size() && i+k < ind.second && s1[ind.first+k] == s2[i+k]; ++k);
            mx = max(mx, k);
        }
        brute_lcss.push_back(mx);
    }
    return brute_lcss;
}

#include <random>
#include <cassert>

int main() {
    default_random_engine re;
    const int cases = 10000;
    const int max_len = 100, alpha_sz = 2;
    for (int tc = 0; tc < cases; ++tc) {
        vector<int> s1(re()%max_len), s2(re()%max_len);
        for (int i = 0; i < s1.size(); ++i) {
            s1[i] = re()%alpha_sz;
        }
        for (int i = 0; i < s2.size(); ++i) {
            s2[i] = re()%alpha_sz;
        }
        LCSFinder lcs(s1, s2);

        // Test suffix array construction
        auto s = lcs.GetS();
        auto sa = lcs.GetSA();

        // Naive suffix array construction using O(N^2 log N) time
        vector<int> brute_sa(s.size());
        iota(brute_sa.begin(), brute_sa.end(), 0);
        sort(brute_sa.begin(), brute_sa.end(), [&](int i, int j) {
            for (int k = 0; i+k < s.size() && j+k < s.size(); ++k) {
                if (s[i+k] != s[j+k]) return s[i+k] < s[j+k];
            }
            // Tie-break the same way suffix array construction does
            return i > j;
        });

        cout << "tc " << tc << " " << s1.size() << " " << s2.size() << endl;
        assert(brute_sa == sa);

        // Test LCP
        int a = re()%s.size(), b = re()%s.size();
        int lcp = lcs.FindLCP(a, b);
        int brute_lcp = 0;
        for (int k = 0; a+k < s.size() && b+k < s.size(); ++k) {
            if (s[a+k] != s[b+k]) break;
            brute_lcp++;
        }
        cout << "lcp " << a << " " << b << " " << lcp << " " << brute_lcp << endl;
        assert(lcp == brute_lcp);

        // Test LCSs
        vector<pair<int, int>> inds;
        int s2idx = 0;
        for (int i = 0; i < s1.size(); ++i) {
            if (s2idx < s2.size())
                inds.emplace_back(i, s2idx);
            if (s2idx < s2.size() && re()%2) {
                s2idx++;
            }
        }

        // Naive LCSs algorithm
        auto brute_lcss = BruteForceLCSs(inds, s1, s2);

        auto lcss = lcs.ComputeAllLCSs(inds);
        for (int i = 0; i < inds.size(); ++i) {
            cout << inds[i].first << " " << inds[i].second << " " << lcss[i] << endl;
        }
        assert(brute_lcss == lcss);
    }
}
