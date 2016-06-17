import sys, getopt
import os
import subprocess
from subprocess import Popen, PIPE
from ROOT import *
import ROOT
import re

########################
#	Okay, so what's this?
#		input either a single run argument or two runs
#		Output some important info - number of files scanned, number of flags thrown
#		Make folder of timefunc plots
########################

if len(sys.argv) < 2:
    print "Need two arguments"
    sys.exit()
if len(sys.argv) == 2:
	print "Come on, add one more argument"
if len(sys.argv) == 3:
	if sys.argv[2] < sys.argv[1]:
		print "Please put those in the right order. We both know I could switch those around, but I won't."
		sys.exit()
	else:
		bottom = int(sys.argv[1])
		top = int(sys.argv[2])

if os.path.isfile("dqm_status.root") == False:
	print "No dqm_status, so we can't really do anything here"
	sys.exit()
else:
	f = ROOT.TFile("dqm_status.root")

# Now, let's see if we've even gone over the runs we're asking for
f.status.GetEntry(f.status.GetEntries()-1)
if top > f.status.run_number:
	print "You haven't scanned a run that high"
	sys.exit()
f.status.GetEntry(0)
if bottom < f.status.run_number:
	print "You haven't scanned a run that low"
	sys.exit()

# Okay. Fucking great. We made it this far! Now, call C++ to finish the job
os.system('./summaryPlots %i %i'%(bottom,top))

f.Close()
