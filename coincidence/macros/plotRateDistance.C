plotRateDistance(){
  const Float_t omegaInv = 1./2/TMath::Pi();

  // Corsika MC (eff tel = 80%)
  const Int_t nMC = 10;
  Float_t mrpceff = 0.95;
  Float_t teleff = mrpceff*mrpceff*mrpceff;
  Float_t distMC[nMC] = {200,400,600,800,1000,1200,1400,1600,1800,2000};
  Float_t rateMC[nMC] = {143.767120, 63.287670, 39.794521, 23.150684, 13.356164, 6.917808, 5.753425, 2.739726, 1.849315, 1.849315};
  Float_t edistMC[nMC];
  Float_t erateMC[nMC] = {3.138003, 2.082011, 1.650955, 1.259231, 0.956455, 0.688348, 0.627750, 0.433189, 0.355901, 0.355901};

  for(Int_t i=0;i < nMC;i++){
    edistMC[i] = 0;
    rateMC[i] *= omegaInv*teleff*teleff;
    erateMC[i] *= omegaInv*teleff*teleff;
  }
  TGraphErrors *gMC = new TGraphErrors(10,distMC,rateMC,edistMC,erateMC);
  gMC->SetLineColor(2);
  gMC->SetLineWidth(2);


  const Int_t n = 8;
  Float_t dist[n] = {15,96,204,418,520,627,1075,1182};
  Float_t errdist[n];
  Float_t rates[n] = {2900,143.6,80.5,15.5,18.4,22,7.5,6.2};
  Float_t errrates[n] = {10,2.6,1.2,1.9,2,1.3,1.1};

  Float_t accept[n] = {0.88,0.73,0.73,0.73,0.73,1.02,0.73,0.86};

  Float_t flux[n],errflux[n];

  for(Int_t i=0;i < n;i++){
    flux[i] = rates[i]/accept[i]*omegaInv;
    errflux[i] = errrates[i]/accept[i]*omegaInv;
    errflux[i] = TMath::Sqrt(errflux[i]*errflux[i] + flux[i]*flux[i]*0.2*0.2);
    errdist[i] = dist[i]*0.01;
    errdist[i] = TMath::Sqrt(errdist[i]*errdist[i] + 5*5);
  }

  TH1D *h = new TH1D("h","",1000,0,2000);
  SetHisto(h);

  TGraphErrors *g = new TGraphErrors(n,dist,flux,errdist,errflux);

  TCanvas *c = new TCanvas();

  SetCanvas(c);
  c->SetLogy();
  h->Draw();
  g->Draw("P");
  g->SetMarkerStyle(20);

  h->SetTitle("Rates not corrected for efficiency;distance (m);rate (m^{4} sr day)^{-1}");
  h->SetMaximum(1000);
  h->SetMinimum(0.1);

  DrawLogo();

  TF1 *f = new TF1("f","[0]*TMath::Power(x*0.001,[1])",0,3000);
  f->SetLineColor(4);
  f->SetParameter(0,1000);
  f->SetParameter(1,-1.4);
  g->Fit(f);
  ((TF1 *) g->GetListOfFunctions()->At(0))->SetRange(0,2000);
  gMC->Draw("L");

  TLegend *leg = new TLegend(0.3,0.59,0.94,0.89);
  leg->SetBorderSize(0);
  leg->SetHeader(Form("#Chi^{2}/(n.d.f.) = %.2f",f->GetChisquare()/f->GetNDF()));
  leg->SetFillStyle(0);
  leg->AddEntry(g,"Data Run-3","LP");
  leg->AddEntry(f,Form("fit: (%.1f #pm %.1f) (x/km)^{%.2f #pm %.2f}",f->GetParameter(0)+0.005,f->GetParError(0)+0.005,f->GetParameter(1)+0.005,f->GetParError(1)+0.005),"L");
  leg->AddEntry(gMC,Form("Corsika: 10^{5}<E<10^{7} GeV, #varepsilon_{MRPC}=%.2f",mrpceff),"L");
  leg->Draw("SAME");


}


void DrawLogo(Float_t x = 0.75,Float_t y = 0.99,Int_t Pilot=1){
  TLatex *eeesh = new TLatex(x+0.0075,y-0.0075,"EEE");
  eeesh->SetTextSize(31);
  TLatex *eee = new TLatex(x,y,"EEE");
  eee->SetTextSize(0.05*1.5);
  eeesh->SetTextSize(0.05*1.5);
  eee->SetTextFont(42);
  eeesh->SetTextFont(42);

  eeesh->SetTextColor(kGray);
  eee->SetTextColor(kBlue);

  eee->SetTextAlign(33);
  eeesh->SetTextAlign(33);

  eee->SetNDC(1);
  eeesh->SetNDC(1);


  eeesh->Draw();
  eee->Draw();

  TLatex *sub = new TLatex(x,y,"#splitline{Extreme Energy Events}{La Scienza nelle Scuole}");
  sub->SetTextSize(0.05*0.6);
  sub->SetTextFont(42);
  sub->SetTextColor(kBlue);
  sub->SetNDC(1);
  sub->SetTextAlign(13);
  sub->Draw();

  TLatex *pre = new TLatex(x,y-0.1,"preliminary");
  pre->SetTextSize(0.05*1.2);
  pre->SetTextFont(42);
  pre->SetTextColor(kBlue);
  pre->SetNDC(1);
  pre->SetTextAlign(13);
  pre->Draw();

  return;

  TLatex *run;
  if(Pilot==0) run = new TLatex(x+0.1,y-0.17,"Run-0 + Run-1 + Run-2 + Run-3");
  else if(Pilot==1) run = new TLatex(x+0.1,y-0.17,"Run-1 + Run-2 + Run3");
  else if(Pilot==2) run = new TLatex(x+0.1,y-0.17,"Run-2 + Run-3");
  else if(Pilot==3) run = new TLatex(x+0.1,y-0.17,"Run-3");
  else if(Pilot==-1) run = new TLatex(x+0.1,y-0.17,"Run-1");
  else if(Pilot==-2) run = new TLatex(x+0.1,y-0.17,"Run-2");
  else if(Pilot==-3) run = new TLatex(x+0.1,y-0.17,"Run-3");
  run->SetTextSize(0.05*0.75);
  run->SetTextFont(42);
  run->SetTextColor(kBlue);
  run->SetNDC(1);
  run->SetTextAlign(23);
  run->Draw();
}

void SetCanvas(TCanvas *c){
  c->SetLeftMargin(0.15);
  c->SetRightMargin(0.05);
  c->SetBottomMargin(0.15);
  c->SetTopMargin(0.1);
}

void SetHisto(TH1 *h){
  h->SetStats(0);
  char titolo[400];

  h->GetXaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetTitleSize(0.05);

  h->GetXaxis()->SetNdivisions(409);
  h->GetYaxis()->SetNdivisions(409);

  h->GetXaxis()->SetTitleOffset(1.2);
  h->GetYaxis()->SetTitleOffset(1.2);

//   h->SetMaximum(Int_t(h->GetMaximum()/50*1.025 + 1) * 50);
//   h->SetMinimum(Int_t(h->GetMinimum()/50 - 1) * 50);

}

