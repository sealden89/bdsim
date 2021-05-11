import glob
import multiprocessing as mp
import numpy as np
import os

# requires python >3.6

def Chunks(l, n):
    """ Yield successive n-sized chunks from l."""
    return [l[i:i+n] for i in range(0,len(l),n)]

def Combine(output, files):
    command = "bdsimCombine " + output + " " + " ".join(files)
    print("Combinding ",files[0],"to",files[-1])
    os.system(command)

def ReduceRun(globcommand, nPerChunk, outputprefix, nCPUs=4):
    """
    In parallel, apply bdsimCombine to the globcommand set of files combining
    nPerChunk into an output file.

    chunkermp.ReduceRun("testfiles/*.root", 14, "testfiles-merge/", nCPUs=7)
    """
    files = glob.glob(globcommand)
    print(len(files), "files to be combined in chunks of ",nPerChunk)

    # find number of integers required for output file name
    nchars = int(np.ceil(np.log10(len(files))))

    # append underscore if not a dir
    prefix = outputprefix if outputprefix.endswith('/') else outputprefix+"_"
    
    chunks = Chunks(files, nPerChunk)
    if len(chunks[-1]) == 1:
        chunks[-2].extend(chunks[-1])
        chunks.pop()
    chunkname = [prefix + str(i).zfill(nchars)+".root" for i in range(len(chunks))]

    p = mp.Pool(processes=nCPUs)
    p.starmap(Combine, zip(chunkname, chunks))
        
        

