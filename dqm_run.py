import sys, getopt
import os
import subprocess
from subprocess import Popen, PIPE
import ROOT
from ROOT import *
import re

# runbuffer is how many blank runs we'll try before accepting that we really are at the end
runbuffer = 300
listout = "pathlist.txt"

if len(sys.argv) == 2:
	thisrun = int(sys.argv[1])
if len(sys.argv) == 1:
	#Gotta find the last run run
	if os.path.isfile("dqm_status.root"):
		f = ROOT.TFile("dqm_status.root")
		f.status.GetEntry(f.status.GetEntries()-1)
		thisrun = int(f.status.run_number)+1
		f.Close()
	else:
		print "Got no dqm_status file!"
		sys.exit()

print "Alright, let's start runs at %i" %thisrun

notdoneyet = True;
# First, let's just make sure that there are runs in this range, so we check the next couple run numbers for daq_hist files
for rn in range(0,runbuffer+1):
	cmd1 = 'samweb count-files "file_name like daq_hist_%%.root  and run_number = %s"' %str(thisrun+rn)
	nfiles = os.popen(cmd1).read()
	if int(nfiles) > 0:
		break
	if rn == runbuffer:
		print "We're done with runs in this area, I guess."
		sys.exit();

# Now, loop through until we can't loop any more
while notdoneyet:
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
			os.system("ifdh cp "+str(filepath+File)+" paths/.")
		outf.close()


	print "Run num: %s, Files in run: %s" %(thisrun, len(files))
	if len(files) > 0:
		if os.path.isfile("plots/plots_"+str(thisrun)+"/") == False:
			os.mkdir("plots/plots_"+str(thisrun))
	os.system("./swizzlePlots %i"%(thisrun))
	os.system("rm -f paths/*.root")

	for rn in range(1,runbuffer+1):
		cmd1 = 'samweb count-files "file_name like daq_hist_%%.root  and run_number = %s"' %str(thisrun+rn)
		nfiles = os.popen(cmd1).read()
		if int(nfiles) > 0:
			break
		if rn == runbuffer:
			notdoneyet = False
			print "All done, I think"

	#notdoneyet = False
	thisrun+=1
	os.remove(listout)

cmd4 = 'samweb count-files "file_name like daq_hist_%%.root  and run_number > %s"' %str(thisrun+rn)
nleft = int(os.popen(cmd4).read())
print "And we've got %s files remaining unread, (though that could be due to runs that ain't finished yet)" %str(nleft)
