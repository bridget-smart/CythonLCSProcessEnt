import numpy as np
import bisect 

class LCSFinder:
    def __init__(self, s1, s2):

        '''
        Expects s1 and s2 to be lists.
        '''

        self.s1sz = len(s1)
        self.s2sz = len(s2)

        self.suff_arr = []
        self.prank = [] # doing lots of appends so using list

        # Assumes int min is never used in s1 and s2 so it can be 
        # used a special character which is < all other characters.

        self.s = s1 + [np.iinfo(np.int32).min] + s2

        self.BuildSuffixArray()


    def FindLCP(self, i, j):
        # Catches cases when precomp wasn't done or empty strings
        if len(self.prank) == 0:
            return -1

        pid = len(self.prank) - 1
        p2 = 1 << pid
        ans = 0

        s_size = len(self.s)
        # Essentially do binary counting on matching prefix size using prank
        while ((pid >= 0) and (i < s_size) and (j < s_size)):
            if (i + p2 <= s_size) and (j + p2 <= s_size) and (self.prank[pid][i]) == (self.prank[pid][j]):
                i += p2
                j += p2
                ans += p2

            pid -= 1
            p2 //= 2 # to return int

        return ans


    def GetS(self):
        return self.s

    def GetSA(self):
        return self.suff_arr
    
    
    def ComputeAllLCSs(self, inds):
        ans = []
        sa_loc = [0] * len(self.s)
        for i in range(len(self.s)):
            sa_loc[self.suff_arr[i]] = i
        active = set()
        idx = 0
        for ind in inds:
            s1idx, s2idx = ind[0], self.s1sz+1+idx
            while idx < ind[1]:
                active.add(sa_loc[s2idx])
                idx += 1
                s2idx = self.s1sz+1+idx
            lcs = 0
            it = bisect.bisect_left(sorted(active), sa_loc[s1idx])
            while it < len(active):
                match = self.FindLCP(self.suff_arr[list(active)[it]], s1idx)
                if self.suff_arr[list(active)[it]]+match > s2idx:
                    match = s2idx-self.suff_arr[list(active)[it]]
                else:
                    lcs = max(lcs, match)
                    break
                lcs = max(lcs, match)
                it += 1
            it = bisect.bisect_left(sorted(active), sa_loc[s1idx])
            while it > 0:
                it -= 1
                match = self.FindLCP(self.suff_arr[list(active)[it]], s1idx)
                if self.suff_arr[list(active)[it]]+match > s2idx:
                    match = s2idx-self.suff_arr[list(active)[it]]
                else:
                    lcs = max(lcs, match)
                    break
                lcs = max(lcs, match)
            ans.append(lcs)
        return ans

    def BuildSuffixArray(self):

        '''
        This function is a part of an implementation of the longest common substring (LCS) algorithm, 
        which is used to find the longest common substring between two input strings. The function works 
        by building a suffix array for the input string s.

        The function starts by initializing the suffix array suff_arr to be a vector of integers from
        0 to the size of s-1. The function then remaps the input string to a 0 to n-1 alphabet to simplify
        the subsequent operations. This is done by sorting the characters in the input string,
        removing duplicates, and then mapping each character in the input string to its corresponding 
        index in the sorted, unique character list.

        The next step is to build the suffix array. The function starts by initializing two vectors of integers 
        rank and tmp_rank to be of the same size as the input string. rank is initialized to be the alphabet 
        indices of the characters in the input string. The function then loops through power-of-2 prefixes
        of the input string and builds the suffix array incrementally.

        At each iteration of the loop, the function generates a new suffix array by sorting the suffixes 
        in the current suffix array. The suffixes are sorted by comparing their rank values, which 
        correspond to their alphabetical order in the remapped alphabet. For suffixes that have 
        the same rank, the comparison is based on their rank values at the next power-of-2 prefixes. 
        This is done by comparing r2i and r2j, which correspond to the rank values of the suffixes
        at the next power-of-2 prefixes.

        The function then updates the rank values for the new suffix array. The rank of each suffix
        in the new suffix array is determined based on its rank value and the rank values of the 
        suffixes immediately preceding it in the old suffix array. If the rank value of a suffix 
        is different from the rank value of the preceding suffix or if the rank value of the next 
        power-of-2 prefix of the suffix is different from that of the preceding suffix, then the 
        rank index is incremented.

        The function repeats this process until all prefixes of the input string have been 
        considered. The function then stores the rank

        Doing correct thing!!!
        '''
        s_size = len(self.s)
        self.suff_arr = [x for x in range(s_size)]

        # Remap to a 0 to n-1 alphabet
        alphabet = sorted(self.s)
        alphabet = list(dict.fromkeys(alphabet))
        self.s = [alphabet.index(e) for e in self.s] # if e != np.iinfo(np.int32).min else e # we can leave the intmin in since it should -> 0

        self.prank = [] # sets prank to an empty vector
        rank = self.s.copy()
        p2 = 1

        while p2<=s_size:
            self.prank.append(rank)
            self.suff_arr.sort(key=lambda i: (rank[i], rank[i+p2] if i+p2 < s_size else -1))

            ridx = 0
            
            tmp_rank = [0] * len(rank)
            tmp_rank[self.suff_arr[0]] = 0


            for i in range(1, s_size):
                # value_when_true if condition else value_when_false

                r2 = rank[self.suff_arr[i] + p2] if (self.suff_arr[i] + p2) < s_size else -1

                r2prev = rank[self.suff_arr[i - 1] + p2] if self.suff_arr[i - 1] + p2 < s_size else -1

                if (rank[self.suff_arr[i]] != rank[self.suff_arr[i - 1]]) or (r2 != r2prev):
                    ridx += 1

                tmp_rank[self.suff_arr[i]] = ridx

            rank = tmp_rank

            p2 = p2*2
 