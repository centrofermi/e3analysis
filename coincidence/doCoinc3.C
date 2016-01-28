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
const Int_t nbint = 100;
const Float_t tmin = -10000; //ns
const Float_t tmax = 10000; //ns

// periods
Int_t yearRange[2] = {2014,2015};
Int_t monthRange[2] = {1,12};
Int_t dayRange[2] = {1,31};

// thresholds for good runs
Int_t hitevents[3] = {10000,10000,10000};
Float_t fracGT[3] = {0.9,0.9,0.9};
Float_t rateMin[3] = {20,20,20};
Float_t rateMax[3] = {60,60,60};
Float_t minmissingHitFrac[3] = {-1,-1,-1};
Float_t maxmissingHitFrac[3] = {1,1,1};

// thresholds for good events
Float_t maxchisquare = 10;
Float_t maxthetarel = 360;
Float_t timeCutOn12 = 2000;

// telescope settings (Bologna setting as example)
Float_t angle12 = -2.80569863;
Float_t distance12 = 0;
Float_t angle13 = -2.80569863;
Float_t distance13 = 0;

Float_t deltatCorr12 = 0; // knows shift in gps time difference for a given pair of telescopes
Float_t deltatCorr13 = 0; // knows shift in gps time difference for a given pair of telescopes
// extra corrections
Bool_t recomputeThetaRel = kFALSE; // if true correction below are applied to adjust the phi angles of the telescopes
Float_t phi1Corr = 0; // in degrees
Float_t phi2Corr = 0; // in degrees
Float_t phi3Corr = 0; // in degrees

