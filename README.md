
See UsefulNotes.pdf for more detail.

## Notation

Let the string from the source and target processes be denoted, $S$ and $T$ respectively. 

The realisations from each process are drawn from a random process with a finite sample space containing values from the set $\mathcal{V}$. 

When applied to language, $\mathcal{V}$ represents the vocabulary with each value representing a distinct word and $|\mathcal{V}|$ representing the true vocabulary size.

For the sequence $S$, we let $S_i^j, i \leq j$ represent the subsequence given by $(S_i,S_{i+1},...,S_j)$.

To estimate the Shannon cross entropy rate between $S$ and $T$, at an instant $i$, we estimate the entropy rate between $S_0^i$ and $T_{i+1}^n$. That is, we consider the cross entropy between $S_0,...,S_i$ and $T_{i+1},...,T_n$.


## Entropy estimators
 
For each value of $m$, we calculate the match lengths $l$, such that $S_m,...,S_{m+l} = T_i,...,T_{i+l}$ for $m+l\leq i$. We then set, $\Lambda_i = \max(l) + 1$, which represents the length of the longest unseen sequence beginning at instant $i$.


To estimate the entropy, we calculate each value of $\Lambda_i$ for $i \leq n$, and use one of the following approximations proposed by Kontoyiannis:

(a) 
$$\lim_{n \to \infty} \dfrac{1}{n} \sum_{i=1}^n \dfrac{\Lambda_i^n}{\log n} \to \dfrac{1}{H}$$

(b)
$$\lim_{n \to \infty} \dfrac{1}{n} \sum_{i=1}^n \dfrac{\Lambda_i^i}{\log i} \to \dfrac{1}{H}$$

(c)
$$\lim_{n \to \infty} \dfrac{1}{n} \sum_{i=1}^n \dfrac{\Lambda_i^i}{\log n} \to \dfrac{1}{H}$$

which converges almost surely (Ornstein 1993., Wyner 1989., Kontoyiannis 1998).

## Suffix Array

