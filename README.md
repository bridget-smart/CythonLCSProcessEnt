# Notes from 20/02/2022

Today I set about making either a:
1. working version of the C++ code which seems to be causing kernel crashes OR
2. making a version in python (to see if crashes are due to SWIG)

Got a working version in python (LCS) then tried to use Cython to speed things up
These are located in the RewriteinPython Folder.

There is an implentation which only uses LISTS (no numpy arrays)
and which has cython types (LIST_typed)

These sped up the code but it is still slower than the C++ code.

Then I went about checking the speed (see in TestingLCSFindervsCythonvsOriginal)

and found
1. Results don't match between cython implenetatino and the PE / LCSFinder (Max's code)

I then started checking code against lcs_finder2.cpp which was the original version of code sent by Max on the 8th of April 2022. This matches the python implenetaiton (Cython) but doesn't match the PE and LCS implenetations.

Found a minimal broken example

s1 = np.array([1, 1, 3, 0, 4, 1, 4, 4, 2, 1])
s2 = np.array([0, 1, 0, 1, 3, 2, 1, 4, 0, 0])

See details in TestingLCSFindervsCythonvsOriginal > testing.ipynb

The true result should give
lambda = length of longest subsequence 

The goal for $S$ and $T$ is to find the longest match length. i.e. we find
$\max{L_n} + 1$. 
$L_n$ is smallest integer $l$ such that the target sequence $T_0^{l-1}$ does not start in the past of the source $S_{-n}^{-1}$.

$$
T_0^{l-1} \neq S_{-m}^{-m+l-1},
$$

From max's comments, the LCS finder code gives: The corresponding entry in the return list is the length of the longest string that starts in the range s2[0..j)
and that matches a prefix of the string s1[i..n). Note that the right end point of the matching substring in s2 may go up to or past j.

So 
T = s1
S = s2

inds = [(0,1),(1,2),(2,3),...,(8,9)]

So, for (3,4), we want to find the length of match between s2[0,4) and s1(3,...)

ind   0  1  2  3
s2 : [0, 1, 0, 1)

ind   3, ...
s1 : [0, 4, 1, 4, 4, 2, 1)

Longest match has length 1.

Process Entropy gives: 2 (lambda = 3)

For the FastEnt LCS

So we are comparing
source 0 to n, [1 1 3 0 4 1 4 4 2 1] is compared to target to 1, [0]
source 1 to n, [1 3 0 4 1 4 4 2 1] is compared to target to 2, [0 1]
source 2 to n, [3 0 4 1 4 4 2 1] is compared to target to 3, [0 1 0]
source 3 to n, [0 4 1 4 4 2 1] is compared to target to 4, [0 1 0 1]
source 4 to n, [4 1 4 4 2 1] is compared to target to 5, [0 1 0 1 3]
source 5 to n, [1 4 4 2 1] is compared to target to 6, [0 1 0 1 3 2]
source 6 to n, [4 4 2 1] is compared to target to 7, [0 1 0 1 3 2 1]
source 7 to n, [4 2 1] is compared to target to 8, [0 1 0 1 3 2 1 4]
source 8 to n, [2 1] is compared to target to 9, [0 1 0 1 3 2 1 4 0]
source 9 to n, [1] is compared to target to 10, [0 1 0 1 3 2 1 4 0 0]
match lengths of (0, 1, 0, 2, 1, 0, 1, 1, 1, 1)


Output does match both ProcessEntropy and FastEnt.

This is because we have the '2' in index 7

source 7 to n, [4 2 1] is compared to target to 8, [0 1 0 1 3 2 1 4]

It does not match lcs_finder2.cpp which gives (0, 1, 0, 2, 1, 0, 2, 1, 1, 1)
and LCSV2 which gives array([1, 2, 1, 3, 2, 1, 3, 2, 2, 2]) (and LCS)

This code isn't constraining the target to not overlap. For the given example:
With 
source = [1, 1, 3, 0, 4, 1, 4, 4, 2, 1]
target = [0, 1, 0, 1, 3, 2, 1, 4, 0, 0]

looking at matches from target [i:]
with anything starting in source[:i]

should be

1,...
compared with 
.., 1, 0, 1, 3, 2, 1, 4, 0, 0
match of 1

1,1,...
compared with 
.., 0, 1, 3, 2, 1, 4, 0, 0
match of 0

1,1,3,...
compared with
...,1,3,2,1,4,0,0
match of 2

1,1,3,0,...
compared with 
...,3, 2, 1, 4, 0, 0
match of 1

1,1,3,0,4,...
compared with 
..., 2, 1, 4, 0, 0
match of 0

1,1,3,0,4,1...
compared with 
..., 1, 4, 0, 0
match of 1

1,1,3,0,4,1,4...
compared with 
..., 4, 0, 0
match of 1

1,1,3,0,4,1,4,4...
compared with 
..., 0, 0
match of 1

1,1,3,0,4,1,4,4,2...
compared with 
..., 0
match of 1

giving (0),1,0,2,1,0,1,1,1,1
lambdas -> 1,2,1,3,2,1,2,2,2,2


*** WITH OVERLAP ***
i.e. matching target to anything in source stating before i

1,( 1, 3, 0, 4, 1, 4, 4, 2, 1)...
compared with 
.., 1, 0, 1, 3, 2, 1, 4, 0, 0
match of 1

1,1,( 3, 0, 4, 1, 4, 4, 2, 1)...
compared with 
.., 0, 1, 3, 2, 1, 4, 0, 0
match of 0

1,1,3,(0, 4, 1, 4, 4, 2, 1)...
compared with
...,1,3,2,1,4,0,0
match of 2

1,1,3,0,(4, 1, 4, 4, 2, 1)...
compared with 
...,3, 2, 1, 4, 0, 0
match of 1

1,1,3,0,4,(1, 4, 4, 2, 1)...
compared with 
..., 2, 1, 4, 0, 0
match of 0

1,1,3,0,4,1,(4, 4, 2, 1)...
compared with 
..., 1, 4, 0, 0
match of 2 ***** DIFFERENT

1,1,3,0,4,1,4,(4,2,1)...
compared with 
..., 4, 0, 0
match of 1

1,1,3,0,4,1,4,4,(2,1)...
compared with 
..., 0, 0
match of 1

1,1,3,0,4,1,4,4,2,(1)...
compared with 
..., 0
match of 1

giving (0),1,0,2,1,0,2,1,1,1
lambdas -> 1,2,1,3,2,1,3,2,2,2


Ultimately, this seems to be a 'sense' making choice.

BUT - within the functions we only let things overlap to an index, not a time. So for time based applications we need to remove the overlap.


