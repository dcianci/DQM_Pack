#include "swizzlePlots.h"

using namespace std;

bool makePlots(int bottom, int top){

	int N_FLAGS = 0;
	int N_RUNS = 0;
	TH1D * hEPR = new TH1D("Events","Events Per Run; Run Number; Number of Events",(top - bottom),bottom,top);
	TH1D * hCBC_01 = new TH1D("Compression","Crate by Crate Compression; Run Number; Compression Factor",(top - bottom),bottom,top);
	TH1D * hCBC_02 = new TH1D("Compression","Crate 2 Compression; Run Number; Compression Factor",(top - bottom),bottom,top);
	TH1D * hCBC_03 = new TH1D("Compression","Crate 3 Compression; Run Number; Compression Factor",(top - bottom),bottom,top);
	TH1D * hCBC_04 = new TH1D("Compression","Crate 4 Compression; Run Number; Compression Factor",(top - bottom),bottom,top);
	TH1D * hCBC_05 = new TH1D("Compression","Crate 5 Compression; Run Number; Compression Factor",(top - bottom),bottom,top);
	TH1D * hCBC_06 = new TH1D("Compression","Crate 6 Compression; Run Number; Compression Factor",(top - bottom),bottom,top);
	TH1D * hCBC_07 = new TH1D("Compression","Crate 7 Compression; Run Number; Compression Factor",(top - bottom),bottom,top);
	TH1D * hCBC_08 = new TH1D("Compression","Crate 8 Compression; Run Number; Compression Factor",(top - bottom),bottom,top);
	TH1D * hCBC_09 = new TH1D("Compression","Crate 9 Compression; Run Number; Compression Factor",(top - bottom),bottom,top);
	TH2D * hSampleDiffRWMandBNB = new TH2D("Sample Difference","Sample Difference from BNB to RWM; Run Number; Frames",(top - bottom),bottom,top,32,352,384);
	TH2D * hMeanCompression = new TH2D("Compression","Total Event Compression Factor; Run Number; Compression Factor",(top - bottom),bottom,top,100,3,6);


	TFile* f = new TFile("dqm_status.root","READ");
	TNtuple* st = (TNtuple*)(f->Get("status"));
	TTree* dt = (TTree*)(f->Get("timeFuncs"));

	// Status vars
	float r_number, n_files, n_events, n_flags;

	st->SetBranchAddress("run_number",&r_number);
	st->SetBranchAddress("n_files",&n_files);
	st->SetBranchAddress("n_events",&n_events);
	st->SetBranchAddress("n_flags",&n_flags);

	// Timefuncs vars
	int run_number;
	float cbc_01, cbc_02, cbc_03, cbc_04, cbc_05, cbc_06, cbc_07, cbc_08, cbc_09;
	float cbc_01err, cbc_02err, cbc_03err, cbc_04err, cbc_05err, cbc_06err, cbc_07err, cbc_08err, cbc_09err;
	TH1D *sampleDiffBetweenRWMandBNB = new TH1D("Frame Difference","Frame Difference from BNB to RWM;Frames;",32,352,384);
	TH1D *meanCompression = new TH1D("Compression", "Total Event Compression Factor; Compression Factor",100,3,6);

	dt->SetBranchAddress("run_number",&run_number);
	dt->SetBranchAddress("cbc_01",&cbc_01);	dt->SetBranchAddress("cbc_01err",&cbc_01err);
	dt->SetBranchAddress("cbc_02",&cbc_02);	dt->SetBranchAddress("cbc_02err",&cbc_02err);
	dt->SetBranchAddress("cbc_03",&cbc_03);	dt->SetBranchAddress("cbc_03err",&cbc_03err);
	dt->SetBranchAddress("cbc_04",&cbc_04);	dt->SetBranchAddress("cbc_04err",&cbc_04err);
	dt->SetBranchAddress("cbc_05",&cbc_05);	dt->SetBranchAddress("cbc_05err",&cbc_05err);
	dt->SetBranchAddress("cbc_06",&cbc_06);	dt->SetBranchAddress("cbc_06err",&cbc_06err);
	dt->SetBranchAddress("cbc_07",&cbc_07);	dt->SetBranchAddress("cbc_07err",&cbc_07err);
	dt->SetBranchAddress("cbc_08",&cbc_08);	dt->SetBranchAddress("cbc_08err",&cbc_08err);
	dt->SetBranchAddress("cbc_09",&cbc_09);	dt->SetBranchAddress("cbc_09err",&cbc_09err);
	dt->SetBranchAddress("sampleDiffBetweenRWMandBNB",&sampleDiffBetweenRWMandBNB);
	dt->SetBranchAddress("meanCompression",&meanCompression);

	// First, let's go through the status ntuple
	for(int i = 0; i < st->GetEntries(); i++){
		st->GetEntry(i);
		if(r_number < bottom)	continue;
		if(r_number > top) break;

		// Fill up n_events plots
		hEPR->SetBinContent(r_number-bottom,n_events);

		N_RUNS ++;
		N_FLAGS += n_flags;
		if(n_flags > 0){
			std::cout << "Run: " << r_number << " has thrown " << n_flags << " warning flags. Might want to check that out" << std::endl;
		}
	}
	std::cout << "We just scanned over " << (top - bottom) << " runs, of which " << N_RUNS << " had processed files for me to look at." << std::endl;
	std::cout << "Oh, and we found " << N_FLAGS << " warnings over that range." << std::endl;



	double means[9];
	int runs = 0;
	for(int i = 0; i < 9; i++)
		means[i] = 0;

	// Now, make the plots
	for(int j = 0; j < dt->GetEntries(); j++){
		dt->GetEntry(j);

		if(run_number < bottom)	continue;
		if(run_number > top) break;

		hCBC_01->SetBinContent(run_number-bottom,cbc_01);	hCBC_01->SetBinError(run_number-bottom,cbc_01err);
		hCBC_02->SetBinContent(run_number-bottom,cbc_02);	hCBC_02->SetBinError(run_number-bottom,cbc_02err);
		hCBC_03->SetBinContent(run_number-bottom,cbc_03);	hCBC_03->SetBinError(run_number-bottom,cbc_03err);
		hCBC_04->SetBinContent(run_number-bottom,cbc_04);	hCBC_04->SetBinError(run_number-bottom,cbc_04err);
		hCBC_05->SetBinContent(run_number-bottom,cbc_05);	hCBC_05->SetBinError(run_number-bottom,cbc_05err);
		hCBC_06->SetBinContent(run_number-bottom,cbc_06);	hCBC_06->SetBinError(run_number-bottom,cbc_06err);
		hCBC_07->SetBinContent(run_number-bottom,cbc_07);	hCBC_07->SetBinError(run_number-bottom,cbc_07err);
		hCBC_08->SetBinContent(run_number-bottom,cbc_08);	hCBC_08->SetBinError(run_number-bottom,cbc_08err);
		hCBC_09->SetBinContent(run_number-bottom,cbc_09);	hCBC_09->SetBinError(run_number-bottom,cbc_09err);
		means[0] += cbc_01;
		means[1] += cbc_02;
		means[2] += cbc_03;
		means[3] += cbc_04;
		means[4] += cbc_05;
		means[5] += cbc_06;
		means[6] += cbc_07;
		means[7] += cbc_08;
		means[8] += cbc_09;
		runs ++;
		// Now, let's fill up the sampleDiffBetweenRWMandBNB
		for(int hs = 1; hs <= 32; hs++){
			// so... something is wrong here
			hSampleDiffRWMandBNB->SetBinContent(run_number-bottom,hs,sampleDiffBetweenRWMandBNB->GetBinContent(hs));
		}
		for(int mc = 1; mc <= 100; mc++){
			hMeanCompression->SetBinContent(run_number-bottom,mc,meanCompression->GetBinContent(mc));
		}
	}

	for(int k = 0; k < 9; k++)
		means[k] /= runs;

	// Okay, so now we've hopefully filled all those histos. now, let's draw 'em!
	gStyle->SetOptStat(0000);
	gStyle->SetOptFit(0000);
	gStyle->SetErrorX(0.0001);
	gStyle->SetPadBorderMode(0);
	gStyle->SetPadBottomMargin(0.15);
	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetPadRightMargin(0.05);
	gStyle->SetFrameBorderMode(0);
	gStyle->SetCanvasBorderMode(0);
	gStyle->SetPalette(1);
	gStyle->SetCanvasColor(0);
	gStyle->SetPadColor(0);

	TCanvas* canv = new TCanvas();
	hEPR->Draw();
	canv->SaveAs(("summaries/EventsPerRun_"+to_string(bottom)+"-"+to_string(top)+".eps").c_str());

	hCBC_01->SetMinimum(2);
	hCBC_01->SetMaximum(7);
	hCBC_01->SetMarkerStyle(10);
	hCBC_01->SetMarkerSize(.35);
	hCBC_01->SetLineColor(kBlue);
	hCBC_01->SetMarkerColor(kBlue);
	hCBC_01->Draw("ep");
	TLine* m1 = new TLine(bottom,means[0],top,means[0]);
	m1->SetLineStyle(3);
	m1->SetLineColor(kBlue);
	m1->Draw("same");
	hCBC_02->SetMarkerStyle(10);
	hCBC_02->SetMarkerSize(.35);
	hCBC_02->SetLineColor(kCyan);
	hCBC_02->SetMarkerColor(kCyan);
	hCBC_02->Draw("epsame");
	TLine* m2 = new TLine(bottom,means[1],top,means[1]);
	m2->SetLineStyle(3);
	m2->SetLineColor(kCyan);
	m2->Draw("same");
	hCBC_03->SetMarkerStyle(10);
	hCBC_03->SetMarkerSize(.35);
	hCBC_03->SetLineColor(kGreen);
	hCBC_03->SetMarkerColor(kGreen);
	hCBC_03->Draw("epsame");
	TLine* m3 = new TLine(bottom,means[2],top,means[2]);
	m3->SetLineStyle(3);
	m3->SetLineColor(kGreen);
	m3->Draw("same");
	hCBC_04->SetMarkerStyle(10);
	hCBC_04->SetMarkerSize(.35);
	hCBC_04->SetLineColor(kYellow+1);
	hCBC_04->SetMarkerColor(kYellow+1);
	hCBC_04->Draw("epsame");
	TLine* m4 = new TLine(bottom,means[3],top,means[3]);
	m4->SetLineStyle(3);
	m4->SetLineColor(kYellow+1);
	m4->Draw("same");
	hCBC_05->SetMarkerStyle(10);
	hCBC_05->SetMarkerSize(.35);
	hCBC_05->SetLineColor(kOrange+7);
	hCBC_05->SetMarkerColor(kOrange+7);
	hCBC_05->Draw("epsame");
	TLine* m5 = new TLine(bottom,means[4],top,means[4]);
	m5->SetLineStyle(3);
	m5->SetLineColor(kOrange+7);
	m5->Draw("same");
	hCBC_06->SetMarkerStyle(10);
	hCBC_06->SetMarkerSize(.35);
	hCBC_06->SetLineColor(kRed);
	hCBC_06->SetMarkerColor(kRed);
	hCBC_06->Draw("epsame");
	TLine* m6 = new TLine(bottom,means[5],top,means[5]);
	m6->SetLineStyle(3);
	m6->SetLineColor(kRed);
	m6->Draw("same");
	hCBC_07->SetMarkerStyle(10);
	hCBC_07->SetMarkerSize(.35);
	hCBC_07->SetLineColor(kMagenta);
	hCBC_07->SetMarkerColor(kMagenta);
	hCBC_07->Draw("epsame");
	TLine* m7 = new TLine(bottom,means[6],top,means[6]);
	m7->SetLineStyle(3);
	m7->SetLineColor(kMagenta);
	m7->Draw("same");
	hCBC_08->SetMarkerStyle(10);
	hCBC_08->SetMarkerSize(.35);
	hCBC_08->SetLineColor(kViolet+7);
	hCBC_08->SetMarkerColor(kViolet+7);
	hCBC_08->Draw("epsame");
	TLine* m8 = new TLine(bottom,means[7],top,means[7]);
	m8->SetLineStyle(3);
	m8->SetLineColor(kViolet+7);
	m8->Draw("same");
	hCBC_09->SetMarkerStyle(10);
	hCBC_09->SetMarkerSize(.35);
	hCBC_09->SetLineColor(kGreen+4);
	hCBC_09->SetMarkerColor(kGreen+4);
	hCBC_09->Draw("epsame");
	TLine* m9 = new TLine(bottom,means[8],top,means[8]);
	m9->SetLineStyle(3);
	m9->SetLineColor(kGreen+4);
	m9->Draw("same");

	TLegend * leg = new TLegend(0.6,0.7,0.93,0.88);
	leg-> SetNColumns(3);

	leg->AddEntry(hCBC_01,"Crate 1","p");
	leg->AddEntry(hCBC_02,"Crate 2","p");
	leg->AddEntry(hCBC_03,"Crate 3","p");
	leg->AddEntry(hCBC_04,"Crate 4","p");
	leg->AddEntry(hCBC_05,"Crate 5","p");
	leg->AddEntry(hCBC_06,"Crate 6","p");
	leg->AddEntry(hCBC_07,"Crate 7","p");
	leg->AddEntry(hCBC_08,"Crate 8","p");
	leg->AddEntry(hCBC_09,"Crate 9","p");
	leg->Draw();

	canv->SaveAs(("summaries/CBC_09_"+to_string(bottom)+"-"+to_string(top)+".eps").c_str());

	hSampleDiffRWMandBNB->Draw("col");
	canv->SaveAs(("summaries/SampleDiffRWMtoBNB_"+to_string(bottom)+"-"+to_string(top)+".eps").c_str());
	hMeanCompression->Draw("col");
	canv->SaveAs(("summaries/MeanCompression_"+to_string(bottom)+"-"+to_string(top)+".eps").c_str());

	return true;
}


bool summaryPlots(int bottom, int top){
	return makePlots(bottom, top);
}

#if !defined(__CINT__) || defined (__MAKECINT__)
int main(int argc, char* argv[])
{
	int low = atoi(argv[1]);
	int high = atoi(argv[2]);
	return summaryPlots(low,high);
}
#endif
