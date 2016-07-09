import sys
import os
import ROOT
from ROOT import *
import Image
import ImageTk
import Tkinter

plotlist = ["triggerRates", "dTBetweenTriggersAllLin", "dTBetweenTriggersAllLog", "dTBetweenTriggersBNB", "dTBetweenTriggersNuMI", "dTBetweenTriggersEXT",
		"PMT_flash_dtFromTrigger", "ROtoTrigFrameDiffBNB", "ROtoTrigSampleDiffBNB", "ROtoTrigFrameDiffNuMI", "ROtoTrigSampleDiffNuMI", "ROtoTrigFrameDiffEXT",
		"ROtoTrigSampleDiffEXT", "BNBTrigToRWMFrameDiff", "BNBTrigToRWMSampleDiff", "BNBTrigToRWMTimeDiff", "TriggerSampleValue", "TriggerFrameDifference",
		"TriggerSampleDifference", "TriggerFrameDifferenceBNB", "TriggerSampleDifferenceBNB", "TriggerFrameDifferenceNuMI", "TriggerSampleDifferenceNuMI",
		"TriggerFrameDifferenceEXT", "TriggerSampleDifferenceEXT", "MeanCompression", "compressionByEvent", "compressionByCrate", "MeanCompressionCrates_"]
summarylist = ["EventsPerRun_","CBC_09_","SampleDiffRWMtoBNB_","MeanCompression_","MeanCompressionCrate_"]

#First, let's get the bottom and top
if os.path.isfile("dqm_status.root") == False:
	print "No dqm_status, so we can't really do anything here"
	sys.exit()
else:
	f = ROOT.TFile("dqm_status.root")

f.status.GetEntry(f.status.GetEntries()-1)
run_top = f.status.run_number
f.status.GetEntry(0)
run_bottom = f.status.run_number

top = Tkinter.Tk()
# Code to add widgets will go here...
optionframe = Tkinter.LabelFrame(top, text="Data Quality Monitor")
optionframe.pack()

# Refresh Button
def cmdRefresh():
   os.system("python dqm_run.py")
   f.status.GetEntry(f.status.GetEntries()-1)
   run_top = f.status.run_number
   l_bottom.configure(text="Runs range from %i to %i"%(run_bottom,run_top))
R = Tkinter.Button(optionframe, text ="Refresh", command = cmdRefresh)
R.grid(row=0,column=0)

l_bottom = Tkinter.Label(optionframe, text="Runs range from %i to %i"%(run_bottom,run_top))
l_bottom.grid(row=1,column=0,columnspan=2)

l_run = Tkinter.Label(optionframe, text="Run Number")
l_run.grid(row=2,column=0)
e_run = Tkinter.Entry(optionframe, width=6)
e_run.grid(row=2,column=1)
e_run.insert(0,"%i"%(run_bottom))
def runframe():
	thisrun = int(e_run.get());
	if os.path.isdir("plots/plots_%i"%(thisrun))==False:
		print "We don't have that file scanned. Sorry."
		return

	fr = Tkinter.Toplevel()
	fr.geometry('800x650')
	fr.title("Run %i"%(thisrun))
	l_count = Tkinter.Label(fr,text="0")
	l_count.pack(side=Tkinter.TOP)

	l_plot = Tkinter.Label(fr,width=800,height=600)
	l_plot.pack(side=Tkinter.BOTTOM)
	png = Image.open('plots/plots_%i/'%(thisrun)+plotlist[0]+'%i.png'%(thisrun))
	png = png.resize((800,600),Image.ANTIALIAS)
	img = ImageTk.PhotoImage(png)
	l_plot.configure(image=img)

	def changeup():
		ct=int(l_count.cget("text"))
		if ct==len(plotlist)-1: return
		png = Image.open('plots/plots_%i/'%(thisrun)+plotlist[ct+1]+'%i.png'%(thisrun))
		png = png.resize((800,600),Image.ANTIALIAS)
		img = ImageTk.PhotoImage(png)
		l_plot.image=img
		l_plot.configure(image=img)
		l_count.configure(text=ct+1)
	def changedown():
		ct=int(l_count.cget("text"))
		if ct==0: return
		png = Image.open('plots/plots_%i/'%(thisrun)+plotlist[ct-1]+'%i.png'%(thisrun))
		png = png.resize((800,600),Image.ANTIALIAS)
		img = ImageTk.PhotoImage(png)
		l_plot.image=img
		l_plot.configure(image=img)
		l_count.configure(text=ct-1)
	b_next = Tkinter.Button(fr, text ="NEXT", command=changeup).pack(side=Tkinter.RIGHT)
	b_prev = Tkinter.Button(fr, text ="PREV", command=changedown).pack(side=Tkinter.LEFT)
	fr.mainloop();
s_run = Tkinter.Button(optionframe, text ="Submit", command = runframe)
s_run.grid(row=2,column=2)

l_range = Tkinter.Label(optionframe, text="Run Range")
l_range.grid(row=3,column=0)
e_rangebottom = Tkinter.Entry(optionframe, width=6)
e_rangebottom.grid(row=3,column=1)
e_rangebottom.insert(0,"%i"%(run_bottom))
e_rangetop = Tkinter.Entry(optionframe, width=6)
e_rangetop.grid(row=3,column=2)
e_rangetop.insert(0,"%i"%(run_top))
def cmdRunRange():
	#Check if runs are within bounds
	runbottom = int(e_rangebottom.get());
	runtop = int(e_rangetop.get());
	if min(runbottom,runtop) != runbottom or runbottom != runtop or runbottom < run_bottom or runtop > run_top:
		print "Those entries aren't right..."
		return
	os.system("python dqm_summary.py %i %i"%(runbottom,runtop))

	fr = Tkinter.Toplevel()
	fr.geometry('800x650')
	fr.title("Runs %i through %i"%(runbottom,runtop))
	l_count = Tkinter.Label(fr,text="0")
	l_count.pack(side=Tkinter.TOP)

	l_plot = Tkinter.Label(fr,width=800,height=600)
	l_plot.pack(side=Tkinter.BOTTOM)
	png = Image.open('summaries/'+summarylist[0]+'%i-%i.png'%(runbottom,runtop))
	png = png.resize((800,600),Image.ANTIALIAS)
	img = ImageTk.PhotoImage(png)
	l_plot.configure(image=img)

	def changeup():
		ct=int(l_count.cget("text"))
		if ct==len(summarylist)-1: return
		png = Image.open('summaries/'+summarylist[ct+1]+'%i-%i.png'%(runbottom,runtop))
		png = png.resize((800,600),Image.ANTIALIAS)
		img = ImageTk.PhotoImage(png)
		l_plot.image=img
		l_plot.configure(image=img)
		l_count.configure(text=ct+1)
	def changedown():
		ct=int(l_count.cget("text"))
		if ct==0: return
		png = Image.open('summaries/'+summarylist[ct-1]+'%i-%i.png'%(runbottom,runtop))
		png = png.resize((800,600),Image.ANTIALIAS)
		img = ImageTk.PhotoImage(png)
		l_plot.image=img
		l_plot.configure(image=img)
		l_count.configure(text=ct-1)
	b_next = Tkinter.Button(fr, text ="NEXT", command=changeup).pack(side=Tkinter.RIGHT)
	b_prev = Tkinter.Button(fr, text ="PREV", command=changedown).pack(side=Tkinter.LEFT)
	fr.mainloop()
s_range = Tkinter.Button(optionframe, text ="Submit", command = cmdRunRange)
s_range.grid(row=3,column=3)

top.mainloop()
