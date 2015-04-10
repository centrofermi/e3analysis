/***************************************************
***                                              ***
***   correlation_EEE.C       	   		 ***
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

void correlation_EEE(Double_t DiffCut = 0.1)
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
	config.open("./config_correlation_EEE.txt", ios::in);
	//Check the existence of the config file
	if(!config.is_open()){
		cout << "Please check the config file!" << endl;
		break;
	}
   
	TString tmp1, tmp2, tmp3, tmp4;
	config >> tmp1; // Read the first line of the config file (telescope code 1)
	char *tel_code1 = new char[tmp1.Length() + 1];
	tel_code1 = tmp1.Data();
	config >> tmp2; // Read the first line of the config file (telescope code 2)
	char *tel_code2 = new char[tmp2.Length() + 1];
	tel_code2 = tmp2.Data();
	config >> tmp3; // Read the second line of the config file (date)
	char *date = new char[tmp3.Length() + 1];
	date = tmp3.Data();
	config >> tmp4; // Read the third line of the config file (data path)
	char *path = new char[tmp4.Length() + 1];
	path = tmp4.Data();

// select date
       Int_t year,month,day;
       sscanf(date,"%d-%d-%d",&year,&month,&day);

//
// Input files
//
	TFile *f1 = new TFile(Form("%s%s-%s-merged.root",path,tel_code1,date));
	TTree *t1 = (TTree*)f1->Get("Events");
	TFile *f2 = new TFile(Form("%s%s-%s-merged.root",path,tel_code2,date));
	TTree *t2 = (TTree*)f2->Get("Events");
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
	t1->GetEntry(        0); {ctime1 = (Double_t ) Seconds1 + (Double_t ) Nanoseconds1*1E-09; t1min = ctime1;}
	t1->GetEntry(nent1 - 1); {ctime1 = (Double_t ) Seconds1 + (Double_t ) Nanoseconds1*1E-09; t1max = ctime1;}
	t2->GetEntry(        0); {ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09; t2min = ctime2;}
	t2->GetEntry(nent2 - 1); {ctime2 = (Double_t ) Seconds2 + (Double_t ) Nanoseconds2*1E-09; t2max = ctime2;}
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
	//cout << "Starting, ending cell for tree 1: " << firstCelTrackLength1 << ", " << lastCelTrackLength1 << endl;
	//cout << "Starting, ending cell for tree 2: " << firstCelTrackLength2 << ", " << lastCelTrackLength2 << endl;
//
// define complete cell range
//
	const Int_t ncells = (Int_t)TMath::Max(lastCelTrackLength1, lastCelTrackLength2);
	cout << "#cells: " << ncells << endl;
	cout<<"Working..."<<endl;
//	
// define index collector for all cells
//
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
	TFile *fileout = new TFile(Form("%s%s-%s-%s.root",path,tel_code1,tel_code2,date), "RECREATE");
	TTree *treeout = new TTree("tree", "Delta T");
	Int_t e1, e2;	
	Double_t DiffTime;
	Float_t ThetaRel;
        treeout->Branch("year", &year, "year/I");
        treeout->Branch("month", &month, "month/I");
        treeout->Branch("day", &day, "day/I");
	treeout->Branch("ctime1", &ctime1, "ctime1/D");
	treeout->Branch("ChiSquare1", &ChiSquare1, "ChiSquare1/F");
	treeout->Branch("TimeOfFlight1", &TimeOfFlight1, "TimeOfFlight1/F");
	treeout->Branch("TrackLength1", &TrackLength1, "TrackLength1/F");
	treeout->Branch("Theta1", &Theta1, "Theta1/F");
	treeout->Branch("Phi1", &Phi1, "Phi1/F");
        treeout->Branch("RunNumber1",&RunNumber1,"RunNumber1/I");
        treeout->Branch("EventNumber1",&EventNumber1,"EventNumber1/I");
	treeout->Branch("ctime2", &ctime2, "ctime2/D");
	treeout->Branch("ChiSquare2", &ChiSquare2, "ChiSquare2/F");
	treeout->Branch("TimeOfFlight2", &TimeOfFlight2, "TimeOfFlight2/F");
	treeout->Branch("TrackLength2", &TrackLength2, "TrackLength2/F");
	treeout->Branch("Theta2", &Theta2, "Theta2/F");
	treeout->Branch("Phi2", &Phi2, "Phi2/F");
        treeout->Branch("RunNumber2",&RunNumber2,"RunNumber2/I");
        treeout->Branch("EventNumber2",&EventNumber2,"EventNumber2/I");
	treeout->Branch("DiffTime", &DiffTime, "DiffTime/D");
	treeout->Branch("ThetaRel", &ThetaRel, "ThetaRel/F");


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
				if(TMath::Abs(DiffTime) <= DiffCut*1E9 && StatusCode1 == 0 && StatusCode2 == 0) treeout->Fill();
			}
		}
	}
//
// Closing files
//
	f1->Close();
	f2->Close();
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
