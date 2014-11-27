
// allego anche la macro addDT.C
// che aggiunge ad Events le seguenti quantità (utilizzate nell'analisi del Mu decay)
// DT[0] intertempo rispetto all evento precedente
// DT[1] intertempo rispetto all evento succssivo
// betaS beta dell'evento successivo
// ZDirs ZDir dell'evento successivo
// ChiS  ChiSquare dell'evento successivo
//
// Si utilizza cosi (specificando il nome del file):
// root -b -q addDT.C\(\"BARI-01-2014-10-26-2014-11-04.root\"\)
// attenzione il file originario viene modificato.


void addDT(TString fname){

    TFile* F1;
    F1 = TFile::Open(fname.Data(),"update");
	UInt_t Seconds[1];
	ULong64_t Nanoseconds[1];
	Float_t ZDir[1];
	Float_t ToF[1];
	Float_t Len[1];
	Float_t Chi[1];
	Float_t DTi[2];
	Float_t Sec[3];
	UInt_t Oldsec=0;
	ULong64_t Oldnan=0;
	UInt_t postsec=0;
	ULong64_t postnan=0;
    TTree* Events=F1->Get("Events");
	Events->SetBranchAddress( "Seconds", Seconds);
	Events->SetBranchAddress( "Nanoseconds", Nanoseconds);
	Events->SetBranchAddress( "ZDir", ZDir);
	Events->SetBranchAddress( "TimeOfFlight", ToF);
	Events->SetBranchAddress( "TrackLength", Len);
	Events->SetBranchAddress( "ChiSquare", Chi);
	TString branchName=Form("DTi[2]/F");
	TBranch *dtBranch = Events->Branch("DTi",DTi,branchName.Data());
	branchName=Form("betaS/F:ZDirS/F:ChiS/F");
	TBranch *PBranch = Events->Branch("Secondary",Sec,branchName.Data());

	int ninterv = 10000;
	long long int nentries = Events->GetEntries();
	for (long long int ee=0; ee<nentries; ee++){
	  if ( ee%ninterv == 0) cout << fname.Data() << "processed " << (100.*ee/(nentries)) << "% event " << ee << endl;
	  Events->GetEntry(ee);
	  if (Seconds[0]==0){ 
	    DTi[1]=0;
	    DTi[0]=0;
	    Sec[0]=0;
	    Sec[1]=0;
	    Sec[2]=0;
	  }
	  else 
	    {
	      postsec = 0;
	      postnan = 0;
	      long long int ee2 = ee+1; 
	      while(postsec == 0 && ee2!=nentries-1) {
		Events->GetEntry(ee2);
		postsec = Seconds[0];
		postnan = Nanoseconds[0];
		//		  cout << "tst: " << ee2 << " " << ee << endl;
		ee2++;
	      }
	      Sec[0]=(Len[0]/ToF[0])/29.97; //beta evento successivo
	      Sec[1]=ZDir[0];               //ZDir evento successivo
	      Sec[2]=Chi[0];                //ChiSquare evento successivo
	      //	  cout << "found: " << postsec << endl;
	      Events->GetEntry(ee);
	      DTi[0]=(int) (Seconds[0]-Oldsec)+1.e-9*((int)(Nanoseconds[0]-Oldnan));
	      DTi[1]=(int) (postsec-Seconds[0])+1.e-9*((int)(postnan-Nanoseconds[0]));
	    }
	  if (ee==nentries-1 || Seconds[0]==0) DTi[1]=0;
	  if (ee==0 || Seconds[0]==0) DTi[0]=0;
	  //	  cout << DTi[0] << endl;
	  // cout << DTi[0] << " | " << Nanoseconds[0] << " " << Oldnan << " | " << Seconds[0] << " " << Oldsec << " | " << (int) (Nanoseconds[0] - Oldnan) << " | " << " | " << Seconds[0] - Oldsec << " | " << endl;
	  if (Seconds[0]!=0){
	    Oldsec=Seconds[0];
	    Oldnan=Nanoseconds[0];}
	  dtBranch->Fill();
	  PBranch->Fill();
	}
	Events->Write("",TObject::kOverwrite);   
	F1->Close();
	return;
}
