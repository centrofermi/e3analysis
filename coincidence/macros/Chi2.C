{
  const int n = 5;
  Float_t allLodi = 18796;
  Float_t allBolo = 10656;

  Float_t x[n] = {50,100,200,400,800};
  Float_t ex[n],ey[n];
  Float_t y[n] = {13462,14669,15762,16885,17917};

  Float_t ey2[n];
  Float_t y2[n] = {9119,9510,9850,10121,10377};

  for(Int_t i=0;i < n;i++){
    y[i] /= allLodi;
    y2[i] /= allBolo;
    ex[i] = 0;
    ey[i] = TMath::Sqrt(y[i]*(1-y[i])/allLodi);
    ey2[i] = TMath::Sqrt(y2[i]*(1-y2[i])/allBolo);
    
  }

  TH1D *h = new TH1D("h",";#chi^{2}-max;N(#chi^{2}<#chi^{2}-max)/N(no cuts)",100,0,1000);
  h->Draw();
  h->SetStats(0);
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetTitleSize(0.05);
  h->GetXaxis()->SetNdivisions(409);
  h->GetYaxis()->SetNdivisions(409);
  h->SetMinimum(0);
  h->SetMaximum(1);

  TGraphErrors *g = new TGraphErrors(n,x,y,ex,ey);
  g->Draw("P");
  g->SetMarkerStyle(20);

  TGraphErrors *g2 = new TGraphErrors(n,x,y2,ex,ey2);
  g2->Draw("P");
  g2->SetMarkerStyle(21);
  g2->SetMarkerColor(2);
  g2->SetLineColor(2);

  TLegend *leg = new TLegend(0.6,0.3,0.85,0.5);
  leg->SetFillStyle(0);
  leg->AddEntry(g,"LODI","LP");
  leg->AddEntry(g2,"BOLO","LP");
  leg->Draw("SAME");
}
