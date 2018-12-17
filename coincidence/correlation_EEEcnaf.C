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
void correlation_EEE(const char *mydata=NULL,const char *mysc1=NULL,const char *mysc2=NULL,const char *mypath=NULL,bool kNoConfigFile=kFALSE,Double_t delay1=0,Double_t delay2=0,Double_t DiffCut = 0.1,Double_t corrWindow=1E-4,Bool_t isMC=kFALSE);

int main(int argc,char *argv[]){

  printf("DEBUG\n");

  char *date = NULL;
  char *sc1 = NULL;
  char *sc2 = NULL;
  char *path = NULL;

  Double_t delay1=0;
  Double_t delay2=0;
  Double_t cell=0.1;
  Double_t window=1E-4;

  Bool_t isMC = kFALSE;

  printf("list of options to overwrite the config file infos:\n");
  printf("-d DATE = to pass the date from line command\n");
  printf("-s SCHOOL_1 SCHOOL_2 = to pass the schools from line command\n");
  printf("-p PATH = to pass the path of the reco dirs\n");
  printf("\nOptional features:\n");
  printf("-cell cell_time_window = cell time window in seconds (default = 0.1)\n");
  printf("-window corr_time_window = correlation time window to define output in seconds (default = 1E-4)\n");
  printf("-delay1 time_delay = time delay of telescope1 in seconds\n");
  printf("-delay2 time_delay = time delay of telescope2 in seconds\n");
  printf("-mc = for mc simulations\n");

  int kNoConfigFile = 0;

  for(Int_t i=1;i < argc;i++){
    if(CfrString(argv[i],"-d")){
      if(i+1 > argc){
        printf("date is missing\n");
	return 1;
      }
       
      date = argv[i+1];
      i++;
      kNoConfigFile++;
    }
    if(CfrString(argv[i],"-s")){                       
      if(i+2 > argc){
        printf("Two schools name have to be provided\n");
        return 2;
      }
 
      sc1 = argv[i+1];
      sc2 = argv[i+2];

      i+=2;
      kNoConfigFile++;
   }
   if(CfrString(argv[i],"-p")){
      if(i+1 > argc){
        printf("path is missing\n");
        return 1;
      }

      path = argv[i+1];
      i++;  
      kNoConfigFile++;
    }

   if(CfrString(argv[i],"-delay1")){
      if(i+1 > argc){
        printf("time delay (tel1) missing\n");
        return 1;
      }
      sscanf(argv[i+1],"%lf",&delay1);
      printf("Add a time delay for telescope1 of %f s\n",delay1);
      i++;  
   }
   
   if(CfrString(argv[i],"-delay2")){
     if(i+1 > argc){
        printf("time delay (tel2) missing\n");
        return 1;
      }
      sscanf(argv[i+1],"%lf",&delay2);
      printf("Add a time delay for telescope2 of %f s\n",delay2);

      i++;  
    }

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

   if(CfrString(argv[i],"-mc")){
     isMC=kTRUE;
   }

  }

  correlation_EEE(date,sc1,sc2,path,kNoConfigFile==3,delay1,delay2,cell,window,isMC);

  return 0;
}


