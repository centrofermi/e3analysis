
//****************************************************************************************************
//
// e3COrrelation.C
//
// Created by F.Pilo on May 2015
//
//****************************************************************************************************

#include <algorithm>

#include <TFile.h>
#include <TTree.h>
#include <Riostream.h>
#include <TMath.h>
#include <TString.h>
#include <TArrayI.h>
#include <TChain.h>
#include <TRandom.h>
#include <TH1.h>
#include <TStopwatch.h>

#include "e3Correlation.h"
#include "e3RunDbConn.h"

using namespace std;

// correlation_EEE function
//------------------------------------------------------------
void correlation_EEE(const char *mydata,const char *mysc1,const char *mysc2,const char *mypath,bool usedb,Double_t DiffCut)
{

  int _status=0;        ///< Process status
  int _ret=0;         
  int _vLevel=0;      ///< Verbosity level

  TStopwatch _timer;
  _timer.Start();

  const char *tel_code1;
  const char *tel_code2;
  tel_code1 = mysc1,tel_code2 = mysc2;
  const char *path=mypath;

  e3RunDbConn *_mysqlCon = NULL;

  TChain *t1 = new TChain("Events");
  TChain *t2 = new TChain("Events");
    
  Double_t ctime1, ctime2;
  Float_t Theta1,Phi1,Theta2,Phi2;

  //======================================== 
  // Init TH1 histograms
  //========================================  
  
  TH1I *hexposure1 = new TH1I("hexposure1","Active run number in a given second;second - time_first_event;Run number telescope 1",100000,0,100000);
  TH1I *hexposure2 = new TH1I("hexposure2","Active run number in a given second;second - time_first_event;Run number telescope 2",100000,0,100000);

  TH1I *hUniqueRunIdPerRun1 = new TH1I("hUniqueRunIdPerRun1","UniqueRunId of the run for telescope 1;Run number;UniqueRunId",500,0,500);
  TH1I *hUniqueRunIdPerRun2 = new TH1I("hUniqueRunIdPerRun2","UniqueRunId of the run for telescope 2;Run number;UniqueRunId",500,0,500);

  TH1I *htimePerRun1 = new TH1I("htimePerRun1","time duration of the run for telescope 1;Run number;Duration (s)",500,0,500);
  TH1I *htimePerRun2 = new TH1I("htimePerRun2","time duration of the run for telescope 2;Run number;Duration (s)",500,0,500);

  TH1F *hAllPerRun1 = new TH1F("hAllPerRun1","Rate of all events per Run telescope 1;Run number;Rate of events (Hz)",500,0,500);
  TH1F *hAllPerRun2 = new TH1F("hAllPerRun2","Rate of all events per Run telescope 2;Run number;Rate of events (Hz)",500,0,500);

  TH1F *hEventPerRun1 = new TH1F("hEventPerRun1","Rate of events with hits per Run telescope 1;Run number;Rate of events (Hz)",500,0,500);
  TH1F *hEventPerRun2 = new TH1F("hEventPerRun2","Rate of events with hits per Run telescope 2;Run number;Rate of events (Hz)",500,0,500);

  TH1F *hGoodTrackPerRun1 = new TH1F("hGoodTrackPerRun1","Fraction of good tracks (#Chi^{2} < 10) per Run telescope 1;Run number;Fraction of Good Tracks",500,0,500);
  TH1F *hGoodTrackPerRun2 = new TH1F("hGoodTrackPerRun2","Fraction of good tracks (#Chi^{2} < 10) per Run telescope 2;Run number;Fraction of Good Tracks",500,0,500);

  if(usedb){

    //======================================== 
    // Init db connection
    //========================================  
    
    _mysqlCon = new e3RunDbConn("131.154.96.193","eee","eee-monitoring","eee_rundb2");
    _mysqlCon->SetVerbosity(_vLevel); //VLEVEL
    _mysqlCon->Init();

    //======================================== 
    // Get run list
    //========================================  
    
    string STATIONID;
    string _dtWinLow, _dtWinUp;
    istringstream _iss(mydata);
    vector<string> _fileNameList;

    getline(_iss, _dtWinLow, '/');
    getline(_iss, _dtWinUp);
    if(_dtWinUp.empty()){
      
      _dtWinLow = _dtWinLow.substr(0,_dtWinLow.find("_"));
      cout<<"[e3Correlation.correlation_EEE - WARNING] ALL the runs collected on "<<_dtWinLow<<" will be analysed"<<endl;
      _dtWinLow.append("_00:00:00");
      _dtWinUp = _dtWinLow;
      int day = atoi(_dtWinLow.substr(_dtWinLow.find("_")-2,2).c_str());
      stringstream  buffer; buffer.fill('0'); buffer<<setw(2)<<(day+1);
      _dtWinUp.replace(_dtWinUp.find("_")-2,2,buffer.str().c_str());
    }

    _fileNameList.clear(); STATIONID=mysc1;
    if(_mysqlCon->GetRunList(_fileNameList,STATIONID,_dtWinLow,_dtWinUp,1)!=0){
      exit(EXIT_FAILURE);
    }
    else{
      if(_vLevel>5){
	/* Print _fileNameList vector to console */
	cout<<"[e3Correlation.correlation_EEE - INFO] Query results:"<<endl;
	copy(_fileNameList.begin(), _fileNameList.end(), ostream_iterator<string>(cout, "\n"));
      }
      for (vector<string>::iterator it = _fileNameList.begin(); it!=_fileNameList.end(); ++it)
	t1->Add((*it).c_str());
    }

    _fileNameList.clear(); STATIONID=mysc2;
    if(_mysqlCon->GetRunList(_fileNameList,STATIONID,_dtWinLow,_dtWinUp,1)!=0){
      exit(EXIT_FAILURE);
    }
    else{
      if(_vLevel>5){
	/* Print _fileNameList vector to console */
	cout<<"[e3Correlation.correlation_EEE - INFO] Query results:"<<endl;
	copy(_fileNameList.begin(), _fileNameList.end(), ostream_iterator<string>(cout, "\n"));
      }
      for (vector<string>::iterator it = _fileNameList.begin(); it!=_fileNameList.end(); ++it)
	t2->Add((*it).c_str());
    }
    
    //======================================== 
    // Close db connection
    //========================================  
    
    _mysqlCon->CloseConn();

  }
  else{

    char *date;
    
    //========================================  
    // Select date
    //========================================  

    char *search="_";
    date=strsep((char**)&mydata,search);
    cout<<"[e3Correlation.correlation_EEE - WARNING] Using <file_list> option ALL the runs collected on "<<date
	<<" will be analysed."<<endl; 
        
    // Int_t year,month,day;
    // sscanf(date,"%d-%d-%d",&year,&month,&day);

    //========================================  
    // Create run lists
    //========================================  

    char filename[300], *listname;
    
    _ret = system(Form("ls %s/%s/%s/*.root >./tmp/lista_%s_%s",path,tel_code1,date,tel_code1,date));
    if(_ret!=0){
      cerr<<"[e3Correlation.correlation_EEE - ERROR] Unable to execute system command. Returned error code "<<_ret<<endl;
      exit(EXIT_FAILURE);
    }

    _ret = system(Form("ls %s/%s/%s/*.root >./tmp/lista_%s_%s",path,tel_code2,date,tel_code2,date));
    if(_ret!=0){
      cerr<<"[e3Correlation.correlation_EEE - ERROR] Unable to execute system command. Returned error code "<<_ret<<endl;
      exit(EXIT_FAILURE);
    }

    //========================================  
    // Populate TChains and close list files
    //========================================  

    listname = Form("./tmp/lista_%s_%s",tel_code1,date);
    FILE *f1 = fopen(listname,"r");
    if(f1){
      while(fscanf(f1,"%s",filename)==1){ t1->Add(filename);}
      fclose(f1);
    }
    else{
      cerr<<"[e3Correlation.correlation_EEE - ERROR] Unable to open file "<<listname<<endl;
      exit(EXIT_FAILURE);
    }

    listname = Form("./tmp/lista_%s_%s",tel_code2,date);
    FILE *f2 = fopen(listname,"r");
    if(f2){
      while(fscanf(f2,"%s",filename)==1) t2->Add(filename);
      fclose(f2);
    }
    else{
      cerr<<"[e3Correlation.correlation_EEE - ERROR] Unable to open file "<<listname<<endl;
      exit(EXIT_FAILURE);
    }

  }

  _timer.Stop();
  cout << endl<<"[e3Correlation.correlation_EEE - INFO] TChains poulated - Cpu time: "
       << _timer.CpuTime()*1000
       << " - Real time: " << _timer.RealTime()*1000<< " msec."
       << endl;
  _timer.Start();
  
  //========================================  
  // Define input tree structure	
  //========================================  

  UInt_t RunNumber1, RunNumber2;
  UInt_t EventNumber1, EventNumber2;
  UInt_t StatusCode1, StatusCode2;
  UInt_t Seconds1, Seconds2;
  ULong64_t Nanoseconds1, Nanoseconds2;
  Float_t XDir1, YDir1, ZDir1;
  Float_t XDir2, YDir2, ZDir2;
  Float_t ChiSquare1, ChiSquare2;
  Float_t TimeOfFlight1, TimeOfFlight2;
  Float_t TrackLength1, TrackLength2;
  Double_t DeltaTime1, DeltaTime2;
  ULong64_t UniqueRunId1, UniqueRunId2;

  t1->SetBranchAddress("RunNumber",&RunNumber1);
  t1->SetBranchAddress("EventNumber",&EventNumber1);
  t1->SetBranchAddress("StatusCode",&StatusCode1);
  t1->SetBranchAddress("Seconds",&Seconds1);
  t1->SetBranchAddress("Nanoseconds",&Nanoseconds1);
  t1->SetBranchAddress("XDir",&XDir1);
  t1->SetBranchAddress("YDir",&YDir1);
  t1->SetBranchAddress("ZDir",&ZDir1);
  t1->SetBranchAddress("ChiSquare", &ChiSquare1);
  t1->SetBranchAddress("TimeOfFlight", &TimeOfFlight1);
  t1->SetBranchAddress("TrackLength", &TrackLength1);
  t1->SetBranchAddress("DeltaTime", &DeltaTime1);
  t1->SetBranchAddress("UniqueRunId", &UniqueRunId1);

  t2->SetBranchAddress("RunNumber",&RunNumber2);
  t2->SetBranchAddress("EventNumber",&EventNumber2);
  t2->SetBranchAddress("StatusCode",&StatusCode2);
  t2->SetBranchAddress("Seconds",&Seconds2);
  t2->SetBranchAddress("Nanoseconds",&Nanoseconds2);
  t2->SetBranchAddress("XDir",&XDir2);
  t2->SetBranchAddress("YDir",&YDir2);
  t2->SetBranchAddress("ZDir",&ZDir2);
  t2->SetBranchAddress("ChiSquare", &ChiSquare2);
  t2->SetBranchAddress("TimeOfFlight", &TimeOfFlight2);
  t2->SetBranchAddress("TrackLength", &TrackLength2);
  t2->SetBranchAddress("DeltaTime", &DeltaTime2);
  t2->SetBranchAddress("UniqueRunId", &UniqueRunId2);

  Int_t nent1 = t1->GetEntries();
  if(nent1==0){ 
    cerr<<"[e3Correlation.correlation_EEE - ERROR]  No events for telescope "<<tel_code1<<". Exiting ..."<<endl; 
    exit(EXIT_FAILURE); 
  }
  Int_t nent2 = t2->GetEntries();
  if(nent2==0){ 
    cerr<<"[e3Correlation.correlation_EEE - ERROR]  No events for telescope "<<tel_code2<<". Exiting ..."<<endl; 
    exit(EXIT_FAILURE); 
  }

  //========================================  
  // Find time range
  //========================================  

  Double_t startTime;

  Double_t t1min, t1max, t2min, t2max, range1, range2;
  struct tm e3start; char *timestr;
  e3start.tm_hour = 0; e3start.tm_min = 0; e3start.tm_sec = 0; e3start.tm_isdst = 0;
  e3start.tm_year = 107; e3start.tm_mon = 0; e3start.tm_mday = 1;
  time_t e3starttime = mktime(&e3start) - timezone; 
  time_t eventtime;
  Int_t i1 = 0;StatusCode1=1;
  while(StatusCode1) {
    t1->GetEntry(        i1); 
    ctime1 = (Double_t ) Seconds1 + (Double_t ) Nanoseconds1*1E-09;
    t1min = ctime1;
    i1++;
  }
  eventtime = e3starttime + Seconds1; 
  timestr = asctime(gmtime(&eventtime)); timestr[strlen(timestr)-1] = '\0';
  cout<<endl<<"[e3Correlation.correlation_EEE - INFO] Run list time info for "<<tel_code1<<endl;
  cout << "\tRun start " << Seconds1 << " s (" << timestr << ") - event num: " << i1 << "/" << nent1 << endl;

  startTime = Seconds1;

  i1 = nent1 - 5; StatusCode1=1;	
  while(StatusCode1) {
    t1->GetEntry( i1);
    ctime1 = (Double_t ) Seconds1 + (Double_t ) Nanoseconds1*1E-09;
    t1max = ctime1;
    i1--;
  }
  eventtime = e3starttime + Seconds1; 
  timestr = asctime(gmtime(&eventtime)); timestr[strlen(timestr)-1] = '\0';
  cout << "\tRun end " << Seconds1 << " (" << timestr << ") - event num: " << i1 << "/" << nent1  << endl;
  range1 = t1max - t1min;
  cout.setf(ios::fixed);
  cout << "\tTime interval: " << t1min << " --> " << t1max << ", DeltaT = " << range1 << " s" << endl;


  Int_t i2 = 0; StatusCode2=1;
  while(StatusCode2) {
    t2->GetEntry(        i2);
    ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09;
    t2min = ctime2;
    i2++;
  }
  eventtime = e3starttime + Seconds2; 
  timestr = asctime(gmtime(&eventtime)); timestr[strlen(timestr)-1] = '\0';
  cout<<endl<<"[e3Correlation.correlation_EEE - INFO] Run list time info for "<<tel_code2<<endl;
  cout << "\tRun start " << Seconds2 << " (" << timestr << ") - event num: " << i2 << "/" << nent2 << endl;

  if(startTime > Seconds2) startTime = Seconds2;
  i2 = nent2 - 1; StatusCode2=1;
  while(StatusCode2) {
    t2->GetEntry(i2); 
    ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09; 
    t2max = ctime2;
    i2--;
  }
  eventtime = e3starttime + Seconds2; 
  timestr = asctime(gmtime(&eventtime)); timestr[strlen(timestr)-1] = '\0';
  cout << "\tRun end " << Seconds2 << " (" << timestr << ") - event num: " << i2 << "/" << nent2 << endl;

  range2 = t2max - t2min;
  cout.setf(ios::fixed);
  cout << "\tTime interval: " << t2min << " --> " << t2max << ", DeltaT = " << range2 << " s" << endl;

  cout<<endl<<"[e3Correlation.correlation_EEE - INFO] Analysed time interval"<<endl;
  cout << "\tCommon time interval:  ";
  Double_t ctmin = TMath::Max(t1min, t2min);
  eventtime = e3starttime + ctmin; 
  timestr = asctime(gmtime(&eventtime)); timestr[strlen(timestr)-1] = '\0';
  cout << "from " << timestr;
  Double_t ctmax = TMath::Min(t1max, t2max);
  eventtime = e3starttime + ctmax; 
  timestr = asctime(gmtime(&eventtime)); timestr[strlen(timestr)-1] = '\0';
  cout << " to " << timestr;
  cout << ", DeltaT = " << (ctmax-ctmin) << " s" << endl;

  Double_t tmin = TMath::Min(t1min, t2min);
  Double_t tmax = TMath::Max(t1max, t2max);

  //========================================  
  // Collect info on runs duration and rate
  //========================================  

  ULong64_t  cUniqueRunId; Int_t cbin;
  
  cUniqueRunId=0, cbin=0;
  for(Int_t e1=0; e1<nent1; e1++) {
    t1->GetEntry(e1);
    if(UniqueRunId1!=cUniqueRunId){ 
      // cout<<cUniqueRunId<<" - "<<UniqueRunId1<<" i.e. "
      // 	  <<" STATIONID "<<(Int_t) (UniqueRunId1/1E9)<<" - "
      // 	  <<" DAYS FROM EEESTART "<<(Int_t)((UniqueRunId1-(Int_t) (UniqueRunId1/1E9)*1E9)/1E5)<<" - "
      // 	  <<" RUNID "<<(Int_t)(UniqueRunId1-(Int_t) (UniqueRunId1/1E5)*1E5)
      // 	  <<endl;
      // eventtime = e3starttime + 24*60*60*((Int_t)((UniqueRunId1-(Int_t) (UniqueRunId1/1E9)*1E9)/1E5));
      // timestr = asctime(gmtime(&eventtime)); timestr[strlen(timestr)-1] = '\0';
      // cout<<timestr<<endl;
      // cout<<gmtime(&eventtime)->tm_year+1900<<endl;
      // cout<<gmtime(&eventtime)->tm_mon+1<<endl;
      // cout<<gmtime(&eventtime)->tm_mday<<endl;

      cbin++; 
      cUniqueRunId=UniqueRunId1; 
      hUniqueRunIdPerRun1->SetBinContent(cbin,UniqueRunId1);
    }

    hexposure1->SetBinContent(hexposure1->FindBin(Seconds1-startTime),cbin);

    hAllPerRun1->Fill(cbin);
    hAllPerRun1->Fill(cbin);
    if(StatusCode1==0){
      hEventPerRun1->Fill(cbin); 
      if(ChiSquare1 < 10) hGoodTrackPerRun1->Fill(cbin);
    }

  }
  hGoodTrackPerRun1->Divide(hEventPerRun1);

  cUniqueRunId=0; cbin=0;
  for(Int_t e2=0; e2<nent2; e2++) {
    t2->GetEntry(e2);
    if(UniqueRunId2!=cUniqueRunId){ 
      cbin++; 
      cUniqueRunId=UniqueRunId2; 
      hUniqueRunIdPerRun2->SetBinContent(cbin,UniqueRunId2);
    }

    hexposure2->SetBinContent(hexposure2->FindBin(Seconds2-startTime),cbin);

    hAllPerRun2->Fill(cbin);
    if(StatusCode2==0){
      hEventPerRun2->Fill(cbin);    
      if(ChiSquare2 < 10) hGoodTrackPerRun2->Fill(cbin);
    }
  }
  hGoodTrackPerRun2->Divide(hEventPerRun2);

  for(Int_t i=1; i<100000; i++){
    if(hexposure1->GetBinContent(i) > 0)
      htimePerRun1->Fill(hexposure1->GetBinContent(i));
   
    if(hexposure2->GetBinContent(i) > 0)
      htimePerRun2->Fill(hexposure2->GetBinContent(i));

  }

  hAllPerRun1->Divide(htimePerRun1);
  hAllPerRun2->Divide(htimePerRun2);

  hEventPerRun1->Divide(htimePerRun1);
  hEventPerRun2->Divide(htimePerRun2);

  _timer.Stop();
  cout << endl<<"[e3Correlation.correlation_EEE - INFO] Collected info on Runs duration and rate - Cpu time: "
       << _timer.CpuTime() *1000
       << " - Real time: " << _timer.RealTime() *1000 << " msec."
       << endl;
  _timer.Start();

  //========================================  
  // Chain mesh: define starting time cell for both trees
  //========================================  

  cout<<endl<<"[e3Correlation.correlation_EEE - INFO] Time cell definition"<<endl;
  cout << "\tTime cell width: " << DiffCut << " s" <<endl;
  Int_t firstCelTrackLength1 = (Int_t)((t1min - tmin) / DiffCut);
  Int_t firstCelTrackLength2 = (Int_t)((t2min - tmin) / DiffCut);
  Int_t lastCelTrackLength1  = (Int_t)((t1max - tmin) / DiffCut);
  Int_t lastCelTrackLength2  = (Int_t)((t2max - tmin) / DiffCut);
  cout << "\tStarting, ending cell for tree 1: " << firstCelTrackLength1 << ", " << lastCelTrackLength1 << endl;
  cout << "\tStarting, ending cell for tree 2: " << firstCelTrackLength2 << ", " << lastCelTrackLength2 << endl;

  //========================================  
  // Define complete cell range
  //========================================  

  const Int_t ncells = (Int_t)TMath::Max(lastCelTrackLength1, lastCelTrackLength2);
  cout << "\tTotal number of cells: " << ncells << endl;

  cout<<endl<<"Working..."<<endl;

  //========================================  
  // Define index collector for all cells
  //========================================  

  TArrayI *cell = new TArrayI[ncells];
  for (Int_t i = 0; i < ncells; i++) cell[i].Set(0);

  //=================================================
  // Loop on TTree #2 and add each entry to corresponding cell
  //=================================================

  Int_t cellIndex, size;
  for (Int_t i = 0; i < nent2; i++) {
    t2->GetEntry(i);
    ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09;
    cellIndex = (Int_t)((ctime2 - tmin) / DiffCut);
    if (cellIndex >= 0 && cellIndex < ncells) {
      size = cell[cellIndex].GetSize();
      cell[cellIndex].Set(size+1);
      cell[cellIndex][size] = i;
    }
  }

  //=================================================
  // Define output correlation Trees
  //=================================================

  TString _fileOutName;  
  string _strOut(mydata); 
  replace( _strOut.begin(), _strOut.end(), '/', '_');

  _fileOutName+=tel_code1;   _fileOutName+="_";   _fileOutName+=tel_code2;
  _fileOutName+="_";   _fileOutName+=_strOut.c_str(); _fileOutName+=".root"; 
  TFile *fileout = new TFile(_fileOutName, "RECREATE");
  fileout->ls();

  TTree *treeout = new TTree("tree", "Delta T");
  Int_t e1, e2;	
  Double_t DiffTime;
  Float_t ThetaRel;
  treeout->Branch("ctime1", &ctime1, "ctime1/D");
  treeout->Branch("ChiSquare1", &ChiSquare1, "ChiSquare1/F");
  treeout->Branch("TimeOfFlight1", &TimeOfFlight1, "TimeOfFlight1/F");
  treeout->Branch("TrackLength1", &TrackLength1, "TrackLength1/F");
  treeout->Branch("Theta1", &Theta1, "Theta1/F");
  treeout->Branch("Phi1", &Phi1, "Phi1/F");
  treeout->Branch("UniqueRunId1",&UniqueRunId1,"UniqueRunId1/I");
  treeout->Branch("EventNumber1",&EventNumber1,"EventNumber1/I");
  treeout->Branch("ctime2", &ctime2, "ctime2/D");
  treeout->Branch("ChiSquare2", &ChiSquare2, "ChiSquare2/F");
  treeout->Branch("TimeOfFlight2", &TimeOfFlight2, "TimeOfFlight2/F");
  treeout->Branch("TrackLength2", &TrackLength2, "TrackLength2/F");
  treeout->Branch("Theta2", &Theta2, "Theta2/F");
  treeout->Branch("Phi2", &Phi2, "Phi2/F");
  treeout->Branch("UniqueRunId2",&UniqueRunId2,"UniqueRunId2/I");
  treeout->Branch("EventNumber2",&EventNumber2,"EventNumber2/I");
  treeout->Branch("DiffTime", &DiffTime, "DiffTime/D");
  treeout->Branch("ThetaRel", &ThetaRel, "ThetaRel/F");

  // fill tree with quality check per run
  Float_t ratePerRun,ratePerRunAll,FractionGoodTrack;
  Int_t timeduration,uniquerunid1,uniquerunid2;

  TTree *treeTel1 = new TTree("treeTel1", "run information of telescope 1");
  treeTel1->Branch("uniquerunid1", &uniquerunid1, "uniquerunid1/I");
  treeTel1->Branch("timeduration",&timeduration,"timeduration/I");
  treeTel1->Branch("ratePerRun",&ratePerRunAll,"ratePerRun/F");
  treeTel1->Branch("rateHitPerRun",&ratePerRun,"rateHitPerRun/F");
  treeTel1->Branch("FractionGoodTrack",&FractionGoodTrack,"FractionGoodTrack/F");

  TTree *treeTel2 = new TTree("treeTel2", "run information of telescope 2");
  treeTel2->Branch("uniquerunid2", &uniquerunid2, "uniquerunid2/I");
  treeTel2->Branch("timeduration",&timeduration,"timeduration/I");
  treeTel2->Branch("ratePerRun",&ratePerRunAll,"ratePerRun/F");
  treeTel2->Branch("rateHitPerRun",&ratePerRun,"rateHitPerRun/F");
  treeTel2->Branch("FractionGoodTrack",&FractionGoodTrack,"FractionGoodTrack/F");

  TTree *treeTimeCommon = new TTree("treeTimeCommon", "time duration overlap run by run for the two telescopes");
  treeTimeCommon->Branch("uniquerunid1", &uniquerunid1, "uniquerunid1/I");
  treeTimeCommon->Branch("uniquerunid2", &uniquerunid2, "uniquerunid2/I");
  treeTimeCommon->Branch("timeduration",&timeduration,"timeduration/I");

  //=================================================
  // Fill the infos
  //=================================================

  for(Int_t irun=1; irun<=500; irun++){
    if(htimePerRun1->GetBinContent(irun) > 0){
      uniquerunid1 = hUniqueRunIdPerRun1->GetBinContent(irun);
      timeduration = htimePerRun1->GetBinContent(irun);
      ratePerRun = hEventPerRun1->GetBinContent(irun);
      ratePerRunAll = hAllPerRun1->GetBinContent(irun);
      FractionGoodTrack = hGoodTrackPerRun1->GetBinContent(irun);
      treeTel1->Fill();
    }
    if(htimePerRun2->GetBinContent(irun) > 0){
      uniquerunid2 = hUniqueRunIdPerRun2->GetBinContent(irun);
      timeduration = htimePerRun2->GetBinContent(irun);
      ratePerRun = hEventPerRun2->GetBinContent(irun);
      ratePerRunAll = hAllPerRun2->GetBinContent(irun);
      FractionGoodTrack = hGoodTrackPerRun2->GetBinContent(irun);
      treeTel2->Fill();
    }
  }

  Int_t overlapTime[500][500] = {{},{}};

  // count overlapping seconds
  for(Int_t is=1;is<100000;is++){
    if(hexposure1->GetBinContent(is) > 0 && hexposure2->GetBinContent(is) > 0)
      overlapTime[Int_t(hexposure1->GetBinContent(is))][Int_t(hexposure2->GetBinContent(is))]++;
  }

  for(Int_t irun=1;irun<500;irun++){
    for(Int_t jrun=1;jrun<500;jrun++){
      if(overlapTime[irun][jrun]>0){
  	uniquerunid1 = hUniqueRunIdPerRun1->GetBinContent(irun);
  	uniquerunid2 = hUniqueRunIdPerRun2->GetBinContent(jrun);
  	timeduration = overlapTime[irun][jrun];
  	treeTimeCommon->Fill();
      }      
    }
  }
        
  //=================================================
  // Correlation loop
  //=================================================  

  int last_e1 = 0;
  for(e1 = 0; e1 < nent1; e1++) {

    if (!(e1 % 10000)){
      _timer.Stop();
      cout << "\r[e3Correlation.correlation_EEE - INFO] Correlating entry #" << e1;
      cout <<". Last "<<e1-last_e1<<" entries required Cpu time: "
	   << _timer.CpuTime()*1000
	   << " - Real time: " << _timer.RealTime()*1000<< " msec."
	   << flush;
      _timer.Start();
    }

    t1->GetEntry(e1);
    // Calculate Theta1, Phi1
    calculateThetaPhi(XDir1, YDir1, ZDir1, Theta1, Phi1);
    ctime1 = (Double_t ) Seconds1 + (Double_t ) Nanoseconds1*1E-09;
    cellIndex = (Int_t)((ctime1 - tmin) / DiffCut);

    for (Int_t i = cellIndex - 1; i <= cellIndex + 1; i++) {
      if (i < 0 || i >= ncells) continue;

      for (Int_t j = 0; j < cell[i].GetSize(); j++) {
    	size = cell[cellIndex].GetSize();
    	e2 = cell[i].At(j);
    	t2->GetEntry(e2); 
    	ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09; 

    	//DiffTime= ctime1 - ctime2;    
    	if((Seconds1-Seconds2)==0) DiffTime= (Double_t ) Nanoseconds1 - (Double_t ) Nanoseconds2;
    	else DiffTime=((Double_t ) Seconds1 - (Double_t ) Seconds2)*1E9 + ((Double_t ) Nanoseconds1 - (Double_t ) Nanoseconds2);
 
    	// Calculate Theta2, Phi2
    	calculateThetaPhi(XDir2, YDir2, ZDir2, Theta2, Phi2);
    	ThetaRel=TMath::ACos(TMath::Cos(Theta1*TMath::DegToRad())*TMath::Cos(Theta2*TMath::DegToRad())+TMath::Sin(Theta1*TMath::DegToRad())*TMath::Sin(Theta2*TMath::DegToRad())*TMath::Cos(Phi2*TMath::DegToRad()-Phi1*TMath::DegToRad()))/TMath::DegToRad();

    	if(StatusCode1) ChiSquare1 = 1000;
    	if(StatusCode2) ChiSquare2 = 1000;

    	if(TMath::Abs(DiffTime) <= 1E-4*1E9  && StatusCode1 == 0 && StatusCode2 == 0) treeout->Fill();
      }
    }
  }

  //=================================================
  // Close output files
  //=================================================  

  cout << endl;
  fileout->cd();
  treeout->Write();
  treeTel1->Write();
  treeTel2->Write();
  treeTimeCommon->Write();
  fileout->Close();
  cout<<"\n[e3Correlation.correlation_EEE - INFO] Correlation tree completed."<<endl<<endl;

  _timer.Stop();

  return;

}

