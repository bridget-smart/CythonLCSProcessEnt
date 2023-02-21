import numpy as np
import bisect 

class LCSFinder:
    def __init__(self, s1, s2):
        self.s1sz = len(s1)
        self.s2sz = len(s2)
        self.suff_arr = []
        self.prank = [] 
        self.s = np.concatenate((s1, [np.iinfo(np.int32).min], s2))
        self.BuildSuffixArray()

    def FindLCP(self, i, j):
        if len(self.prank) == 0:
            return -1
        pid = len(self.prank) - 1
        p2 = 1 << pid
        ans = 0
        s_size = len(self.s)
        while ((pid >= 0) and (i < s_size) and (j < s_size)):
            if (i + p2 <= s_size) and (j + p2 <= s_size) and (self.prank[pid][i]) == (self.prank[pid][j]):
                i += p2
                j += p2
                ans += p2
            pid -= 1
            p2 //= 2
        return ans

    def GetS(self):
        return self.s

    def GetSA(self):
        return self.suff_arr

    def BuildSuffixArray(self):
        s_size = len(self.s)
        self.suff_arr = [x for x in range(s_size)]
        alphabet = sorted(self.s)
        alphabet = list(dict.fromkeys(alphabet))
        self.s = [alphabet.index(e) for e in self.s]
        self.prank = []
        rank = self.s.copy()
        p2 = 1
        while p2<=s_size:
            self.prank.append(rank)
            self.suff_arr.sort(key=lambda i: (rank[i], rank[i+p2] if i+p2 < s_size else -1))
            ridx = 0
            tmp_rank = [0] * len(rank)
            tmp_rank[self.suff_arr[0]] = 0
            for i in range(1, s_size):
                r2 = rank[self.suff_arr[i] + p2] if (self.suff_arr[i] + p2) < s_size else -1
                r2prev = rank[self.suff_arr[i - 1] + p2] if self.suff_arr[i - 1] + p2 < s_size else -1
                if (rank[self.suff_arr[i]] != rank[self.suff_arr[i - 1]]) or (r2 != r2prev):
                    ridx += 1
                tmp_rank[self.suff_arr[i]] = ridx
            rank = tmp_rank
            p2 = p2*2

    def ComputeAllLCSs(self, inds):
        ans = []
        s_size = len(self.s)
        sa_loc = [0] * s_size
        for i in range(s_size):
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
            sorted_active = sorted(active)
            it = bisect.bisect_left(sorted_active, sa_loc[s1idx])
            if it < len(sorted_active):
                lcs = max([lcs,self.FindLCP(self.suff_arr[sorted_active[it]], s1idx)])
            it = bisect.bisect_left(sorted_active, sa_loc[s1idx])
            if it != 0:
                it -=1
                lcs = max([lcs,self.FindLCP(self.suff_arr[sorted_active[it]], s1idx)])
            ans.append(lcs)
            active = set(active)
        return ans