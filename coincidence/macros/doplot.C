doplot(){
  const Int_t n = 8;
  Float_t dist[n] = {15,96,204,418,520,627,1075,1182};
  Float_t errdist[n];
  Float_t rates[n] = {3904,133,80.5,23.9,13.2,22,7.9,6.2};
  Float_t errrates[n] = {54,2.5,1.3,3.7,0.6,2,1.3,1.1};

  Float_t accept[n] = {0.88,0.62,0.73,0.68,0.43,1.02,0.73,0.86};

  Float_t flux[n],errflux[n];

  for(Int_t i=0;i < n;i++){
    flux[i] = rates[i]/accept[i];
    errflux[i] = errrates[i]/accept[i];
    errflux[i] = TMath::Sqrt(errflux[i]*errflux[i]);// + flux[i]*flux[i]*0.2*0.2);
    errdist[i] = dist[i]*0.01;
    errdist[i] = TMath::Sqrt(errdist[i]*errdist[i] + 5*5);
  }

  TH1D *h = new TH1D("h","",600,0,1200);
  SetHisto(h);

  TGraphErrors *g = new TGraphErrors(n,dist,flux,errdist,errflux);

  TCanvas *c = new TCanvas("c1","c1",1000,800);
  c->Divide(1,2);
  c->Update();
  c->cd(1)->SetPad(0., 0.2, 1, 1);
  c->cd(2)->SetPad(0, 0., 1, 0.2 );

  SetCanvas(c->cd(1));
  SetCanvas(c->cd(2));

  c->cd(1)->SetLogy();
  h->Draw();
  g->Draw("P");
  g->SetMarkerStyle(20);

  h->SetTitle("Rates not corrected for efficiency;distance (m);rate (m^{4} sr day)^{-1}");
  h->SetMaximum(5000);
  h->SetMinimum(5);

  const Int_t nOld = 8;
  Float_t x[] = {15,96,204,418,520,627, 1075,1182};
  Float_t ex[] = {0,0,0,0,0,0,0,0};
  Float_t y[] = {2593,120.5,58.8,20,11.8,17.6,7,5.2};
  Float_t ey[] = {67,1.3,1.2,3.7,0.6,1.6,1.2,0.6};

  Float_t ratio[nOld],erratio[nOld];

  Float_t acc[] = {0.88,0.62,0.73,0.68,0.43,1.02,0.73,0.86};

  for(Int_t i=0;i<nOld;i++){
    y[i] /= acc[i];
    ey[i] /= acc[i];
    ey[i] = sqrt(ey[i]*ey[i]);// + y[i]*y[i]*0.2*0.2);

    Int_t j = i;

    ratio[i] = flux[j]/y[i];
    erratio[i] = sqrt(errflux[j]*errflux[j]/flux[j]/flux[j] + ey[i]*ey[i]/y[i]/y[i]);

    if(accept[j] != acc[i]) printf("Acceptance changed in Run-3 (d=%fm) from %f to %f\n",x[i],acc[i],accept[j]);
  }

  TGraphErrors *g2 = new TGraphErrors(nOld,x,y,ex,ey);
  g2->Draw("P");
  g2->SetMarkerStyle(21);
  g2->SetMarkerColor(2);
  g2->SetLineColor(2);

  TLegend *leg = new TLegend(0.6,0.5,0.9,0.8);
  leg->SetFillStyle(0);
  leg->AddEntry(g,"Run-2 new analyzer","LP");
  leg->AddEntry(g2,"Run-2 old analyzer","LP");
  leg->Draw("SAME");
  
  //  DrawLogo();

  c->cd(2)->SetGridy();
  c->cd(2)->SetBottomMargin(0.3);

  TH1D *h2 = new TH1D("h2","",600,0,1200);
  SetHisto(h2);
  h2->SetTitle(";distance (m);ratio");
  h2->SetMaximum(2);
  h2->SetMinimum(0);
  h2->Draw();

  h2->GetXaxis()->SetTitleSize(0.2);
  h2->GetYaxis()->SetTitleSize(0.2);

  h2->GetXaxis()->SetLabelSize(0.15);
  h2->GetYaxis()->SetLabelSize(0.15);

  h2->GetXaxis()->SetNdivisions(409);
  h2->GetYaxis()->SetNdivisions(409);

  h2->GetXaxis()->SetTitleOffset(0.7);
  h2->GetYaxis()->SetTitleOffset(0.28);

  TGraphErrors *g2 = new TGraphErrors(nOld,x,ratio,ex,erratio);
  g2->SetMarkerStyle(20);
  g2->Draw("P");

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

void SetCanvas(TVirtualPad *c){
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

