#include<stdio.h>
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TF2.h"
#include "TLeaf.h"
#include "TMath.h"
#include "TTree.h"
#include "TLegend.h"
#include "TPaveText.h"

// setting for histos
const Int_t nbint = 49;
const Float_t tmin = -10000; //ns
const Float_t tmax = 10000; //ns

// periods
Int_t yearRange[2] = {2014,2016};
Int_t monthRange[2] = {1,12};
Int_t dayRange[2] = {1,31};

// thresholds for good runs
Int_t hitevents[2] = {10000,10000};
Float_t fracGT[2] = {0.9,0.9};
Float_t rateMin[2] = {10,10};
Float_t rateMax[2] = {70,70};
Float_t minmissingHitFrac[2] = {-1,-1};
Float_t maxmissingHitFrac[2] = {1,1};

// experiment to autocorrect for efficiency variation
Float_t refRate[2] = {23,23};

// thresholds for good events
Float_t maxchisquare = 10;
Float_t maxthetarel = 30;

// telescope settings
Float_t angle = 177.78; //deg
Float_t distance=1344;

Float_t deltatCorr = 0; // knows shift in gps time difference for a given pair of telescopes (bolo 2000)
// extra corrections
Bool_t recomputeThetaRel = kTRUE; // if true correction below are applied to adjust the phi angles of the telescopes
Float_t phi1Corr = 0; // in degrees
Float_t phi2Corr = 0; // in degrees (bolo -28)

