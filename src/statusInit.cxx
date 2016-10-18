// statusInit.cxx by Davio Cianci
//
//		This macro's job is to initialize the dqm_status.root file, which is our recordkeeping root file where we save all the important
//	stuff that we want to monitor from one run to the next.
//		It's got two components:
//			status (TNtuple): holds run number, number of files for that run, number of events in that run and how many error flags it's thrown
//			timeFuncs (TTree): holds more interesting stuff that we want to monitor over several runs, like compression info and sample diff between frames.
//				This is where you'd put new variables to monitor if you wanted to keep track of runs.
//
/////////

#include "swizzlePlots.h"

using namespace std;

bool stInit(){

	TFile* f = new TFile("dqm_status.root","RECREATE");
	TNtuple* st = new TNtuple("status","status","run_number:n_files:n_events:n_flags");
	TTree* dt = new TTree("timeFuncs","timeFuncs");

	int run_number;
	float cbc_01, cbc_02, cbc_03, cbc_04, cbc_05, cbc_06, cbc_07, cbc_08, cbc_09;
	float cbc_01err, cbc_02err, cbc_03err, cbc_04err, cbc_05err, cbc_06err, cbc_07err, cbc_08err, cbc_09err;
	TH1D *sampleDiffBetweenRWMandBNB = new TH1D("Sample Difference","Sample Difference from BNB to RWM;Frames;",32,352,384);
	TH1D *meanCompression = new TH1D("Compression", "Mean Compression; Compression Factor",100,3,6);
	TH1D* meanCompression_c1 = new TH1D("C1", "Mean Compression; Compression Factor",100,3,6);
	TH1D* meanCompression_c2 = new TH1D("C2", "Mean Compression; Compression Factor",100,3,6);
	TH1D* meanCompression_c3 = new TH1D("C3", "Mean Compression; Compression Factor",100,3,6);
	TH1D* meanCompression_c4 = new TH1D("C4", "Mean Compression; Compression Factor",100,3,6);
	TH1D* meanCompression_c5 = new TH1D("C5", "Mean Compression; Compression Factor",100,3,6);
	TH1D* meanCompression_c6 = new TH1D("C6", "Mean Compression; Compression Factor",100,3,6);
	TH1D* meanCompression_c7 = new TH1D("C7", "Mean Compression; Compression Factor",100,3,6);
	TH1D* meanCompression_c8 = new TH1D("C8", "Mean Compression; Compression Factor",100,3,6);
	TH1D* meanCompression_c9 = new TH1D("C9", "Mean Compression; Compression Factor",100,3,6);

	dt->Branch("run_number",&run_number);
	dt->Branch("cbc_01",&cbc_01);	dt->Branch("cbc_01err",&cbc_01err);
	dt->Branch("cbc_02",&cbc_02);	dt->Branch("cbc_02err",&cbc_02err);
	dt->Branch("cbc_03",&cbc_03);	dt->Branch("cbc_03err",&cbc_03err);
	dt->Branch("cbc_04",&cbc_04);	dt->Branch("cbc_04err",&cbc_04err);
	dt->Branch("cbc_05",&cbc_05);	dt->Branch("cbc_05err",&cbc_05err);
	dt->Branch("cbc_06",&cbc_06);	dt->Branch("cbc_06err",&cbc_06err);
	dt->Branch("cbc_07",&cbc_07);	dt->Branch("cbc_07err",&cbc_07err);
	dt->Branch("cbc_08",&cbc_08);	dt->Branch("cbc_08err",&cbc_08err);
	dt->Branch("cbc_09",&cbc_09);	dt->Branch("cbc_09err",&cbc_09err);
	dt->Branch("sampleDiffBetweenRWMandBNB",&sampleDiffBetweenRWMandBNB);
	dt->Branch("meanCompression",&meanCompression);
	dt->Branch("meanCompression_c1", &meanCompression_c1);
	dt->Branch("meanCompression_c2", &meanCompression_c2);
	dt->Branch("meanCompression_c3", &meanCompression_c3);
	dt->Branch("meanCompression_c4", &meanCompression_c4);
	dt->Branch("meanCompression_c5", &meanCompression_c5);
	dt->Branch("meanCompression_c6", &meanCompression_c6);
	dt->Branch("meanCompression_c7", &meanCompression_c7);
	dt->Branch("meanCompression_c8", &meanCompression_c8);
	dt->Branch("meanCompression_c9", &meanCompression_c9);

	st->Write();
	dt->Write();
	f->Close();

	return true;
}



bool statusInit(){
	return stInit();
}

#if !defined(__CINT__) || defined (__MAKECINT__)
int main()
{
	return statusInit();
}
#endif
