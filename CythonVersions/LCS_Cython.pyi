import numpy as np
import bisect
class LCSFinder:
    cdef public int s1sz
    cdef public int s2sz
    cdef public object suff_arr
    cdef public object prank
    cdef public object s
    def __init__(self, s1, s2):
        ...
    def FindLCP(self, i, j):
        ...
    def GetS(self):
        ...
    def GetSA(self):
        ...
    def ComputeAllLCSs(self, inds):
        ...
    def BuildSuffixArray(self):
        ...
