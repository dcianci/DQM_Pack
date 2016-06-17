import sys
import os
from array import array

########################
# Okay. So what do we want?
# Takes two arguments starting run number and index for naming (if you want to run again)
# Create dqm_status.root to fill with flag info, run numbers and time dependent functions
# Call dqm_run with single arument (run number)
########################

if len(sys.argv) < 2:
    print "Need at least one argument"
    sys.exit()
if len(sys.argv) == 2:
    if os.path.isfile("dqm_status.root") == False:
		#Outsource this to c++ since pyroot is awful at making ttrees
		os.system("./statusInit")

os.system("python dqm_run.py "+sys.argv[1])