void correlation_EEE(const char *mydata,const char *mysc1,const char *mysc2,const char *mypath,bool kNoConfigFile,Double_t extdelay1,Double_t extdelay2,Double_t DiffCut,Double_t corrWindow,Bool_t isMC)
{

  Double_t delay1  =extdelay1;
  Double_t delay2  =extdelay2;


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


        const char *tel_code1;
	const char *tel_code2;
	const char *date;
	const char *path;

        if(! kNoConfigFile){
	  ifstream config;
	  config.open("./config_correlation_EEE.txt", ios::in);
	  //Check the existence of the config file
	  if(!config.is_open()){
	    cout << "Please check the config file (config_correlation_EEE.txt)!" << endl;
	    return;
	  }
	  
	  TString tmp1, tmp2, tmp3, tmp4;
	  config >> tmp1; // Read the first line of the config file (telescope code 1)
	  tel_code1 = new char[tmp1.Length() + 1];
	  if(! mysc1) tel_code1 = tmp1.Data();
	  else tel_code1 = mysc1;
	  config >> tmp2; // Read the first line of the config file (telescope code 2)
	  tel_code2 = new char[tmp2.Length() + 1];
	  if(! mysc2) tel_code2 = tmp2.Data();
	  else tel_code2 = mysc2;
	  config >> tmp3; // Read the second line of the config file (date)
	  date = new char[tmp3.Length() + 1];
	  if(! mydata) date = tmp3.Data();
	  else date=mydata;
	  config >> tmp4; // Read the third line of the config file (data path)
	  path = new char[tmp4.Length() + 1];
	  if(! mypath) path = tmp4.Data();
	  else path=mypath;
	}
        else{
          tel_code1 = mysc1,tel_code2 = mysc2,date=mydata,path=mypath;  
        }

// select date
	Int_t year,month,day;
        sscanf(date,"%d-%d-%d",&year,&month,&day);


//
// Input files
//
        system(Form("ls %s/%s/%s/*.root >lista%s%s_1",path,tel_code1,date,tel_code1,date));
        system(Form("ls %s/%s/%s/*.root >lista%s%s_2",path,tel_code2,date,tel_code2,date));

        TChain *t1 = new TChain("Events");
        TChain *t1h = new TChain("Header");
        TChain *t1g = new TChain("gps");
        TChain *t1w = new TChain("Weather");
        FILE *f1 = fopen(Form("lista%s%s_1",tel_code1,date),"r");
        char filename[300];
        while(fscanf(f1,"%s",filename)==1){
	  t1->Add(filename);
	  t1h->Add(filename);
	  t1g->Add(filename);
	  t1w->Add(filename);
	}
        fclose(f1);

        TChain *t2 = new TChain("Events");
        TChain *t2h = new TChain("Header");
	TChain *t2g = new TChain("gps");
	TChain *t2w = new TChain("Weather");
	FILE *f2 = fopen(Form("lista%s%s_2",tel_code2,date),"r");
        while(fscanf(f2,"%s",filename)==1){
	  t2->Add(filename);
	  t2h->Add(filename);
	  t2g->Add(filename);
	  t2w->Add(filename);
	}
        fclose(f2);

//
// Define input tree structure	
//
	UInt_t RunNumber1, RunNumber2;
	UInt_t EventNumber1, EventNumber2;
	UInt_t StatusCode1, StatusCode2;
	UInt_t Seconds1, Seconds2;
	ULong64_t Nanoseconds1, Nanoseconds2;
//	ULong64_t UniqueRunId1, UniqueRunId2;
        Int_t ntracks1,ntracks2;
	Float_t nsat1,nsat2;
	Int_t nsat1gps=0,nsat2gps=0;
	Int_t maskT1,maskM1,maskB1;
	Int_t maskT2,maskM2,maskB2;
	Float_t pressure1,tin1,tout1;
	Float_t pressure2,tin2,tout2;

	// track by track info
	Float_t XDir1[24], YDir1[24], ZDir1[24];
	Float_t XDir2[24], YDir2[24], ZDir2[24];
	Float_t ChiSquare1[24], ChiSquare2[24];
	Float_t TimeOfFlight1[24], TimeOfFlight2[24];
	Float_t TrackLength1[24], TrackLength2[24];
	Float_t DeltaTime1[24], DeltaTime2[24];
	Double_t tweather1,tweather2;

        if(t1h->GetLeaf("nSatellites")) t1h->SetBranchAddress("nSatellites", &nsat1);
        if(t2h->GetLeaf("nSatellites")) t2h->SetBranchAddress("nSatellites", &nsat2);
        if(t1h->GetLeaf("DeadChMaskBot")) t1h->SetBranchAddress("DeadChMaskBot", &maskB1);
        if(t2h->GetLeaf("DeadChMaskBot")) t2h->SetBranchAddress("DeadChMaskBot", &maskB2);
        if(t1h->GetLeaf("DeadChMaskMid")) t1h->SetBranchAddress("DeadChMaskMid", &maskM1);
        if(t2h->GetLeaf("DeadChMaskMid")) t2h->SetBranchAddress("DeadChMaskMid", &maskM2);
        if(t1h->GetLeaf("DeadChMaskTop")) t1h->SetBranchAddress("DeadChMaskTop", &maskT1);
        if(t2h->GetLeaf("DeadChMaskTop")) t2h->SetBranchAddress("DeadChMaskTop", &maskT2);

        if(t1g->GetLeaf("nSatellites")) t1g->SetBranchAddress("nSatellites", &nsat1gps);
        if(t2g->GetLeaf("nSatellites")) t2g->SetBranchAddress("nSatellites", &nsat2gps);

        if(t1w->GetLeaf("Pressure")) t1w->SetBranchAddress("Pressure", &pressure1);
        if(t1w->GetLeaf("IndoorTemperature")) t1w->SetBranchAddress("IndoorTemperature", &tin1);
        if(t1w->GetLeaf("OutdoorTemperature")) t1w->SetBranchAddress("OutdoorTemperature", &tout1);
        if(t1w->GetLeaf("Seconds")) t1w->SetBranchAddress("Seconds", &tweather1);
        if(t2w->GetLeaf("Pressure")) t2w->SetBranchAddress("Pressure", &pressure2);
        if(t2w->GetLeaf("IndoorTemperature")) t2w->SetBranchAddress("IndoorTemperature", &tin2);
        if(t2w->GetLeaf("OutdoorTemperature")) t2w->SetBranchAddress("OutdoorTemperature", &tout2);
        if(t2w->GetLeaf("Seconds")) t2w->SetBranchAddress("Seconds", &tweather2);


	t1->SetBranchAddress("RunNumber",&RunNumber1);
	t1->SetBranchAddress("EventNumber",&EventNumber1);
	t1->SetBranchAddress("StatusCode",&StatusCode1);
	t1->SetBranchAddress("Seconds",&Seconds1);
	t1->SetBranchAddress("Nanoseconds",&Nanoseconds1);
        if(t1->GetLeaf("Ntracks")) t1->SetBranchAddress("Ntracks", &ntracks1);
	t1->SetBranchAddress("XDir",XDir1);
	t1->SetBranchAddress("YDir",YDir1);
	t1->SetBranchAddress("ZDir",ZDir1);
	t1->SetBranchAddress("ChiSquare", ChiSquare1);
	t1->SetBranchAddress("TimeOfFlight", TimeOfFlight1);
	t1->SetBranchAddress("TrackLength", TrackLength1);
	t1->SetBranchAddress("DeltaTime", DeltaTime1);
//	t1->SetBranchAddress("UniqueRunId", &UniqueRunId1);
	t2->SetBranchAddress("RunNumber",&RunNumber2);
	t2->SetBranchAddress("EventNumber",&EventNumber2);
	t2->SetBranchAddress("StatusCode",&StatusCode2);
	t2->SetBranchAddress("Seconds",&Seconds2);
	t2->SetBranchAddress("Nanoseconds",&Nanoseconds2);
//	t2->SetBranchAddress("UniqueRunId", &UniqueRunId2);
        if(t2->GetLeaf("Ntracks")) t2->SetBranchAddress("Ntracks", &ntracks2);
	t2->SetBranchAddress("XDir",XDir2);
	t2->SetBranchAddress("YDir",YDir2);
	t2->SetBranchAddress("ZDir",ZDir2);
	t2->SetBranchAddress("ChiSquare", ChiSquare2);
	t2->SetBranchAddress("TimeOfFlight", TimeOfFlight2);
	t2->SetBranchAddress("TrackLength", TrackLength2);
	t2->SetBranchAddress("DeltaTime", DeltaTime2);
//	t2->SetBranchAddress("UniqueRunId", &UniqueRunId2);


	Int_t nent1 = t1->GetEntries();
	Int_t nent2 = t2->GetEntries();

	Double_t ctime1, ctime2;
	Float_t Theta1,Phi1,Theta2,Phi2;

        // replace delay if nsat==0 
        Bool_t kreplacedelay=kTRUE;
        for(Int_t i=0;i<t1h->GetEntries();i++){
          t1h->GetEvent(i);
          if(t1h->GetLeaf("nSatellites")->GetValue() > 0) kreplacedelay=kFALSE;
        }
        if(kreplacedelay && !isMC){ 
           extdelay1=17;
           delay1=17;
           printf("Delay 1 replace to 17 s\n");
        }

        kreplacedelay=kTRUE;
        for(Int_t i=0;i<t2h->GetEntries();i++){
          t2h->GetEvent(i);
          if(t2h->GetLeaf("nSatellites")->GetValue() > 0) kreplacedelay=kFALSE;
        }
        if(kreplacedelay && !isMC){
           extdelay2=17;
           delay2=17;
           printf("Delay 2 replace to 17 s\n");
        }

//      
// Find time range
//

	printf("Find time range\n");

        Double_t startTime;

	Double_t t1min, t1max, t2min, t2max, range1, range2;
        Int_t i1 = 0;StatusCode1=1;
        while(StatusCode1) {t1->GetEntry(        i1); ctime1 = (Double_t ) Seconds1 - delay1 + (Double_t ) Nanoseconds1*1E-09; t1min = ctime1;i1++;}
        cout << "start " << Seconds1 - delay1 << endl;        

        startTime = Seconds1-delay1;

        i1 = nent1 - 5; StatusCode1=1;	
        while(StatusCode1) {t1->GetEntry( i1); ctime1 = (Double_t ) Seconds1 - delay1 + (Double_t ) Nanoseconds1*1E-09; t1max = ctime1;i1--;}
        cout << "end " << Seconds1 -delay1 << " " << StatusCode1 << endl;
        Int_t i2 = 0; StatusCode2=1;
	while(StatusCode2) {t2->GetEntry(        i2); ctime2 = (Double_t ) Seconds2 - delay2 + (Double_t ) Nanoseconds2*1E-09; t2min = ctime2;i2++;}
        cout << "start " << Seconds2 - delay2 << endl;
        if(startTime > Seconds2-delay2) startTime = Seconds2-delay2;
        i2 = nent2 - 1; StatusCode2=1;
	while(StatusCode2) {t2->GetEntry(i2); ctime2 = (Double_t ) Seconds2 -delay2 + (Double_t ) Nanoseconds2*1E-09; t2max = ctime2;i2--;}
        cout << "end " << Seconds2 -delay2<< endl;

	range1 = t1max - t1min;
	range2 = t2max - t2min;
	cout.setf(ios::fixed);
	cout <<"N.entry1 = "<< nent1<<"   N.entry2 = "<<nent2 << endl;
	cout << "Time range file 1: " << t1min << " --> " << t1max << ", range = " << range1 << endl;
	cout << "Time range file 2: " << t2min << " --> " << t2max << ", range = " << range2 << endl;
	Double_t tmin = TMath::Min(t1min, t2min);
	Double_t tmax = TMath::Max(t1max, t2max);
	cout << "Common measure time interval = "<<(TMath::Min(t1max, t2max)-TMath::Max(t1min, t2min))<< " s"<<endl;

        if((TMath::Min(t1max, t2max)-TMath::Max(t1min, t2min)) < 0) return;

// collect info on run duration and rate
       for(Int_t e1 = 0; e1 < nent1; e1++) {
                t1->GetEntry(e1);
                RunNumber1 = RunNumber1%500;

                hexposure1->SetBinContent(hexposure1->FindBin(Seconds1-delay1-startTime),RunNumber1);

                hAllPerRun1->Fill(RunNumber1);
                if(StatusCode1==0){
                    hEventPerRun1->Fill(RunNumber1); 
                    if(ChiSquare1[0] < 10) hGoodTrackPerRun1->Fill(RunNumber1);
                }
       }
       hGoodTrackPerRun1->Divide(hEventPerRun1);

       for(Int_t e2 = 0; e2 < nent2; e2++) {
                t2->GetEntry(e2);
                RunNumber2 = RunNumber2%500;
                hexposure2->SetBinContent(hexposure2->FindBin(Seconds2-delay2-startTime),RunNumber2);

                hAllPerRun2->Fill(RunNumber2);
                if(StatusCode2==0){
                    hEventPerRun2->Fill(RunNumber2);    
                    if(ChiSquare2[0] < 10) hGoodTrackPerRun2->Fill(RunNumber2);
                }
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


	printf("Define cells\n");

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
	for (Int_t i = 0; i < nent2; i++) {
		t2->GetEntry(i);
		ctime2 = (Double_t ) Seconds2 -delay2+ (Double_t ) Nanoseconds2*1E-09;
		cellIndex = (Int_t)((ctime2 - tmin) / DiffCut);
		if (cellIndex >= 0 && cellIndex < ncells && StatusCode2==0) {
			size = cell[cellIndex].GetSize();
			cell[cellIndex].Set(size+1);
			cell[cellIndex][size] = i;
		}
	}
//	
// Define output correlation tree
//
	TFile *fileout = new TFile(Form("%s/%s-%s-%s.root",".",tel_code1,tel_code2,date), "RECREATE");
        fileout->ls();

	// fill tree with quality check per run
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

	TTree *treeout = new TTree("tree", "Delta T");
	Int_t e1, e2;	
	Double_t DiffTime;
	Float_t ThetaRel;
	Float_t Xcore,Ycore,Energy1,Energy2;
	if(isMC){
	  t1->SetBranchAddress("xCore",&Xcore);
	  t1->SetBranchAddress("yCore",&Ycore);
	  t1->SetBranchAddress("Energy",&Energy1);
	  t2->SetBranchAddress("Energy",&Energy2);

	  treeout->Branch("xCore", &Xcore, "xCore/F");
	  treeout->Branch("yCore", &Ycore, "yCore/F");
	  treeout->Branch("Energy1", &Energy1, "Energy1/F");
	  treeout->Branch("Energy2", &Energy2, "Energy2/F");
	}
        treeout->Branch("year", &year, "year/I");
        treeout->Branch("month", &month, "month/I");
        treeout->Branch("day", &day, "day/I");
	treeout->Branch("ctime1", &ctime1, "ctime1/D");
	treeout->Branch("ChiSquare1", ChiSquare1, "ChiSquare1/F");
	treeout->Branch("TimeOfFlight1", TimeOfFlight1, "TimeOfFlight1/F");
	treeout->Branch("TrackLength1", TrackLength1, "TrackLength1/F");
	treeout->Branch("Theta1", &Theta1, "Theta1/F");
	treeout->Branch("Phi1", &Phi1, "Phi1/F");
        treeout->Branch("RunNumber1",&RunNumber1,"RunNumber1/I");
        treeout->Branch("EventNumber1",&EventNumber1,"EventNumber1/I");
        if(t1->GetLeaf("Ntracks")) treeout->Branch("Ntracks1", &ntracks1,"Ntracks1/I");
 	treeout->Branch("Nsatellite1", &nsat1gps,"Nsatellite1/I");
	treeout->Branch("ctime2", &ctime2, "ctime2/D");
	treeout->Branch("ChiSquare2", ChiSquare2, "ChiSquare2/F");
	treeout->Branch("TimeOfFlight2", TimeOfFlight2, "TimeOfFlight2/F");
	treeout->Branch("TrackLength2", TrackLength2, "TrackLength2/F");
	treeout->Branch("Theta2", &Theta2, "Theta2/F");
	treeout->Branch("Phi2", &Phi2, "Phi2/F");
        treeout->Branch("RunNumber2",&RunNumber2,"RunNumber2/I");
        treeout->Branch("EventNumber2",&EventNumber2,"EventNumber2/I");
        if(t2->GetLeaf("Ntracks")) treeout->Branch("Ntracks2", &ntracks2,"Ntracks2/I");
 	treeout->Branch("Nsatellite2", &nsat2gps,"Nsatellite2/I");

	treeout->Branch("DiffTime", &DiffTime, "DiffTime/D");
	treeout->Branch("ThetaRel", &ThetaRel, "ThetaRel/F");


	for(e1 = 0; e1 < nent1; e1++) {
		if (!(e1 % 10000)) cout << "\rCorrelating entry #" << e1 << flush;
		t1->GetEntry(e1);
	        RunNumber1 = RunNumber1%500;

                if(StatusCode1) continue;

		if(t1->GetLeaf("GpsID")) t1g->GetEntry(t1->GetLeaf("GpsID")->GetValue());

		// Calculate Theta1, Phi1
		CalculateThetaPhi(XDir1[0], YDir1[0], ZDir1[0], Theta1, Phi1);

                // check if hightech gps and add 17 seconds
                if(nsat1==0 && extdelay1==0 && !isMC){
                    //printf("school 1 hightech gps -> add 17 seconds\n");
                    delay1=17;
                }
                else delay1=extdelay1;  

		ctime1 = (Double_t ) Seconds1 -delay1+ (Double_t ) Nanoseconds1*1E-09;
		cellIndex = (Int_t)((ctime1 - tmin) / DiffCut);
		for (Int_t i = cellIndex - 1; i <= cellIndex + 1; i++) {
			if (i < 0 || i >= ncells) continue;
			for (Int_t j = 0; j < cell[i].GetSize(); j++) {
				size = cell[cellIndex].GetSize();
				e2 = cell[i].At(j);
				t2->GetEntry(e2); 
                                RunNumber2 = RunNumber2%500;

				if(StatusCode2) continue;

				if(t2->GetLeaf("GpsID")) t2g->GetEntry(t2->GetLeaf("GpsID")->GetValue());

                                // check if hightech gps and add 17 seconds
                                if(nsat2==0 && extdelay2==0 && !isMC){
                                    //printf("school 2 hightech gps -> add 17 seconds\n");
                                    delay2=17;
                                }
                                else delay2=extdelay2;

				ctime2 = (Double_t ) Seconds2 - delay2 + (Double_t ) Nanoseconds2*1E-09; 
				//DiffTime= ctime1 - ctime2;    
				if((Seconds1-delay1-Seconds2+delay2)==0) DiffTime= (Double_t ) Nanoseconds1 - (Double_t ) Nanoseconds2;
				else DiffTime=((Double_t ) Seconds1 -delay1 - (Double_t ) Seconds2 + delay2)*1E9 + ((Double_t ) Nanoseconds1 - (Double_t ) Nanoseconds2); 
				// Calculate Theta2, Phi2
				CalculateThetaPhi(XDir2[0], YDir2[0], ZDir2[0], Theta2, Phi2);
				ThetaRel=TMath::ACos(TMath::Cos(Theta1*TMath::DegToRad())*TMath::Cos(Theta2*TMath::DegToRad())+TMath::Sin(Theta1*TMath::DegToRad())*TMath::Sin(Theta2*TMath::DegToRad())*TMath::Cos(Phi2*TMath::DegToRad()-Phi1*TMath::DegToRad()))/TMath::DegToRad();

                                if(StatusCode1) ChiSquare1[0] = 1000;
                                if(StatusCode2) ChiSquare2[0] = 1000;

				if(TMath::Abs(DiffTime) <= corrWindow && StatusCode1 == 0 && StatusCode2 == 0) treeout->Fill();
			}
		}
	}
//
// Closing files
//
	cout << endl;
	fileout->cd();
	treeout->Write();
        treeTel1->Write();
        treeTel2->Write();
        treeTimeCommon->Write();
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

