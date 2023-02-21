from numba import jit, prange

@jit(nopython=True, fastmath=True) 
def find_lambda_jit_m(target, source):
    """
    Finds the longest subsequence of the target array, 
    starting from index 0, that is contained in the source array.
    Returns the length of that subsequence + 1.
    
    i.e. returns the length of the shortest subsequence starting at 0 
    that has not previously appeared.
    
    Args:
        target: NumPy array, preferable of type int.
        source: NumPy array, preferable of type int.
    
    Returns:
        Integer of the length.
        
    """
    
    source_size = source.shape[0]-1
    target_size = target.shape[0]-1
    t_max = 0
    c_max = 0

    for si in range(0, source_size+1):
        if source[si] == target[0]:
            c_max = 1
            for ei in range(1,min(target_size+1, source_size - si+1)):
                if(source[si+ei] != target[ei]):
                    break
                else:
                    c_max = c_max+1

            if c_max > t_max:
                t_max = c_max 
                
    return t_max+1




@jit(nopython=True, parallel=True)
def get_all_lambdas_m(target, source, relative_pos, lambdas):
    """ 
    Finds all the the longest subsequences of the target, 
    that are contained in the sequence of the source,
    with the source cut-off at the location set in relative_pos.
    
    See function find_lambda_jit for description of 
        Lambda_i(target|source)
    
    Args:
        target: Array of ints, usually corresponding to hashed words.
        
        source: Array of ints, usually corresponding to hashed words.
        
        relative_pos: list of integers with the same length as target denoting the                                                               
            relative time ordering of target vs. source. These integers tell us the 
            position relative_pos[x] = i in source such that all symbols in source[:i] 
            occurred before the x-th word in target.  
            
        lambdas: A pre-made array of length(target), usually filled with zeros. 
            Used for efficiency reasons.
        
    Return:
        A list of ints, denoting the value for Lambda for each index in the target. 
    
    """
    i = 0
    while relative_pos[i] == 0: # Preassign first values to avoid check
        lambdas[i] = 1
        i+=1

    # Calculate lambdas
    for i in prange(i, len(target)):
        lambdas[i] = find_lambda_jit_m(target[i:], source[:relative_pos[i]]) 
            
    return lambdas


@jit(nopython=True, fastmath=True) 
def find_lambda_jit(target, source):
    """
    Finds the longest subsequence of the target array, 
    starting from index 0, that is contained in the source array.
    Returns the length of that subsequence + 1.
    
    i.e. returns the length of the shortest subsequence starting at 0 
    that has not previously appeared.
    
    Args:
        target: NumPy array, preferable of type int.
        source: NumPy array, preferable of type int.
    
    Returns:
        Integer of the length.
        
    """
    
    source_size = source.shape[0]-1
    target_size = target.shape[0]-1
    t_max = 0
    c_max = 0

    for si in range(0, source_size+1):
        if source[si] == target[0]:
            c_max = 1
            for ei in range(1,min(target_size, source_size - si+1)):
                if(source[si+ei] != target[ei]):
                    break
                else:
                    c_max = c_max+1

            if c_max > t_max:
                t_max = c_max 
                
    return t_max+1




@jit(nopython=True, parallel=True)
def get_all_lambdas(target, source, relative_pos, lambdas):
    """ 
    Finds all the the longest subsequences of the target, 
    that are contained in the sequence of the source,
    with the source cut-off at the location set in relative_pos.
    
    See function find_lambda_jit for description of 
        Lambda_i(target|source)
    
    Args:
        target: Array of ints, usually corresponding to hashed words.
        
        source: Array of ints, usually corresponding to hashed words.
        
        relative_pos: list of integers with the same length as target denoting the                                                               
            relative time ordering of target vs. source. These integers tell us the 
            position relative_pos[x] = i in source such that all symbols in source[:i] 
            occurred before the x-th word in target.  
            
        lambdas: A pre-made array of length(target), usually filled with zeros. 
            Used for efficiency reasons.
        
    Return:
        A list of ints, denoting the value for Lambda for each index in the target. 
    
    """
    i = 0
    while relative_pos[i] == 0: # Preassign first values to avoid check
        lambdas[i] = 1
        i+=1

    # Calculate lambdas
    for i in prange(i, len(target)):
        lambdas[i] = find_lambda_jit(target[i:], source[:relative_pos[i]]) 
            
    return lambdas


def timeseries_cross_entropy(time_tweets_target, time_tweets_source):

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
    print(relative_pos)
    lambdas = np.zeros(len(target), dtype = np.uint32) # Premake for efficiency
    
    return get_all_lambdas(target, source, relative_pos, lambdas)