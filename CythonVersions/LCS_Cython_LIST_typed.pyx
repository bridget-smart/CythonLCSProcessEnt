import numpy as np
import bisect 

cdef class LCSFinder:
    cdef public int s1sz
    cdef public int s2sz
    cdef public list suff_arr
    cdef public list prank
    cdef public list s

    def __cinit__(self, s1, s2):
        '''
        Expects s1 and s2 to be lists.
        '''
        self.s1sz = len(s1)
        self.s2sz = len(s2)
        self.suff_arr = []
        self.prank = []
        self.s = s1 + [np.iinfo(np.int32).min] + s2
        self.BuildSuffixArray()

    cpdef list GetS(self):
        return self.s

    cpdef list GetSA(self):
        return self.suff_arr

    cdef int FindLCP(self, int i, int j):
        # Catches cases when precomp wasn't done or empty strings
        if len(self.prank) == 0:
            return -1
        cdef int pid = len(self.prank) - 1
        cdef int p2 = 1 << pid
        cdef int ans = 0
        cdef int s_size = len(self.s)
        # Essentially do binary counting on matching prefix size using prank
        while ((pid >= 0) and (i < s_size) and (j < s_size)):
            if (i + p2 <= s_size) and (j + p2 <= s_size) and (self.prank[pid][i]) == (self.prank[pid][j]):
                i += p2
                j += p2
                ans += p2
            pid -= 1
            p2 //= 2 # to return int
        return ans

    cpdef list ComputeAllLCSs(self, inds):
        cdef list ans = []
        cdef int i
        cdef int s_size = len(self.s)
        cdef list sa_loc = [0] * s_size
        for i in range(s_size):
            sa_loc[self.suff_arr[i]] = i
        cdef set active = set()
        cdef int idx = 0
        cdef tuple ind
        cdef list lactive

        for ind in inds:
            s1idx, s2idx = ind[0], self.s1sz+1+idx
            while idx < ind[1]:
                active.add(sa_loc[s2idx])
                idx += 1
                s2idx = self.s1sz+1+idx
            lcs = 0
            it = bisect.bisect_left(sorted(active), sa_loc[s1idx])
            lactive = list(active)
            while it < len(active):
                match = self.FindLCP(self.suff_arr[lactive[it]], s1idx)
                if self.suff_arr[lactive[it]]+match > s2idx:
                    match = s2idx-self.suff_arr[lactive[it]]
                else:
                    lcs = max(lcs, match)
                    break
                lcs = max(lcs, match)
                it += 1
            it = bisect.bisect_left(sorted(active), sa_loc[s1idx])
            while it > 0:
                it -= 1
                match = self.FindLCP(self.suff_arr[lactive[it]], s1idx)
                if self.suff_arr[lactive[it]]+match > s2idx:
                    match = s2idx-self.suff_arr[lactive[it]]
                else:
                    lcs = max(lcs, match)
                    break
                lcs = max(lcs, match)
            ans.append(lcs)
        return ans

    cdef void BuildSuffixArray(self):
        cdef int s_size = len(self.s)
        self.suff_arr = [x for x in range(s_size)]

        # Remap to a 0 to n-1 alphabet
        cdef list alphabet = sorted(self.s)
        alphabet = list(dict.fromkeys(alphabet))
        self.s = [alphabet.index(e) for e in self.s] # if e != np.iinfo(np.int32).min else e # we can leave the intmin in since it should -> 0

        self.prank = [] # sets prank to an empty vector
        cdef list rank = self.s.copy()
        cdef int p2 = 1

        cdef int ridx
        cdef int i
        cdef list tmp_rank

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
