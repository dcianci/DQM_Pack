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

bool makePlots(int rnum){

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

	if(N_FILES == 0)
		logger << "Run - " << rnum << ", No filepaths found.\n";

	// Now, let's keep track of how many warnings this run sets off
	int nt_flags = 0;
	int n_events = 0;
	if(N_FILES){

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

		double averageTrigRate, averageTrigRateBNB, averageTrigRateNuMI, averageTrigRateEXT;
		int nBNB = 0;
		int nNuMI = 0;
		int nEXT = 0;

		double triggerTime, RO_RWMtriggerTime, PMT_waveform_times[400];
		int triggerBitBNB, triggerBitNuMI, triggerBitEXT,
			triggerFrame, triggerSample, FEM5triggerFrame, FEM5triggerSample, FEM6triggerFrame, FEM6triggerSample,
			RO_BNBtriggerFrame, RO_BNBtriggerSample, RO_RWMtriggerFrame, RO_RWMtriggerSample, RO_NuMItriggerFrame, RO_NuMItriggerSample, RO_EXTtriggerFrame, RO_EXTtriggerSample,
			TPC1triggerFrame, TPC2triggerFrame, TPC3triggerFrame, TPC4triggerFrame, TPC5triggerFrame, TPC6triggerFrame, TPC7triggerFrame, TPC8triggerFrame, TPC9triggerFrame, TPC1triggerSample, TPC2triggerSample, TPC3triggerSample, TPC4triggerSample, TPC5triggerSample, TPC6triggerSample, TPC7triggerSample, TPC8triggerSample, TPC9triggerSample,
			ADCwords_crate0, ADCwords_crate1, ADCwords_crate2, ADCwords_crate3, ADCwords_crate4, ADCwords_crate5, ADCwords_crate6, ADCwords_crate7, ADCwords_crate8, ADCwords_crate9, NumWords_crate1, NumWords_crate2, NumWords_crate3, NumWords_crate4, NumWords_crate5, NumWords_crate6, NumWords_crate7, NumWords_crate8, NumWords_crate9,
			N_PMT_waveforms;

		TH1D* hDeltaTBetweenTriggers = new TH1D("Time between triggers","Time between triggers",300,1e-2,1);
		TH1D* hDeltaTBetweenTriggersBNB = new TH1D("Time between triggers (BNB)","Time between triggers (BNB)",100,1e-2,1);
		TH1D* hDeltaTBetweenTriggersNuMI = new TH1D("Time between triggers (NuMI)","Time between triggers (NuMI)",100,1e-2,1);
		TH1D* hDeltaTBetweenTriggersEXT = new TH1D("Time between triggers (EXT)","Time between triggers (EXT)",100,1e-2,1);
		hDeltaTBetweenTriggers->SetTitle("time between triggers;time / s; events");
		hDeltaTBetweenTriggersBNB->SetTitle("(BNB) time between triggers;time / s; events");
		hDeltaTBetweenTriggersNuMI->SetTitle("(NuMI) time between triggers;time / s; events");
		hDeltaTBetweenTriggersEXT->SetTitle("(EXT) time between triggers;time / s; events");

		TH1D* hFrameDiff = new TH1D("Frame Difference", "Frame Difference; Frame;", 9,-5,5);
		TH1D* hSampleDiff = new TH1D("Sample Difference", "Sample Difference; Sample;", 9, -5, 5);
		TH1D* hFrameDiffBNB = new TH1D("Frame Diff (BNB)", "Frame Difference (BNB); Frame;", 9,-5,5);
		TH1D* hSampleDiffBNB = new TH1D("Sample Diff (BNB)", "Sample Difference (BNB); Sample;", 9, -5, 5);
		TH1D* hFrameDiffNuMI = new TH1D("Frame Diff (NuMI)", "Frame Difference (NuMI); Frame;", 9,-5,5);
		TH1D* hSampleDiffNuMI = new TH1D("Sample Diff (NuMI", "Sample Difference (NuMI); Sample;", 9, -5, 5);
		TH1D* hFrameDiffEXT = new TH1D("Frame Diff (EXT)", "Frame Difference (EXT); Frame;", 9,-5,5);
		TH1D* hSampleDiffEXT = new TH1D("Sample Diff (EXT)", "Sample Difference (EXT); Sample;", 9, -5, 5);
		TH1D* hFrameDiffBetweenROAndBNBTrig = new TH1D("Frame DiffRO (BNB)","Frame Diff from RO to BNB Trigger; Frame;",9,-5,5);
		TH1D* hFrameDiffBetweenROAndNuMITrig = new TH1D("Frame DiffRO (NuMI)","Frame Diff from RO to NuMI Trigger; Frame;",9,-5,5);
		TH1D* hFrameDiffBetweenROAndEXTTrig = new TH1D("Frame DiffRO (EXT)","Frame Diff from RO to EXT Trigger; Frame;",9,-5,5);
		TH1D* hSampleDiffBetweenROAndBNBTrig = new TH1D("Sample DiffRO (BNB)","Sample Diff from RO to BNB Trigger; Sample;",19,-10,10);
		TH1D* hSampleDiffBetweenROAndNuMITrig = new TH1D("Sample DiffRO (NuMI)","Sample Diff from RO to NuMI Trigger; Sample;",600,-300,300);
		TH1D* hSampleDiffBetweenROAndEXTTrig = new TH1D("Sample DiffRO (EXT)","Sample Diff from RO to EXT Trigger; Sample;",600,-300,300);
		TH1D* hSampleDiffBetweenRWMAndBNBTrig = new TH1D("","Sample Difference from BNB to RWM; Samples;",32,352,384);
		TH1D* hFrameDiffBetweenRWMAndBNBTrig = new TH1D("Frame Difference","Frame Difference from BNB to RWM;Frames;",9,-5,5);
		TH1D* hTimeDiffBetweenRWMAndBNBTrig = new TH1D("", "Time Difference from BNB to RWM; #mus",32,5.5,6);
		TH1D* hTimeDiffTriggerToWaveForm = new TH1D("","Time Difference Between PMT Waveform and Trigger; #mu s",64,-3200,4800);
		TH1D* hTriggerSampleValue = new TH1D("Trigger Sample","Trigger Sample",102400,1,102401);

		TH1D* hCrateByCrateCompression = new TH1D("", "Crate-by-crate Compression; Crate Number; Compression Factor",9,0,9);
		TH1D* hMeanCompression = new TH1D("Compression", "Mean Compression; Compression Factor",100,3,6);
		TH1D* hMeanCompression_c1 = new TH1D("C1", "Mean Compression; Compression Factor",100,3,6);
		TH1D* hMeanCompression_c2 = new TH1D("C2", "Mean Compression; Compression Factor",100,3,6);
		TH1D* hMeanCompression_c3 = new TH1D("C3", "Mean Compression; Compression Factor",100,3,6);
		TH1D* hMeanCompression_c4 = new TH1D("C4", "Mean Compression; Compression Factor",100,3,6);
		TH1D* hMeanCompression_c5 = new TH1D("C5", "Mean Compression; Compression Factor",100,3,6);
		TH1D* hMeanCompression_c6 = new TH1D("C6", "Mean Compression; Compression Factor",100,3,6);
		TH1D* hMeanCompression_c7 = new TH1D("C7", "Mean Compression; Compression Factor",100,3,6);
		TH1D* hMeanCompression_c8 = new TH1D("C8", "Mean Compression; Compression Factor",100,3,6);
		TH1D* hMeanCompression_c9 = new TH1D("C9", "Mean Compression; Compression Factor",100,3,6);

		TH1D* hEventByEventCompression = new TH1D("", "Event-by-event Compression; Event Number; Compression Factor",500,0,500);

		int ADCwordsEvent,NumWordsEvent;
		long int ADCwords0 = 0; long int ADCwords1 = 0; long int ADCwords2 = 0; long int ADCwords3 = 0; long int ADCwords4 = 0; long int ADCwords5 = 0; long int ADCwords6 = 0; long int ADCwords7 = 0; long int ADCwords8 = 0; long int ADCwords9 = 0;
		long int NumWords0 = 0; long int NumWords1 = 0; long int NumWords2 = 0; long int NumWords3 = 0; long int NumWords4 = 0; long int NumWords5 = 0; long int NumWords6 = 0; long int NumWords7 = 0; long int NumWords8 = 0; long int NumWords9 = 0;

		std::cout << "Starting to loop through everything." << std::endl;

		for(int i = 0; i < int(N_FILES); i++){
			if(i % 5 == 0)
				std::cout << "Path: " << i << "/" << N_FILES << std::endl;

			if(f_daq) f_daq->Close();
			f_daq = new TFile(paths[i].c_str());
			t_tree = (TTree*)(f_daq->Get("Debug/tMyTree"));

			t_tree->SetBranchAddress("triggerTime", &triggerTime);
			t_tree->SetBranchAddress("triggerBitBNB", &triggerBitBNB);
			t_tree->SetBranchAddress("triggerBitNuMI", &triggerBitNuMI);
			t_tree->SetBranchAddress("triggerBitEXT", &triggerBitEXT);
			t_tree->SetBranchAddress("triggerFrame",&triggerFrame);
			t_tree->SetBranchAddress("triggerSample",&triggerSample);
			t_tree->SetBranchAddress("FEM5triggerFrame",&FEM5triggerFrame);
			t_tree->SetBranchAddress("FEM5triggerSample",&FEM5triggerSample);
			t_tree->SetBranchAddress("FEM6triggerFrame",&FEM6triggerFrame);
			t_tree->SetBranchAddress("FEM6triggerSample",&FEM6triggerSample);
			t_tree->SetBranchAddress("RO_BNBtriggerFrame",&RO_BNBtriggerFrame);
			t_tree->SetBranchAddress("RO_BNBtriggerSample",&RO_BNBtriggerSample);
			t_tree->SetBranchAddress("RO_RWMtriggerFrame",&RO_RWMtriggerFrame);
			t_tree->SetBranchAddress("RO_RWMtriggerSample",&RO_RWMtriggerSample);
			t_tree->SetBranchAddress("RO_RWMtriggerTime",&RO_RWMtriggerTime);
			t_tree->SetBranchAddress("RO_NuMItriggerFrame",&RO_NuMItriggerFrame);
			t_tree->SetBranchAddress("RO_NuMItriggerSample",&RO_NuMItriggerSample);
			t_tree->SetBranchAddress("RO_EXTtriggerFrame",&RO_EXTtriggerFrame);
			t_tree->SetBranchAddress("RO_EXTtriggerSample",&RO_EXTtriggerSample);
			t_tree->SetBranchAddress("TPC1triggerFrame",&TPC1triggerFrame);
			t_tree->SetBranchAddress("TPC2triggerFrame",&TPC2triggerFrame);
			t_tree->SetBranchAddress("TPC3triggerFrame",&TPC3triggerFrame);
			t_tree->SetBranchAddress("TPC4triggerFrame",&TPC4triggerFrame);
			t_tree->SetBranchAddress("TPC5triggerFrame",&TPC5triggerFrame);
			t_tree->SetBranchAddress("TPC6triggerFrame",&TPC6triggerFrame);
			t_tree->SetBranchAddress("TPC7triggerFrame",&TPC7triggerFrame);
			t_tree->SetBranchAddress("TPC8triggerFrame",&TPC8triggerFrame);
			t_tree->SetBranchAddress("TPC9triggerFrame",&TPC9triggerFrame);
			t_tree->SetBranchAddress("TPC1triggerSample",&TPC1triggerSample);
			t_tree->SetBranchAddress("TPC2triggerSample",&TPC2triggerSample);
			t_tree->SetBranchAddress("TPC3triggerSample",&TPC3triggerSample);
			t_tree->SetBranchAddress("TPC4triggerSample",&TPC4triggerSample);
			t_tree->SetBranchAddress("TPC5triggerSample",&TPC5triggerSample);
			t_tree->SetBranchAddress("TPC6triggerSample",&TPC6triggerSample);
			t_tree->SetBranchAddress("TPC7triggerSample",&TPC7triggerSample);
			t_tree->SetBranchAddress("TPC8triggerSample",&TPC8triggerSample);
			t_tree->SetBranchAddress("TPC9triggerSample",&TPC9triggerSample);
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
			t_tree->SetBranchAddress("N_PMT_waveforms",&N_PMT_waveforms);
			t_tree->SetBranchAddress("PMT_waveform_times",PMT_waveform_times);

			for(int j = 0; j < t_tree->GetEntries(); j++){
				t_tree->GetEntry(j);

				n_events ++;
				if (lastFrame >0){
					int frameDiff = triggerFrame - lastFrame;
					int sampleDiff = triggerSample - lastSample;
					double timeDiff = triggerTime/1e6 - lastTime;
					hDeltaTBetweenTriggers->Fill(timeDiff);
					if (triggerBitBNB && lastTimeBNB > 0){
						nBNB+=1;
						double timeDiffBNB = triggerTime/1e6 - lastTimeBNB;
						hDeltaTBetweenTriggersBNB->Fill(timeDiffBNB);
					}
					if (triggerBitNuMI && lastTimeNuMI > 0){
						nNuMI+=1;
						double timeDiffNuMI = triggerTime/1e6 - lastTimeNuMI;
						hDeltaTBetweenTriggersNuMI->Fill(timeDiffNuMI);
					}
					if (triggerBitEXT && lastTimeEXT > 0){
						nEXT+=1;
						double timeDiffEXT = triggerTime/1e6 - lastTimeEXT;
						hDeltaTBetweenTriggersEXT->Fill(timeDiffEXT);
					}
				}

				lastFrame = triggerFrame;
				lastSample = triggerSample;
				lastTime = triggerTime/1e6;
				if (triggerBitBNB){
					lastTimeBNB = triggerTime/1e6;
					if(nBNB == 0) nBNB+=1;
				}
				if (triggerBitNuMI){
					lastTimeNuMI = triggerTime/1e6;
					if(nNuMI == 0) nNuMI += 1;
				}
				if (triggerBitEXT){
					lastTimeEXT = triggerTime/1e6;
					if(nEXT == 0) nEXT += 1;
				}

				// Now, we've gotta find if this is the last or first event in the run, which isn't trivial since these files aren't stored in any kind of sensical order
				if(lastTime > finalEventTime)
					finalEventTime = lastTime;
				if(triggerBitBNB && (lastTime > finalEventTimeBNB))
					finalEventTimeBNB = lastTime;
				if(triggerBitNuMI && (lastTime > finalEventTimeNuMI))
					finalEventTimeNuMI = lastTime;
				if(triggerBitEXT && (lastTime > finalEventTimeEXT))
					finalEventTimeEXT = lastTime;

				if(firstEventTime < 0 || lastTime < firstEventTime)
					firstEventTime = lastTime;
				if(triggerBitBNB && (firstEventTimeBNB < 0 || lastTime < firstEventTimeBNB))
					firstEventTimeBNB = lastTimeBNB;
				if(triggerBitNuMI && (firstEventTimeNuMI < 0 || lastTime < firstEventTimeNuMI))
					firstEventTimeNuMI = lastTimeNuMI;
				if(triggerBitEXT && (firstEventTimeEXT < 0 || lastTime < firstEventTimeEXT))
					firstEventTimeEXT = lastTimeEXT;

				int _triggerFrame = triggerFrame;
				int _triggerSample = triggerSample / 32;
				int _FEM5triggerFrame = FEM5triggerFrame;
				int _FEM5triggerSample = FEM5triggerSample;
				int _FEM6triggerFrame = FEM6triggerFrame;
				int _FEM6triggerSample = FEM6triggerSample;
				hTriggerSampleValue->Fill(triggerSample);

				for(int kyle = 0; kyle < N_PMT_waveforms; kyle++){
					if (PMT_waveform_times[kyle] > 1e-200 && PMT_waveform_times[kyle] < 1e50){
						if(abs(PMT_waveform_times[kyle] - triggerTime) >= .5)
							hTimeDiffTriggerToWaveForm->Fill( PMT_waveform_times[kyle] - triggerTime );
						if (PMT_waveform_times[kyle] - triggerTime > 4800){
							logger << "WARNING: Overflow! " << PMT_waveform_times[kyle] - triggerTime << ", " << PMT_waveform_times[kyle] << "\n";
							nt_flags+=1;
						}
						if (PMT_waveform_times[kyle] - triggerTime < -3200){
							logger << "WARNING: Underflow! " << PMT_waveform_times[kyle] - triggerTime << ", " << PMT_waveform_times[kyle] << "\n";
							nt_flags+=1;
						}
					}
				}

				hFrameDiff->Fill(_FEM5triggerFrame - _triggerFrame);
				hSampleDiff->Fill(_FEM5triggerSample - _triggerSample);
				if (triggerBitBNB){
					hFrameDiffBNB->Fill(_FEM5triggerFrame - _triggerFrame);
					hSampleDiffBNB->Fill(_FEM5triggerSample - _triggerSample);
					hFrameDiffBetweenROAndBNBTrig->Fill(_triggerFrame - RO_BNBtriggerFrame);
					hSampleDiffBetweenROAndBNBTrig->Fill(_triggerSample - RO_BNBtriggerSample);
					if (RO_RWMtriggerFrame > 0){
						hFrameDiffBetweenRWMAndBNBTrig->Fill(RO_RWMtriggerFrame - triggerFrame); //RWM signal
						hSampleDiffBetweenRWMAndBNBTrig->Fill(RO_RWMtriggerSample - triggerSample); //RWM signali
						hTimeDiffBetweenRWMAndBNBTrig->Fill(RO_RWMtriggerTime - triggerTime);
						// If our sample diff is outside our window, that's bad
						if(RO_RWMtriggerSample - triggerSample <= 365 || RO_RWMtriggerSample - triggerSample >= 374){
							logger << "WARNING: Samples received outside the RWM to BNB trigger window : " << RO_RWMtriggerSample - triggerSample << "\n";
							nt_flags+=1;
						}
						_samplediff.push_back(RO_RWMtriggerSample - triggerSample);
						_framediff.push_back(RO_RWMtriggerFrame - triggerFrame);
					}
				}
				if (triggerBitNuMI){
					hFrameDiffNuMI->Fill(_FEM5triggerFrame - _triggerFrame);
					hSampleDiffNuMI->Fill(_FEM5triggerSample - _triggerSample );
					hFrameDiffBetweenROAndNuMITrig->Fill(_triggerFrame - RO_NuMItriggerFrame);
					hSampleDiffBetweenROAndNuMITrig->Fill(_triggerSample - RO_NuMItriggerSample);
				}
				if (triggerBitEXT){
					hFrameDiffEXT->Fill(_FEM5triggerFrame - _triggerFrame);
					hSampleDiffEXT->Fill(_FEM5triggerSample - _triggerSample);
					hFrameDiffBetweenROAndEXTTrig->Fill(_triggerFrame - RO_EXTtriggerFrame);
					hSampleDiffBetweenROAndEXTTrig->Fill(_triggerSample - RO_EXTtriggerSample);
				}

				int referenceFrame = _FEM5triggerFrame;
				int referenceSample = _FEM5triggerSample;
				if(_FEM5triggerFrame != referenceFrame){
					logger << "WARNING: Frames don't agree! (PMT FEM5) : " << FEM5triggerFrame << ", " << referenceFrame << "\n";
					nt_flags+=1;
				}
				if (FEM6triggerFrame != referenceFrame){
					logger << "WARNING: Frames don't agree! (PMT FEM6) : " << FEM6triggerFrame << ", " << referenceFrame << "\n";
					nt_flags+=1;
				}
				if (TPC1triggerFrame != referenceFrame){
					logger << "WARNING: Frames don't agree! (TPC1) : " << TPC1triggerFrame << ", " << referenceFrame << "\n";
					nt_flags+=1;
				}
				if (TPC2triggerFrame != referenceFrame){
					logger << "WARNING: Frames don't agree! (TPC2) : " << TPC2triggerFrame << ", " << referenceFrame << "\n";
					nt_flags+=1;
				}
				if (TPC3triggerFrame != referenceFrame){
					logger << "WARNING: Frames don't agree! (TPC3) : " << TPC3triggerFrame << ", " << referenceFrame << "\n";
					nt_flags+=1;
				}
				if (TPC4triggerFrame != referenceFrame){
					logger << "WARNING: Frames don't agree! (TPC4) : " << TPC4triggerFrame << ", " << referenceFrame << "\n";
					nt_flags+=1;
				}
				if (TPC5triggerFrame != referenceFrame){
					logger << "WARNING: Frames don't agree! (TPC5) : " << TPC5triggerFrame << ", " << referenceFrame << "\n";
					nt_flags+=1;
				}
				if (TPC6triggerFrame != referenceFrame){
					logger << "WARNING: Frames don't agree! (TPC6) : " << TPC6triggerFrame << ", " << referenceFrame << "\n";
					nt_flags+=1;
				}
				if (TPC7triggerFrame != referenceFrame){
					logger << "WARNING: Frames don't agree! (TPC7) : " << TPC7triggerFrame << ", " << referenceFrame << "\n";
					nt_flags+=1;
				}
				if (TPC8triggerFrame != referenceFrame){
					logger << "WARNING: Frames don't agree! (TPC8) : " << TPC8triggerFrame << ", " << referenceFrame << "\n";
					nt_flags+=1;
				}
				if (TPC9triggerFrame != referenceFrame){
					logger << "WARNING: Frames don't agree! (TPC9) : " << TPC9triggerFrame << ", " << referenceFrame << "\n";
					nt_flags+=1;
				}
				if (FEM5triggerSample != referenceSample){
					logger << "WARNING: Samples don't agree! (PMT FEM5) : " << FEM5triggerSample << ", " << referenceSample << "\n";
					nt_flags+=1;
				}
				if (FEM6triggerSample != referenceSample){
					logger << "WARNING: Samples don't agree! (PMT FEM6) : " << FEM6triggerSample << ", " << referenceSample << "\n";
					nt_flags+=1;
				}
				if (TPC1triggerSample != referenceSample){
					logger << "WARNING: Samples don't agree! (TPC1) : " << TPC1triggerSample << ", " << referenceSample << "\n";
					nt_flags+=1;
				}
				if (TPC2triggerSample != referenceSample){
					logger << "WARNING: Samples don't agree! (TPC2) : " << TPC2triggerSample << ", " << referenceSample << "\n";
					nt_flags+=1;
				}
				if (TPC3triggerSample != referenceSample){
					logger << "WARNING: Samples don't agree! (TPC3) : " << TPC3triggerSample << ", " << referenceSample << "\n";
					nt_flags+=1;
				}
				if (TPC4triggerSample != referenceSample){
					logger << "WARNING: Samples don't agree! (TPC4) : " << TPC4triggerSample << ", " << referenceSample << "\n";
					nt_flags+=1;
				}
				if (TPC5triggerSample != referenceSample){
					logger << "WARNING: Samples don't agree! (TPC5) : " << TPC5triggerSample << ", " << referenceSample << "\n";
					nt_flags+=1;
				}
				if (TPC6triggerSample != referenceSample){
					logger << "WARNING: Samples don't agree! (TPC6) : " << TPC6triggerSample << ", " << referenceSample << "\n";
					nt_flags+=1;
				}
				if (TPC7triggerSample != referenceSample){
					logger << "WARNING: Samples don't agree! (TPC7) : " << TPC7triggerSample << ", " << referenceSample << "\n";
					nt_flags+=1;
				}
				if (TPC8triggerSample != referenceSample){
					logger << "WARNING: Samples don't agree! (TPC8) : " << TPC8triggerSample << ", " << referenceSample << "\n";
					nt_flags+=1;
				}
				if (TPC9triggerSample != referenceSample){
					logger << "WARNING: Samples don't agree! (TPC9) : " << TPC9triggerSample << ", " << referenceSample << "\n";
					nt_flags+=1;
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
					hMeanCompression->Fill(ADCwordsEvent / float(NumWordsEvent));
					hMeanCompression_c1->Fill(ADCwords_crate1 / float(NumWords_crate1));
					hMeanCompression_c2->Fill(ADCwords_crate2 / float(NumWords_crate2));
					hMeanCompression_c3->Fill(ADCwords_crate3 / float(NumWords_crate3));
					hMeanCompression_c4->Fill(ADCwords_crate4 / float(NumWords_crate4));
					hMeanCompression_c5->Fill(ADCwords_crate5 / float(NumWords_crate5));
					hMeanCompression_c6->Fill(ADCwords_crate6 / float(NumWords_crate6));
					hMeanCompression_c7->Fill(ADCwords_crate7 / float(NumWords_crate7));
					hMeanCompression_c8->Fill(ADCwords_crate8 / float(NumWords_crate8));
					hMeanCompression_c9->Fill(ADCwords_crate9 / float(NumWords_crate9));
					hEventByEventCompression->SetBinContent(j,ADCwordsEvent / float(NumWordsEvent));
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

		if (lastTime == firstEventTime){
			logger << "WARNING: lastTime = firstEventTime = " << lastTime << ". Skipping run for now because I don't know what to do. \n";
			return false;
		}
		averageTrigRate = t_tree->GetEntries() / (finalEventTime - firstEventTime);
		if (nBNB)
			averageTrigRateBNB = nBNB / (finalEventTimeBNB - firstEventTimeBNB);
		else
			averageTrigRateBNB = 0;
		if (nNuMI)
			averageTrigRateNuMI = nNuMI / (finalEventTimeNuMI - firstEventTimeNuMI);
		else
			averageTrigRateNuMI = 0;
		if (nEXT)
			averageTrigRateEXT = nEXT / (finalEventTimeEXT - firstEventTimeEXT);
		else
			averageTrigRateEXT = 0;

		logger << "Rates: total, BNB, NuMI, EXT\n";
		logger << averageTrigRate << ", " << averageTrigRateBNB << ", " << averageTrigRateNuMI << ", " << averageTrigRateEXT << "\n";
		logger << "Diff = " << (averageTrigRate - averageTrigRateBNB - averageTrigRateNuMI - averageTrigRateEXT) << "\n";

		TH1D * hTriggerRates = new TH1D("","Average Trigger Rates; Trigger Type; Rate (Hz)",4,0,4);
		hTriggerRates->GetXaxis()->SetBinLabel(1, "BNB");
		hTriggerRates->GetXaxis()->SetBinLabel(2, "NuMI");
		hTriggerRates->GetXaxis()->SetBinLabel(3, "EXT");
		hTriggerRates->GetXaxis()->SetBinLabel(4, "TOTAL");
		hTriggerRates->SetBinContent(1, averageTrigRateBNB);
		hTriggerRates->SetBinContent(2, averageTrigRateNuMI);
		hTriggerRates->SetBinContent(3, averageTrigRateEXT);
		hTriggerRates->SetBinContent(4, averageTrigRate);
		hTriggerRates->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/triggerRates"+to_string(rnum)+".eps").c_str());

		canv->SetLogy(true);
		hDeltaTBetweenTriggers->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/dTBetweenTriggersAllLin"+to_string(rnum)+".eps").c_str());

		canv->SetLogx(true);
		hDeltaTBetweenTriggers->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/dTBetweenTriggersAllLog"+to_string(rnum)+".eps").c_str());

		canv->SetLogx(false);
		hDeltaTBetweenTriggersBNB->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/dTBetweenTriggersBNB"+to_string(rnum)+".eps").c_str());

		hDeltaTBetweenTriggersNuMI->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/dTBetweenTriggersNuMI"+to_string(rnum)+".eps").c_str());

		hDeltaTBetweenTriggersEXT->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/dTBetweenTriggersEXT"+to_string(rnum)+".eps").c_str());

		canv->SetLogy(false);

		hTimeDiffTriggerToWaveForm->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/PMT_flash_dtFromTrigger"+to_string(rnum)+".eps").c_str());


		gStyle->SetOptStat("nemou");
		hFrameDiffBetweenROAndBNBTrig->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/ROtoTrigFrameDiffBNB"+to_string(rnum)+".eps").c_str());
		hSampleDiffBetweenROAndBNBTrig->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/ROtoTrigSampleDiffBNB"+to_string(rnum)+".eps").c_str());
		hFrameDiffBetweenROAndNuMITrig->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/ROtoTrigFrameDiffNuMI"+to_string(rnum)+".eps").c_str());
		hSampleDiffBetweenROAndNuMITrig->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/ROtoTrigSampleDiffNuMI"+to_string(rnum)+".eps").c_str());
		hFrameDiffBetweenROAndEXTTrig->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/ROtoTrigFrameDiffEXT"+to_string(rnum)+".eps").c_str());
		hSampleDiffBetweenROAndEXTTrig->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/ROtoTrigSampleDiffEXT"+to_string(rnum)+".eps").c_str());
		hFrameDiffBetweenRWMAndBNBTrig->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/BNBTrigToRWMFrameDiff"+to_string(rnum)+".eps").c_str());
		hSampleDiffBetweenRWMAndBNBTrig->Draw();
		sampleDiffBetweenRWMandBNB = hSampleDiffBetweenRWMAndBNBTrig;
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/BNBTrigToRWMSampleDiff"+to_string(rnum)+".eps").c_str());
		hTimeDiffBetweenRWMAndBNBTrig->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/BNBTrigToRWMTimeDiff"+to_string(rnum)+".eps").c_str());

		gStyle->SetOptStat("meou");
		hTriggerSampleValue->GetXaxis()->SetLimits(-10,102410);
		hTriggerSampleValue->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/TriggerSampleValue"+to_string(rnum)+".eps").c_str());

		gStyle->SetOptStat("men");
		hFrameDiff->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/TriggerFrameDifference"+to_string(rnum)+".eps").c_str());
		hSampleDiff->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/TriggerSampleDifference"+to_string(rnum)+".eps").c_str());
		hFrameDiffBNB->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/TriggerFrameDifferenceBNB"+to_string(rnum)+".eps").c_str());
		hSampleDiffBNB->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/TriggerSampleDifferenceBNB"+to_string(rnum)+".eps").c_str());
		hFrameDiffNuMI->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/TriggerFrameDifferenceNuMI"+to_string(rnum)+".eps").c_str());
		hSampleDiffNuMI->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/TriggerSampleDifferenceNuMI"+to_string(rnum)+".eps").c_str());
		hFrameDiffEXT->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/TriggerFrameDifferenceEXT"+to_string(rnum)+".eps").c_str());
		hSampleDiffEXT->Draw();
		if (save)
			canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/TriggerSampleDifferenceEXT"+to_string(rnum)+".eps").c_str());

		if (NumWords1){
			hCrateByCrateCompression->SetBinContent(1,ADCwords1/float(NumWords1));
			cbc_01 = ADCwords1/float(NumWords1);
			cbc_01err = hCBC_01->GetRMS();
		}
		if (NumWords2){
			hCrateByCrateCompression->SetBinContent(2,ADCwords2/float(NumWords2));
			cbc_02 = ADCwords2/float(NumWords2);
			cbc_02err = hCBC_02->GetRMS();
		}
		if (NumWords3){
			hCrateByCrateCompression->SetBinContent(3,ADCwords3/float(NumWords3));
			cbc_03 = ADCwords3/float(NumWords3);
			cbc_03err = hCBC_03->GetRMS();
		}
		if (NumWords4){
			hCrateByCrateCompression->SetBinContent(4,ADCwords4/float(NumWords4));
			cbc_04 = ADCwords4/float(NumWords4);
			cbc_04err = hCBC_04->GetRMS();
		}
		if (NumWords5){
			hCrateByCrateCompression->SetBinContent(5,ADCwords5/float(NumWords5));
			cbc_05 = ADCwords5/float(NumWords5);
			cbc_05err = hCBC_05->GetRMS();
		}
		if (NumWords6){
			hCrateByCrateCompression->SetBinContent(6,ADCwords6/float(NumWords6));
			cbc_06 = ADCwords6/float(NumWords6);
			cbc_06err = hCBC_06->GetRMS();
		}
		if (NumWords7){
			hCrateByCrateCompression->SetBinContent(7,ADCwords7/float(NumWords7));
			cbc_07 = ADCwords7/float(NumWords7);
			cbc_07err = hCBC_07->GetRMS();
		}
		if (NumWords8){
			hCrateByCrateCompression->SetBinContent(8,ADCwords8/float(NumWords8));
			cbc_08 = ADCwords8/float(NumWords8);
			cbc_08err = hCBC_08->GetRMS();
		}
		if (NumWords9){
			hCrateByCrateCompression->SetBinContent(9,ADCwords9/float(NumWords9));
			cbc_09 = ADCwords9/float(NumWords9);
			cbc_09err = hCBC_09->GetRMS();
		}

		gStyle->SetOptStat("men");
		hMeanCompression->Draw();
		meanCompression = hMeanCompression;
		meanCompression_c1 = hMeanCompression_c1;
		meanCompression_c2 = hMeanCompression_c2;
		meanCompression_c3 = hMeanCompression_c3;
		meanCompression_c4 = hMeanCompression_c4;
		meanCompression_c5 = hMeanCompression_c5;
		meanCompression_c6 = hMeanCompression_c6;
		meanCompression_c7 = hMeanCompression_c7;
		meanCompression_c8 = hMeanCompression_c8;
		meanCompression_c9 = hMeanCompression_c9;

		if (save)
			if (NumWordsEvent)
				canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/MeanCompression"+to_string(rnum)+".eps").c_str());
		gStyle->SetOptStat(0000);
		hEventByEventCompression->SetMinimum(0);
		hEventByEventCompression->SetMaximum(9);
		hEventByEventCompression->Draw();
		if (save)
			if (NumWordsEvent)
				canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/compressionByEvent"+to_string(rnum)+".eps").c_str());
		hCrateByCrateCompression->SetMaximum(9);
		hCrateByCrateCompression->SetMinimum(0);
		hCrateByCrateCompression->Draw();
		if (save)
			if (NumWordsEvent)
				canv->SaveAs((plotloc+"plots_"+to_string(rnum)+"/compressionByCrate"+to_string(rnum)+".eps").c_str());

		f_daq->Close();
	}

	// Alright, now that we're done, print all of our flags and lets close that fucking file
	std::cout << "Fill 'em up!" << std::endl;

	f_status = new TFile("dqm_status.root","update");
	nt_status = (TNtuple*)(f_status->Get("status"));

	// Now, let's fill up the timefuncs
	t_timefuncs = (TTree*)(f_status->Get("timeFuncs"));

	t_timefuncs->SetBranchAddress("run_number",&run_number);
	t_timefuncs->SetBranchAddress("cbc_01",&cbc_01);	t_timefuncs->SetBranchAddress("cbc_01err",&cbc_01err);
	t_timefuncs->SetBranchAddress("cbc_02",&cbc_02);	t_timefuncs->SetBranchAddress("cbc_02err",&cbc_02err);
	t_timefuncs->SetBranchAddress("cbc_03",&cbc_03);	t_timefuncs->SetBranchAddress("cbc_03err",&cbc_03err);
	t_timefuncs->SetBranchAddress("cbc_04",&cbc_04);	t_timefuncs->SetBranchAddress("cbc_04err",&cbc_04err);
	t_timefuncs->SetBranchAddress("cbc_05",&cbc_05);	t_timefuncs->SetBranchAddress("cbc_05err",&cbc_05err);
	t_timefuncs->SetBranchAddress("cbc_06",&cbc_06);	t_timefuncs->SetBranchAddress("cbc_06err",&cbc_06err);
	t_timefuncs->SetBranchAddress("cbc_07",&cbc_07);	t_timefuncs->SetBranchAddress("cbc_07err",&cbc_07err);
	t_timefuncs->SetBranchAddress("cbc_08",&cbc_08);	t_timefuncs->SetBranchAddress("cbc_08err",&cbc_08err);
	t_timefuncs->SetBranchAddress("cbc_09",&cbc_09);	t_timefuncs->SetBranchAddress("cbc_09err",&cbc_09err);
	t_timefuncs->SetBranchAddress("sampleDiffBetweenRWMandBNB",&sampleDiffBetweenRWMandBNB);
	t_timefuncs->SetBranchAddress("meanCompression",&meanCompression);
	t_timefuncs->SetBranchAddress("meanCompression_c1", &meanCompression_c1);
	t_timefuncs->SetBranchAddress("meanCompression_c2", &meanCompression_c2);
	t_timefuncs->SetBranchAddress("meanCompression_c3", &meanCompression_c3);
	t_timefuncs->SetBranchAddress("meanCompression_c4", &meanCompression_c4);
	t_timefuncs->SetBranchAddress("meanCompression_c5", &meanCompression_c5);
	t_timefuncs->SetBranchAddress("meanCompression_c6", &meanCompression_c6);
	t_timefuncs->SetBranchAddress("meanCompression_c7", &meanCompression_c7);
	t_timefuncs->SetBranchAddress("meanCompression_c8", &meanCompression_c8);
	t_timefuncs->SetBranchAddress("meanCompression_c9", &meanCompression_c9);
	if(N_FILES)
		t_timefuncs->Fill();

	nt_status->Fill(rnum,N_FILES,n_events,nt_flags);
	nt_status->Write();
	t_timefuncs->Write();
	f_status->Close();

	logger.close();

	return false;
}


bool swizzlePlots(int rnum){
	return makePlots(rnum);
}

#if !defined(__CINT__) || defined (__MAKECINT__)
int main(int argc, char* argv[])
{
	int num = atoi(argv[0]);
	return swizzlePlots(num);
}
#endif
