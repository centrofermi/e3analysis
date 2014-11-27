
// Allego anche la macro savodt.C che ho utilizzato per produrre i plot
// dell'analisi dei decadimenti del Mu root -l savodt.C
// (necessario creare la cartella png_cl in cui vanno i files)

//#include "slices.C"

void savodt() {
 gROOT->Reset(); 


gStyle->SetCanvasColor(kWhite);
gStyle->SetPadGridY(kTRUE);
gStyle->SetPadGridX(kTRUE);

gStyle->SetOptStat(0);
// gStyle->SetOptStat(1111);
// gStyle->SetOptFit(1);
  gStyle->SetOptFit(0);
  gStyle->SetStatX(0.87);
  gStyle->SetStatH(0.08);
   gStyle->SetStatY(0.95);
 // gStyle->SetStatW(0.19);

gStyle->SetLabelSize(0.045);

gStyle->SetNumberContours(511);
gStyle->SetPalette(1);
 gStyle->SetErrorX(0.2);

char nfile[500];

 int nrun;
 TChain* ch = new TChain("Events");
  ch->Add("ADDED/SAVO*.root");
  ch->Add("ADDED/FRA*.root");
  //  ch->Add("ADDED/BAR*.root");
 Events->SetAlias("beta","1./((TimeOfFlight/TrackLength)*29.97)");
 double x[1000];
 double y[1000];

 TCanvas* cplot = new TCanvas("cplot"," ",1200,800);

 ///*  
 TH2D* beta = new TH2D("beta","beta",100,86,180,200,0,20);
 TGraph* sgml = new TGraph(beta->GetNbinsX(),x,y);
 TGraph* sgmh = new TGraph(beta->GetNbinsX(),x,y);
 Events->Draw("abs(TimeOfFlight):TrackLength>>beta","ChiSquare<10 && StatusCode==0","colz");
 //SliceNormalizeX(beta,0.499,sgml,sgmh,1,0);
 gPad->SetLogz();
 sgml->Fit("pol1");
 beta->Draw("colz");
 beta->GetYaxis()->SetRangeUser(0,10);
 sgml->Draw("psame");
 sgml->SetMarkerStyle(8);
 sgml->SetMarkerSize(1.1);
 sgml->GetFunction("pol1")->SetLineColor(1);
 sgml->GetFunction("pol1")->SetLineWidth(3);
 beta->GetXaxis()->SetTitleOffset(1.5);
 beta->GetXaxis()->SetTitle("TrackLength (cm)");
 beta->GetYaxis()->SetTitle("|TimeOfFlight| (ns)");
 cplot->SaveAs("png_cl/beta.root");
 cplot->SaveAs("png_cl/beta.png");
 

 TH1D* bres = new TH1D("bres","bres",200,0,4);
 bres->Sumw2();
 Events->Draw("((abs(TimeOfFlight)/TrackLength)*29.97)>>bres","ChiSquare<10 && StatusCode==0","colz");
 bres->Fit("gaus");
 bres->SetLineWidth(3);
 bres->SetMarkerStyle(8);
 bres->GetXaxis()->SetTitle("1/Beta");
 bres->GetXaxis()->SetTitleOffset(1.5);
 gPad->SetLogy();
 cplot->SaveAs("png_cl/BetaResolution.root");
 cplot->SaveAs("png_cl/BetaResolution.png");
 gPad->SetLogy(0);


 TH2D* betaud = new TH2D("betaud","betaud",30,85,180,800,-20,20);
 Events->Draw("-(TimeOfFlight):TrackLength>>betaud","ChiSquare<10 && StatusCode==0 && TimeOfFlight>=0","colz");
 TGraph* sggmu = new TGraph(betaud->GetNbinsX(),x,y);
 TGraph* sggmd = new TGraph(betaud->GetNbinsX(),x,y);
 TGraph* sggmm = new TGraph(betaud->GetNbinsX(),x,y);
 SliceNormalizeX(betaud,0.499,sggmu,sggmd,1,0);
 Events->Draw("(TimeOfFlight):TrackLength>>betaud","ChiSquare<10 && StatusCode==0 && TimeOfFlight>=0","colz");
 SliceNormalizeX(betaud,0.499,sggmm,sggmd,1,0);
 TH2D* upg = new TH2D("upg","upg",30,85,180,800,-20,20);
 Events->Draw("(TimeOfFlight):TrackLength>>upg","TimeOfFlight<0 && ChiSquare<10 && StatusCode==0","colz");
 SliceNormalizeX(upg,0.499,sggmm,sggmd,1,0);
 betaud->Add(upg);
 // betaud->Smooth();
 
 // gPad->SetLogz();
 sggmu->Fit("pol1");
 betaud->Draw("colz");
 betaud->GetYaxis()->SetRangeUser(-9,9);
 sggmu->Draw("psame");
 sggmu->SetMarkerStyle(8);
 sggmu->SetMarkerSize(1.1);
 sggmu->GetFunction("pol1")->SetLineColor(1);
 sggmu->GetFunction("pol1")->SetLineWidth(3);
 betaud->GetXaxis()->SetTitleOffset(1.5);
 betaud->GetXaxis()->SetTitle("TrackLength (cm)");
 betaud->GetYaxis()->SetTitle("TimeOfFlight (ns)");
 cplot->SaveAs("png_cl/betaud.root");
 cplot->SaveAs("png_cl/betaud.png");


 TH2D* plotchi = new TH2D("plotchi","plotchi",400,-2.5,2.5,100,0,2);
 Events->Draw("log10(1+ChiSquare):1./((TimeOfFlight/TrackLength)*29.97)>>plotchi","StatusCode==0","colz");
 plotchi->GetXaxis()->SetTitle("beta");
 plotchi->GetYaxis()->SetTitle("log(1+ChiSquare)");
 plotchi->GetXaxis()->SetTitleOffset(1.5);
 //plotchi->Smooth();
 gPad->SetLogz();
 cplot->SaveAs("png_cl/plotchi.root");
 cplot->SaveAs("png_cl/plotchi.png");


 TH2D* plotdt = new TH2D("plotdt","plotdt",400,-2.5,2.5,100,-7,-0.01);
 Events->Draw("log10(DTi[0]):beta>>plotdt","StatusCode==0","colz");
 plotdt->GetXaxis()->SetTitle("beta");
 plotdt->GetYaxis()->SetTitle("Log_{10}(Time difference previous/[s]) ");
 plotdt->GetXaxis()->SetTitleOffset(1.5);
// plotdt->Smooth();
 gPad->SetLogz();
 cplot->SaveAs("png_cl/plotdt.root");
 cplot->SaveAs("png_cl/plotdt.png");

 TH2D* plotdt1 = new TH2D("plotdt1","plotdt1",400,-2.5,2.5,100,-7,-0.011);
 Events->Draw("log10(DTi[1]):beta>>plotdt1","StatusCode==0","colz");
 plotdt1->GetXaxis()->SetTitle("beta");
 plotdt1->GetYaxis()->SetTitle("Log_{10}(Time difference next/[s]) ");
 plotdt1->GetXaxis()->SetTitleOffset(1.5);
 // plotdt1->Smooth();
 gPad->SetLogz();
 cplot->SaveAs("png_cl/plotdt1.root");
 cplot->SaveAs("png_cl/plotdt1.png");


TH2D* chidt = new TH2D("chidt","chidt",400,-2,2,100,-7,-0.01);
 Events->Draw("log10(DTi[0]):beta*log10(1+ChiSquare)/abs(beta)>>chidt","StatusCode==0 && abs(abs(beta)-1)<0.5","colz");
 chidt->GetXaxis()->SetTitle("log(1+Chisquare)*sign(direction)");
 chidt->GetYaxis()->SetTitle("Log_{10}(Time difference previous/[s]) ");
 chidt->GetXaxis()->SetTitleOffset(1.5);
 // chidt->Smooth();
 gPad->SetLogz();
 cplot->SaveAs("png_cl/chidt.root");
 cplot->SaveAs("png_cl/chidt.png");

 //*/
TH2D* primb = new TH2D("primb","primb",400,-2.5,2.5,100,0,2);
 Events->Draw("log10(1+ChiSquare):beta>>primb","StatusCode==0 && abs(abs(betaS)-1)<0.3 && betaS<0 && log10(DTi[1])<-4.5 && log10(1+ChiS)<1.7","colz");
 primb->GetXaxis()->SetTitle("Beta Primary");
 primb->GetYaxis()->SetTitle("log_{10}(1+ChiSquare)");
 primb->GetXaxis()->SetTitleOffset(1.5);
 gPad->SetLogz();
 cplot->SaveAs("png_cl/primBeta.root");
 cplot->SaveAs("png_cl/primBeta.png");


TH2D* zdirbet = new TH2D("zdirbet","zdirbet",400,-2.5,2.5,100,0,2);
 Events->Draw("log10(1+ChiSquare):beta>>zdirbet","StatusCode==0 && abs(abs(betaS)-1)<0.3  && log10(1+ChiS)<1.7","colz");
 zdirbet->GetXaxis()->SetTitle("Beta");
 zdirbet->GetYaxis()->SetTitle("log_{10}(1+ChiSquare)");
 zdirbet->GetXaxis()->SetTitleOffset(1.5);
 gPad->SetLogz();
 cplot->SaveAs("png_cl/chiBeta.root");
 cplot->SaveAs("png_cl/chiBeta.png");

 ///*
TH2D* delay = new TH2D("delay","delay",5000,0,5e-6,100,0,10);
 Events->Draw("1./(beta*ZDir)+1./(ZDirS):DTi[1]>>delay","StatusCode==0 && abs(abs(betaS)-1)<0.5 && betaS<0 && beta>0 && beta<1 && log10(DTi[1])<-4.5 && log10(1+ChiS)<1.7","colz");
 Events->Draw("1./(ZDir)+1./(ZDirS):DTi[1]>>+delay","StatusCode==0 && abs(abs(betaS)-1)<0.5 && betaS<0 && beta>0 && beta>1 && log10(DTi[1])<-4.5 && log10(1+ChiS)<1.7","colz");
 delay->GetXaxis()->SetTitle("Observed Delay");
 delay->GetYaxis()->SetTitle("ToF Delay");
 delay->GetXaxis()->SetTitleOffset(1.5);
 gPad->SetLogz();
 cplot->SaveAs("png_cl/FSdelay.root");
 cplot->SaveAs("png_cl/FSdelay.png");
 //*/


 TH2D* betaud = new TH2D("betaud","betaud",20,85,180,300,-20,20);
 Events->Draw("(TimeOfFlight):TrackLength>>betaud","log10(1.+ChiSquare)<1.7 && StatusCode==0","colz");
 TGraph* sggmu = new TGraph(betaud->GetNbinsX(),x,y);
 TGraph* sggmd = new TGraph(betaud->GetNbinsX(),x,y);
 TGraph* sggmm = new TGraph(betaud->GetNbinsX(),x,y);
 SliceNormalizeX(betaud,0.499,sggmu,sggmd,1,0);
 // betaud->Smooth();
 
 // gPad->SetLogz();
 betaud->GetXaxis()->SetTitleOffset(1.5);
 betaud->GetXaxis()->SetTitle("TrackLength (cm)");
 betaud->GetYaxis()->SetTitle("TimeOfFlight (ns)");
 cplot->SaveAs("png_cl/betaudt.root");
 cplot->SaveAs("png_cl/betaudt.png");

 Events->Draw("log10(DTi[0])>>intmn","StatusCode==0 && abs(abs(beta)-1)<0.3 && beta<0 && DTi[0]<0.5 && DTi[0]>1.e-7 && log10(1+ChiSquare)<1.7","");
 Events->Draw("log10(DTi[0])>>intmp","StatusCode==0 && abs(abs(beta)-1)<0.3 && beta>0 && DTi[0]<0.5 && DTi[0]>1.e-7 && log10(1+ChiSquare)<1.7","same");
 intmn->GetXaxis()->SetTitle("log(Time Lag)");
 intmn->SetLineWidth(3);
 intmn->SetLineColor(2);
 intmp->SetLineWidth(3);
 intmn->DrawNormalized();
 intmp->DrawNormalized("same");
 gPad->SetLogy();
 TF1* fdec = new TF1("fdec","[1]*exp(-(pow(10,x)/[0]))*pow(10,x)*log(10)",-7,0);
 fdec->SetParameter(0,0.02);
 intmp->Fit("fdec");
 TF1* fdec2 = new TF1("fdec2","[1]*exp(-(pow(10,x)/[0]))*pow(10,x)*log(10)+[2]*exp(-(pow(10,x)/[3]))*pow(10,x)*log(10)",-7,0);
 fdec2->FixParameter(0,fdec->GetParameter(0));
 fdec2->SetParameter(1,fdec->GetParameter(1));
 fdec2->FixParameter(3,2.e-6);
 fdec2->SetParameter(2,0.01*(fdec->GetParameter(1)));
 intmn->GetXaxis()->SetRangeUser(-6.8,0);
 intmn->Fit("fdec2");
 fdec2->ReleaseParameter(0);
 fdec2->ReleaseParameter(3);
 intmn->Fit("fdec2");
 intmn->GetXaxis()->SetRangeUser(7,0);
 intmp->GetFunction("fdec")->SetLineColor(7);
 intmp->GetFunction("fdec")->SetLineWidth(3);
 intmn->GetFunction("fdec2")->SetLineColor(kGreen+2);
 intmn->GetFunction("fdec2")->SetLineWidth(3);
 intmn->SetMarkerStyle(8);
 intmp->SetMarkerStyle(8);
 intmn->SetMarkerColor(2);
 intmp->SetMarkerColor(4);
 intmp->Draw("e");
 intmn->Draw("esame");
 intmp->GetXaxis()->SetTitle("Log_{10}(Time Difference/[s])");
 intmp->GetXaxis()->SetTitleOffset(1.5);
 cplot->SaveAs("png_cl/FSintmp.root");
 cplot->SaveAs("png_cl/FSintmp.png");


 return;

}