void doCoincVIAR_01_02(const char *fileIn="coincVIAR_0102.root"){
  Int_t adayMin = (yearRange[0]-2007) * 1000 + monthRange[0]*50 + dayRange[0];
  Int_t adayMax = (yearRange[1]-2007) * 1000 + monthRange[1]*50 + dayRange[1];

  angle *= TMath::DegToRad();

  // define some histos
  TH1F *hDeltaTheta = new TH1F("hDeltaTheta","#Delta#theta below the peak (500 ns);#Delta#theta (#circ)",100,-60,60);
  TH1F *hDeltaPhi = new TH1F("hDeltaPhi","#Delta#phi below the peak (500 ns);#Delta#phi (#circ)",200,-360,360);
  TH1F *hDeltaThetaBack = new TH1F("hDeltaThetaBack","#Delta#theta out of the peak (> 1000 ns) - normalized;#Delta#theta (#circ)",100,-60,60);
  TH1F *hDeltaPhiBack = new TH1F("hDeltaPhiBack","#Delta#phi out of the peak (> 1000 ns)  - normalized;#Delta#phi (#circ)",200,-360,360);
  TH1F *hThetaRel = new TH1F("hThetaRel","#theta_{rel} below the peak (500 ns);#theta_{rel} (#circ)",100,0,120);
  TH1F *hThetaRelBack = new TH1F("hThetaRelBack","#theta_{rel} out of the peak (> 1000 ns)  - normalized;#theta_{rel} (#circ)",100,0,120);

  TH2F *hAngle = new TH2F("hAngle",";#Delta#theta (#circ);#Delta#phi (#circ}",20,-60,60,20,-360,360);
  TH2F *hAngleBack = new TH2F("hAngleBack",";#Delta#theta (#circ);#Delta#phi (#circ}",20,-60,60,20,-360,360);


  TFile *f = new TFile(fileIn);
  TTree *t = (TTree *) f->Get("tree");
  
  TTree *tel[2];
  tel[0] = (TTree *) f->Get("treeTel1");
  tel[1] = (TTree *) f->Get("treeTel2");
  
  TTree *telC = (TTree *) f->Get("treeTimeCommon");
  
  // quality info of runs
  Bool_t runstatus[2][10][12][31][500]; //#telescope, year-2007, month, day, run
  Float_t effTel[2][10][12][31][500];

  Float_t rateGT;

  if(tel[0] && tel[1]){
    for(Int_t i=0;i < 2;i++){ // loop on telescopes
      for(Int_t j=0;j < tel[i]->GetEntries();j++){ // loop on runs
	tel[i]->GetEvent(j);
	rateGT = tel[i]->GetLeaf("FractionGoodTrack")->GetValue()*tel[i]->GetLeaf("rateHitPerRun")->GetValue();

	Int_t aday = (tel[i]->GetLeaf("year")->GetValue()-2007) * 1000 + tel[i]->GetLeaf("month")->GetValue()*50 + tel[i]->GetLeaf("day")->GetValue();

	if(aday < adayMin || aday > adayMax) continue;
	if(tel[i]->GetLeaf("FractionGoodTrack")->GetValue() < fracGT[i]) continue; // cut on fraction of good track
	if(tel[i]->GetLeaf("timeduration")->GetValue()*tel[i]->GetLeaf("rateHitPerRun")->GetValue() < hitevents[i]) continue; // cut on the number of event
	if(rateGT < rateMin[i] || rateGT > rateMax[i]) continue; // cut on the rate
	if(tel[i]->GetLeaf("run")->GetValue() > 499) continue; // run < 500

	Float_t missinghitfrac = (tel[i]->GetLeaf("ratePerRun")->GetValue()-tel[i]->GetLeaf("rateHitPerRun")->GetValue()-2)/(tel[i]->GetLeaf("ratePerRun")->GetValue()-2);
	if(missinghitfrac < minmissingHitFrac[i] || missinghitfrac > maxmissingHitFrac[i]) continue;

	
	runstatus[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] = kTRUE;
	effTel[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] = 1;//rateGT/refRate[i];
	

      }
    }
  }
  else{
    telC = NULL;
  }
  
  Int_t n = t->GetEntries();

  // counter for seconds
  Int_t nsec = 0;
  Int_t nsecGR = 0; // for good runs
  Int_t isec = -1; // used only in case the tree with time info is not available

  if(telC){
    for(Int_t i=0; i < telC->GetEntries();i++){
      telC->GetEvent(i);
      nsec += telC->GetLeaf("timeduration")->GetValue(); 
      
      
      
      if(telC->GetLeaf("run")->GetValue() > 499 || telC->GetLeaf("run2")->GetValue() > 499) continue;
      
      if(!runstatus[0][Int_t(telC->GetLeaf("year")->GetValue())-2007][Int_t(telC->GetLeaf("month")->GetValue())][Int_t(telC->GetLeaf("day")->GetValue())][Int_t(telC->GetLeaf("run")->GetValue())]) continue;
      
      if(!runstatus[1][Int_t(telC->GetLeaf("year")->GetValue())-2007][Int_t(telC->GetLeaf("month")->GetValue())][Int_t(telC->GetLeaf("day")->GetValue())][Int_t(telC->GetLeaf("run2")->GetValue())]) continue;
      
      nsecGR += telC->GetLeaf("timeduration")->GetValue(); 
    }
  }
  
  char title[300];
  TH1F *h;
  
  sprintf(title,"correction assuming #Delta#phi = %4.2f, #DeltaL = %.1f m;#Deltat (ns);entries",angle,distance);
  
  h = new TH1F("hCoinc",title,nbint,tmin,tmax);
  
  Float_t DeltaT;
  Float_t phiAv,thetaAv,corr;
  
  Float_t Theta1,Theta2;
  Float_t Phi1,Phi2;
  
  Float_t v1[3],v2[3],vSP; // variable to recompute ThetaRel on the fly
  Float_t eff = 1; 
  
  for(Int_t i=0;i<n;i++){
    t->GetEvent(i);
    
    if(t->GetLeaf("RunNumber1") && (t->GetLeaf("RunNumber1")->GetValue() > 499 || t->GetLeaf("RunNumber2")->GetValue() > 499)) continue;
  
    if(tel[0] && !runstatus[0][Int_t(t->GetLeaf("year")->GetValue())-2007][Int_t(t->GetLeaf("month")->GetValue())][Int_t(t->GetLeaf("day")->GetValue())][Int_t(t->GetLeaf("RunNumber1")->GetValue())]) continue;
    
    if(tel[1] && !runstatus[1][Int_t(t->GetLeaf("year")->GetValue())-2007][Int_t(t->GetLeaf("month")->GetValue())][Int_t(t->GetLeaf("day")->GetValue())][Int_t(t->GetLeaf("RunNumber2")->GetValue())]) continue;


    eff = effTel[0][Int_t(t->GetLeaf("year")->GetValue())-2007][Int_t(t->GetLeaf("month")->GetValue())][Int_t(t->GetLeaf("day")->GetValue())][Int_t(t->GetLeaf("RunNumber1")->GetValue())];
    eff *= effTel[1][Int_t(t->GetLeaf("year")->GetValue())-2007][Int_t(t->GetLeaf("month")->GetValue())][Int_t(t->GetLeaf("day")->GetValue())][Int_t(t->GetLeaf("RunNumber2")->GetValue())];
    
    Int_t timec = t->GetLeaf("ctime1")->GetValue();
    
    if(! telC){
      if(isec == -1) isec = timec;
      
      if(timec != isec){
	if(timec - isec < 20){
	  //	printf("diff = %i\n",timec-isec);
	  nsec +=(timec - isec);
	  nsecGR +=(timec - isec);
	}
	isec = timec;
    }
    }

    Float_t thetarel = t->GetLeaf("ThetaRel")->GetValue();
    Theta1 = (t->GetLeaf("Theta1")->GetValue())*TMath::DegToRad();
    Theta2 = t->GetLeaf("Theta2")->GetValue()*TMath::DegToRad();
    Phi1 = t->GetLeaf("Phi1")->GetValue()*TMath::DegToRad();
    Phi2 = t->GetLeaf("Phi2")->GetValue()*TMath::DegToRad();
    
    if(recomputeThetaRel){ // recompute ThetaRel applying corrections
      Phi1 -= phi1Corr*TMath::DegToRad();
      Phi2 -= phi2Corr*TMath::DegToRad();
      if(Phi1 > 2*TMath::Pi()) Phi1 -= 2*TMath::Pi();
      if(Phi1 < 0) Phi1 += 2*TMath::Pi();
      if(Phi2 > 2*TMath::Pi()) Phi2 -= 2*TMath::Pi();
      if(Phi2 < 0) Phi2 += 2*TMath::Pi();
      
      v1[0] = TMath::Sin(Theta1)*TMath::Cos(Phi1);
      v1[1] = TMath::Sin(Theta1)*TMath::Sin(Phi1);
      v1[2] = TMath::Cos(Theta1);
      v2[0] = TMath::Sin(Theta2)*TMath::Cos(Phi2);
      v2[1] = TMath::Sin(Theta2)*TMath::Sin(Phi2);
      v2[2] = TMath::Cos(Theta2);
      
      v1[0] *= v2[0];
      v1[1] *= v2[1];
      v1[2] *= v2[2];
      
      vSP = v1[0] + v1[1] + v1[2];
      
      thetarel = TMath::ACos(vSP)*TMath::RadToDeg();
    }
    
    // cuts
    if(thetarel > maxthetarel) continue;
    if(t->GetLeaf("ChiSquare1")->GetValue() > maxchisquare) continue;
    if(t->GetLeaf("ChiSquare2")->GetValue() > maxchisquare) continue;
    
    
    DeltaT = t->GetLeaf("DiffTime")->GetValue();
    
    // get primary direction
    if(TMath::Abs(Phi1-Phi2) < TMath::Pi()) phiAv = (Phi1+Phi2)*0.5;
    else phiAv = (Phi1+Phi2)*0.5 + TMath::Pi();
    thetaAv = (Theta1+Theta2)*0.5;
    
    // extra cuts if needed
    //    if(TMath::Cos(Phi1-Phi2) < 0.) continue;
    
    corr = distance * TMath::Sin(thetaAv)*TMath::Cos(phiAv-angle)/2.99792458000000039e-01 + deltatCorr;
    
    h->Fill(DeltaT-corr,1./eff);
    if(TMath::Abs(DeltaT-corr) < 500){
      hDeltaTheta->Fill((Theta1-Theta2)*TMath::RadToDeg());
      hDeltaPhi->Fill((Phi1-Phi2)*TMath::RadToDeg());
      hThetaRel->Fill(thetarel);
      hAngle->Fill((Theta1-Theta2)*TMath::RadToDeg(),(Phi1-Phi2)*TMath::RadToDeg());
    }
    else if(TMath::Abs(DeltaT-corr) > 1000 && TMath::Abs(DeltaT-corr) < 6000){
      hDeltaThetaBack->Fill((Theta1-Theta2)*TMath::RadToDeg());
      hDeltaPhiBack->Fill((Phi1-Phi2)*TMath::RadToDeg());
      hThetaRelBack->Fill(thetarel);
      hAngleBack->Fill((Theta1-Theta2)*TMath::RadToDeg(),(Phi1-Phi2)*TMath::RadToDeg());
    }
  }
  
  h->SetStats(0);

  hDeltaThetaBack->Sumw2();
  hDeltaPhiBack->Sumw2();
  hThetaRelBack->Sumw2();
  hDeltaThetaBack->Scale(0.1);
  hDeltaPhiBack->Scale(0.1);
  hThetaRelBack->Scale(0.1);
  hAngleBack->Scale(0.1);
  hAngle->Add(hAngleBack,-1);


  Float_t val,eval;
  TCanvas *c1=new TCanvas();
  TF1 *ff = new TF1("ff","[0]*[4]/[2]/sqrt(2*TMath::Pi())*TMath::Exp(-(x-[1])*(x-[1])*0.5/[2]/[2]) + [3]*[4]/6/[2]");
  ff->SetParName(0,"signal");
  ff->SetParName(1,"mean");
  ff->SetParName(2,"sigma");
  ff->SetParName(3,"background");
  ff->SetParName(4,"bin width");
  ff->SetParameter(0,42369);
  ff->SetParameter(1,0);
  ff->SetParLimits(2,10,1000);
  ff->SetParameter(2,350); // fix witdh if needed
  ff->SetParameter(3,319);
  ff->FixParameter(4,(tmax-tmin)/nbint); // bin width

  ff->SetNpx(1000);
  
  h->Fit(ff);
  
  val = ff->GetParameter(2);
  eval = ff->GetParError(2);
  
  printf("significance = %f\n",ff->GetParameter(0)/sqrt(ff->GetParameter(0) + ff->GetParameter(3)));

  h->Draw();
  
  TF1 *func1 = (TF1 *)  h->GetListOfFunctions()->At(0);
  
  func1->SetLineColor(2);
  h->SetLineColor(4);
  
  TPaveText *text = new TPaveText(1500,(h->GetMinimum()+(h->GetMaximum()-h->GetMinimum())*0.6),9500,h->GetMaximum());
  text->SetFillColor(0);
  sprintf(title,"width = %5.1f #pm %5.1f",func1->GetParameter(2),func1->GetParError(2));
  text->AddText(title);
  sprintf(title,"signal (S) = %5.1f #pm %5.1f",func1->GetParameter(0),func1->GetParError(0));
  text->AddText(title);
  sprintf(title,"background (B) (3#sigma) = %5.1f #pm %5.1f",func1->GetParameter(3),func1->GetParError(3));
  text->AddText(title);
  sprintf(title,"significance (S/#sqrt{S+B}) = %5.1f",func1->GetParameter(0)/sqrt(func1->GetParameter(0)+func1->GetParameter(3)));
  text->AddText(title);
  
  text->SetFillStyle(0);
  text->SetBorderSize(0);
  
  text->Draw("SAME");
  
  printf("n_day = %f\nn_dayGR = %f\n",nsec*1./86400,nsecGR*1./86400);

  text->AddText(Form("rate = %f #pm %f per day",func1->GetParameter(0)*86400/nsecGR,func1->GetParError(0)*86400/nsecGR));

  TFile *fo = new TFile("outputVIAR-01-02.root","RECREATE");
  h->Write();
  hDeltaTheta->Write();
  hDeltaPhi->Write();
  hThetaRel->Write();
  hDeltaThetaBack->Write();
  hDeltaPhiBack->Write();
  hThetaRelBack->Write();
  hAngle->Write();
  fo->Close();
  
}


