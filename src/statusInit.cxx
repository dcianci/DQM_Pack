#include "swizzlePlots.h"

using namespace std;

bool stInit(){

	TFile* f = new TFile("dqm_status.root","RECREATE");
	TNtuple* st = new TNtuple("status","status","run_number:n_files:n_events:n_flags");
	TTree* dt = new TTree("timeFuncs","timeFuncs");

	int run_number;
	float cbc_01, cbc_02, cbc_03, cbc_04, cbc_05, cbc_06, cbc_07, cbc_08, cbc_09;
	float cbc_01err, cbc_02err, cbc_03err, cbc_04err, cbc_05err, cbc_06err, cbc_07err, cbc_08err, cbc_09err;
	TH1D *sampleDiffBetweenRWMandBNB = new TH1D("Frame Difference","Frame Difference from BNB to RWM;Frames;",32,352,384);
	TH1D *meanCompression = new TH1D("Compression", "Mean Compression; Compression Factor",100,3,6);

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