The implementation in [ProcessEntropy](https://github.com/tobinsouth/ProcessEntropy) uses (c).

## Algorithm for Process Ent

- Generally we do not allow overlaps here

The goal for $S$ and $T$ is to find the longest match length. i.e. we find
$\max{L_n} + 1$. 
$L_n$ is smallest integer $l$ such that the target sequence $T_0^{l-1}$ does not start in the past of the source $S_{-n}^{-1}$.

$$
T_0^{l-1} \neq S_{-m}^{-m+l-1},
$$

for $1\leq m \leq n$. Where $n$ is the history size of the source.

Generally, the process is:

- Iterates through possible $i$ values (start of target sequence)
- For each $i$, returns the longest match length for any sequence in the source which starts before the start of the target sequence.

\section{Algorithm for Fast Ent (math version)}

\section{Algorithm Outline}


This algorithm employs properties of a sorted suffix array to allow the longest match length to be found in O(1) with O(N) precomputation. This is a property of range minimum queries using a sorted suffix array. 

Recall that a sorted suffix array produces a vector of indices which each refer to a suffix sorted in lexicographical order.

Eg. string aabab
has the following suffixes, which we can refer to using the following indices of the string (aabab) as described below:

Suffix:

         b
        ab
       bab
      abab
     aabab

Index:

        5
        4
        3
        2
        1
 
So, we could sort these suffixes and represent this order as follows:
 
sorted suffix array :         1 4 2 5 3
 
In parallel, we can also compute the longest common prefix between adjacent suffixes in the sorted suffix array. Eg the first value, 1, represents the length of the longest common prefix between suffixes starting at index 1 (aabab) and 4 (ab).
    
longest common prefix table :  1 2 0 1 0 (the final zero is chosen arbitrarily). 

On the longest common prefix table we can use a range minimum query (RMQ) to find the length of the longest common prefix between two suffixes. For example the longest common prefix between suffixes 4 and 3 is given by the minimum of the corresponding section of the longest common prefix table (inclusive at the start and exclusive at the end).

SSA : 1 *[4 2 5 3)*

LCP : 1 *[2 0 1 0)*

which is 0. 

Prefix doubling allows the LCP, SSA and RMQ to be constructed simultaneously.

Given the two strings of interest $S$ and $T$, where we are looking for a match starting at $i$ in $S$ which starts prior to index $j$ in $T$.

Begin by constructing a mega string given by $S$ + $n+1$ + $T$ where $n$ is the length of the alphabet and $n+1$ is a symbol not in our alphabet. This prevents a match from starting in $S$ and ending in $T$.

With this string we can construct the LCP, SSA and RMA and resolve the query with (i,j) by finding the first element which corresponds to a suffix in $T$ starting before $j$ to both the left and right of the suffix corresponding to $i$. By the range minimum property of the SSA, this guarantees us that we will get the longest possible prefix as one of these two values.  We call these $a$ and $b$. By taking the $\max(a,b)$, we can guarantee that we have the LCP which matches our query.

Then after, there is some clever stuff using the fact that we constrain $i$ and $j$ to be monotonically increasing to add elements to a binary tree to make sure we aren't doing extra calculations than what we need to do.

## Make equivalent to Process Entropy

In ProcessEntropy, 'slices' are defined using the vector relativepos. 
We find that in ProcessEntropy,

relativepos[x] = i

which gives us that everything in source[:x] occured before target[i:].

To make this equivalent to the FastProcessEntropy implementation, we take:
l\_t =  list of tuples with (x,i)

The following function makes the two equivalent:

    def timeseries_cross_entropy_FE_entropy(time_tweets_target, time_tweets_source):

        '''
        ** FAST ENTROPY ***

        Modified version of the timeseries_cross_entropy function (removed please sanitize), to make equivalent between the original and the FastEntropy implementation.

        Function expects two inputs in the form of a list of time-tweet tuples:

        [(time, [list of integers]),....]
        '''

        decorated_target = [ (time,"target",tweet) for time,tweet in time_tweets_target ]
        decorated_source = [ (time,"source",tweet) for time,tweet in time_tweets_source ]

        # Join time series:
        time_tweets = decorated_target + decorated_source

        # Sort in place by time:                                                                                                                 
        time_tweets.sort()

        # Loop over combined tweets and build word vectors and target->source relative_pos:                                                                     
        target, source, relative_pos = [], [], []
        for time,user,tweet in time_tweets:
            words = tweet
            if user == "target":
                target.extend(words)
                relative_pos.extend( [len(source)]*len(words) )
            else:                                                                                                                        
                source.extend(words)

        target = np.array(target, dtype = np.uint32)
        source = np.array(source, dtype = np.uint32)
        relative_pos = np.array(relative_pos, dtype = np.uint32)

        # set up objects
        source = lcs.Vector1D([int(x) for x in ([np.floor(x) for x in source])])
        target = lcs.Vector1D([int(x) for x in ([np.floor(x) for x in target])])

        ob = lcs.LCSFinder(target,source) # s1 and then s2

        l_t =  lcs.Vector2D(tuple((i,int(relative_pos[i])) for i in range(len(relative_pos))))

        fastentropy_lambdas = np.array([x+1 for x in ob.ComputeAllLCSs(l_t)])

        return fastentropy_lambdas

We can pretty quickly see the speed up. For small sequence lengths, `ProcessEntropy` is slightly faster, but once $N$ gets large, the improved complexity wins.

## Included Code

- Testing ProcessEntropy vs FastEnt This folder includes all code to reproduce the results in this notebook. It performs speed and similarity testing between the LCSFinder package and the Process Entropy package.
- Cython Versions This folder includes all working cython versions. The version LCSCython is the oldest version (code doesn't work properly. LCSCythonLISTtyped contains a version of LCSCython which doesn't use any numpy arrays and implements Cython typing for a speed boost. LCSV2 contains a version which matches lcsfinder2.cpp (allows overlaps).
- LCS.py contains the working python implementation which matches lcsfinder2.cpp.
- LCSlistonly.py contains the version of python using only lists, but it is an old version (matches LCSCythonLIST)
- OriginalCCodeWithComments Contains the old version of the C++ code which has explanatory comments (used to write python version)
- processentfunctions.py contains process entropy functions to allow for local testing and removing the bugs.
- testingCythonVersions performs a speed test on the three Cython versions.
- TestingProcessEntLCSFinderCython compares how the three approaches perform.
- OriginalLCSCCode contains lcsfinder2.cpp the newer version of Max's code which allows overlap.

