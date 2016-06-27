/***************************************************
***                                              ***
***    correlation_EEE.C    		         ***
***                                              ***
***   This program looks for the coincicidences  ***
***   between 2 EEE telescopes. It uses a        ***
***   configuration file to to choose the two    ***
***   telescopes under investigation and specify ***
***   the input data directory.The output file   ***
***   contains a tree with basic information     ***
***   about the tracks and the time difference   ***
***   between the events collected by the 2      ***
***   telescopes.                                ***
***                                              ***
***   Authors: P.La Rocca, F.Riggi               ***
***   Contacts: paola.larocca@ct.infn.it         ***
***             francesco.riggi@ct.infn.it       ***
***                                              ***
***************************************************/



#include <TFile.h>
#include <TTree.h>
#include <Riostream.h>
#include <TMath.h>
#include <stdio.h>
#include <TString.h>
#include <TArrayI.h>
#include <TChain.h>
#include "TRandom.h"
#include <TH1I.h>
#include <TH1F.h>
#include <TLeaf.h>

bool CfrString(const char *str1,const char *str2);
void CalculateThetaPhi(Float_t &cx, Float_t &cy, Float_t &cz, Float_t &teta, Float_t &phi);
void correlation_EEE(const char *coinc1,const char *coinc2,const char *out,Double_t DiffCut=0.1,Double_t corrWindow=1E-4);

int main(int argc,char *argv[]){
  
  if(argc < 4)
    printf("you need to specify fileinput1 fileinput2 fileoutput\n");

  char *in1 = argv[1];
  char *in2 = argv[2];
  char *out = argv[3];

  Double_t cell=0.1;
  Double_t window=1E-4;

  printf("list of options to overwrite the config file infos:\n");
  printf("-cell cell_time_window = cell time window in seconds (default = 0.1)\n");
  printf("-window corr_time_window = correlation time window to define output in seconds (default = 1E-4)\n");

  int kNoConfigFile = 0;

  for(Int_t i=4;i < argc;i++){
   if(CfrString(argv[i],"-cell")){
      if(i+1 > argc){
        printf("cell time window missing\n");
        return 1;
      }
      sscanf(argv[i+1],"%lf",&cell);
      i++;  
    }

   if(CfrString(argv[i],"-window")){
      if(i+1 > argc){
        printf("correlation time window missing\n");
        return 1;
      }
      sscanf(argv[i+1],"%lf",&window);
      i++;  
    }

  }

  correlation_EEE(in1,in2,out,cell,window);

  return 0;
}


