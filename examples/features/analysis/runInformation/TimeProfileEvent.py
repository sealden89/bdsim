#!/usr/bin/env python

"""
This script prudces an event run duration summary and a pdf historgram for a BDSIM rootevent format output file.
The summary contains grand total event run time from all recorded events, the mean run duration per event and the standard error on 
the mean time [stdev/sqrt(nparticles)]. 

(!) The grand total event run duration is not the same as the total run duration quoted in the output file as it only considers the
    time spent events and does not include the io 

<PATH_TO_SCRIPT> --filepath=<PATH_TO_FILE>                             - to execute on a file
<PATH_TO_SCRIPT> --dirpath=<PATH_TO_FILE> --ofilename=<FILENAME>       - to execute on a file and specify summary file name
                                       
options: dirpath, ofilename, verbose 

Example: ./TimeProfileEvent.py --filepath=afile.root            to process a file in the current working dir              

Notes: 
1. No quotation marks needed for option strings (eg. --filepath="/path/file" -> --filepath=/path/file)
2. Absolute and relative paths are supported
3. In the filepath option ./ is not needed to specify current folder (eg. --filepath=./folder/file -> --filepath=folder/file)
5. Only rootevent files can be processed (specified by the presence of "event" somewhere in the filename)
7. Default summary text file name is <FILE_NAME>_EventSummary.txt
8. Default histogram file name is <FILE_NAME>_EventHisto.pdf
8. Add -v flag to the execution for additional prinouts wih information
"""

import optparse
import glob as _glob
import os as _os
import time as _time
import numpy as _np
import warnings as _warnings
import ROOT as _ROOT
import root_numpy as _rnp
import matplotlib.mlab as _mlab
import matplotlib.pyplot as _plt


class TimeProfileEvent:
    def __init__(self, filepath, debug=False):

        cwd        = _os.getcwd()
        path       = filepath.split("/")
        filename   = path[-1]                         #last element is the filename

        if(path[0]=="/"):                             #when absolute filepath is given
            dirpath = path[:-1]
        elif(path[0]=="." and path[1]=="/"):          #when ./ is used to specify current folder
            path=path[1:]
            dirpath = cwd+"/".join(path[:-1])         #when relative filepath is given
        else:
            dirpath = cwd+"/"+"/".join(path[:-1])     #when relative filepath is given

        print "Filename: ", filename
        print "Drectory: ", dirpath

        if "event" in filename and ".root" in filename:                
            self.filename    = filename[:-5]
            self.filepath    = filepath
            self.dirpath     = dirpath
            
        else:
            raise IOError('Invalid file format, rootevent file (something.root) required')

                
        self.dirname    = self.dirpath.split("/",-1)[-1] 
        self.debug      = debug
        self.duration   = []
        self.nparticles = 0
        print "Time profile> Reading file:", self.dirpath+self.filename
        print self.filepath
        print self.dirpath

    def getRunDuration(self):
        """
        Extracts the event duration variable from the specified rootevent output file. 
        """
        _os.chdir(self.dirpath)
        rootin          = _ROOT.TFile(self.filepath)
        run             = rootin.Get("Event")
        duration        = _rnp.tree2array(run, branches="Info.duration")
        self.nparticles = len(duration)
    
        self.duration.append(duration)
        
        print "Particles: ", self.nparticles
        print "Duration:", self.duration

    def writeSummaryFile(self, filename="default"):
        """
        Writes a summary file of run durations from output files in the target directory including mean run time 
        and standard deviation
        """

        if not self.duration: #Dont write summary file if no rootevent files are present in directory
            return
        
        if filename == "default":
            ofname = self.filename+"_EventSummary.txt"
        elif str(filename)[-3:] == ".txt":
            ofname = filename
        else:
            ofname = str(filename)+".txt"

        duration_mean = _np.mean(self.duration)
        duration_std  = _np.std(self.duration)/_np.sqrt(self.nparticles)
        duration_tot  = _np.sum(self.duration)
            
        # write standard deviations to file
        _os.chdir(self.dirpath)
        with open(ofname, 'w') as outfile:
            timestamp = _time.strftime("%Y/%m/%d-%H:%M:%S")
            t  = timestamp+' '+self.filepath+'\n\nEvent duration summary \n\n'
            h  = 'Number of events: '+str(self.nparticles)+', Total event duration: '+str(duration_tot)+'s, Average event duration: '+str(duration_mean)+" +- "+str(duration_std)+" s \n\n"
            outfile.writelines(t)
            outfile.writelines(h)
                
        print "Time profile> Summary written to:", ofname

    def writeHistogram(self, filename="default"):
        """
        Writes a histogram of event durations from output files in the target directory
        """

        if not self.duration: #Dont write summary file if no rootevent files are present in directory
            return
        
        if filename == "default":
            ofname = self.filename+"_EventHisto.pdf"
        elif str(filename)[-3:] == ".pdf":
            ofname = filename
        else:
            ofname = str(filename)+".pdf"
            
        # write histogram
        print len(self.duration)
        n, bins, patches = _plt.hist(self.duration, 30, normed=False, facecolor='green', alpha=0.75)
        _plt.xlabel('Event duration (s)')
        _plt.ylabel('Count')
        #plt.axis([40, 160, 0, 0.03])
        _plt.grid(True)
        _plt.savefig(ofname, bbox_inches='tight')
                
        print "Time profile> Histogram written to:", ofname


def Run(filepath, ofilename, verbose):
    t = TimeProfileEvent(filepath, verbose)
    t.getRunDuration()
    
    t.writeSummaryFile(ofilename)
    t.writeHistogram(ofilename)

def Main():
    usage = ''
    parser = optparse.OptionParser(usage)
    #parser.add_option('-c','--clean',     action='store_true',default=False,help=Clean.__doc__)
    parser.add_option('-f','--filepath',  action='store',     dest="filepath",  type="string", default=None)
    parser.add_option('-w','--ofilename',  action='store',     dest="ofilename",  type="string", default="default")
    parser.add_option('-v','--verbose',   action='store_true',dest="verbose",                  default=False)

    options,args = parser.parse_args()

    if args:
        print "ERROR when parsing, leftover arguments", args
        raise SystemExit
    

    Run(options.filepath, options.ofilename, options.verbose)

if __name__ == "__main__":
    Main()
