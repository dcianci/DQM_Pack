import sys, getopt
import os
import subprocess
from subprocess import Popen, PIPE
import ROOT
from ROOT import *
import re

# runbuffer is how many blank runs we'll try before accepting that we really are at the end
listout = "pathlist.txt"

if len(sys.argv) == 2:
	thisrun = int(sys.argv[1])
	thisevent = int(sys.argv[2])
else:
	print "no thanks."
	sys.exit()

outf = open(listout,'w')

# Make our list of pathslistout = "runList.txt"
filenames = ''
cmd2 = 'samweb list-files "file_name like daq_hist_%%.root  and run_number = %s"' %str(thisrun)
filenames = os.popen(cmd2).read()
files = filenames.splitlines()
if len(files) > 0:
	firstfile = list(files)[0]
	cmd3 = 'samweb locate-file %s' %(firstfile)
	firstfile_loc = os.popen(cmd3).read()

	# Now, truncate the dumb samweb format extra stuff off of the path
	filepath = re.split(':|\(',firstfile_loc)[1]+'/'
	for File in filenames.splitlines():
		#outf.write(str(filepath+File+"\n"))
		outf.write(str(File+"\n"))
		os.system("cp "+str(filepath+File)+" paths/.")
	outf.close()


print "Run num: %s, Files in run: %s" %(thisrun, len(files))
if len(files) > 0:
	if os.path.isfile("plots/plots_"+str(thisrun)+"/") == False:
		os.mkdir("plots/plots_"+str(thisrun))
os.system("./swizzlePlots %i %i"%(thisrun, thisevent))
os.system("rm -f paths/*.root")
os.remove(listout)
