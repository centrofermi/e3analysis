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

Int_t countBits(Int_t word);

// (1)
// setting for histos
const Int_t nbint = 200;
const Float_t tmin = -10000; //ns
const Float_t tmax = 10000; //ns

// (2)
// periods
Int_t yearRange[2] = {2014,2016};
Int_t monthRange[2] = {1,12};
Int_t dayRange[2] = {1,31};

// thresholds for good runs
Int_t hitevents[2] = {10000,10000};
Float_t fracGT[2] = {0.8,0.8};
Float_t rateMin[2] = {10,10};
Float_t rateMax[2] = {70,70};

// old cut on the missing rate (replaced by the number of dead strips)
Float_t minmissingHitFrac[2] = {-1,-1};
Float_t maxmissingHitFrac[2] = {1,1};

// number of deadstrips allowed
Int_t ndeadBotMax[2] = {23,23};
Int_t ndeadBotMin[2] = {0,0};
Int_t ndeadMidMax[2] = {23,23};
Int_t ndeadMidMin[2] = {0,0};
Int_t ndeadTopMax[2] = {23,23};
Int_t ndeadTopMin[2] = {0,0};

// requirement on the number of satellites in the run (average)
Float_t minAvSat[2] = {0.,0.};
Float_t maxAvSat[2] = {10,10};

// time difference between weather info and the start of the run (it is negative!) allowed (in seconds)
Int_t minWeathTimeDelay[2] = {-999999,-999999};
Int_t maxWeathTimeDelay[2] = {1,1};

// experiment to autocorrect for efficiency variation
Float_t refRate[2] = {23,23};

// (3)
// thresholds for good events
Float_t maxchisquare = 10*10/2;
Float_t maxthetarel = 360;
Int_t satEventThr = 7; // minimum number of sattellite required in each event

// (4)
// telescope settings
Float_t angle = 72.1926821864; //deg
Float_t distance=520;

Float_t deltatCorr = -150; // knows shift in gps time difference for a given pair of telescopes (bolo 2000)
// extra corrections
Bool_t recomputeThetaRel = kTRUE; // if true correction below are applied to adjust the phi angles of the telescopes
Float_t phi1Corr = 236; // in degrees (the one stored in the header)
Float_t phi2Corr = 100; // in degrees

void doCoincCAGL_01_02new(const char *fileIn="coincCAGL_0102n.root"){
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
  Int_t nStripDeadBot[2][10][12][31][500];
  Int_t nStripDeadMid[2][10][12][31][500];
  Int_t nStripDeadTop[2][10][12][31][500];

  // sat info
  Float_t NsatAv[2][10][12][31][500];

  // weather info
  Float_t pressureTel[2][10][12][31][500];
  Float_t TempInTel[2][10][12][31][500];
  Float_t TempOutTel[2][10][12][31][500];
  Float_t timeWeath[2][10][12][31][500];

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
		
	// active strip maps
	if(tel[i]->GetLeaf("maskB")) nStripDeadBot[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] = countBits(Int_t(tel[i]->GetLeaf("maskB")->GetValue()));
	if(tel[i]->GetLeaf("maskM")) nStripDeadMid[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] = countBits(Int_t(tel[i]->GetLeaf("maskM")->GetValue()));
	if(tel[i]->GetLeaf("maskT")) nStripDeadTop[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] = countBits(Int_t(tel[i]->GetLeaf("maskT")->GetValue()));

	if(nStripDeadBot[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] > ndeadBotMax[i] || nStripDeadBot[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] < ndeadBotMin[i]) continue;
	if(nStripDeadMid[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] > ndeadMidMax[i] || nStripDeadMid[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] < ndeadMidMin[i]) continue;
	if(nStripDeadTop[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] > ndeadTopMax[i] || nStripDeadTop[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] < ndeadTopMin[i]) continue;

	// nsat averaged  per run
	if(tel[i]->GetLeaf("nSat")) NsatAv[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] = tel[i]->GetLeaf("nSat")->GetValue();


	if(NsatAv[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] < minAvSat[i] || NsatAv[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] > maxAvSat[i]) continue;

	// weather info
	if(tel[i]->GetLeaf("Pressure")) pressureTel[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] = tel[i]->GetLeaf("Pressure")->GetValue();
	if(tel[i]->GetLeaf("IndoorTemperature")) TempInTel[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] = tel[i]->GetLeaf("IndoorTemperature")->GetValue();
	if(tel[i]->GetLeaf("OutdoorTemperature")) TempOutTel[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] = tel[i]->GetLeaf("OutdoorTemperature")->GetValue();
	if(tel[i]->GetLeaf("TimeWeatherUpdate")) timeWeath[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] = tel[i]->GetLeaf("TimeWeatherUpdate")->GetValue();

	if(timeWeath[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] < minWeathTimeDelay[i] ||  timeWeath[i][Int_t(tel[i]->GetLeaf("year")->GetValue())-2007][Int_t(tel[i]->GetLeaf("month")->GetValue())][Int_t(tel[i]->GetLeaf("day")->GetValue())][Int_t(tel[i]->GetLeaf("run")->GetValue())] > maxWeathTimeDelay[i]) continue;

	// Set good runs
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

  Float_t neventsGR = 0;
  Float_t neventsGRandSat = 0;

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
  Int_t nsatel1cur,nsatel2cur,ntrack1,ntrack2;

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
    
    nsatel1cur = t->GetLeaf("Nsatellite1")->GetValue();
    nsatel2cur = t->GetLeaf("Nsatellite2")->GetValue();
    ntrack1 = t->GetLeaf("Ntracks1")->GetValue();
    ntrack2 = t->GetLeaf("Ntracks2")->GetValue();

    if(recomputeThetaRel){ // recompute ThetaRel applying corrections
      Phi1 += phi1Corr*TMath::DegToRad();
      Phi2 += phi2Corr*TMath::DegToRad();
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
    

    neventsGR++;

    // reject events with not enough satellites
    if(nsatel1cur < satEventThr || nsatel1cur < satEventThr) continue;

    neventsGRandSat++;
    
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
  
  // correct nsecGR for the event rejected because of the number of satellites (event by event cut)
  nsecGR *= neventsGRandSat/neventsGR;

  printf("n_day = %f\nn_dayGR = %f\n",nsec*1./86400,nsecGR*1./86400);

  text->AddText(Form("rate = %f #pm %f per day",func1->GetParameter(0)*86400/nsecGR,func1->GetParError(0)*86400/nsecGR));

  TFile *fo = new TFile("outputCAGL-01-02.root","RECREATE");
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

Int_t countBits(Int_t word){
  Int_t nactive = 0;
  while (word > 0) {           // until all bits are zero
    if ((word & 1) == 1)     // check lower bit
      nactive++;
    word >>= 1;              // shift bits, removing lower bit
  }
  return nactive;
}
