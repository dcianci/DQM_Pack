#include "swizzlePlots.h"

using namespace std;

bool save = true;
bool debug = true;
std::string plotloc = "plots/";
std::string logloc = "logs/";

std::vector <std::string> paths;
std::vector <float> _framediff, _samplediff;

TFile* f_daq,* f_status;
TCanvas* canv;
TNtuple *nt_status;
TTree* t_tree, *t_timefuncs;

// Time stability variables
int run_number;
float cbc_01, cbc_02, cbc_03, cbc_04, cbc_05, cbc_06, cbc_07, cbc_08, cbc_09;
float cbc_01err, cbc_02err, cbc_03err, cbc_04err, cbc_05err, cbc_06err, cbc_07err, cbc_08err, cbc_09err;
TH1D *sampleDiffBetweenRWMandBNB, *meanCompression;
TH1D *meanCompression_c1, *meanCompression_c2, *meanCompression_c3, *meanCompression_c4, *meanCompression_c5, *meanCompression_c6, *meanCompression_c7, *meanCompression_c8, *meanCompression_c9;
TH1D* hCBC_01 = new TH1D("", "",100,0,10); TH1D* hCBC_02 = new TH1D("", "",100,0,10); TH1D* hCBC_03 = new TH1D("", "",100,0,10);
TH1D* hCBC_04 = new TH1D("", "",100,0,10); TH1D* hCBC_05 = new TH1D("", "",100,0,10); TH1D* hCBC_06 = new TH1D("", "",100,0,10);
TH1D* hCBC_07 = new TH1D("", "",100,0,10); TH1D* hCBC_08 = new TH1D("", "",100,0,10); TH1D* hCBC_09 = new TH1D("", "",100,0,10);

