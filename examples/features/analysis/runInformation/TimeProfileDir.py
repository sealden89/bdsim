#!/usr/bin/env python

"""
This script prudces a run duration summary for BDSIM rootevent format output files in a directory.
The summary contains grand total run time from all files, the mean run duration per file, the standard error on 
the mean time [stdev/sqrt(nfiles)] and a list of files with total run duration from each. 

<PATH_TO_SCRIPT>                                                        - to execute in the current working directory
<PATH_TO_SCRIPT> --dirpath=<PATH_TO_DIRECROTY>                          - to execute in another directory
<PATH_TO_SCRIPT> --dirpath=<PATH_TO_DIRECROTY> --ofilename=<FILENAME>   - to execute in another directory and 
                                                                          set a differnt summary file name
                                       
options: dirpath, ofilename, verbose 

Example: ./TimeProfileDir.py                            to process current working direcctory              

Notes: 
1. No quotation marks needed for option strings (eg. --filepath="/path/file" -> --filepath=/path/file)
2. Absolute and relative paths are supported
3. In the filepath option ./ is not needed to specify current folder (eg. --filepath=./folder/file -> --filepath=folder/file)
5. Only rootevent files in the target directory are processed (specified by the presence of "event" somewhere in the filename)
6. Does not recurse down sub-directories in the target directory
7. Default summary text file name is <DIR_NAME>_durationSummary.txt
8. Add -v flag to the execution for additional prinouts wih information
9. No summary file is produced if no rootevent format files are found in the target directory
"""

import optparse
import glob as _glob
import os as _os
import time as _time
import numpy as _np
import warnings as _warnings
import ROOT as _ROOT
import root_numpy as _rnp


class TimeProfileOutput:
    def __init__(self, dirpath="cwd",debug=False):
        if dirpath == "cwd":
            self.dirpath = _os.getcwd()
            
        else:
            if(_os.path.isdir(str(dirpath))):
                self.dirpath = dirpath
            else:
                _warnings.warn("No such directory: "+str(dirpath))
                _os._exit(1)
                
        self.dirname = self.dirpath.split("/",-1)[-1] 
        self.debug   = debug
        self.duration = {}
        print "Time profile> Reading dir:", self.dirpath

    def getRunDuration(self):
        """
        Extracts the run time variable from all bdsim rootevent output files in the 
        current directory and stores it in a dictionary. Does not recurse into 
        sub-directories
        """
        
        rootfnames   = _glob.glob(self.dirpath+"/*.root")
        rootevfnames = [fname for fname in rootfnames if "event" in fname]

        if not rootevfnames:
            print "Time profile> No rootevent format files found"
            return
        
        for fname in rootevfnames:
            rootin   = _ROOT.TFile(fname)
            run      = rootin.Get("Run")
            duration = _rnp.tree2array(run, branches="Info.duration")[0]
            
            rname = fname.split('/', -1)[-1]
            self.duration[rname] = duration
            
            if self.debug:
                print "File: ",rname, ", Run duration: ", duration, "s"

    def writeSummaryFile(self, filename="default"):
        """
        Writes a summary file of run durations from output files in the target directory including mean run time 
        and standard deviation
        """

        if not self.duration: #Dont write summary file if no rootevent files are present in directory
            return
        
        if filename == "default":
            ofname = self.dirname+"_durationSummary.txt"
        elif str(filename)[-3:] == ".txt":
            ofname = filename
        else:
            ofname = str(filename)+".txt"

        durations = [self.duration[name] for name in self.duration]
        duration_mean = _np.mean(durations)
        duration_ste  = _np.std(durations)/_np.sqrt(len(durations))
        duration_tot  = _np.sum(durations)
            
        # write standard deviations to file
        _os.chdir(self.dirpath)
        with open(ofname, 'w') as outfile:
            timestamp = _time.strftime("%Y/%m/%d-%H:%M:%S")
            t  = timestamp+' '+self.dirname+'\n\nRun duration summary \n\n'
            h  = 'Total run time: '+str(duration_tot)+'s, Average run time per file: '+str(duration_mean)+" +- "+str(duration_ste)+" s \n\n"
            h2 = 'Run time per file: \n' 
            outfile.writelines(t)
            outfile.writelines(h)
            outfile.writelines(h2)
                   
            for fname in self.duration:
                s = fname +" - "+str(self.duration[fname])+" s \n"
                outfile.writelines(s)
                
        print "Time profile> Summary written to:", ofname


def Run(dirpath, ofilename, verbose):
    t = TimeProfileOutput(dirpath, verbose)
    t.getRunDuration()
    t.writeSummaryFile(ofilename)

def Main():
    usage = ''
    parser = optparse.OptionParser(usage)
    #parser.add_option('-c','--clean',     action='store_true',default=False,help=Clean.__doc__)
    parser.add_option('-f','--dirpath',  action='store',     dest="dirpath",  type="string", default="cwd")
    parser.add_option('-w','--ofilename',  action='store',     dest="ofilename",  type="string", default="default")
    parser.add_option('-v','--verbose',   action='store_true',dest="verbose",                  default=False)

    options,args = parser.parse_args()

    if args:
        print "ERROR when parsing, leftover arguments", args
        raise SystemExit
    

    Run(options.dirpath, options.ofilename, options.verbose)

if __name__ == "__main__":
    Main()
