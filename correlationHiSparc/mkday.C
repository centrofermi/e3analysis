Float_t xfac = 40075E3 / 360.; // m
Float_t	yfac = xfac * TMath::Cos(52.3559919*TMath::DegToRad());
Float_t c = 3E8; // m/s
Float_t cinv = 1./c;

Float_t lat[] = {
	52.3558963,
	52.3552940,
        52.3562600,
        52.3571852,
        52.3572524,
        52.3571869,
        52.3559919,
        52.3563528,
	52.3545604,
	52.3558817,
	52.3564362,
	52.3558917,
	52.3558738,
	52.3558385,
	52.3558991,
	52.3558883
	};
Float_t lon[] = {
	4.9509827,
	4.9500921,
        4.9529440,
        4.9543821,
        4.9483850,
        4.9519807,
        4.9514708,
        4.9507174,
        4.9556983,
	4.9510053,
	4.9485824,
	4.9509833,
	4.9510129,
	4.9511586,
	4.950988,
	4.9510058
	};
Float_t alt[] = {
	56.18,
	57.52,
        51.44,
        54.79,
        47.60,
        45.47,
        59.76,
        51.69,
        56.84,
        56.09,
        57.86,
        52.30,
	52.65,
	52.22,
	57.65,
	55.78
	};

void mkday(const char *date){
   TFile *f = new TFile(Form("hisparc500-%s.root",date));
   TTree *t = (TTree *) f->Get("t1");
   Int_t nev = t->GetEntries();

   Int_t sec,ns,ntrack;
   Long_t nslong;
   Float_t vx=0,vy=0,vz=0,tof;
   Int_t ind;

   Flaot_t xdirt[100],ydir[100],zdir[100],chi2[100];

   t->SetBranchAddress("Seconds",&sec);
   t->SetBranchAddress("NanoSeconds",&ns);
   t->SetBranchAddress("NumStations",&ntrack);
   
   TFile *fout = new TFile(Form("HS-0500-%s.root",date),"RECREATE");
   TTree *tout = new TTree("cosmics","cosmics");
   tout->Branch("Seconds",&sec,"Seconds/I");
   tout->Branch("Nanoseconds",&nslong,"Nanoseconds/L");
   tout->Branch("Ntracks",&ntrack,"Ntracks/I");
   tout->Branch("XDir",&vx,"XDir/F");
   tout->Branch("YDir",&vy,"YDir/F");
   tout->Branch("ZDir",&vz,"ZDir/F");
   tout->Branch("XDirTrack",xdirt,"XDirTrack[Ntracks]/F");
   tout->Branch("YDirTrack",ydirt,"YDirTrack[Ntracks]/F");
   tout->Branch("ZDirTrack",zdirt,"ZDirTrack[Ntracks]/F");
   tout->Branch("ChiSquareTrack",chi2,"ChiSquareTrack[Ntracks]/F");


   Float_t sx,sy,st,st2,sxt,syt,dx,dy,delta;

   for(Int_t i=0;i<nev;i++){
     t->GetEvent(i);
     nslong=ns;

/*
     sx=0,sy=0,sx2=0,sy2=0,st=0,sxt=0,syt=0,st2=0;

     for(Int_t j=0;j < ntrack;j++){
        tof = t->GetLeaf("TimeStation")->GetValue(j);
        ind = t->GetLeaf("IndexStation")->GetValue(j);
        dx = lon[ind];
        dy = lat[ind];

	sx += dx;
	sy += dy;
        st += tof;
        st2 += tof*tof;
	sxt += dx*tof;
        syt += dy*tof;
     }

     delta = 1./(ntrack*st2 - st*st);

     vx = delta * (ntrack*sxt - sx*st);
     vy = delta * (ntrack*syt - sy*st);

     vx *= -xfac*1E9*cinv;
     vy	*= -yfac*1E9*cinv;

     printf("%e %e\n",vx,vy);

     vz = TMath::Sqrt(1-vx*vx-vy*vy);
*/
     tout->Fill();
   }

  tout->Write();
  fout->Close();
}