void doCoinc3(const char *fileIn="SAVO-01-SAVO-02-SAVO-03-2016-01-26.root"){
  Int_t adayMin = (yearRange[0]-2007) * 1000 + monthRange[0]*50 + dayRange[0];
  Int_t adayMax = (yearRange[1]-2007) * 1000 + monthRange[1]*50 + dayRange[1];

  // define some histos
  TH1F *hDeltaTheta12 = new TH1F("hDeltaTheta12","#Delta#theta_{12} below the peak (500 ns);#Delta#theta (#circ)",100,-60,60);
  TH1F *hDeltaPhi12 = new TH1F("hDeltaPhi12","#Delta#phi_{12} below the peak (500 ns);#Delta#phi (#circ)",200,-360,360);
  TH1F *hDeltaThetaBack12 = new TH1F("hDeltaThetaBack12","#Delta#theta_{12} out of the peak (> 1000 ns) - normalized;#Delta#theta (#circ)",100,-60,60);
  TH1F *hDeltaPhiBack12 = new TH1F("hDeltaPhiBack12","#Delta#phi_{12} out of the peak (> 1000 ns)  - normalized;#Delta#phi (#circ)",200,-360,360);
  TH1F *hThetaRel12 = new TH1F("hThetaRel12","#theta_{rel}_{12} below the peak (500 ns);#theta_{rel} (#circ)",100,0,120);
  TH1F *hThetaRelBack12 = new TH1F("hThetaRelBack12","#theta_{rel}_{12} out of the peak (> 1000 ns)  - normalized;#theta_{rel} (#circ)",100,0,120);

  TH1F *hDeltaTheta13 = new TH1F("hDeltaTheta13","#Delta#theta_{13} below the peak (500 ns);#Delta#theta (#circ)",100,-60,60);
  TH1F *hDeltaPhi13 = new TH1F("hDeltaPhi13","#Delta#phi_{13} below the peak (500 ns);#Delta#phi (#circ)",200,-360,360);
  TH1F *hDeltaThetaBack13 = new TH1F("hDeltaThetaBack13","#Delta#theta_{13} out of the peak (> 1000 ns) - normalized;#Delta#theta (#circ)",100,-60,60);
  TH1F *hDeltaPhiBack13 = new TH1F("hDeltaPhiBack13","#Delta#phi_{13} out of the peak (> 1000 ns)  - normalized;#Delta#phi (#circ)",200,-360,360);
  TH1F *hThetaRel13 = new TH1F("hThetaRel13","#theta_{rel}_{13} below the peak (500 ns);#theta_{rel} (#circ)",100,0,120);
  TH1F *hThetaRelBack13 = new TH1F("hThetaRelBack13","#theta_{rel}_{13} out of the peak (> 1000 ns)  - normalized;#theta_{rel} (#circ)",100,0,120);


  TFile *f = new TFile(fileIn);
  TTree *t = (TTree *) f->Get("tree");
  
  TTree *tel[3];
  tel[0] = (TTree *) f->Get("treeTel1");
  tel[1] = (TTree *) f->Get("treeTel2");
  tel[2] = (TTree *) f->Get("treeTel3");
  
  TTree *telC = (TTree *) f->Get("treeTimeCommon");
  
  // quality info of runs
  Bool_t runstatus[3][10][12][31][500]; //#telescope, year-2007, month, day, run
  
  if(tel[0] && tel[1] && tel[2]){
    for(Int_t i=0;i < 3;i++){ // loop on telescopes
      for(Int_t j=0;j < tel[i]->GetEntries();j++){ // loop on runs
	tel[i]->GetEvent(j);
	Int_t aday = (tel[i]->GetLeaf("year")->GetValue()-2007) * 1000 + tel[i]->GetLeaf("month")->GetValue()*50 + tel[i]->GetLeaf("day")->GetValue();

	if(aday < adayMin || aday > adayMax) continue;
	if(tel[i]->GetLeaf("FractionGoodTrack")->GetValue() < fracGT[i]) continue; // cut on fraction of good track
	if(tel[i]->GetLeaf("timeduration")->GetValue()*tel[i]->GetLeaf("rateHitPerRun")->GetValue() < hitevents[i]) continue; // cut on the number of event
	if(tel[i]->GetLeaf("ratePerRun")->GetValue() < rateMin[i] || tel[i]->GetLeaf("ratePerRun")->GetValue() > rateMax[i]) continue; // cut on the rate
	if(tel[i]->GetLeaf("run")->GetValue() > 499) continue; // run < 500

	Float_t missinghitfrac = (tel[i]->GetLeaf("ratePerRun")->GetValue()-tel[i]->GetLeaf("rateHitPerRun")->GetValue()-2)/(tel[i]->GetLeaf("ratePerRun")->GetValue()-2);
	if(missinghitfrac < minmissingHitFrac[i] || missinghitfrac > maxmissingHitFrac[i]) continue;

	
	runstatus[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] = kTRUE;
	
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
      
      
      
      if(telC->GetLeaf("run")->GetValue() > 499 || telC->GetLeaf("run2")->GetValue() > 499 || telC->GetLeaf("run3")->GetValue() > 499) continue;
      
      if(!runstatus[0][Int_t(telC->GetLeaf("year")->GetValue())-2007][Int_t(telC->GetLeaf("month")->GetValue())][Int_t(telC->GetLeaf("day")->GetValue())][Int_t(telC->GetLeaf("run")->GetValue())]) continue;
      
      if(!runstatus[1][Int_t(telC->GetLeaf("year")->GetValue())-2007][Int_t(telC->GetLeaf("month")->GetValue())][Int_t(telC->GetLeaf("day")->GetValue())][Int_t(telC->GetLeaf("run2")->GetValue())]) continue;

      if(!runstatus[2][Int_t(telC->GetLeaf("year")->GetValue())-2007][Int_t(telC->GetLeaf("month")->GetValue())][Int_t(telC->GetLeaf("day")->GetValue())][Int_t(telC->GetLeaf("run2")->GetValue())]) continue;
      
      nsecGR += telC->GetLeaf("timeduration")->GetValue(); 
    }
  }
  
  char title[600];
  TH1F *h;
  TH2F *h2;

  sprintf(title,"correction assuming #Delta#phi_{12} = %4.2f, #DeltaL_{12} = %.1f m, #Delta#phi_{13} = %4.2f, #DeltaL_{13} = %.1f m;#Deltat_{13} (ns) when |#Deltat_{12}| < %i ns;entries",angle12,distance12,angle13,distance13,timeCutOn12);
  h = new TH1F("hCoinc",title,nbint,tmin,tmax);
  sprintf(title,"correction assuming #Delta#phi_{12} = %4.2f, #DeltaL_{12} = %.1f m, #Delta#phi_{13} = %4.2f, #DeltaL_{13} = %.1f m;#Deltat_{12} (ns);#Deltat_{13} (ns);entries",angle12,distance12,angle13,distance13,timeCutOn12);
  h2 = new TH2F("hCoinc2D",title,nbint,tmin,tmax,nbint,tmin,tmax);
  
  Float_t DeltaT12,DeltaT13;
  Float_t phiAv,thetaAv,corr12,corr13;
  
  Float_t Theta1,Theta2,Theta3;
  Float_t Phi1,Phi2,Phi3;
  
  Float_t v1[3],v2[3],v3[3],vSP12,vSP13; // variable to recompute ThetaRel on the fly
  
  for(Int_t i=0;i<n;i++){
    t->GetEvent(i);
    
    // if(t->GetLeaf("RunNumber1") && (t->GetLeaf("RunNumber1")->GetValue() > 499 || t->GetLeaf("RunNumber2")->GetValue() > 499) || t->GetLeaf("RunNumber3")->GetValue() > 499)) continue;
  
    // if(tel[0] && !runstatus[0][Int_t(t->GetLeaf("year")->GetValue())-2007][Int_t(t->GetLeaf("month")->GetValue())][Int_t(t->GetLeaf("day")->GetValue())][Int_t(t->GetLeaf("RunNumber1")->GetValue())]) continue;
    
    // if(tel[1] && !runstatus[1][Int_t(t->GetLeaf("year")->GetValue())-2007][Int_t(t->GetLeaf("month")->GetValue())][Int_t(t->GetLeaf("day")->GetValue())][Int_t(t->GetLeaf("RunNumber2")->GetValue())]) continue;
  
    // if(tel[2] && !runstatus[2][Int_t(t->GetLeaf("year")->GetValue())-2007][Int_t(t->GetLeaf("month")->GetValue())][Int_t(t->GetLeaf("day")->GetValue())][Int_t(t->GetLeaf("RunNumber2")->GetValue())]) continue;
    
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

    Float_t thetarel12 = t->GetLeaf("ThetaRel12")->GetValue();
    Float_t thetarel13 = t->GetLeaf("ThetaRel13")->GetValue();
    Theta1 = t->GetLeaf("Theta1")->GetValue()*TMath::DegToRad();
    Theta2 = t->GetLeaf("Theta2")->GetValue()*TMath::DegToRad();
    Theta3 = t->GetLeaf("Theta3")->GetValue()*TMath::DegToRad();
    Phi1 = t->GetLeaf("Phi1")->GetValue()*TMath::DegToRad();
    Phi2 = t->GetLeaf("Phi2")->GetValue()*TMath::DegToRad();
    Phi3 = t->GetLeaf("Phi3")->GetValue()*TMath::DegToRad();
    
    if(recomputeThetaRel){ // recompute ThetaRel applying corrections
      Phi1 -= phi1Corr*TMath::DegToRad();
      Phi2 -= phi2Corr*TMath::DegToRad();
      Phi3 -= phi3Corr*TMath::DegToRad();
      if(Phi1 > 2*TMath::Pi()) Phi1 -= 2*TMath::Pi();
      if(Phi1 < 0) Phi1 += 2*TMath::Pi();
      if(Phi2 > 2*TMath::Pi()) Phi2 -= 2*TMath::Pi();
      if(Phi2 < 0) Phi2 += 2*TMath::Pi();
      if(Phi3 > 2*TMath::Pi()) Phi3 -= 2*TMath::Pi();
      if(Phi3 < 0) Phi3 += 2*TMath::Pi();
      
      v1[0] = TMath::Sin(Theta1)*TMath::Cos(Phi1);
      v1[1] = TMath::Sin(Theta1)*TMath::Sin(Phi1);
      v1[2] = TMath::Cos(Theta1);
      v2[0] = TMath::Sin(Theta2)*TMath::Cos(Phi2);
      v2[1] = TMath::Sin(Theta2)*TMath::Sin(Phi2);
      v2[2] = TMath::Cos(Theta2);
      v3[0] = TMath::Sin(Theta3)*TMath::Cos(Phi3);
      v3[1] = TMath::Sin(Theta3)*TMath::Sin(Phi3);
      v3[2] = TMath::Cos(Theta3);
      
      v2[0] *= v1[0];
      v2[1] *= v1[1];
      v2[2] *= v1[2];
      v3[0] *= v1[0];
      v3[1] *= v1[1];
      v3[2] *= v1[2];
      
      vSP12 = v2[0] + v2[1] + v2[2];
      vSP13 = v3[0] + v3[1] + v3[2];
      
      thetarel12 = TMath::ACos(vSP12)*TMath::RadToDeg();
      thetarel13 = TMath::ACos(vSP13)*TMath::RadToDeg();
    }
    
    // cuts
    if(thetarel12 > maxthetarel) continue;
    if(thetarel13 > maxthetarel) continue;
    if(t->GetLeaf("ChiSquare1")->GetValue() > maxchisquare) continue;
    if(t->GetLeaf("ChiSquare2")->GetValue() > maxchisquare) continue;
    if(t->GetLeaf("ChiSquare3")->GetValue() > maxchisquare) continue;
    
    DeltaT12 = t->GetLeaf("DiffTime12")->GetValue();
    DeltaT13 = t->GetLeaf("DiffTime13")->GetValue();
    
    // get primary direction
    if(TMath::Abs(Phi1-Phi2) < TMath::Pi()) phiAv = (Phi1+Phi2)*0.5;
    else phiAv = (Phi1+Phi2)*0.5 + TMath::Pi();
    if(TMath::Abs(phiAv-Phi3) < TMath::Pi()) phiAv = (phiAv*2+Phi2)*0.33333333333;
    else if(phiAv > Phi3) phiAv = (phiAv*2+Phi3+2*TMath::Pi())*0.33333333333;
    else phiAv = (phiAv*2+4*TMath::Pi()+Phi3)*0.33333333333;


    thetaAv = (Theta1+Theta2+Theta3)*0.333333333333;
    
    // extra cuts if needed
    //    if(TMath::Cos(Phi1-Phi2) < 0.) continue;
    
    corr12 = distance12 * TMath::Sin(thetaAv)*TMath::Cos(phiAv-angle12)/2.99792458000000039e-01 + deltatCorr12;
    corr13 = distance13 * TMath::Sin(thetaAv)*TMath::Cos(phiAv-angle13)/2.99792458000000039e-01 + deltatCorr13;
    
    if(TMath::Abs(DeltaT12-corr12) < timeCutOn12) h->Fill(DeltaT13-corr13);
    h2->Fill(DeltaT12-corr12,DeltaT13-corr13);

    if(TMath::Abs(DeltaT12-corr12) < 500){
      hDeltaTheta12->Fill((Theta1-Theta2)*TMath::RadToDeg());
      hDeltaPhi12->Fill((Phi1-Phi2)*TMath::RadToDeg());
      hThetaRel12->Fill(thetarel12);
    }
    else if(TMath::Abs(DeltaT12-corr12) > 1000 && TMath::Abs(DeltaT12-corr12) < 6000){
      hDeltaThetaBack12->Fill((Theta1-Theta2)*TMath::RadToDeg());
      hDeltaPhiBack12->Fill((Phi1-Phi2)*TMath::RadToDeg());
      hThetaRelBack12->Fill(thetarel12);
    }

    if(TMath::Abs(DeltaT13-corr13) < 500){
      hDeltaTheta13->Fill((Theta1-Theta3)*TMath::RadToDeg());
      hDeltaPhi13->Fill((Phi1-Phi3)*TMath::RadToDeg());
      hThetaRel13->Fill(thetarel13);
    }
    else if(TMath::Abs(DeltaT13-corr13) > 1000 && TMath::Abs(DeltaT13-corr13) < 6000){
      hDeltaThetaBack13->Fill((Theta1-Theta3)*TMath::RadToDeg());
      hDeltaPhiBack13->Fill((Phi1-Phi3)*TMath::RadToDeg());
      hThetaRelBack13->Fill(thetarel13);
    }
  }
  
  h->SetStats(0);

  hDeltaThetaBack12->Sumw2();
  hDeltaPhiBack12->Sumw2();
  hThetaRelBack12->Sumw2();
  hDeltaThetaBack12->Scale(0.1);
  hDeltaPhiBack12->Scale(0.1);
  hThetaRelBack12->Scale(0.1);
  hDeltaThetaBack13->Sumw2();
  hDeltaPhiBack13->Sumw2();
  hThetaRelBack13->Sumw2();
  hDeltaThetaBack13->Scale(0.1);
  hDeltaPhiBack13->Scale(0.1);
  hThetaRelBack13->Scale(0.1);

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
  ff->SetParameter(2,150); // fix witdh if needed
  ff->SetParameter(3,319);
  ff->FixParameter(4,20000./nbint); // bin width
  
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

  TFile *fo = new TFile("output-SAVO-010203.root","RECREATE");
  h->Write();
  h2->Write();
  hDeltaTheta12->Write();
  hDeltaPhi12->Write();
  hThetaRel12->Write();
  hDeltaThetaBack12->Write();
  hDeltaPhiBack12->Write();
  hThetaRelBack12->Write();
  hDeltaTheta13->Write();
  hDeltaPhi13->Write();
  hThetaRel13->Write();
  hDeltaThetaBack13->Write();
  hDeltaPhiBack13->Write();
  hThetaRelBack13->Write();
  fo->Close();
  
}


