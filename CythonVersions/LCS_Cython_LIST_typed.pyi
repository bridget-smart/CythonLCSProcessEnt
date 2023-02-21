import numpy as np
import bisect
class LCSFinder:
    cdef public int s1sz
    cdef public int s2sz
    cdef public list suff_arr
    cdef public list prank
    cdef public list s
    def __cinit__(self, s1, s2):
        ...
    def GetS(self) -> list:
        ...
    def GetSA(self) -> list:
        ...
    def FindLCP(self, i:int, j:int) -> int:
        ...
    def ComputeAllLCSs(self, inds) -> list:
        ...
    def BuildSuffixArray(self) -> None:
        ...