// correlation_EEE3 function
//------------------------------------------------------------
void correlation_EEE3(const char *mydata,Double_t DiffCut)
{

//
// Open and read the configuration file
//
	ifstream config;
	config.open("./config_correlation_EEE3.txt", ios::in);
	//Check the existence of the config file
	if(!config.is_open()){
		cout << "Please check the config file!" << endl;
		return;
	}
   
	TString tmp1, tmp2, tmp3, tmp4, tmp5;
	config >> tmp1; // Read the first line of the config file (telescope code 1)
	const char *tel_code1 = new char[tmp1.Length() + 1];
	tel_code1 = tmp1.Data();
	config >> tmp2; // Read the first line of the config file (telescope code 2)
	const char *tel_code2 = new char[tmp2.Length() + 1];
	tel_code2 = tmp2.Data();
	config >> tmp3; // Read the second line of the config file (date)
	const char *tel_code3 = new char[tmp3.Length() + 1];
	tel_code3 = tmp3.Data();
	config >> tmp4; // Read the second line of the config file (date)
	const char *date = new char[tmp3.Length() + 1];
	if(! mydata) date = tmp4.Data();
        else date=mydata;
	config >> tmp5; // Read the third line of the config file (data path)
	const char *path = new char[tmp5.Length() + 1];
	path = tmp5.Data();
//
// Input files
//
        system(Form("ls %s/%s/%s/*.root >lista%s_1",path,tel_code1,date,date));
        system(Form("ls %s/%s/%s/*.root >lista%s_2",path,tel_code2,date,date));
        system(Form("ls %s/%s/%s/*.root >lista%s_3",path,tel_code3,date,date));

        TChain *t1 = new TChain("Events");
        FILE *f1 = fopen(Form("lista%s_1",date),"r");
        char filename[300];
        while(fscanf(f1,"%s",filename)==1) t1->Add(filename);
        fclose(f1);

        TChain *t2 = new TChain("Events");
        FILE *f2 = fopen(Form("lista%s_2",date),"r");
        while(fscanf(f2,"%s",filename)==1) t2->Add(filename);
        fclose(f2);

        TChain *t3 = new TChain("Events");
        FILE *f3 = fopen(Form("lista%s_3",date),"r");
        while(fscanf(f3,"%s",filename)==1) t3->Add(filename);
        fclose(f3);

//
// Define input tree structure	
//
	UInt_t RunNumber1, RunNumber2, RunNumber3;
	UInt_t EventNumber1, EventNumber2, EventNumber3;
	UInt_t StatusCode1, StatusCode2, StatusCode3;
	UInt_t Seconds1, Seconds2, Seconds3;
	ULong64_t Nanoseconds1, Nanoseconds2, Nanoseconds3;
	Float_t XDir1, YDir1, ZDir1;
	Float_t XDir2, YDir2, ZDir2;
	Float_t XDir3, YDir3, ZDir3;
	Float_t ChiSquare1, ChiSquare2, ChiSquare3;
	Float_t TimeOfFlight1, TimeOfFlight2, TimeOfFlight3;
	Float_t TrackLength1, TrackLength2, TrackLength3;
	Double_t DeltaTime1, DeltaTime2, DeltaTime3;
	ULong64_t UniqueRunId1, UniqueRunId2, UniqueRunId3;

	t1->SetBranchAddress("RunNumber",&RunNumber1);
	t1->SetBranchAddress("EventNumber",&EventNumber1);
	t1->SetBranchAddress("StatusCode",&StatusCode1);
	t1->SetBranchAddress("Seconds",&Seconds1);
	t1->SetBranchAddress("Nanoseconds",&Nanoseconds1);
	t1->SetBranchAddress("XDir",&XDir1);
	t1->SetBranchAddress("YDir",&YDir1);
	t1->SetBranchAddress("ZDir",&ZDir1);
	t1->SetBranchAddress("ChiSquare", &ChiSquare1);
	t1->SetBranchAddress("TimeOfFlight", &TimeOfFlight1);
	t1->SetBranchAddress("TrackLength", &TrackLength1);
	t1->SetBranchAddress("DeltaTime", &DeltaTime1);
	t1->SetBranchAddress("UniqueRunId", &UniqueRunId1);

	t2->SetBranchAddress("RunNumber",&RunNumber2);
	t2->SetBranchAddress("EventNumber",&EventNumber2);
	t2->SetBranchAddress("StatusCode",&StatusCode2);
	t2->SetBranchAddress("Seconds",&Seconds2);
	t2->SetBranchAddress("Nanoseconds",&Nanoseconds2);
	t2->SetBranchAddress("XDir",&XDir2);
	t2->SetBranchAddress("YDir",&YDir2);
	t2->SetBranchAddress("ZDir",&ZDir2);
	t2->SetBranchAddress("ChiSquare", &ChiSquare2);
	t2->SetBranchAddress("TimeOfFlight", &TimeOfFlight2);
	t2->SetBranchAddress("TrackLength", &TrackLength2);
	t2->SetBranchAddress("DeltaTime", &DeltaTime2);
	t2->SetBranchAddress("UniqueRunId", &UniqueRunId2);

	t3->SetBranchAddress("RunNumber",&RunNumber3);
	t3->SetBranchAddress("EventNumber",&EventNumber3);
	t3->SetBranchAddress("StatusCode",&StatusCode3);
	t3->SetBranchAddress("Seconds",&Seconds3);
	t3->SetBranchAddress("Nanoseconds",&Nanoseconds3);
	t3->SetBranchAddress("XDir",&XDir3);
	t3->SetBranchAddress("YDir",&YDir3);
	t3->SetBranchAddress("ZDir",&ZDir3);
	t3->SetBranchAddress("ChiSquare", &ChiSquare3);
	t3->SetBranchAddress("TimeOfFlight", &TimeOfFlight3);
	t3->SetBranchAddress("TrackLength", &TrackLength3);
	t3->SetBranchAddress("DeltaTime", &DeltaTime3);
	t3->SetBranchAddress("UniqueRunId", &UniqueRunId3);

	Int_t nent1 = t1->GetEntries();
	Int_t nent2 = t2->GetEntries();
	Int_t nent3 = t3->GetEntries();

	Double_t ctime1, ctime2, ctime3;
	Float_t Theta1,Phi1,Theta2,Phi2,Theta3,Phi3;

//      
// Find time range
//
	Double_t t1min, t1max, t2min, t2max, t3min, t3max, range1, range2, range3;
        Int_t i1 = 0;StatusCode1=1;
        while(StatusCode1) {t1->GetEntry(        i1); ctime1 = (Double_t ) Seconds1 + (Double_t ) Nanoseconds1*1E-09; t1min = ctime1;i1++;}
        cout << "start " << Seconds1 << endl;
        i1 = nent1 - 5; StatusCode1=1;	
        while(StatusCode1) {t1->GetEntry( i1); ctime1 = (Double_t ) Seconds1 + (Double_t ) Nanoseconds1*1E-09; t1max = ctime1;i1--;}
        cout << "end " << Seconds1 << " " << StatusCode1 << endl;
        Int_t i2 = 0; StatusCode2=1;
	while(StatusCode2) {t2->GetEntry(        i2); ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09; t2min = ctime2;i2++;}
        cout << "start " << Seconds2 << endl;
        i2 = nent2 - 1; StatusCode2=1;
	while(StatusCode2) {t2->GetEntry(i2); ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09; t2max = ctime2;i2--;}
        cout << "end " << Seconds2 << endl;
        Int_t i3 = 0; StatusCode3=1;
	while(StatusCode3) {t3->GetEntry(        i3); ctime3 = (Double_t ) Seconds3 + (Double_t ) Nanoseconds3*1E-09; t3min = ctime3;i3++;}
        cout << "start " << Seconds3 << endl;
        i3 = nent3 - 1; StatusCode3=1;
	while(StatusCode3) {t3->GetEntry(i3); ctime3 = (Double_t ) Seconds3 + (Double_t ) Nanoseconds3*1E-09; t3max = ctime3;i3--;}
        cout << "end " << Seconds3 << endl;

	range1 = t1max - t1min;
	range2 = t2max - t2min;
	range3 = t3max - t3min;
	cout.setf(ios::fixed);
	cout <<"N.entry1 = "<< nent1<<"   N.entry2 = "<<nent2 <<"   N.entry3 = "<<nent3 << endl;
	cout << "Time range file 1: " << t1min << " --> " << t1max << ", range = " << range1 << endl;
	cout << "Time range file 2: " << t2min << " --> " << t2max << ", range = " << range2 << endl;
	cout << "Time range file 3: " << t3min << " --> " << t3max << ", range = " << range3 << endl;
	Double_t tmin = TMath::Min(t1min, t2min);
	tmin = TMath::Min(tmin, t3min);
	Double_t tmax = TMath::Max(t1max, t2max);
	tmax = TMath::Max(tmax, t3max);
	Double_t tminr = TMath::Max(t1min, t2min);
	tminr = TMath::Max(tminr, t3min);
	Double_t tmaxr = TMath::Min(t1max, t2max);
	tmaxr = TMath::Min(tmaxr, t3max);
	cout << "Common measure time interval = "<<tmaxr - tminr<< " s"<<endl;
//
// Chain mesh: define starting time cell for both trees
//
	Int_t firstCelTrackLength1 = (Int_t)((t1min - tmin) / DiffCut);
	Int_t firstCelTrackLength2 = (Int_t)((t2min - tmin) / DiffCut);
	Int_t firstCelTrackLength3 = (Int_t)((t3min - tmin) / DiffCut);
	Int_t lastCelTrackLength1  = (Int_t)((t1max - tmin) / DiffCut);
	Int_t lastCelTrackLength2  = (Int_t)((t2max - tmin) / DiffCut);
	Int_t lastCelTrackLength3  = (Int_t)((t3max - tmin) / DiffCut);
	cout << "Starting, ending cell for tree 1: " << firstCelTrackLength1 << ", " << lastCelTrackLength1 << endl;
	cout << "Starting, ending cell for tree 2: " << firstCelTrackLength2 << ", " << lastCelTrackLength2 << endl;
	cout << "Starting, ending cell for tree 3: " << firstCelTrackLength3 << ", " << lastCelTrackLength3 << endl;
//
// define complete cell range
//
	Int_t maxcell = (Int_t)TMath::Max(lastCelTrackLength1, lastCelTrackLength2);
	const Int_t ncells = (Int_t)TMath::Max(maxcell, lastCelTrackLength3);
	cout << "#cells: " << ncells << endl;
	cout<<"Working..."<<endl;
//	
// define index collector for all cells
//
        cout << ncells << endl;
	TArrayI *cell = new TArrayI[ncells];
	for (Int_t i = 0; i < ncells; i++) cell[i].Set(0);
	TArrayI *cellB = new TArrayI[ncells];
	for (Int_t i = 0; i < ncells; i++) cellB[i].Set(0);
//	
// loop on TTree #2 and add each entry to corresponding cell
//
	Int_t cellIndex, size;
	for (Int_t i = 0; i < nent2; i++) {
		t2->GetEntry(i);
		ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09;
		cellIndex = (Int_t)((ctime2 - tmin) / DiffCut);
		if (cellIndex >= 0 && cellIndex < ncells) {
			size = cell[cellIndex].GetSize();
			cell[cellIndex].Set(size+1);
			cell[cellIndex][size] = i;
		}
	}

	Int_t sizeB;
	for (Int_t i = 0; i < nent3; i++) {
		t3->GetEntry(i);
		ctime3 = (Double_t ) Seconds3 + (Double_t ) Nanoseconds3*1E-09;
		cellIndex = (Int_t)((ctime3 - tmin) / DiffCut);
		if (cellIndex >= 0 && cellIndex < ncells) {
			sizeB = cellB[cellIndex].GetSize();
			cellB[cellIndex].Set(sizeB+1);
			cellB[cellIndex][sizeB] = i;
		}
	}

//	
// Define output correlation tree
//
	TFile *fileout = new TFile(Form("%s/%s-%s-%s-%s.root",".",tel_code1,tel_code2,tel_code3,date), "RECREATE");
        fileout->ls();
	TTree *treeout = new TTree("tree", "Delta T");
	Int_t e1, e2, e3;	
	Double_t DiffTime;
	Double_t DiffTimeB;
	Float_t ThetaRel;
	Float_t ThetaRelB;
	Float_t ThetaRelC;
	treeout->Branch("ctime1", &ctime1, "ctime1/D");
	treeout->Branch("ChiSquare1", &ChiSquare1, "ChiSquare1/F");
	treeout->Branch("TimeOfFlight1", &TimeOfFlight1, "TimeOfFlight1/F");
	treeout->Branch("TrackLength1", &TrackLength1, "TrackLength1/F");
	treeout->Branch("Theta1", &Theta1, "Theta1/F");
	treeout->Branch("Phi1", &Phi1, "Phi1/F");
	treeout->Branch("ctime2", &ctime2, "ctime2/D");
	treeout->Branch("ChiSquare2", &ChiSquare2, "ChiSquare2/F");
	treeout->Branch("TimeOfFlight2", &TimeOfFlight2, "TimeOfFlight2/F");
	treeout->Branch("TrackLength2", &TrackLength2, "TrackLength2/F");
	treeout->Branch("Theta2", &Theta2, "Theta2/F");
	treeout->Branch("Phi2", &Phi2, "Phi2/F");
	treeout->Branch("ctime3", &ctime3, "ctime3/D");
	treeout->Branch("ChiSquare3", &ChiSquare3, "ChiSquare3/F");
	treeout->Branch("TimeOfFlight3", &TimeOfFlight3, "TimeOfFlight3/F");
	treeout->Branch("TrackLength3", &TrackLength3, "TrackLength3/F");
	treeout->Branch("Theta3", &Theta3, "Theta3/F");
	treeout->Branch("Phi3", &Phi3, "Phi3/F");
	treeout->Branch("DiffTime12", &DiffTime, "DiffTime12/D");
	treeout->Branch("DiffTime13", &DiffTimeB, "DiffTime13/D");
	treeout->Branch("ThetaRel12", &ThetaRel, "ThetaRel12/F");
	treeout->Branch("ThetaRel13", &ThetaRelB, "ThetaRel13/F");
	treeout->Branch("ThetaRel23", &ThetaRelC, "ThetaRel23/F");

	for(e1 = 0; e1 < nent1; e1++) {
		if (!(e1 % 10000)) cout << "\rCorrelating entry #" << e1 << flush;
		t1->GetEntry(e1);
		// Calculate Theta1, Phi1
		calculateThetaPhi(XDir1, YDir1, ZDir1, Theta1, Phi1);
		ctime1 = (Double_t ) Seconds1 + (Double_t ) Nanoseconds1*1E-09;
		cellIndex = (Int_t)((ctime1 - tmin) / DiffCut);
		for (Int_t i = cellIndex - 1; i <= cellIndex + 1; i++) {
			if (i < 0 || i >= ncells) continue;
			for (Int_t j = 0; j < cell[i].GetSize(); j++) {
				size = cell[cellIndex].GetSize();
				e2 = cell[i].At(j);
				t2->GetEntry(e2); 
				ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09; 
				//DiffTime= ctime1 - ctime2;    
				if((Seconds1-Seconds2)==0) DiffTime= (Double_t ) Nanoseconds1 - (Double_t ) Nanoseconds2;
				else DiffTime=((Double_t ) Seconds1 - (Double_t ) Seconds2)*1E9 + ((Double_t ) Nanoseconds1 - (Double_t ) Nanoseconds2); 
				// Calculate Theta2, Phi2
				calculateThetaPhi(XDir2, YDir2, ZDir2, Theta2, Phi2);
				ThetaRel=TMath::ACos(TMath::Cos(Theta1*TMath::DegToRad())*TMath::Cos(Theta2*TMath::DegToRad())+TMath::Sin(Theta1*TMath::DegToRad())*TMath::Sin(Theta2*TMath::DegToRad())*TMath::Cos(Phi2*TMath::DegToRad()-Phi1*TMath::DegToRad()))/TMath::DegToRad();

                                if(StatusCode1) ChiSquare1 = 1000;
                                if(StatusCode2) ChiSquare2 = 1000;

				if(TMath::Abs(DiffTime) <= 1E-4*1E9){
                                  for (Int_t ii = cellIndex - 1; ii <= cellIndex + 1; ii++) {
				    for (Int_t k = 0; k < cellB[ii].GetSize(); k++) {
				      sizeB = cellB[cellIndex].GetSize();
				      e3 = cellB[ii].At(k);
				      t3->GetEntry(e3); 
				      ctime3 = (Double_t ) Seconds3 + (Double_t ) Nanoseconds3*1E-09; 
				      if((Seconds1-Seconds3)==0) DiffTimeB= (Double_t ) Nanoseconds1 - (Double_t ) Nanoseconds3;
				      else DiffTimeB=((Double_t ) Seconds1 - (Double_t ) Seconds3)*1E9 + ((Double_t ) Nanoseconds1 - (Double_t ) Nanoseconds3);
				      calculateThetaPhi(XDir3, YDir3, ZDir3, Theta3, Phi3);
				      ThetaRelB=TMath::ACos(TMath::Cos(Theta1*TMath::DegToRad())*TMath::Cos(Theta3*TMath::DegToRad())+TMath::Sin(Theta1*TMath::DegToRad())*TMath::Sin(Theta3*TMath::DegToRad())*TMath::Cos(Phi3*TMath::DegToRad()-Phi1*TMath::DegToRad()))/TMath::DegToRad();
				      ThetaRelC=TMath::ACos(TMath::Cos(Theta2*TMath::DegToRad())*TMath::Cos(Theta3*TMath::DegToRad())+TMath::Sin(Theta2*TMath::DegToRad())*TMath::Sin(Theta3*TMath::DegToRad())*TMath::Cos(Phi3*TMath::DegToRad()-Phi2*TMath::DegToRad()))/TMath::DegToRad();
				      if(StatusCode3) ChiSquare3 = 1000;


				      if(TMath::Abs(DiffTimeB) <= 1E-4*1E9 && StatusCode1 == 0 && StatusCode2 == 0 && StatusCode3 == 0)
				        treeout->Fill();
                                    }
				  }
				}
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


// correlation_EEEAC function
//------------------------------------------------------------
void correlation_EEEAC(const char *mydata,Double_t DiffCut)
{

//
// Open and read the configuration file
//
	ifstream config;
	config.open("./config_correlation_EEE.txt", ios::in);
	//Check the existence of the config file
	if(!config.is_open()){
		cout << "Please check the config file!" << endl;
		return;
	}
   
	TString tmp1, tmp2, tmp3, tmp4;
	config >> tmp1; // Read the first line of the config file (telescope code 1)
	const char *tel_code1 = new char[tmp1.Length() + 1];
	tel_code1 = tmp1.Data();
	config >> tmp2; // Read the first line of the config file (telescope code 2)
	const char *tel_code2 = new char[tmp1.Length() + 1];
	tel_code2 = tmp1.Data();
	config >> tmp3; // Read the second line of the config file (date)
	const char *date = new char[tmp3.Length() + 1];
	if(! mydata) date = tmp3.Data();
        else date=mydata;
	config >> tmp4; // Read the third line of the config file (data path)
	const char *path = new char[tmp4.Length() + 1];
	path = tmp4.Data();
//
// Input files
//
        system(Form("ls %s/%s/%s/*.root >lista%s",path,tel_code1,date,date));

        TChain *t1 = new TChain("Events");
        FILE *f1 = fopen(Form("lista%s",date),"r");
        char filename[300];
        while(fscanf(f1,"%s",filename)==1) t1->Add(filename);
        fclose(f1);

        TChain *t2 = new TChain("Events");
        FILE *f2 = fopen(Form("lista%s",date),"r");
        while(fscanf(f2,"%s",filename)==1) t2->Add(filename);
        fclose(f2);

//
// Define input tree structure	
//
	UInt_t RunNumber1, RunNumber2;
	UInt_t EventNumber1, EventNumber2;
	UInt_t StatusCode1, StatusCode2;
	UInt_t Seconds1, Seconds2;
	ULong64_t Nanoseconds1, Nanoseconds2;
	Float_t XDir1, YDir1, ZDir1;
	Float_t XDir2, YDir2, ZDir2;
	Float_t ChiSquare1, ChiSquare2;
	Float_t TimeOfFlight1, TimeOfFlight2;
	Float_t TrackLength1, TrackLength2;
	Double_t DeltaTime1, DeltaTime2;
	ULong64_t UniqueRunId1, UniqueRunId2;

	t1->SetBranchAddress("RunNumber",&RunNumber1);
	t1->SetBranchAddress("EventNumber",&EventNumber1);
	t1->SetBranchAddress("StatusCode",&StatusCode1);
	t1->SetBranchAddress("Seconds",&Seconds1);
	t1->SetBranchAddress("Nanoseconds",&Nanoseconds1);
	t1->SetBranchAddress("XDir",&XDir1);
	t1->SetBranchAddress("YDir",&YDir1);
	t1->SetBranchAddress("ZDir",&ZDir1);
	t1->SetBranchAddress("ChiSquare", &ChiSquare1);
	t1->SetBranchAddress("TimeOfFlight", &TimeOfFlight1);
	t1->SetBranchAddress("TrackLength", &TrackLength1);
	t1->SetBranchAddress("DeltaTime", &DeltaTime1);
	t1->SetBranchAddress("UniqueRunId", &UniqueRunId1);

	t2->SetBranchAddress("RunNumber",&RunNumber2);
	t2->SetBranchAddress("EventNumber",&EventNumber2);
	t2->SetBranchAddress("StatusCode",&StatusCode2);
	t2->SetBranchAddress("Seconds",&Seconds2);
	t2->SetBranchAddress("Nanoseconds",&Nanoseconds2);
	t2->SetBranchAddress("XDir",&XDir2);
	t2->SetBranchAddress("YDir",&YDir2);
	t2->SetBranchAddress("ZDir",&ZDir2);
	t2->SetBranchAddress("ChiSquare", &ChiSquare2);
	t2->SetBranchAddress("TimeOfFlight", &TimeOfFlight2);
	t2->SetBranchAddress("TrackLength", &TrackLength2);
	t2->SetBranchAddress("DeltaTime", &DeltaTime2);
	t2->SetBranchAddress("UniqueRunId", &UniqueRunId2);

	Int_t nent1 = t1->GetEntries();
	Int_t nent2 = t2->GetEntries();

	Double_t ctime1, ctime2;
	Float_t Theta1,Phi1,Theta2,Phi2;

//      
// Find time range
//
	Double_t t1min, t1max, t2min, t2max, range1, range2;
        Int_t i1 = 0;StatusCode1=1;
        while(StatusCode1) {t1->GetEntry(        i1); ctime1 = (Double_t ) Seconds1 + (Double_t ) Nanoseconds1*1E-09; t1min = ctime1;i1++;}
        cout << "start " << Seconds1 << endl;
        i1 = nent1 - 5; StatusCode1=1;	
        while(StatusCode1) {t1->GetEntry( i1); ctime1 = (Double_t ) Seconds1 + (Double_t ) Nanoseconds1*1E-09; t1max = ctime1;i1--;}
        cout << "end " << Seconds1 << " " << StatusCode1 << endl;
        Int_t i2 = 0; StatusCode2=1;
	while(StatusCode2) {t2->GetEntry(        i2); ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09; t2min = ctime2;i2++;}
        cout << "start " << Seconds2 << endl;
        i2 = nent2 - 1; StatusCode2=1;
	while(StatusCode2) {t2->GetEntry(i2); ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09; t2max = ctime2;i2--;}
        cout << "end " << Seconds2 << endl;

	range1 = t1max - t1min;
	range2 = t2max - t2min;
	cout.setf(ios::fixed);
	cout <<"N.entry1 = "<< nent1<<"   N.entry2 = "<<nent2 << endl;
	cout << "Time range file 1: " << t1min << " --> " << t1max << ", range = " << range1 << endl;
	cout << "Time range file 2: " << t2min << " --> " << t2max << ", range = " << range2 << endl;
	Double_t tmin = TMath::Min(t1min, t2min);
	Double_t tmax = TMath::Max(t1max, t2max);
	cout << "Common measure time interval = "<<(TMath::Min(t1max, t2max)-TMath::Max(t1min, t2min))<< " s"<<endl;
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
		ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09;
		cellIndex = (Int_t)((ctime2 - tmin) / DiffCut);
		if (cellIndex >= 0 && cellIndex < ncells) {
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
	TTree *treeout = new TTree("tree", "Delta T");
	Int_t e1, e2;	
	Double_t DiffTime;
	Float_t ThetaRel;
	treeout->Branch("ctime1", &ctime1, "ctime1/D");
	treeout->Branch("ChiSquare1", &ChiSquare1, "ChiSquare1/F");
	treeout->Branch("TimeOfFlight1", &TimeOfFlight1, "TimeOfFlight1/F");
	treeout->Branch("TrackLength1", &TrackLength1, "TrackLength1/F");
	treeout->Branch("Theta1", &Theta1, "Theta1/F");
	treeout->Branch("Phi1", &Phi1, "Phi1/F");
	treeout->Branch("ctime2", &ctime2, "ctime2/D");
	treeout->Branch("ChiSquare2", &ChiSquare2, "ChiSquare2/F");
	treeout->Branch("TimeOfFlight2", &TimeOfFlight2, "TimeOfFlight2/F");
	treeout->Branch("TrackLength2", &TrackLength2, "TrackLength2/F");
	treeout->Branch("Theta2", &Theta2, "Theta2/F");
	treeout->Branch("Phi2", &Phi2, "Phi2/F");
	treeout->Branch("DiffTime", &DiffTime, "DiffTime/D");
	treeout->Branch("ThetaRel", &ThetaRel, "ThetaRel/F");


	for(e1 = 0; e1 < nent1; e1++) {
		if (!(e1 % 10000)) cout << "\rCorrelating entry #" << e1 << flush;
		t1->GetEntry(e1);
		// Calculate Theta1, Phi1
		calculateThetaPhi(XDir1, YDir1, ZDir1, Theta1, Phi1);
		ctime1 = (Double_t ) Seconds1 + (Double_t ) Nanoseconds1*1E-09;
		cellIndex = (Int_t)((ctime1 - tmin) / DiffCut);
		for (Int_t i = cellIndex - 1; i <= cellIndex + 1; i++) {
			if (i < 0 || i >= ncells) continue;
			for (Int_t j = 0; j < cell[i].GetSize(); j++) {
				size = cell[cellIndex].GetSize();
				e2 = cell[i].At(j);
                                if(e2 >= e1) continue; //to skip autocorrelation
				t2->GetEntry(e2); 
				ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09; 
				//DiffTime= ctime1 - ctime2;    
				if((Seconds1-Seconds2)==0) DiffTime= (Double_t ) Nanoseconds1 - (Double_t ) Nanoseconds2;
				else DiffTime=((Double_t ) Seconds1 - (Double_t ) Seconds2)*1E9 + ((Double_t ) Nanoseconds1 - (Double_t ) Nanoseconds2); 
				// Calculate Theta2, Phi2
				calculateThetaPhi(XDir2, YDir2, ZDir2, Theta2, Phi2);
				ThetaRel=TMath::ACos(TMath::Cos(Theta1*TMath::DegToRad())*TMath::Cos(Theta2*TMath::DegToRad())+TMath::Sin(Theta1*TMath::DegToRad())*TMath::Sin(Theta2*TMath::DegToRad())*TMath::Cos(Phi2*TMath::DegToRad()-Phi1*TMath::DegToRad()))/TMath::DegToRad();
				if(DiffTime > 0 && TMath::Abs(DiffTime) <= 1E-4*1E9 && StatusCode1 == 0 && StatusCode2 == 0) treeout->Fill();
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

//  calculateThetaPhi function
//------------------------------------------------------------
void calculateThetaPhi(Float_t &cx, Float_t &cy, Float_t &cz, Float_t &teta, Float_t &phi){

  teta=TMath::ACos(cz)*TMath::RadToDeg();
  if(cx==0 && cy>0)  phi=90.0;
  if(cx==0 && cy<0)  phi=270.0;
  if(cx>0 || cx<0) phi=TMath::ATan(cy/cx)*TMath::RadToDeg();
  if(cx<0)          phi=phi+180.0;
  if(cx>0 && cy<0)   phi=360.0+phi;

  Double_t deltaphi=gRandom->Gaus(0.0,4.0);
  phi=phi+deltaphi;
  if(phi<0.)   phi=360+phi;
  if(phi>360.) phi=phi-360.0;

}


