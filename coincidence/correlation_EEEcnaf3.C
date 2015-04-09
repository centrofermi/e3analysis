/***************************************************
***                                              ***
***							correlation_EEE.C							   ***
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

void CalculateThetaPhi(Float_t &cx, Float_t &cy, Float_t &cz, Float_t &teta, Float_t &phi);
void correlation_EEE(const char *mydata=NULL,Double_t DiffCut = 0.1);

int main(int argc,char *argv[]){

  if(argc>1){
     correlation_EEE(argv[1]);
  }

}


void correlation_EEE(const char *mydata,Double_t DiffCut)
{
//
// This macro correlates the events measured by two EEE telescopes according to their GPS time
// within a time window = DiffCut
//
// Data are read from the two ROOT trees created for each telescope
//

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
		CalculateThetaPhi(XDir1, YDir1, ZDir1, Theta1, Phi1);
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
				CalculateThetaPhi(XDir2, YDir2, ZDir2, Theta2, Phi2);
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
				      CalculateThetaPhi(XDir3, YDir3, ZDir3, Theta3, Phi3);
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
