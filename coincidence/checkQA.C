checkQA(const char *file="coincSAVO-01_SAVO-02.root",const char *sch1="SAVO-01",const char *sch2="SAVO-02"){
  TFile *f = new TFile(file);
  TTree *tel[2];
  tel[0] = (TTree *) f->Get("treeTel1");
  tel[1] = (TTree *) f->Get("treeTel2");
  TTree *tCom = (TTree *) f->Get("treeTimeCommon");

  Int_t year,month,day,run,time;
  Float_t rate,rateHit,fracGT;

  Int_t days[2] = {0,0};
  Int_t daysPrev[2] = {-1,-1};
  
  for(Int_t it=0;it<2;it++){
    Int_t nentr = tel[it]->GetEntries();
    for(Int_t i=0;i<nentr;i++){
      tel[it]->GetEvent(i);
      day = tel[it]->GetLeaf("day")->GetValue();
      // counts numbre of days
      if(daysPrev[it] != day) daysPrev[it] = day,days[it]++;
    }
  }

  TProfile *hrate[2];
  hrate[0] = new TProfile("hrate1",Form("Trigger rate %s (no GPS events);date;rate (Hz)",sch1),days[0],0,day[0]);
  hrate[1] = new TProfile("hrate2",Form("Trigger rate %s (no GPS events);date;rate (Hz)",sch2),days[1],0,days[1]);
  TProfile *hrateHit[2];
  hrateHit[0] = new TProfile("hrateHit1",Form("Hits rate %s;date;rate (Hz)",sch1),days[0],0,day[0]);
  hrateHit[1] = new TProfile("hrateHit2",Form("Hits rate %s;date;rate (Hz)",sch2),days[1],0,days[1]);
  TProfile *hrateMiss[2];
  hrateMiss[0] = new TProfile("hrateMiss1",Form("(Trigger(no GPS) - Hits rate)/Trigger rate(no GPS) %s;date;rate (Hz)",sch1),days[0],0,day[0]);
  hrateMiss[1] = new TProfile("hrateMiss2",Form("(Trigger(no GPS) - Hits rate)/Trigger rate(no GPS) %s;date;rate (Hz)",sch2),days[1],0,days[1]);
  TProfile *hGTfrac[2];
  hGTfrac[0] = new TProfile("hGTfrac1",Form("Fraction of Good Tracks (#chi^{2} < 10) %s;date;rate (Hz)",sch1),days[0],0,day[0]);
  hGTfrac[1] = new TProfile("hGTfrac2",Form("Fraction of Good Tracks (#chi^{2} < 10) %s;date;rate (Hz)",sch2),days[1],0,days[1]);
  
  

  for(Int_t it=0;it<2;it++){
    Int_t nentr = tel[it]->GetEntries();
    for(Int_t i=0;i<nentr;i++){
      tel[it]->GetEvent(i);
      year = tel[it]->GetLeaf("year")->GetValue();
      month = tel[it]->GetLeaf("month")->GetValue();
      day = tel[it]->GetLeaf("day")->GetValue();
      run = tel[it]->GetLeaf("run")->GetValue();
      time = tel[it]->GetLeaf("timeduration")->GetValue();
      rate = tel[it]->GetLeaf("ratePerRun")->GetValue();
      rateHit = tel[it]->GetLeaf("rateHitPerRun")->GetValue();
      fracGT = tel[it]->GetLeaf("FractionGoodTrack")->GetValue();

      hrate[it]->Fill(Form("%02d/%02d/%02d",day,month,year),rate-2,time);
      hrateHit[it]->Fill(Form("%02d/%02d/%02d",day,month,year),rateHit,time);
      hrateMiss[it]->Fill(Form("%02d/%02d/%02d",day,month,year),(rate-2-rateHit)/(rate-2),time);
      hGTfrac[it]->Fill(Form("%02d/%02d/%02d",day,month,year),fracGT,time);
    }
    hrate[it]->SetMinimum(0);
    hrateHit[it]->SetMinimum(0);
    hrateMiss[it]->SetMinimum(0);
  }

  TCanvas *crate = new TCanvas("crate","rate vs day");
  crate->Divide(2,3);
  crate->cd(1);
  hrate[0]->Draw();
  crate->cd(2);
  hrate[1]->Draw();
  crate->cd(3);
  hrateHit[0]->Draw();
  crate->cd(4);
  hrateHit[1]->Draw();
  crate->cd(5);
  hrateMiss[0]->Draw();
  crate->cd(6);
  hrateMiss[1]->Draw();

  TCanvas *cGTfrac = new TCanvas("cGTfrac","Fraction of Good Tracks");
  cGTfrac->Divide(1,2);
  cGTfrac->cd(1);
  hGTfrac[0]->Draw();
  cGTfrac->cd(2);
  hGTfrac[1]->Draw();

  TFile *fout = new TFile("EEEcoincQA.root","RECREATE");
  crate->Write();
  cGTfrac->Write();
  fout->Close();
}