void correlation_EEE(const char *coinc1,const char *coinc2,const char *out,Double_t DiffCut,Double_t corrWindow)
{
//
// This macro correlates the events measured by two EEE telescopes according to their GPS time
// within a time window = DiffCut
//
// Data are read from the two ROOT trees created for each telescope
//

  corrWindow*=1E+9;

   TH1I *hexposure1 = new TH1I("hexposure1","Active run number in a given second;second - time_first_event;run number telescope 1",100000,0,100000);
   TH1I *hexposure2 = new TH1I("hexposure2","Active run number in a given second;second - time_first_event;run number telescope 2",100000,0,100000);

   TH1I *htimePerRun1 = new TH1I("htimePerRun1","time duration of the run for telescope 1;run number;duration (s)",500,0,500);
   TH1I *htimePerRun2 = new TH1I("htimePerRun2","time duration of the run for telescope 2;run number;duration (s)",500,0,500);

   TH1F *hAllPerRun1 = new TH1F("hAllPerRun1","Rate of all events per Run telescope 1;Run number;Rate of events (Hz)",500,0,500);
   TH1F *hAllPerRun2 = new TH1F("hAllPerRun2","Rate of all events per Run telescope 2;Run number;Rate of events (Hz)",500,0,500);

   TH1F *hEventPerRun1 = new TH1F("hEventPerRun1","Rate of events with hits per Run telescope 1;Run number;Rate of events (Hz)",500,0,500);
   TH1F *hEventPerRun2 = new TH1F("hEventPerRun2","Rate of events with hits per Run telescope 2;Run number;Rate of events (Hz)",500,0,500);

   TH1F *hGoodTrackPerRun1 = new TH1F("hGoodTrackPerRun1","Fraction of good tracks (#Chi^{2} < 10) per Run telescope 1;Run number;Fraction of Good Tracks",500,0,500);
   TH1F *hGoodTrackPerRun2 = new TH1F("hGoodTrackPerRun2","Fraction of good tracks (#Chi^{2} < 10) per Run telescope 2;Run number;Fraction of Good Tracks",500,0,500);

//
// Open and read the configuration file
//

//
// Input files
//

   TFile *fin1 = new TFile(coinc1);
   TFile *fin2 = new TFile(coinc1);

   TTree *t1 = (TTree *) fin1->Get("tree");
   TTree *tel1 = (TTree *) fin1->Get("treeTel1");
   TTree *tel2 = (TTree *) fin1->Get("treeTel2");
   TTree *t2 = (TTree *) fin2->Get("tree");
   TTree *tel3 = (TTree *) fin2->Get("treeTel1");
   TTree *tel4 = (TTree *) fin2->Get("treeTel2");

   /*
        TChain *t1h = new TChain("Header");
        TChain *t1g = new TChain("gps");
        TChain *t1w = new TChain("Weather");
   */

//
// Define input tree structure	
//
        Int_t year,month,day;

	Int_t RunNumber1, RunNumber2;
	Int_t RunNumber3, RunNumber4;
	Int_t EventNumber1, EventNumber2;
	Int_t EventNumber3, EventNumber4;
	Int_t Seconds1, Seconds2;
	Int_t Seconds3, Seconds4;
	ULong64_t Nanoseconds1, Nanoseconds2;
	ULong64_t Nanoseconds3, Nanoseconds4;
//	ULong64_t UniqueRunId1, UniqueRunId2;
        Int_t ntracks1,ntracks2;
        Int_t ntracks3,ntracks4;
	Float_t nsat1,nsat2;
	Float_t nsat3,nsat4;
 	Int_t nsat1gps=0,nsat2gps=0;
 	Int_t nsat3gps=0,nsat4gps=0;
	Int_t maskT1,maskM1,maskB1;
	Int_t maskT2,maskM2,maskB2;
	Int_t maskT3,maskM3,maskB3;
	Int_t maskT4,maskM4,maskB4;
	Float_t pressure1,tin1,tout1;
	Float_t pressure2,tin2,tout2;
	Float_t pressure3,tin3,tout3;
	Float_t pressure4,tin4,tout4;

	// track by track info
	Float_t Theta1,Phi1,Theta2,Phi2;
	Float_t Theta3,Phi3,Theta4,Phi4;
	Float_t ChiSquare1[24], ChiSquare2[24];
	Float_t ChiSquare3[24], ChiSquare4[24];
	Float_t TimeOfFlight1[24], TimeOfFlight2[24];
	Float_t TimeOfFlight3[24], TimeOfFlight4[24];
	Float_t TrackLength1[24], TrackLength2[24];
	Float_t TrackLength3[24], TrackLength4[24];
	Float_t DeltaTime1[24], DeltaTime2[24];
	Float_t DeltaTime3[24], DeltaTime4[24];
	Double_t tweather1,tweather2;
	Double_t tweather3,tweather4;

	Double_t DiffTime12;
	Double_t DiffTime34;
	Double_t DiffTime13;

        if(t1->GetLeaf("Nsatellites1")) t1->SetBranchAddress("Nsatellites1", &nsat1);
        if(t1->GetLeaf("Nsatellites2")) t1->SetBranchAddress("Nsatellites2", &nsat2);
        if(t2->GetLeaf("Nsatellites1")) t2->SetBranchAddress("Nsatellites1", &nsat3);
        if(t2->GetLeaf("Nsatellites2")) t2->SetBranchAddress("Nsatellites2", &nsat4);
        if(tel1->GetLeaf("maskB")) tel1->SetBranchAddress("maskB", &maskB1);
        if(tel2->GetLeaf("maskB")) tel2->SetBranchAddress("maskB", &maskB2);
        if(tel3->GetLeaf("maskB")) tel3->SetBranchAddress("maskB", &maskB3);
        if(tel4->GetLeaf("maskB")) tel4->SetBranchAddress("maskB", &maskB4);
        if(tel1->GetLeaf("maskM")) tel1->SetBranchAddress("maskM", &maskM1);
        if(tel2->GetLeaf("maskM")) tel2->SetBranchAddress("maskM", &maskM2);
        if(tel3->GetLeaf("maskM")) tel3->SetBranchAddress("maskM", &maskM3);
        if(tel4->GetLeaf("maskM")) tel4->SetBranchAddress("maskM", &maskM4);
        if(tel1->GetLeaf("maskT")) tel1->SetBranchAddress("maskT", &maskT1);
        if(tel2->GetLeaf("maskT")) tel2->SetBranchAddress("maskT", &maskT2);
        if(tel3->GetLeaf("maskT")) tel3->SetBranchAddress("maskT", &maskT3);
        if(tel4->GetLeaf("maskT")) tel4->SetBranchAddress("maskT", &maskT4);

        if(tel1->GetLeaf("nSat")) tel1->SetBranchAddress("nSat", &nsat1gps);
        if(tel2->GetLeaf("nSat")) tel2->SetBranchAddress("nSat", &nsat2gps);
        if(tel3->GetLeaf("nSat")) tel3->SetBranchAddress("nSat", &nsat3gps);
        if(tel4->GetLeaf("nSat")) tel4->SetBranchAddress("nSat", &nsat4gps);

        if(tel1->GetLeaf("Pressure")) tel1->SetBranchAddress("Pressure", &pressure1);
        if(tel1->GetLeaf("IndoorTemperature")) tel1->SetBranchAddress("IndoorTemperature", &tin1);
        if(tel1->GetLeaf("OutdoorTemperature")) tel1->SetBranchAddress("OutdoorTemperature", &tout1);
        if(tel1->GetLeaf("TimeWeatherUpdate")) tel1->SetBranchAddress("TimeWeatherUpdate", &tweather1);
        if(tel2->GetLeaf("Pressure")) tel2->SetBranchAddress("Pressure", &pressure2);
        if(tel2->GetLeaf("IndoorTemperature")) tel2->SetBranchAddress("IndoorTemperature", &tin2);
        if(tel2->GetLeaf("OutdoorTemperature")) tel2->SetBranchAddress("OutdoorTemperature", &tout2);
        if(tel2->GetLeaf("TimeWeatherUpdate")) tel2->SetBranchAddress("TimeWeatherUpdate", &tweather2);
        if(tel3->GetLeaf("Pressure")) tel3->SetBranchAddress("Pressure", &pressure1);
        if(tel3->GetLeaf("IndoorTemperature")) tel3->SetBranchAddress("IndoorTemperature", &tin3);
        if(tel3->GetLeaf("OutdoorTemperature")) tel3->SetBranchAddress("OutdoorTemperature", &tout3);
        if(tel3->GetLeaf("TimeWeatherUpdate")) tel3->SetBranchAddress("TimeWeatherUpdate", &tweather3);
        if(tel4->GetLeaf("Pressure")) tel4->SetBranchAddress("Pressure", &pressure1);
        if(tel4->GetLeaf("IndoorTemperature")) tel4->SetBranchAddress("IndoorTemperature", &tin4);
        if(tel4->GetLeaf("OutdoorTemperature")) tel4->SetBranchAddress("OutdoorTemperature", &tout4);
        if(tel4->GetLeaf("TimeWeatherUpdate")) tel4->SetBranchAddress("TimeWeatherUpdate", &tweather4);


	t1->SetBranchAddress("year",&year);
	t1->SetBranchAddress("month",&month);
	t1->SetBranchAddress("day",&day);
	t1->SetBranchAddress("RunNumber1",&RunNumber1);
	t1->SetBranchAddress("RunNumber2",&RunNumber2);
	t1->SetBranchAddress("EventNumber1",&EventNumber1);
	t1->SetBranchAddress("EventNumber2",&EventNumber2);
	t1->SetBranchAddress("Seconds1",&Seconds1);
	t1->SetBranchAddress("Seconds2",&Seconds2);
	t1->SetBranchAddress("Nanoseconds1",&Nanoseconds1);
	t1->SetBranchAddress("Nanoseconds2",&Nanoseconds2);
        if(t1->GetLeaf("Ntracks1")) t1->SetBranchAddress("Ntracks1", &ntracks1);
        if(t1->GetLeaf("Ntracks2")) t1->SetBranchAddress("Ntracks2", &ntracks2);
	t1->SetBranchAddress("Theta1",&Theta1);
	t1->SetBranchAddress("Phi1",&Phi1);
	t1->SetBranchAddress("Theta2",&Theta2);
	t1->SetBranchAddress("Phi2",&Phi2);
	t1->SetBranchAddress("ChiSquare1", ChiSquare1);
	t1->SetBranchAddress("ChiSquare2", ChiSquare2);
	t1->SetBranchAddress("TimeOfFlight1", TimeOfFlight1);
	t1->SetBranchAddress("TimeOfFlight2", TimeOfFlight2);
	t1->SetBranchAddress("TrackLength1", TrackLength1);
	t1->SetBranchAddress("TrackLength2", TrackLength2);
	t1->SetBranchAddress("DiffTime", &DiffTime12);
	t2->SetBranchAddress("RunNumber1",&RunNumber3);
	t2->SetBranchAddress("RunNumber2",&RunNumber4);
	t2->SetBranchAddress("EventNumber1",&EventNumber3);
	t2->SetBranchAddress("EventNumber2",&EventNumber4);
	t2->SetBranchAddress("Seconds1",&Seconds3);
	t2->SetBranchAddress("Seconds2",&Seconds4);
	t2->SetBranchAddress("Nanoseconds1",&Nanoseconds3);
	t2->SetBranchAddress("Nanoseconds2",&Nanoseconds4);
        if(t2->GetLeaf("Ntracks1")) t2->SetBranchAddress("Ntracks1", &ntracks3);
        if(t2->GetLeaf("Ntracks2")) t2->SetBranchAddress("Ntracks2", &ntracks4);
	t2->SetBranchAddress("Theta1",&Theta3);
	t2->SetBranchAddress("Phi1",&Phi4);
	t2->SetBranchAddress("Theta2",&Theta3);
	t2->SetBranchAddress("Phi2",&Phi4);
	t2->SetBranchAddress("ChiSquare1", ChiSquare3);
	t2->SetBranchAddress("ChiSquare2", ChiSquare4);
	t2->SetBranchAddress("TimeOfFlight1", TimeOfFlight3);
	t2->SetBranchAddress("TimeOfFlight2", TimeOfFlight4);
	t2->SetBranchAddress("TrackLength1", TrackLength3);
	t2->SetBranchAddress("TrackLength2", TrackLength4);
	t2->SetBranchAddress("DiffTime", &DiffTime34);

	Int_t nent12 = t1->GetEntries();
	Int_t nent34 = t2->GetEntries();

	Double_t ctime12, ctime34;
	Double_t ctime2, ctime4;
	t1->SetBranchAddress("ctime1", &ctime12);
	t1->SetBranchAddress("ctime2", &ctime2);
	t2->SetBranchAddress("ctime1", &ctime34);
	t2->SetBranchAddress("ctime2", &ctime4);


//      
// Find time range
//

        Double_t startTime;

	Double_t t1min, t1max, t2min, t2max, range1, range2;
        Int_t i1 = 0;
        t1->GetEntry(        i1); t1min = ctime12;
        cout << "start " << Seconds1 << endl;        

        startTime = Seconds1;

        i1 = nent12 - 1;
	t1->GetEntry( i1); t1max = ctime12;
        cout << "end " << Seconds1 << " " << endl;
        Int_t i2 = 0; 
	t2->GetEntry(        i2); t2min = ctime34;
        cout << "start " << Seconds2 << endl;
        if(startTime > Seconds3) startTime = Seconds2;
        i2 = nent34 - 1; 
	t2->GetEntry(i2); t2max = ctime34;
        cout << "end " << Seconds2<< endl;

	range1 = t1max - t1min;
	range2 = t2max - t2min;
	cout.setf(ios::fixed);
	cout <<"N.entry1 = "<< nent12<<"   N.entry2 = "<<nent34 << endl;
	cout << "Time range file 1: " << t1min << " --> " << t1max << ", range = " << range1 << endl;
	cout << "Time range file 2: " << t2min << " --> " << t2max << ", range = " << range2 << endl;
	Double_t tmin = TMath::Min(t1min, t2min);
	Double_t tmax = TMath::Max(t1max, t2max);
	cout << "Common measure time interval = "<<(TMath::Min(t1max, t2max)-TMath::Max(t1min, t2min))<< " s"<<endl;

// collect info on run duration and rate
       for(Int_t e1 = 0; e1 < nent12; e1++) {
                t1->GetEntry(e1);
                hexposure1->SetBinContent(hexposure1->FindBin(Seconds1-startTime),RunNumber1);

                hAllPerRun1->Fill(RunNumber1);
		hEventPerRun1->Fill(RunNumber1); 
		if(ChiSquare1[0] < 10) hGoodTrackPerRun1->Fill(RunNumber1);
       }
       hGoodTrackPerRun1->Divide(hEventPerRun1);

       for(Int_t e2 = 0; e2 < nent34; e2++) {
                t2->GetEntry(e2);
                hexposure2->SetBinContent(hexposure2->FindBin(Seconds2-startTime),RunNumber2);

                hAllPerRun2->Fill(RunNumber2);
		hEventPerRun2->Fill(RunNumber2);    
		if(ChiSquare2[0] < 10) hGoodTrackPerRun2->Fill(RunNumber2);
       }
       hGoodTrackPerRun2->Divide(hEventPerRun2);

       for(Int_t i=1;i<100000;i++){
         if(hexposure1->GetBinContent(i) > 0)
            htimePerRun1->Fill(hexposure1->GetBinContent(i));
   
         if(hexposure2->GetBinContent(i) > 0)
            htimePerRun2->Fill(hexposure2->GetBinContent(i));

       }

       hAllPerRun1->Divide(htimePerRun1);
       hAllPerRun2->Divide(htimePerRun2);

       hEventPerRun1->Divide(htimePerRun1);
       hEventPerRun2->Divide(htimePerRun2);

//
// Chain mesh: define starting time cell for both trees
//
	Int_t firstCelTrackLength1 = (Int_t)((t1min - tmin) / DiffCut);
	Int_t firstCelTrackLength2 = (Int_t)((t2min - tmin) / DiffCut);
	Int_t lastCelTrackLength1  = (Int_t)((t1max - tmin) / DiffCut);
	Int_t lastCelTrackLength2  = (Int_t)((t2max - tmin) / DiffCut);
	cout << "Starting, ending cell for tree 1: " << firstCelTrackLength1 << ", " << lastCelTrackLength1 << endl;
	cout << "Starting, ending cell for tree 2: " << firstCelTrackLength2 << ", " << lastCelTrackLength2 << endl;
//
// define complete cell range
//
	const Int_t ncells = (Int_t)TMath::Max(lastCelTrackLength1, lastCelTrackLength2);
	cout << "#cells: " << ncells << endl;
	cout<<"Working..."<<endl;
//	
// define index collector for all cells
//
        cout << ncells << endl;
	TArrayI *cell = new TArrayI[ncells];
	for (Int_t i = 0; i < ncells; i++) cell[i].Set(0);
//	
// loop on TTree #2 and add each entry to corresponding cell
//
	Int_t cellIndex, size;
	for (Int_t i = 0; i < nent34; i++) {
		t2->GetEntry(i);
		cellIndex = (Int_t)((ctime34 - tmin) / DiffCut);
		if (cellIndex >= 0 && cellIndex < ncells) {
			size = cell[cellIndex].GetSize();
			cell[cellIndex].Set(size+1);
			cell[cellIndex][size] = i;
		}
	}
//	
// Define output correlation tree
//
	TFile *fileout = new TFile(out, "RECREATE");
        fileout->ls();

	// fill tree with quality check per run
	/*
	Float_t ratePerRun,ratePerRunAll,FractionGoodTrack;
        Int_t timeduration,runnumber,runnumber2;

        TTree *treeTel1 = new TTree("treeTel1", "run information of telescope 1");
        treeTel1->Branch("year", &year, "year/I");
        treeTel1->Branch("month", &month, "month/I");
        treeTel1->Branch("day", &day, "day/I");
        treeTel1->Branch("run", &runnumber, "run/I");
        treeTel1->Branch("timeduration",&timeduration,"timeduration/I");
        treeTel1->Branch("ratePerRun",&ratePerRunAll,"ratePerRun/F");
        treeTel1->Branch("rateHitPerRun",&ratePerRun,"rateHitPerRun/F");
        treeTel1->Branch("FractionGoodTrack",&FractionGoodTrack,"FractionGoodTrack/F");
        if(t1h->GetLeaf("nSatellites")) treeTel1->Branch("nSat",&nsat1,"nSat/F");
        if(t1h->GetLeaf("DeadChMaskBot")) treeTel1->Branch("maskB",&maskB1,"maskB/I");
        if(t1h->GetLeaf("DeadChMaskMid")) treeTel1->Branch("maskM",&maskM1,"maskM/I");
        if(t1h->GetLeaf("DeadChMaskTop")) treeTel1->Branch("maskT",&maskT1,"maskT/I");
	if(t1w->GetLeaf("Pressure"))  treeTel1->Branch("Pressure",&pressure1,"Pressure/F");
        if(t1w->GetLeaf("IndoorTemperature"))  treeTel1->Branch("IndoorTemperature",&tin1,"IndoorTemperature/F");
        if(t1w->GetLeaf("OutdoorTemperature")) treeTel1->Branch("OutdoorTemperature",&tout1,"OutdoorTemperature/F");
        if(t1w->GetLeaf("Seconds")) treeTel1->Branch("TimeWeatherUpdate",&tweather1,"TimeWeatherUpdate/D");

        TTree *treeTel2 = new TTree("treeTel2", "run information of telescope 2");
        treeTel2->Branch("year", &year, "year/I");
        treeTel2->Branch("month", &month, "month/I");
        treeTel2->Branch("day", &day, "day/I");
        treeTel2->Branch("run", &runnumber, "run/I");
        treeTel2->Branch("timeduration",&timeduration,"timeduration/I");
        treeTel2->Branch("ratePerRun",&ratePerRunAll,"ratePerRun/F");
        treeTel2->Branch("rateHitPerRun",&ratePerRun,"rateHitPerRun/F");
        treeTel2->Branch("FractionGoodTrack",&FractionGoodTrack,"FractionGoodTrack/F");
        if(t2h->GetLeaf("nSatellites")) treeTel2->Branch("nSat",&nsat2,"nSat/F");
        if(t2h->GetLeaf("DeadChMaskBot")) treeTel2->Branch("maskB",&maskB2,"maskB/I");
        if(t2h->GetLeaf("DeadChMaskMid")) treeTel2->Branch("maskM",&maskM2,"maskM/I");
        if(t2h->GetLeaf("DeadChMaskTop")) treeTel2->Branch("maskT",&maskT2,"maskT/I");
	if(t2w->GetLeaf("Pressure"))  treeTel2->Branch("Pressure",&pressure2,"Pressure/F");
        if(t2w->GetLeaf("IndoorTemperature"))  treeTel2->Branch("IndoorTemperature",&tin2,"IndoorTemperature/F");
        if(t2w->GetLeaf("OutdoorTemperature")) treeTel2->Branch("OutdoorTemperature",&tout2,"OutdoorTemperature/F");
        if(t2w->GetLeaf("Seconds")) treeTel2->Branch("TimeWeatherUpdate",&tweather2,"TimeWeatherUpdate/D");

        TTree *treeTimeCommon = new TTree("treeTimeCommon", "time duration overlap run by run for the two telescopes");
        treeTimeCommon->Branch("year", &year, "year/I");
        treeTimeCommon->Branch("month", &month, "month/I");
        treeTimeCommon->Branch("day", &day, "day/I");
        treeTimeCommon->Branch("run", &runnumber, "run/I");
        treeTimeCommon->Branch("run2", &runnumber2, "run2/I");
        treeTimeCommon->Branch("timeduration",&timeduration,"timeduration/I");

        // Fill the infos
	Int_t n1run=0,n2run=0;
        for(Int_t i=1;i<=500;i++){
            if(htimePerRun1->GetBinContent(i) > 0){
	        t1h->GetEvent(n1run);
	        t1w->GetEvent(n1run);
		n1run++;
                runnumber = i-1;
                timeduration = htimePerRun1->GetBinContent(i);
                ratePerRun = hEventPerRun1->GetBinContent(i);
                ratePerRunAll = hAllPerRun1->GetBinContent(i);
                FractionGoodTrack = hGoodTrackPerRun1->GetBinContent(i);
		tweather1 -= t1h->GetLeaf("RunStart")->GetValue();;
                treeTel1->Fill();
            }
            if(htimePerRun2->GetBinContent(i) > 0){
	        t2h->GetEvent(n2run);
	        t2w->GetEvent(n2run);
		n2run++;
                runnumber = i-1;
                timeduration = htimePerRun2->GetBinContent(i);
                ratePerRun = hEventPerRun2->GetBinContent(i);
                ratePerRunAll = hAllPerRun2->GetBinContent(i);
                FractionGoodTrack = hGoodTrackPerRun2->GetBinContent(i);
   		tweather2 -= t2h->GetLeaf("RunStart")->GetValue();;
		treeTel2->Fill();
            }
        }

        Int_t noverlap[500][500];
        for(Int_t is=0;is < 500;is++)
          for(Int_t js=0;js < 500;js++)
            noverlap[is][js]=0;


        // count overlapping seconds
        for(Int_t is=1;is<100000;is++){
          if(hexposure1->GetBinContent(is) > 0 && hexposure2->GetBinContent(is) > 0)
            noverlap[Int_t(hexposure1->GetBinContent(is))][Int_t(hexposure2->GetBinContent(is))]++;
        }



        for(Int_t i=1;i<500;i++){
              for(Int_t j=1;j<500;j++){
                if(noverlap[i][j]){
                  runnumber = i;
                  runnumber2 = j;
                  timeduration = noverlap[i][j];
                  treeTimeCommon->Fill();
                }      
              }
        }
	*/   

	TTree *treeout = new TTree("tree", "Delta T");
	Int_t e1, e2;	
	Float_t ThetaRel;
        treeout->Branch("year", &year, "year/I");
        treeout->Branch("month", &month, "month/I");
        treeout->Branch("day", &day, "day/I");
	treeout->Branch("ctime1", &ctime12, "ctime1/D");
	treeout->Branch("ctime2", &ctime2, "ctime1/D");
	treeout->Branch("ChiSquare1", ChiSquare1, "ChiSquare1/F");
	treeout->Branch("ChiSquare2", ChiSquare2, "ChiSquare2/F");
	treeout->Branch("TimeOfFlight1", TimeOfFlight1, "TimeOfFlight1/F");
	treeout->Branch("TimeOfFlight2", TimeOfFlight2, "TimeOfFlight2/F");
	treeout->Branch("TrackLength1", TrackLength1, "TrackLength1/F");
	treeout->Branch("TrackLength2", TrackLength2, "TrackLength2/F");
	treeout->Branch("Theta1", &Theta1, "Theta1/F");
	treeout->Branch("Theta2", &Theta2, "Theta2/F");
	treeout->Branch("Phi1", &Phi1, "Phi1/F");
	treeout->Branch("Phi2", &Phi2, "Phi2/F");
        treeout->Branch("RunNumber1",&RunNumber1,"RunNumber1/I");
        treeout->Branch("RunNumber2",&RunNumber2,"RunNumber2/I");
        treeout->Branch("EventNumber1",&EventNumber1,"EventNumber1/I");
        treeout->Branch("EventNumber2",&EventNumber2,"EventNumber2/I");
        if(t1->GetLeaf("Ntracks1")) treeout->Branch("Ntracks1", &ntracks1,"Ntracks1/I");
        if(t1->GetLeaf("Ntracks2")) treeout->Branch("Ntracks2", &ntracks2,"Ntracks2/I");
 	treeout->Branch("Nsatellite1", &nsat1,"Nsatellite1/I");
 	treeout->Branch("Nsatellite2", &nsat2,"Nsatellite2/I");

	treeout->Branch("ctime3", &ctime34, "ctime3/D");
	treeout->Branch("ctime4", &ctime4, "ctime4/D");
	treeout->Branch("ChiSquare3", ChiSquare3, "ChiSquare3/F");
	treeout->Branch("ChiSquare4", ChiSquare4, "ChiSquare4/F");
	treeout->Branch("TimeOfFlight3", TimeOfFlight3, "TimeOfFlight3/F");
	treeout->Branch("TimeOfFlight4", TimeOfFlight4, "TimeOfFlight4/F");
	treeout->Branch("TrackLength3", TrackLength3, "TrackLength3/F");
	treeout->Branch("TrackLength4", TrackLength4, "TrackLength4/F");
	treeout->Branch("Theta3", &Theta3, "Theta3/F");
	treeout->Branch("Theta4", &Theta4, "Theta4/F");
	treeout->Branch("Phi3", &Phi3, "Phi3/F");
	treeout->Branch("Phi4", &Phi4, "Phi4/F");
        treeout->Branch("RunNumber3",&RunNumber3,"RunNumber3/I");
        treeout->Branch("RunNumber4",&RunNumber4,"RunNumber4/I");
        treeout->Branch("EventNumber3",&EventNumber3,"EventNumber3/I");
        treeout->Branch("EventNumber4",&EventNumber4,"EventNumber4/I");
        if(t2->GetLeaf("Ntracks1")) treeout->Branch("Ntracks3", &ntracks3,"Ntracks3/I");
        if(t2->GetLeaf("Ntracks2")) treeout->Branch("Ntracks4", &ntracks4,"Ntracks4/I");
 	treeout->Branch("Nsatellite3", &nsat3,"Nsatellite3/I");
 	treeout->Branch("Nsatellite4", &nsat4,"Nsatellite4/I");

	treeout->Branch("DiffTime13", &DiffTime13, "DiffTime13/D");


	for(e1 = 0; e1 < nent12; e1++) {
		if (!(e1 % 10000)) cout << "\rCorrelating entry #" << e1 << flush;
		t1->GetEntry(e1);

		// Calculate Theta1, Phi1
		cellIndex = (Int_t)((ctime12 - tmin) / DiffCut);
		for (Int_t i = cellIndex - 1; i <= cellIndex + 1; i++) {
			if (i < 0 || i >= ncells) continue;
			for (Int_t j = 0; j < cell[i].GetSize(); j++) {
				size = cell[cellIndex].GetSize();
				e2 = cell[i].At(j);
				t2->GetEntry(e2); 

				DiffTime13= ctime12 - ctime34;    

				if(TMath::Abs(DiffTime13) <= corrWindow) treeout->Fill();
			}
		}
	}
//
// Closing files
//
	cout << endl;
	fileout->cd();
	treeout->Write();
	fileout->Close();
	cout<<"Correlation tree completed"<<endl;
	
}

// Function to calculate Theta and Phi angles
void CalculateThetaPhi(Float_t &cx, Float_t &cy, Float_t &cz, Float_t &teta, Float_t &phi)
{

	teta=TMath::ACos(cz)*TMath::RadToDeg();
	if(cx==0 & cy>0)  phi=90.0;
	if(cx==0 & cy<0)  phi=270.0;
	if(cx >0 || cx<0) phi=TMath::ATan(cy/cx)*TMath::RadToDeg();
	if(cx<0)          phi=phi+180.0;
	if(cx>0 & cy<0)   phi=360.0+phi;

	Double_t deltaphi=gRandom->Gaus(0.0,4.0);
	phi=phi+deltaphi;
	if(phi<0.)   phi=360+phi;
	if(phi>360.) phi=phi-360.0;

}

bool CfrString(const char *str1,const char *str2){
  int n=0;

  while(str1[n] == str2[n] && str1[n] != '\0'  && str2[n] != '\0' && n < 100){
    n++;
  }

  return (str1[n] == str2[n]);
}

