{
  const Int_t n = 9;
  Float_t dist[n] = {15,96,167,204,418,520,627,1075,1182};
  Float_t errdist[n];
  Float_t rates[n] = {2900,143.6,135,80.5,15.5,18.4,22,7.5,6.2};
  Float_t errrates[n] = {10,2.6,2,1.3,1.2,1.9,2,1.3,1.1};

  Float_t accept[n] = {0.88,0.73,0.73,0.73,0.73,0.73,1.02,0.73,0.86};

  Float_t flux[n],errflux[n];

  for(Int_t i=0;i < n;i++){
    flux[i] = rates[i]/accept[i];
    errflux[i] = errrates[i]/accept[i];
    errflux[i] = TMath::Sqrt(errflux[i]*errflux[i] + flux[i]*flux[i]*0.2*0.2);
    errdist[i] = dist[i]*0.01;
    errdist[i] = TMath::Sqrt(errdist[i]*errdist[i] + 5*5);
  }

  TH1D *h = new TH1D("h","",10000,0,3000);
  TGraphErrors *g = new TGraphErrors(n,dist,flux,errdist,errflux);

  h->Draw();
  g->Draw("P");
  g->SetMarkerStyle(20);

  h->SetTitle("Rate not corrected for efficiency (20% syst);distance (m);rate (m^{4} sr day)^{-1}");
  h->SetMaximum(4000);
}
