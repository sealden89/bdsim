import glob
import numpy as np
import os

def Chunks(l, n):
    """ Yield successive n-sized chunks from l."""
    return [l[i:i+n] for i in range(0,len(l),n)]

def ReduceRun(globcommand, nPerChunk, outputprefix):
    """
    Apply bdsimCombine to the globcommand set of files combining
    nPerchunks into an output file.

    ReduceRun("datadir/*.root", 10, "outputdir/")
    """

    files = glob.glob(globcommand)
    print(len(files), "files to be combined in chunks of ",nPerChunk)

    # find number of integers required for output file name
    nchars = int(np.ceil(np.log10(len(files))))

    # append underscore if not a dir
    prefix = outputprefix if outputprefix.endswith('/') else outputprefix+"_"
    
    chunks = Chunks(files, nPerChunk)
    for i,chunk in enumerate(chunks):
        print('Chunk ',i)
        chunkName = prefix + str(i).zfill(nchars)+".root"
        command = "bdsimCombine "+chunkName + " " + " ".join(chunk)
        print(command)
        os.system(command)
        
        