bool makePlots(int rnum, int evnum){

	run_number = rnum;
	// Read in the pathlists and fill vector
	paths.resize(0);
	std::string thisline;
	ifstream infile;
	infile.open("pathlist.txt");
	while(std::getline(infile,thisline)){
		paths.push_back(thisline);
	}
	infile.close();
	int N_FILES = paths.size();

	// Setup the log file
	ofstream logger;
	std::string logname = logloc + "dqm_log_" + std::to_string(rnum) + ".log";
	logger.open(logname);

	// We want one set of plots for each run, so we're combining all of these files into one giganto-thing.
	canv = new TCanvas();

	double lastFrame = -1;
	double lastSample = -1;
	double lastTime = -1.;
	double lastTimeBNB = -1.;
	double lastTimeNuMI = -1.;
	double lastTimeEXT = -1.;

	double firstEventTime = -1; double finalEventTime = -1;
	double firstEventTimeBNB = -1; double finalEventTimeBNB = -1;
	double firstEventTimeNuMI = -1; double finalEventTimeNuMI = -1;
	double firstEventTimeEXT = -1; double finalEventTimeEXT = -1;

	int ADCwords_crate0, ADCwords_crate1, ADCwords_crate2, ADCwords_crate3, ADCwords_crate4, ADCwords_crate5, ADCwords_crate6, ADCwords_crate7, ADCwords_crate8, ADCwords_crate9, NumWords_crate1, NumWords_crate2, NumWords_crate3, NumWords_crate4, NumWords_crate5, NumWords_crate6, NumWords_crate7, NumWords_crate8, NumWords_crate9;
	int event, thisevent;
	double triggerTime;

	TH1D* hEventByEventCompression = new TH1D("", "Event-by-event Compression; Event Number; Compression Factor",21,evnum-10,evnum+10);
	TH1D* hEventByEventCompression_c1 = new TH1D("", "Event-by-event Compression Crate 1; Event Number; Compression Factor",21,evnum-10,evnum+10);
	TH1D* hEventByEventCompression_c2 = new TH1D("", "Event-by-event Compression Crate 2; Event Number; Compression Factor",21,evnum-10,evnum+10);
	TH1D* hEventByEventCompression_c3 = new TH1D("", "Event-by-event Compression Crate 3; Event Number; Compression Factor",21,evnum-10,evnum+10);
	TH1D* hEventByEventCompression_c4 = new TH1D("", "Event-by-event Compression Crate 4; Event Number; Compression Factor",21,evnum-10,evnum+10);
	TH1D* hEventByEventCompression_c5 = new TH1D("", "Event-by-event Compression Crate 5; Event Number; Compression Factor",21,evnum-10,evnum+10);
	TH1D* hEventByEventCompression_c6 = new TH1D("", "Event-by-event Compression Crate 6; Event Number; Compression Factor",21,evnum-10,evnum+10);
	TH1D* hEventByEventCompression_c7 = new TH1D("", "Event-by-event Compression Crate 7; Event Number; Compression Factor",21,evnum-10,evnum+10);
	TH1D* hEventByEventCompression_c8 = new TH1D("", "Event-by-event Compression Crate 8; Event Number; Compression Factor",21,evnum-10,evnum+10);
	TH1D* hEventByEventCompression_c9 = new TH1D("", "Event-by-event Compression Crate 9; Event Number; Compression Factor",21,evnum-10,evnum+10);

	int ADCwordsEvent,NumWordsEvent;
	long int ADCwords0 = 0; long int ADCwords1 = 0; long int ADCwords2 = 0; long int ADCwords3 = 0; long int ADCwords4 = 0; long int ADCwords5 = 0; long int ADCwords6 = 0; long int ADCwords7 = 0; long int ADCwords8 = 0; long int ADCwords9 = 0;
	long int NumWords0 = 0; long int NumWords1 = 0; long int NumWords2 = 0; long int NumWords3 = 0; long int NumWords4 = 0; long int NumWords5 = 0; long int NumWords6 = 0; long int NumWords7 = 0; long int NumWords8 = 0; long int NumWords9 = 0;

	std::cout << "Figure the right order for these guys" << std::endl;
	std::vector < double > orderVec;

	for(int i = 0; i < int(N_FILES); i++){
		f_daq = new TFile(("paths/"+paths[i]).c_str());
		t_tree = (TTree*)(f_daq->Get("Debug/tMyTree"));
		t_tree->SetBranchAddress("triggerTime", &triggerTime);

		t_tree->GetEntry(0);
		orderVec.push_back(triggerTime);
	}

	double toosmall = 0.;
	double smallest; int ismallest;
	double therighttime = 0.;
	thisevent = 1;

	std::cout << "Starting to loop through everything." << std::endl;
	for(int i = 0; i < int(N_FILES); i++){

		for(int f = 0; f < int(N_FILES); f++){
			if(f == 0){
				smallest = orderVec[0];
				ismallest = 0;
			}
			else if(orderVec[f] < smallest && orderVec[f] > toosmall){
					smallest = orderVec[f];
					ismallest = f;
			}
		}
		toosmall = smallest;

		if(i % 5 == 0)
			std::cout << "Path: " << i << "/" << N_FILES << std::endl;
		if(f_daq) f_daq->Close();
		f_daq = new TFile(("paths/"+paths[ismallest]).c_str());
		t_tree = (TTree*)(f_daq->Get("Debug/tMyTree"));

		std::cout << "Loaded file. Setting branches." << std::endl;

		t_tree->SetBranchAddress("event", &event);
		t_tree->SetBranchAddress("triggerTime", &triggerTime);
		t_tree->SetBranchAddress("ADCwords_crate0",&ADCwords_crate0);
		t_tree->SetBranchAddress("ADCwords_crate1",&ADCwords_crate1);
		t_tree->SetBranchAddress("ADCwords_crate2",&ADCwords_crate2);
		t_tree->SetBranchAddress("ADCwords_crate3",&ADCwords_crate3);
		t_tree->SetBranchAddress("ADCwords_crate4",&ADCwords_crate4);
		t_tree->SetBranchAddress("ADCwords_crate5",&ADCwords_crate5);
		t_tree->SetBranchAddress("ADCwords_crate6",&ADCwords_crate6);
		t_tree->SetBranchAddress("ADCwords_crate7",&ADCwords_crate7);
		t_tree->SetBranchAddress("ADCwords_crate8",&ADCwords_crate8);
		t_tree->SetBranchAddress("ADCwords_crate9",&ADCwords_crate9);
		t_tree->SetBranchAddress("NumWords_crate1",&NumWords_crate1);
		t_tree->SetBranchAddress("NumWords_crate2",&NumWords_crate2);
		t_tree->SetBranchAddress("NumWords_crate3",&NumWords_crate3);
		t_tree->SetBranchAddress("NumWords_crate4",&NumWords_crate4);
		t_tree->SetBranchAddress("NumWords_crate5",&NumWords_crate5);
		t_tree->SetBranchAddress("NumWords_crate6",&NumWords_crate6);
		t_tree->SetBranchAddress("NumWords_crate7",&NumWords_crate7);
		t_tree->SetBranchAddress("NumWords_crate8",&NumWords_crate8);
		t_tree->SetBranchAddress("NumWords_crate9",&NumWords_crate9);
		std::cout << "Looping through entries" << std::endl;

		for(int j = 0; j < t_tree->GetEntries(); j++){

			t_tree->GetEntry(j);
			if(j == 0) std::cout << "TRIGTIME: " << triggerTime << std::endl;
			if(triggerTime < therighttime) std::cout << "BANGBANGBANG" << std::endl;

			if(thisevent < evnum - 10 || thisevent > evnum + 10) {
				thisevent++;
				continue;
			}

			ADCwordsEvent = ADCwords_crate0;
			ADCwordsEvent += ADCwords_crate1;
			ADCwordsEvent += ADCwords_crate2;
			ADCwordsEvent += ADCwords_crate3;
			ADCwordsEvent += ADCwords_crate4;
			ADCwordsEvent += ADCwords_crate5;
			ADCwordsEvent += ADCwords_crate6;
			ADCwordsEvent += ADCwords_crate7;
			ADCwordsEvent += ADCwords_crate8;
			ADCwordsEvent += ADCwords_crate9;
			NumWordsEvent = NumWords_crate1;
			NumWordsEvent += NumWords_crate2;
			NumWordsEvent += NumWords_crate3;
			NumWordsEvent += NumWords_crate4;
			NumWordsEvent += NumWords_crate5;
			NumWordsEvent += NumWords_crate6;
			NumWordsEvent += NumWords_crate7;
			NumWordsEvent += NumWords_crate8;
			NumWordsEvent += NumWords_crate9;
			if (NumWordsEvent){
				hEventByEventCompression->SetBinContent(thisevent-(evnum-10),ADCwordsEvent / float(NumWordsEvent));
				hEventByEventCompression_c1->SetBinContent(thisevent-(evnum-10),ADCwords_crate1 / float(NumWords_crate1));
				hEventByEventCompression_c2->SetBinContent(thisevent-(evnum-10),ADCwords_crate2 / float(NumWords_crate2));
				hEventByEventCompression_c3->SetBinContent(thisevent-(evnum-10),ADCwords_crate3 / float(NumWords_crate3));
				hEventByEventCompression_c4->SetBinContent(thisevent-(evnum-10),ADCwords_crate4 / float(NumWords_crate4));
				hEventByEventCompression_c5->SetBinContent(thisevent-(evnum-10),ADCwords_crate5 / float(NumWords_crate5));
				hEventByEventCompression_c6->SetBinContent(thisevent-(evnum-10),ADCwords_crate6 / float(NumWords_crate6));
				hEventByEventCompression_c7->SetBinContent(thisevent-(evnum-10),ADCwords_crate7 / float(NumWords_crate7));
				hEventByEventCompression_c8->SetBinContent(thisevent-(evnum-10),ADCwords_crate8 / float(NumWords_crate8));
				hEventByEventCompression_c9->SetBinContent(thisevent-(evnum-10),ADCwords_crate9 / float(NumWords_crate9));
			}
			hCBC_01->Fill(ADCwords_crate1/float(NumWords_crate1));
			hCBC_02->Fill(ADCwords_crate2/float(NumWords_crate2));
			hCBC_03->Fill(ADCwords_crate3/float(NumWords_crate3));
			hCBC_04->Fill(ADCwords_crate4/float(NumWords_crate4));
			hCBC_05->Fill(ADCwords_crate5/float(NumWords_crate5));
			hCBC_06->Fill(ADCwords_crate6/float(NumWords_crate6));
			hCBC_07->Fill(ADCwords_crate7/float(NumWords_crate7));
			hCBC_08->Fill(ADCwords_crate8/float(NumWords_crate8));
			hCBC_09->Fill(ADCwords_crate9/float(NumWords_crate9));
			ADCwords0 += ADCwords_crate0;
			ADCwords1 += ADCwords_crate1;
			ADCwords2 += ADCwords_crate2;
			ADCwords3 += ADCwords_crate3;
			ADCwords4 += ADCwords_crate4;
			ADCwords5 += ADCwords_crate5;
			ADCwords6 += ADCwords_crate6;
			ADCwords7 += ADCwords_crate7;
			ADCwords8 += ADCwords_crate8;
			ADCwords9 += ADCwords_crate9;
			NumWords1 += NumWords_crate1;
			NumWords2 += NumWords_crate2;
			NumWords3 += NumWords_crate3;
			NumWords4 += NumWords_crate4;
			NumWords5 += NumWords_crate5;
			NumWords6 += NumWords_crate6;
			NumWords7 += NumWords_crate7;
			NumWords8 += NumWords_crate8;
			NumWords9 += NumWords_crate9;

			thisevent++;
		}
	}
	std::cout << "Hot dog! Everything looped through. Now to print all the plots" << std::endl;

	gStyle->SetOptStat(0000);
 	gStyle->SetOptFit(0000);
 	gStyle->SetPadBorderMode(0);
 	gStyle->SetPadBottomMargin(0.15);
 	gStyle->SetPadLeftMargin(0.15);
 	gStyle->SetPadRightMargin(0.05);
 	gStyle->SetFrameBorderMode(0);
 	gStyle->SetCanvasBorderMode(0);
 	gStyle->SetPalette(1);
 	gStyle->SetCanvasColor(0);
 	gStyle->SetPadColor(0);
	gStyle->SetOptStat(0000);


	hEventByEventCompression->SetMinimum(0);
	hEventByEventCompression->SetMaximum(9);
	hEventByEventCompression->Draw();
	if (save)
		if (NumWordsEvent)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/compressionByEvent"+to_string(rnum)+".eps").c_str());
	hEventByEventCompression_c1->SetMinimum(0);
	hEventByEventCompression_c1->SetMaximum(9);
	hEventByEventCompression_c1->Draw();
	if (save)
		if (NumWords_crate9)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/compressionByEvent_crate1"+to_string(rnum)+".eps").c_str());
	hEventByEventCompression_c2->SetMinimum(0);
	hEventByEventCompression_c2->SetMaximum(9);
	hEventByEventCompression_c2->Draw();
	if (save)
		if (NumWords_crate9)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/compressionByEvent_crate2"+to_string(rnum)+".eps").c_str());
	hEventByEventCompression_c3->SetMinimum(0);
	hEventByEventCompression_c3->SetMaximum(9);
	hEventByEventCompression_c3->Draw();
	if (save)
		if (NumWords_crate9)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/compressionByEvent_crate3"+to_string(rnum)+".eps").c_str());
	hEventByEventCompression_c4->SetMinimum(0);
	hEventByEventCompression_c4->SetMaximum(9);
	hEventByEventCompression_c4->Draw();
	if (save)
		if (NumWords_crate9)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/compressionByEvent_crate4"+to_string(rnum)+".eps").c_str());
	hEventByEventCompression_c5->SetMinimum(0);
	hEventByEventCompression_c5->SetMaximum(9);
	hEventByEventCompression_c5->Draw();
	if (save)
		if (NumWords_crate9)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/compressionByEvent_crate5"+to_string(rnum)+".eps").c_str());
	hEventByEventCompression_c6->SetMinimum(0);
	hEventByEventCompression_c6->SetMaximum(9);
	hEventByEventCompression_c6->Draw();
	if (save)
		if (NumWords_crate9)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/compressionByEvent_crate6"+to_string(rnum)+".eps").c_str());
	hEventByEventCompression_c7->SetMinimum(0);
	hEventByEventCompression_c7->SetMaximum(9);
	hEventByEventCompression_c7->Draw();
	if (save)
		if (NumWords_crate9)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/compressionByEvent_crate7"+to_string(rnum)+".eps").c_str());
	hEventByEventCompression_c8->SetMinimum(0);
	hEventByEventCompression_c8->SetMaximum(9);
	hEventByEventCompression_c8->Draw();
	if (save)
		if (NumWords_crate9)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/compressionByEvent_crate8"+to_string(rnum)+".eps").c_str());
	hEventByEventCompression_c9->SetMinimum(0);
	hEventByEventCompression_c9->SetMaximum(9);
	hEventByEventCompression_c9->Draw();
	if (save)
		if (NumWords_crate9)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/compressionByEvent_crate9"+to_string(rnum)+".eps").c_str());
	f_daq->Close();

	logger.close();

	return false;
}

bool swizzlePlots(int rnum, int evnum){
	return makePlots(rnum, evnum);
}

#if !defined(__CINT__) || defined (__MAKECINT__)
int main(int argc, char* argv[])
{
	int num = atoi(argv[1]);
	int ev = atoi(argv[2]);
	return swizzlePlots(num,ev);
}
#endif
