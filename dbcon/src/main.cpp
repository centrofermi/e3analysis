//////////////////////////////////////////////////////////
// main.cpp 
// e3rundb utility: shows the capabilities of the e3rundb connector class 
//
// Created by F.Pilo 
//
////////////////////////////////////////////////////// 

#include <string>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream> 
#include <cstdlib>
#include <map>

#include "anyoption.h"
#include "e3RunDbConn.h"

using namespace std;

enum STATUS {
  OK_STAT=0x01,
  BAD_STAT=0x00
};

// bool ASCIIDUMP=false;
// float ANGBIN=2.;
// unsigned int ACTCFG=0;
// string DTINTERVAL="";         ///< Day&Time interval. Format: YYMMDDhhmmss_YYMMDDhhmmss 
// string TAINTERVAL="";         ///< Theta Angle interval. Format: lowThetaVal_uppThetaVal in deg. 
// unsigned int VLEVEL=0;

int GetUserOpt(int, char**);
// FILE * fp;
// int stdout_fd, work_fd;
// void capture_cmd_prepare(void);
// void capture_cmd_output(void);

// struct CompareDistanceFromLevel{

//   CompareDistanceFromLevel(double cLevel) : level(cLevel) {}
  
//   bool operator()(double lhs, double rhs){
//     return rhs < level; //upper value
//   }
  
// private:
//   double level;

// };


//! Main routine
/*!
  Main routine.
*/
int main(int argc, char *argv[]) {
  
  int _status=0;        ///< Process status
  int _ret=0;          

  //======================================== 
  // Parse inline command options
  //========================================  

  _status=GetUserOpt(argc, argv);              
  if(_status!=OK_STAT){ 
    if(_status==1) cerr<<"[ltcube.exe - ERROR] GetUserOpt function has returned error code "
		       <<_status<<". Too few arguments. "<<endl;
    exit(EXIT_FAILURE);
  }

//   //======================================== 
//   // Declare a TParameters object. This step is mandatory !
//   // By default, $LTCUBE/ltcube.conf is read.
//   //======================================== 
//   TParameters par;
//   gPAR->GetAll();
//   if(ACTCFG!=0){
//     cout<<"[ltcube.exe - WARNING] GetUserOpt function has returned non-zero ACT Config ("<<ACTCFG<<")."
// 	<<"Changing corresponding gPAR values ..."<<endl;
//     gPAR->FTAttitudeID = (unsigned int) ACTCFG/100;
//     gPAR->FTCoordinateID = (unsigned int) (ACTCFG-gPAR->FTAttitudeID*100)/10;
//     gPAR->FTTimeID = (unsigned int) (ACTCFG-(gPAR->FTAttitudeID*100+gPAR->FTCoordinateID*10));
//   }

//   //======================================== 
//   // Init internal vars
//   //========================================  

//   // AMS Coo&Point related vars
//   //-------------------------
//   float _iss_x, _iss_y, _iss_z;
//   float _iss_alt, _iss_lat, _iss_lon; 
//   float _iss_ctrs_lat=0, _iss_ctrs_lon=0;
//   float _iss_gtod_lat=0, _iss_gtod_lon=0; 
//   float _iss_v, _iss_vtheta, _iss_vphi, dt=0;
//   float _iss_roll=0, _iss_pitch=0, _iss_yaw=0;
//   double _posISS[3], _velISS[3], _yprISS[3];

//   double _zen_eq_dec=0, _zen_eq_ras=0;   ///< AMS pointing in celestial coordinates (spherical)
//   double _zen_eq_dir[3];                 ///< AMS pointing in celestial coordinates (cartesian)
//   double _zen_gal_lat=0, _zen_gal_lon=0; ///< AMS pointing in galactic coordinates (spherical)
//   double _zen_gal_dir[3];                ///< AMS pointing in galactic coordinates (cartesian)

//   double _maxzenangdist = 40.;           ///< AMS zenith maximum angular distance (degree) respect to ISS geocentric radius

//   //DAQ related vars
//   //-------------------------
//   float _lt_sysb0, _lt_sysb1;         ///< AMS Total and FEE only Live-Time

//   // LOOP related vars
//   //-------------------------
//   double _skydir[3];     ///< cartesian celestial coordinates 
//   double ras,dec,l,b;    ///< spherical celestial coordinates 
//   double _delt = (double) gPAR->LTCMapTimeStep;    ///< orbit update time interval (seconds)
//   double _minpa = 0.0;   ///< low limit for phi angle [deg.]
//   double _maxpa = 1.0;   ///< high limit for phi angle [deg.]
//   double _minta = 0.0;   ///< low limit for theta angle [deg.]
//   double _maxta = 1.0;   ///< high limit for theta angle [deg.]
//   double _mod = 0;       ///< ...
//   double _anglezen=0.;   ///< ... [rad]
//   int Norb= 1;           ///< number of complete orbits to simulate
//   int mode= 0;           ///< Exposure: Normal=0, Magnetic=1

//   // Some ISS ORBIT related vars
//   //-------------------------
//   double _altitude = 380e3; //m
//   double _Rearth = R_TERRA;
//   double _a=(_Rearth + _altitude)/_Rearth;   ///< orbit semimajor axis (in Rearth units)
//   double _T=2.*PI*pow(_a,1.5)/sqrt(5.98e24*6.67e-11)*pow(_Rearth,1.5);   ///< period of the orbit (from Kepler's third law)

//   string _rootfile_name;   
//   char *_asciifile_name=new char[50];
//   char *_logfile_name=new char[50];
//   char histo_title[80];
//   char histo_name[80];

//   ofstream pi, pilog;

//   SAMSEventR *_SAMSEventR;


//   if(ASCIIDUMP){
    
//     cout<<"Output ASCII File name> ";
//     while(!(cin >> _asciifile_name)){
//       cin.clear();
//       cin.ignore(INT_MAX,'\n');
//       cout<<"Please insert valid char filename or CONTROL-C to exit"<<endl;
//     } 
//     pi.open(_asciifile_name);
//     strcpy(_logfile_name,_asciifile_name);
//     strcat(_logfile_name,".log");
//     pilog.open(_logfile_name);
//     pilog<<"Log for: "<<_asciifile_name<<endl;
//   }

//   //   cout<<"Normal(0) exposure or Magnetic(1) exposure?> ";
//   //   while(!(cin >> mode)){
//   //     cin.clear();
//   //     cin.ignore(INT_MAX,'\n');
//   //     cout<<"Please insert valid int mode or CONTROL-C to exit"<<endl;
//   //   } 
//   //   if ((mode<0)||(mode>1)){
//   //     cout<<"[ltcube - ERROR] Mode out of range EXIT"<<endl;
//   //     pilog<<"[ltcube - ERROR] Mode out of range:mode= "<<mode<<endl;
//   //     pilog.close();
//   //     exit(8);
//   //   }
//   //   pilog<<"Mode: "<<mode<<endl;

//   if(!TAINTERVAL.empty()){
//     int pos=TAINTERVAL.find("_");
//     _minta = atof(TAINTERVAL.substr(0,pos).c_str());
//     _maxta = atof(TAINTERVAL.substr(pos+1).c_str());
//   }

//   //========================================  
//   // Init ROOT stuff
//   //======================================== 

//   _rootfile_name.append("./data/ltcube_");
//   _rootfile_name.append(DTINTERVAL);
//   string rstr = TAINTERVAL;
//   if(!rstr.empty()){
//     _rootfile_name.append("_");
//     std::replace( rstr.begin(), rstr.end(), '.', 'd'); // replace all '.' to 'd'
//     _rootfile_name.append(rstr);
//   }
//   char _params[80]; sprintf(_params,"_%d%d%d",gPAR->FTAttitudeID,gPAR->FTCoordinateID,gPAR->FTTimeID);
//   _rootfile_name.append(_params);
//   _rootfile_name.append(".root");

//   TFile _rootfile(_rootfile_name.c_str(),"recreate");
//   _rootfile.mkdir("aux_hist");
//   _rootfile.mkdir("aux_graph");

//   _rootfile.cd("aux_hist");
    
//   //********************************************************************/
//   // Initializing ISS position histo
//   //********************************************************************/
//   TH2D *_th_isspos[3]; TString _th_isspos_name, _th_isspos_title;
//   _th_isspos_name.Clear(); _th_isspos_name="TH_ISSPos_2LE";
//   _th_isspos_title.Clear(); _th_isspos_title="ISS 2LE Position";
//   _th_isspos[0] = new TH2D(_th_isspos_name,_th_isspos_title,360,-180.,180.,180,-90.,90.);    
//   _th_isspos[0]->GetXaxis()->SetTitle("Longitude [deg.]");
//   _th_isspos[0]->GetYaxis()->SetTitle("Latitude [deg.]");

//   _th_isspos_name.Clear(); _th_isspos_name="TH_ISSPos_CTRS";
//   _th_isspos_title.Clear(); _th_isspos_title="ISS CTRS Position";
//   _th_isspos[1] = new TH2D(_th_isspos_name,_th_isspos_title,360,-180.,180.,180,-90.,90.);    
//   _th_isspos[1]->GetXaxis()->SetTitle("Longitude [deg.]");
//   _th_isspos[1]->GetYaxis()->SetTitle("Latitude [deg.]");

//   _th_isspos_name.Clear(); _th_isspos_name="TH_ISSPos_GTOD";
//   _th_isspos_title.Clear(); _th_isspos_title="ISS GTOD Position";
//   _th_isspos[2] = new TH2D(_th_isspos_name,_th_isspos_title,360,-180.,180.,180,-90.,90.);    
//   _th_isspos[2]->GetXaxis()->SetTitle("Longitude [deg.]");
//   _th_isspos[2]->GetYaxis()->SetTitle("Latitude [deg.]");

//   //********************************************************************/
//   // Initializing AMS pointing histo
//   //********************************************************************/
//   TH2D *_th_amspoi[6]; TString _th_amspoi_name, _th_amspoi_title;
//   _th_amspoi_name.Clear(); _th_amspoi_name="TH_AMSPoi_LVLH_GALCOO";
//   _th_amspoi_title.Clear(); _th_amspoi_title="AMS Pointing from LVLH Attitude";
//   _th_amspoi[0] = new TH2D(_th_amspoi_name,_th_amspoi_title,360,-180.,180.,180,-90.,90.);    
//   _th_amspoi[0]->GetXaxis()->SetTitle("Galactic longitude [deg.]");
//   _th_amspoi[0]->GetYaxis()->SetTitle("Galactic latitude [deg.]");

//   _th_amspoi_name.Clear(); _th_amspoi_name="TH_AMSPoi_INTL_GALCOO";
//   _th_amspoi_title.Clear(); _th_amspoi_title="AMS Pointing from INTL data";
//   _th_amspoi[1] = new TH2D(_th_amspoi_name,_th_amspoi_title,360,-180.,180.,180,-90.,90.);    
//   _th_amspoi[1]->GetXaxis()->SetTitle("Galactic longitude [deg.]");
//   _th_amspoi[1]->GetYaxis()->SetTitle("Galactic latitude [deg.]");

//   _th_amspoi_name.Clear(); _th_amspoi_name="TH_AMSPoi_STK_GALCOO";
//   _th_amspoi_title.Clear(); _th_amspoi_title="AMS Pointing from STK data";
//   _th_amspoi[2] = new TH2D(_th_amspoi_name,_th_amspoi_title,360,-180.,180.,180,-90.,90.);    
//   _th_amspoi[2]->GetXaxis()->SetTitle("Galactic longitude [deg.]");
//   _th_amspoi[2]->GetYaxis()->SetTitle("Galactic latitude [deg.]");

//   _th_amspoi_name.Clear(); _th_amspoi_name="TH_AMSPoi_LVLH_J2000";
//   _th_amspoi_title.Clear(); _th_amspoi_title="AMS Pointing from LVLH Attitude";
//   _th_amspoi[3] = new TH2D(_th_amspoi_name,_th_amspoi_title,360,-180.,180.,180,-90.,90.);    
//   _th_amspoi[3]->GetXaxis()->SetTitle("Right ascension [deg.]");
//   _th_amspoi[3]->GetYaxis()->SetTitle("Declination [deg.]");

//   _th_amspoi_name.Clear(); _th_amspoi_name="TH_AMSPoi_INTL_J2000";
//   _th_amspoi_title.Clear(); _th_amspoi_title="AMS Pointing from INTL data";
//   _th_amspoi[4] = new TH2D(_th_amspoi_name,_th_amspoi_title,360,-180.,180.,180,-90.,90.);    
//   _th_amspoi[4]->GetXaxis()->SetTitle("Right ascension [deg.]");
//   _th_amspoi[4]->GetYaxis()->SetTitle("Declination [deg.]");

//   _th_amspoi_name.Clear(); _th_amspoi_name="TH_AMSPoi_STK_J2000";
//   _th_amspoi_title.Clear(); _th_amspoi_title="AMS Pointing from STK data";
//   _th_amspoi[5] = new TH2D(_th_amspoi_name,_th_amspoi_title,360,-180.,180.,180,-90.,90.);    
//   _th_amspoi[5]->GetXaxis()->SetTitle("Right ascension [deg.]");
//   _th_amspoi[5]->GetYaxis()->SetTitle("Declination [deg.]");

//   //********************************************************************/
//   // Initializing SAA pointing histo
//   //********************************************************************/
//   TH2D *_th_saapoi[2]; TString _th_saapoi_name, _th_saapoi_title;
//   _th_saapoi_name.Clear(); _th_saapoi_name="TH_SAAPoi_GALCOO";
//   _th_saapoi_title.Clear(); _th_saapoi_title="AMS Pointing from LVLH inside SAA";
//   _th_saapoi[0] = new TH2D(_th_saapoi_name,_th_saapoi_title,360,-180.,180.,180,-90.,90.);    
//   _th_saapoi[0]->GetXaxis()->SetTitle("Galactic longitude [deg.]");
//   _th_saapoi[0]->GetYaxis()->SetTitle("Galactic latitude [deg.]");

//   _th_saapoi_name.Clear(); _th_saapoi_name="TH_SAAPoi_J2000";
//   _th_saapoi_title.Clear(); _th_saapoi_title="AMS Pointing from LVLH inside SAA";
//   _th_saapoi[1] = new TH2D(_th_saapoi_name,_th_saapoi_title,360,-180.,180.,180,-90.,90.);    
//   _th_saapoi[1]->GetXaxis()->SetTitle("Right ascension [deg.]");
//   _th_saapoi[1]->GetYaxis()->SetTitle("Declination [deg.]");

//   //********************************************************************/
//   // Initializing Plots
//   //********************************************************************/
//   TString graph_name;
//   TGraph *_gr_DAQStatus=new TGraph();
//   TGraph *_gr_LiveTime=new TGraph();
//   TGraph *_gr_ZenAngDist=new TGraph();

//   graph_name.Clear();
//   graph_name = "TGraph_DAQStatus";
//   _gr_DAQStatus->SetName(graph_name.Data());

//   graph_name.Clear();
//   graph_name = "TGraph_LiveTime";
//   _gr_LiveTime->SetName(graph_name.Data());

//   graph_name.Clear();
//   graph_name = "TGraph_ZenAngDist";
//   _gr_ZenAngDist->SetName(graph_name.Data());

//    //********************************************************************/
//   // Francesca
//   //********************************************************************/
//   TGraph *th_vs_time = new TGraph();
//   TGraph *phi_vs_time = new TGraph();
//   graph_name.Clear();
//   graph_name = "TGraph_th_vs_time";
//   th_vs_time->SetName(graph_name.Data());
//   graph_name.Clear();
//   graph_name = "TGraph_phi_vs_time";
//   phi_vs_time->SetName(graph_name.Data());

//   TH1D *_th_returned[3]; TString _th_returned_name, _th_returned_title;
//   _th_returned_name.Clear(); _th_returned_name="TH_Returned_Coo";
//   _th_returned_title.Clear(); _th_returned_title="Output of GetAMSCoo";
//   _th_returned[0] = new TH1D(_th_returned_name,_th_returned_title,6,-1,5);    
//   _th_returned[0]->GetXaxis()->SetTitle("Returned value");
//   _th_returned[0]->GetYaxis()->SetTitle("Events");

//   _th_returned_name.Clear(); _th_returned_name="TH_Returned_J2000";
//   _th_returned_title.Clear(); _th_returned_title="Output of GetAMSJ2000Point";
//   _th_returned[1] = new TH1D(_th_returned_name,_th_returned_title,6,-1,5);    
//   _th_returned[1]->GetXaxis()->SetTitle("Returned value");
//   _th_returned[1]->GetYaxis()->SetTitle("Events");

//   _th_returned_name.Clear(); _th_returned_name="TH_Returned_Gal";
//   _th_returned_title.Clear(); _th_returned_title="Output of GetAMSGalPoint";
//   _th_returned[2] = new TH1D(_th_returned_name,_th_returned_title,6,-1,5);    
//   _th_returned[2]->GetXaxis()->SetTitle("Returned value");
//   _th_returned[2]->GetYaxis()->SetTitle("Events");


//  //********************************************************************/
//   // initializing Exposure histo for selected Theta range
//   //********************************************************************/
//   TH2D *_th_livetime_J2000; TString _th_livetime_J2000_name, _th_livetime_J2000_title;
//   _th_livetime_J2000_name.Clear(); _th_livetime_J2000_name="TH_Livetime_J2000";
//   _th_livetime_J2000_title.Clear(); _th_livetime_J2000_title="Live-time [J2000]. Phi Range: 360 deg., Theta Range: ";
//   _th_livetime_J2000_title+= TAINTERVAL; _th_livetime_J2000_title+=" deg.";
//   _th_livetime_J2000 = new TH2D(_th_livetime_J2000_name,_th_livetime_J2000_title,(int)(360/ANGBIN),-180.,180.,(int)(180/ANGBIN),-90.,90.);    
//   _th_livetime_J2000->GetXaxis()->SetTitle("Right ascension [deg.]");
//   _th_livetime_J2000->GetYaxis()->SetTitle("Declination [deg.]");

//   _rootfile.cd();
//   TH2D *_th_livetime_allphi; TString _th_livetime_name, _th_livetime_title;
//   _th_livetime_name.Clear(); _th_livetime_name="TH_Livetime_AllPhi";
//   _th_livetime_title.Clear(); _th_livetime_title="Live-time. Phi Range: 360 deg. Theta Range: ";
//   _th_livetime_title+= TAINTERVAL; _th_livetime_title+=" deg.";
//   _th_livetime_allphi = new TH2D(_th_livetime_name,_th_livetime_title,(int)(360/ANGBIN),-180.,180.,(int)(180/ANGBIN),-90.,90.);    
//   _th_livetime_allphi->GetXaxis()->SetTitle("Galactic longitude [deg.]");
//   _th_livetime_allphi->GetYaxis()->SetTitle("Galactic latitude [deg.]");

//   TH2D *_th_livetime_allphi_oldm;
//   _th_livetime_name.Clear(); _th_livetime_name="TH_Livetime_AllPhi_OldM";
//   _th_livetime_title.Clear(); _th_livetime_title="Live-time (Old Method). Phi Range: 360 deg. Theta Range: ";
//   _th_livetime_title+= TAINTERVAL; _th_livetime_title+=" deg.";
//   _th_livetime_allphi_oldm = new TH2D(_th_livetime_name,_th_livetime_title,(int)(360/ANGBIN),-180.,180.,(int)(180/ANGBIN),-90.,90.);    
//   _th_livetime_allphi_oldm->GetXaxis()->SetTitle("Galactic longitude [deg.]");
//   _th_livetime_allphi_oldm->GetYaxis()->SetTitle("Galactic latitude [deg.]");

//   Double_t _phi_uppvalues[gPAR->LTCMapIncPhiBins];
//   TH2D *_th_livetime[gPAR->LTCMapIncPhiBins];
//   for(int iphi=0; iphi<gPAR->LTCMapIncPhiBins; iphi++){

//     _phi_uppvalues[iphi]=-180.+(360./gPAR->LTCMapIncPhiBins)*(iphi+1);

//     _th_livetime_name.Clear();
//     _th_livetime_name="TH_Livetime_";
//     _th_livetime_name+=iphi;
//     _th_livetime_title.Clear();
//     _th_livetime_title="Live-time. Phi Range: ";
//     _th_livetime_title+=-180.+((360./gPAR->LTCMapIncPhiBins)*iphi);
//     _th_livetime_title+="_";  _th_livetime_title+=-180.+(360./gPAR->LTCMapIncPhiBins)*(iphi+1); 
//     _th_livetime_title+=" deg.";
//     _th_livetime_title+=". Theta Range: ";
//     _th_livetime_title+= TAINTERVAL; _th_livetime_title+=" deg.";
//     _th_livetime[iphi] = new TH2D(_th_livetime_name,_th_livetime_title,(int)(360/ANGBIN),-180.,180.,(int)(180/ANGBIN),-90.,90.);    
//     _th_livetime[iphi]->GetXaxis()->SetTitle("Galactic longitude [deg.]");
//     _th_livetime[iphi]->GetYaxis()->SetTitle("Galactic latitude [deg.]");
    
//     cout<<"[ltcube - INFO] Live-time Histogram name: "<<_th_livetime[iphi]->GetName()<<endl;
//     cout<<"[ltcube - INFO] Live-time Histogram title: "<<_th_livetime[iphi]->GetTitle()<<endl;
//     cout<<"[ltcube - INFO] Live-time Histogram numbinsx: "<<_th_livetime[iphi]->GetXaxis()->GetNbins()<<endl;
//     cout<<"[ltcube - INFO] Live-time Histogram numbinsy: "<<_th_livetime[iphi]->GetYaxis()->GetNbins()<<endl;

//   }

// //   ////// initializing exposure histos for different magnetic bands
// //   //TH2D *h1m,*h2m,*h3m;
// //   TH2D *h1m=new TH2D();
// //   TH2D *h2m=new TH2D();
// //   TH2D *h3m=new TH2D(); 
// //   if(mode==1){

// //     strcpy(histo_title_mag1, histo_title);
// //     strcpy(histo_title_mag2, histo_title);
// //     strcpy(histo_title_mag3, histo_title);

// //     sprintf(histo_name,"h%d1m",rangeza+1);
// //     h1m = new TH2D(histo_name,strcat(histo_title_mag1," Theta_m<0.3"),(int)(360/ANGBIN),-180.,180.,(int)(180/ANGBIN),-90.,90.);
// //     h1m->GetXaxis()->SetTitle("Galactic longitude");
// //     h1m->GetYaxis()->SetTitle("Galactic latitude");

// //     sprintf (histo_name,"h%d2m",rangeza+1);
// //     h2m = new TH2D(histo_name,strcat(histo_title_mag2," Theta_m>0.3 && <0.8 rad"),(int)(360/ANGBIN),-180.,180.,(int)(180/ANGBIN),-90.,90.);
// //     h2m->GetXaxis()->SetTitle("Galactic longitude");
// //     h2m->GetYaxis()->SetTitle("Galactic latitude");

// //     sprintf (histo_name,"h%d3m",rangeza+1);
// //     h3m = new TH2D(histo_name,strcat(histo_title_mag3," Theta_m>0.8 rad"),(int)(360/ANGBIN),-180.,180.,(int)(180/ANGBIN),-90.,90.);
// //     h3m->GetXaxis()->SetTitle("Galactic longitude");
// //     h3m->GetYaxis()->SetTitle("Galactic latitude");

// //   }

//   //=========================================  
//   //Initializing variables for Orbit loop
//   //=========================================  
 	
//   long _Nsteps = 1L;
//   //+1 to always have 1 step
        	    
//   // Accumulate the observing time distribution 
//   // for sky points separated by this binning
//   double _faccum; 
	
//   // Initialize the elapsed time since beginning of the simulation
//   double _elapse = 0.;
  
//   // Initialize the NORUN counter
//   long _norun_count = 0L; 

//   // Initialize the BIGZENANGDIST counter
//   long _bigzenangdist_count = 0L; 

//   // Initialize the SAA passages counter
//   long _saa_count = 0L; 

//   int k=0,i,j;
//   int _nrastep=(int)(360/ANGBIN)+1;
//   int _ndestep=(int)(180/ANGBIN)+1;
//   double *tot =new double[_nrastep*_ndestep];
//   for(j=0; j<_nrastep; j++)
//     for(i=0; i<_ndestep; i++)
//       tot[j*_ndestep+i]=0.;

//   //********************************************************************/
//   //Set Time interval parameters
//   //********************************************************************/
//   time_t rawtime = time(NULL);
//   struct tm inittime = *localtime(&rawtime);
//   struct tm gmtinittime = *gmtime(&rawtime);
//   struct tm finaltime = *localtime(&rawtime);
//   struct tm gmtfinaltime = *gmtime(&rawtime);
//   int time_offset=inittime.tm_hour-gmtinittime.tm_hour;

//   stringstream DTsstream(DTINTERVAL);
//   size_t pos=0;

//   inittime.tm_year = 2000+atoi(DTINTERVAL.substr(pos,2).c_str()) - 1900; pos+=2;
//   inittime.tm_mon = atoi(DTINTERVAL.substr(pos,2).c_str()) - 1; pos+=2;
//   inittime.tm_mday = atoi(DTINTERVAL.substr(pos,2).c_str()); pos+=2;
//   inittime.tm_hour = atoi(DTINTERVAL.substr(pos,2).c_str()) + time_offset; pos+=2;
//   inittime.tm_min = atoi(DTINTERVAL.substr(pos,2).c_str()); pos+=2;
//   inittime.tm_sec = atoi(DTINTERVAL.substr(pos,2).c_str()); pos+=2;

//   pos+=1; //delim
//   finaltime.tm_year = 2000+atoi(DTINTERVAL.substr(pos,2).c_str()) - 1900; pos+=2;
//   finaltime.tm_mon = atoi(DTINTERVAL.substr(pos,2).c_str()) - 1; pos+=2;
//   finaltime.tm_mday = atoi(DTINTERVAL.substr(pos,2).c_str()); pos+=2;
//   finaltime.tm_hour = atoi(DTINTERVAL.substr(pos,2).c_str()) + time_offset; pos+=2;
//   finaltime.tm_min = atoi(DTINTERVAL.substr(pos,2).c_str()); pos+=2;
//   finaltime.tm_sec = atoi(DTINTERVAL.substr(pos,2).c_str()); pos+=2;

//   _Nsteps = mktime(&finaltime) - mktime(&inittime);
//   _Nsteps/= _delt;
//   _Nsteps+= 1;
  
//   //********************************************************************/
//   //Initialize simulated AMSEventR stuff
//   //********************************************************************/
//   _SAMSEventR = new SAMSEventR();
//   _SAMSEventR->Init();
//   //Put initial tile in Event Header in GPS time format (JMDC has GPS time!)
//   _SAMSEventR->SetEventTime(mktime(&inittime)+gpsdiff(mktime(&inittime)),0);
//   _SAMSEventR->SetVerbosity(VLEVEL);

//   TStopwatch timer;

// //========================================  
// //Start Orbit Loop
// //========================================  

//   cout.precision(15);
//   cout<<endl<<"[ltcube - INFO] Starting the Event Loop"<<endl;

//   for (long ti=0; ti<(_Nsteps-1); ti++){

//     //********************************************************************/
//     //Update all simulation parameters at a new time
//     //1.- Increment simulation time by delt
//     //2.- Get Run Summary
//     //    - 
//     //3.- Get Live-Time estimation from database
//     //    - if GetLiveTime function return value > 1, Live-Time is set to 100% 
//     //4.- Get Zenith angular distance respect to ISS geocentric radius vector
//     //********************************************************************/
//     //1.
//     _elapse = _elapse + _delt;


//     if((int)fmod(_elapse,30*60)==0){
//       cout<<"[ltcube - INFO] Elapsed Time: "<<(int)_elapse/60<<" min.";
//       cout<<" - Number of complete orbits (according to simultation): "<<(int)(_elapse/_T);
//       cout<<endl;  
//     }

//     if((int)fmod(_elapse,60)==0){
//       timer.Stop();
//       if(VLEVEL>=1)
// 	cout << " Cpu time for 60 sec simulated orbit: " 
// 	     << timer.CpuTime() 
// 	     << " (" << timer.RealTime()  << ")"
// 	     << endl; 
//       timer.Start();
//     }

//     _SAMSEventR->IncEventTime(_delt,0);

//     //2.
//     //if(_SAMSEventR->CheckNewRun()) cout<<"[ltcube - INFO] New RUN: "<<_SAMSEventR->GetRunID()<<endl; //OLD WAY, NO RUNSUMMARY CSV DATA AVAILABLE
//     _ret = _SAMSEventR->GetRunInfo();
//     _gr_DAQStatus->SetPoint(_gr_DAQStatus->GetN(), _SAMSEventR->JMDCEventTime(), _ret);
//     if(VLEVEL>=3) cout<<"[ltcube -INFO] GetRunInfo returned "<<_ret
// 	<<" for time "<<_SAMSEventR->JMDCEventTime()
// 	<<". Assigned RunID: "<<_SAMSEventR->GetRunID()<<endl;

//     if(_ret>1){
//       _norun_count++;
//       continue;
//     }

//     _SAMSEventR->UpdISSPos();
//     _SAMSEventR->UpdISSAtt();

//     //3.
//     _ret=_SAMSEventR->GetLiveTime(_lt_sysb0, _lt_sysb1, _SAMSEventR->JMDCEventTime());
//     if(VLEVEL>=3) cout<<" Live TIme _ret:"<<_ret<<" - "<<_lt_sysb0<<" "<<_lt_sysb1<<endl;
//     if(_ret>1){ _lt_sysb0=100.; _lt_sysb1=100.; }
//     _lt_sysb0/=100.; _lt_sysb1/=100.;
//     _gr_LiveTime->SetPoint(_gr_LiveTime->GetN(), _SAMSEventR->JMDCEventTime(), _lt_sysb0);

//     //4.
//     if(VLEVEL>=3) cout<<" AMS Zenith angular distance: "<<_SAMSEventR->GetZenAngDist()<<" degree"<<endl;
//     _gr_ZenAngDist->SetPoint(_gr_ZenAngDist->GetN(), _SAMSEventR->JMDCEventTime(), _SAMSEventR->GetZenAngDist());
//     if(_SAMSEventR->GetZenAngDist()>=_maxzenangdist){
//       _bigzenangdist_count++;
//       continue;
//     }

//     //********************************************************************/
//     //Get&Store ISS Position and Attitude
//     //1- ISS Position, Velocity in 2 Line Elements
//     //2- ISS Position in CTRS ... not implemented yet
//     //3- ISS Attitude in 2 Line Elements
//     //********************************************************************/

//     //1.
//     _SAMSEventR->GetISSTRSPos(0, _iss_alt, _iss_lat, _iss_lon);
//     _SAMSEventR->GetISSTRSVel(0, _iss_v, _iss_vtheta, _iss_vphi);
//     _th_isspos[0]->Fill(_iss_lon*R2D, _iss_lat*R2D);

//     //Francesca
//     th_vs_time->SetPoint(th_vs_time->GetN(), _SAMSEventR->JMDCEventTime(), _iss_lat*R2D);
//     phi_vs_time->SetPoint(phi_vs_time->GetN(), _SAMSEventR->JMDCEventTime(), _iss_lon*R2D);

// //     //2.
// //     AMSSetupR::ISSCTRSR _ctrsr_obj; 
// //     //    if(VLEVEL==0) capture_cmd_prepare();
// //     _ret= _SAMSEventR->_AMSSetupR->getISSCTRS(_ctrsr_obj, _SAMSEventR->JMDCEventTime());
// //     //_ret=_fAMSSetupR->getISSCTRS(_ctrsr_obj,xtime);
// //     //if(VLEVEL==0) capture_cmd_output();
// //     if(_ret!=0) cout<<"[ltcube - ERROR] getISSCTRS file function returned "<<_ret<<" for time "<<_SAMSEventR->JMDCEventTime()<<endl;
// //     else{
// //       _iss_ctrs_lat=_ctrsr_obj.theta; _iss_ctrs_lon=_ctrsr_obj.phi;
// //       _th_isspos[1]->Fill(_iss_ctrs_lon*R2D, _iss_ctrs_lat*R2D);
// //     }
// // //     //     _iss_r=_ctrsr_obj.r; _iss_v=_ctrsr_obj.v; _iss_vtheta=_ctrsr_obj.vtheta; _iss_vphi=_ctrsr_obj.vphi;

//     AMSSetupR::ISSGTOD _gtod_obj; 
//     //     if(VLEVEL==0) capture_cmd_prepare();
//     _ret= _SAMSEventR->_AMSSetupR->getISSGTOD(_gtod_obj, _SAMSEventR->JMDCEventTime());
//     //     if(VLEVEL==0) capture_cmd_output();
//     if(_ret!=0) cout<<"[ltcube - ERROR] getISSGTOD file function returned "<<_ret<<" for time "<< _SAMSEventR->JMDCEventTime()<<endl; 
//     else{
//       _iss_gtod_lat=_gtod_obj.theta; _iss_gtod_lon=_gtod_obj.phi;
//       _th_isspos[2]->Fill(_iss_gtod_lon*R2D, _iss_gtod_lat*R2D);
//     }
//     //     r=_gtod_obj.r; v=_gtod_obj.v; vtheta=_gtod_obj.vtheta; vphi=_gtod_obj.vphi;

// //     if(VLEVEL>=3){
// //       cout<<setprecision(15)<<xtime<<setprecision(5)<<endl;
// //       cout<<" Lat "<<_iss_lat*R2D<<" vs "<<_iss_gtod_lat*R2D<<
// // 	" [ diff: "<<(_iss_lat*R2D-_iss_gtod_lat*R2D)<<"]"<<endl;
// //       cout<<" Lon "<<_iss_lon*R2D<<" vs "<<_iss_gtod_lon*R2D<<
// // 	" [ diff: "<<(_iss_lon*R2D-_iss_gtod_lon*R2D)<<"]"<<endl;
// //       cout<<endl;
// //     }

//     //3.
//     _SAMSEventR->GetISSAtt(_iss_roll, _iss_pitch, _iss_yaw);

//     //********************************************************************/
//     //1- Compute AMS Pointing Direction in the J2000 Reference Frame 
//     //   [output in deg.]
//     //2- Convert pointing to Galactic Frame
//     //3- Loop on all the points in the Galactic Reference Frame
//     //********************************************************************/

//     //1.
//     _ret=_SAMSEventR->GetAMSJ2000Point(_zen_eq_ras, _zen_eq_dec, 
//  				       gPAR->FTAttitudeID,
// 				       gPAR->FTCoordinateID,
// 				       gPAR->FTTimeID
// 				       );

//     _th_returned[1]->Fill(_ret);
//     if(_ret<0){
//       cerr<<"[ltcube - ERROR] SAMSEventR::GetAMSJ2000Point function returned "<<_ret<<endl; 
//       exit(EXIT_FAILURE);
//     }
//     else if(_ret>0){
//       if(VLEVEL>=1) cout<<"[ltcube - WARNING] SAMSEventR::GetAMSJ2000Point function returned "<<_ret<<endl; 
//     }

//     _th_amspoi[gPAR->FTAttitudeID-1+3]->Fill(_zen_eq_ras,_zen_eq_dec);
//     Sph2Cart(_zen_eq_ras, _zen_eq_dec, _zen_eq_dir); //<-- Spherical to Cartesian conversion

//     //2.
//     _zen_gal_lon=_zen_eq_ras*D2R; _zen_gal_lat=_zen_eq_dec*D2R; 
//     FT_Equat2Gal(_zen_gal_lon, _zen_gal_lat, 1);  // J2000 to Galactic conversion, NOVAs alg.
//     if(VLEVEL>=10) cout<<"[ltcube - DEBUG] _zen_gal_lon*R2D, _zen_gal_lat*R2D :"<<_zen_gal_lon*R2D<<","<<_zen_gal_lat*R2D<<endl;

//     _ret=_SAMSEventR->GetAMSGalPoint(_zen_gal_lon, _zen_gal_lat, 
// 				       gPAR->FTAttitudeID,
// 				       gPAR->FTCoordinateID,
// 				       gPAR->FTTimeID
// 				       );
//     _th_returned[2]->Fill(_ret);

//     _th_amspoi[gPAR->FTAttitudeID-1]->Fill((_zen_gal_lon),(_zen_gal_lat));
//     Sph2Cart(_zen_gal_lon, _zen_gal_lat, _zen_gal_dir); //<-- Spherical to Cartesian conversion
//     if(VLEVEL>=10) cout<<"[ltcube - DEBUG] _zen_gal_lon, _zen_gal_lat :"<<_zen_gal_lon<<","<<_zen_gal_lat<<endl;

// //     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// //     //DEBUG!!!
// //     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// //     _posISS[0]=_iss_alt; _posISS[1]=_iss_lon; _posISS[2]=_iss_lat; 
// //     _velISS[0]=_iss_v; _velISS[1]=_iss_vphi; _velISS[2]=_iss_vtheta; 
// //     _yprISS[0]=_iss_yaw; _yprISS[1]=_iss_pitch; _yprISS[2]=_iss_roll;
// //     if(VLEVEL>=10){
// //       cout<<"[ltcube - DEBUG] POS ISS"<<_posISS[0]<<" "<<_posISS[1]<<" "<<_posISS[2]<<" "<<endl;
// //       cout<<"[ltcube - DEBUG] VEL ISS"<<_velISS[0]<<" "<<_velISS[1]<<" "<<_velISS[2]<<" "<<endl;
// //       cout<<"[ltcube - DEBUG] YPR ISS"<<_yprISS[0]<<" "<<_yprISS[1]<<" "<<_yprISS[2]<<" "<<endl;
// //     }

// //     Double_t Zvec[]={0,0,1};  Double_t ll=0,bb=0;
// //     Double_t Zvec_module=modulo_vet(Zvec);
// //     Double_t Zvec_theta=TMath::ACos(Zvec[2]/Zvec_module)*R2D;
// //     Double_t Zvec_phi=TMath::ATan2(Zvec[1],Zvec[0])*R2D;
// //     if(VLEVEL>=10) cout<<"[ltcube - DEBUG] Zvec phi and theta: "<<Zvec_phi<<" ("<<TMath::ACos(Zvec[0]/(TMath::Sin(Zvec_theta*D2R)*Zvec_module))*R2D<<") "<<Zvec_theta<<endl;
// //     get_ams_l_b_fromGTOD(Zvec[0], Zvec[1], Zvec[2], ll, bb, _posISS, _velISS, _yprISS, _SAMSEventR->JMDCEventTime());
// //     if(VLEVEL>=10) cout<<"[ltcube - DEBUG] ll, bb :"<<ll<<","<<bb<<endl;

// //     int result=0;
// //     Double_t phi, theta;
// //     int dt=0;
// //     int in_type=1;
// //     _ret=_SAMSEventR->GetAMSCoo(result, phi, theta,
// // 				_zen_gal_lon, _zen_gal_lat, 
// // 				gPAR->FTAttitudeID,
// // 				gPAR->FTCoordinateID,
// // 				gPAR->FTTimeID,
// // 				dt, in_type);

// //     if(VLEVEL>=10) cout<<"[ltcube - DEBUG] Reversed phi and theta: "<<phi<<" "<<theta<<endl;
// //     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// //     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//     //3.
//     int kk=0;

//     if(_SAMSEventR->InsideSAA(_iss_lon,_iss_lat)){
//       _saa_count = _saa_count + 1;
//       if(VLEVEL>=5) cout<<"[ltcube - INFO] saa_count "<<_saa_count<<endl;

//       _th_saapoi[0]->Fill((_zen_gal_lon),(_zen_gal_lat));
//       _th_saapoi[1]->Fill(_zen_eq_ras,_zen_eq_dec);

//     }
//     else if(mode==1){
//       //GEOMAGNETIC not implemented
//       //       geotomag(_a,_latitude,_longitude,&_mlatitude,&_mlongitude);
//       //       //              pilog<<"latitude: "<<latitude<<" "<<"longitude: "<<longitude<<endl;
//       //       //              pilog<<"mlatitude: "<<fabs(mlatitude*D2R)<<endl;
//       //       //              pilog<<" "<<endl;
//     }
//     else{


//       for(l=(-180.+(ANGBIN/2)); l<180.; l+=ANGBIN){ 
// 	k=0;
// 	for(b=(90.-(ANGBIN/2)); b>-90.; b-=ANGBIN){ 

// 	  // here I suppose I can descriminate w.r.t. a point in the sky.

// 	  //OLD METHOD
// 	  Sph2Cart(l, b, _skydir);                                                // Spherical to Cartesian conversion
// 	  vector_dot_product(_skydir, _zen_gal_dir, &_mod, &_anglezen);           // scalar product and theta angle between the two vector

// 	  if(VLEVEL>=10){

// 	    cout<<"[ltcube - DEBUG] Old method (before june 2013)"<<endl;
// 	    cout<<"[ltcube - DEBUG] 1- Skydir: x "<<_skydir[0]<<" y "<<_skydir[1]<<" z "<<_skydir[2]<<" <--  l: "<<l<<" b: "<<b<<endl;
// 	    cout<<"[ltcube - DEBUG] 2- Zen Gal Dir: x "<<_zen_gal_dir[0]<<" y "<<_zen_gal_dir[1]<<" z "<<_zen_gal_dir[2]<<endl;
// 	    cout<<"[ltcube - DEBUG] 3- Scalar Product Mod: "<<_mod<<" AMS theta: "<<_anglezen<<endl;
	  
// // 	    Double_t AMS_x=0, AMS_y=0, AMS_z=0;
// // 	    DEVget_ams_l_b_inverse_fromGTOD(AMS_x, AMS_y, AMS_z, l, b, _posISS, _velISS, _yprISS, _SAMSEventR->JMDCEventTime());
	    
// // 	    get_ams_l_b_fromGTOD(AMS_x, AMS_y, AMS_z, ll, bb, _posISS, _velISS, _yprISS, _SAMSEventR->JMDCEventTime());
// // 	    cout<<" l,b "<<l<<","<<b<<" AMSx,y,z "<<AMS_x<<","<<AMS_y<<","<<AMS_z<<" ll,bb "<<ll<<","<<bb<<endl;

// // 	    Double_t AMSvec[]={AMS_x,AMS_y,AMS_z};
// // 	    Double_t AMS_module=modulo_vet(AMSvec);
// // 	    Double_t AMS_theta=TMath::ACos(AMS_z/AMS_module)*R2D;
// // 	    Double_t AMS_phi=TMath::ACos(AMS_x/(AMS_module*TMath::Sin(AMS_theta*D2R)))*R2D;
// // 	    // vector_dot_product(AMSvec, Zvec, &AMS_module, &AMS_theta);
	    
// // 	    if(VLEVEL>=4) cout<<"[ltcube - DEBUG] module "<<AMS_module<<" theta "<<AMS_theta<<" phi "<<AMS_phi<<" vs "<<_mod<<","<<_anglezen<<endl;

// 	  }

// 	  if ((_anglezen>=_minta) && (_anglezen<_maxta)) {
 	
// 	    _th_livetime_allphi_oldm->Fill(l,b,(_delt*_lt_sysb0));

// 	  }


// 	  //NEW METHOD
// 	  int result=0;
// 	  Double_t AMS_phi, AMS_theta;
// 	  int dt=0; int in_type=1;
// 	  _ret=_SAMSEventR->GetAMSCoo(result, AMS_phi, AMS_theta,
// 				      l, b, 
// 				      gPAR->FTAttitudeID,
// 				      gPAR->FTCoordinateID,
// 				      gPAR->FTTimeID,
// 				      dt, in_type);

// 	  _th_returned[0]->Fill(_ret);

// 	  if(VLEVEL>=10) cout<<"[ltcube - DEBUG] 0- AMS phi: "<<AMS_phi<<" AMS theta: "<<AMS_theta<<" (vs old method (before june 2013) theta: "<<_anglezen<<")"<<endl;

// 	  double *pnt; int _phi_interval=0;
// 	  pnt = std::min_element(_phi_uppvalues, _phi_uppvalues+gPAR->LTCMapIncPhiBins, CompareDistanceFromLevel(AMS_phi));
// 	  _phi_interval=(pnt-_phi_uppvalues);

// 	  if(VLEVEL>=10){
// 	    cout<<"[ltcube - DEBUG] 1- AMS_phi: "<<AMS_phi<<" - Upper values: ";
// 	    for(int iphi=0; iphi<gPAR->LTCMapIncPhiBins; iphi++)
// 	      cout<<_phi_uppvalues[iphi]<<", ";
// 	    cout<<endl;
// 	    cout<<"[ltcube - DEBUG] 2- Phi interval id: "<<_phi_interval<<" [ "<<
// 	      _phi_uppvalues[_phi_interval]-(360./gPAR->LTCMapIncPhiBins)<<" - "<<
// 	      _phi_uppvalues[_phi_interval]<<" ] "<<endl;
// 	  }

// 	  // if ras,dec position within specified theta range, add event and (delt seconds x Live-Time) to exposed time 
// 	  if ((AMS_theta>=_minta) && (AMS_theta<_maxta)) {
 	
// 	    _faccum+=1;
// 	    tot[kk*_ndestep+k]+=_delt;

// 	    _th_livetime_allphi->Fill(l,b,(_delt*_lt_sysb0));
// 	    _th_livetime[_phi_interval]->Fill(l,b,(_delt*_lt_sysb0));

// 	    if(VLEVEL>=5){

// 	      cout<<"[ltcube -INFO] Scalar product with zenith: lon "<<l<<" lat "<<b
// 		  <<" minta "<<_minta<<" maxta "<<_maxta
// 		  <<" AMS theta [deg.] "<<AMS_theta<<endl;

// 	      for(Int_t ibinx=1; ibinx<=_th_livetime_allphi->GetNbinsX(); ibinx++){
// 		for(Int_t ibiny=1; ibiny<=_th_livetime_allphi->GetNbinsY(); ibiny++){
		  
// 		  Int_t ibin=_th_livetime_allphi->GetBin(ibinx, ibiny);
// 		  Double_t ll = _th_livetime_allphi->GetXaxis()->GetBinCenter(ibinx);
// 		  Double_t bb = _th_livetime_allphi->GetYaxis()->GetBinCenter(ibiny);
// 		  if(ll==l  && bb==b)
// 		    cout<<"[ltcube -INFO] Elapsed "<<_elapse<<" ibin: "<<ibinx<<","<<ibiny
// 			<<"("<<ibin<<") galcoo: "<<ll<<" "<<bb<<" -- TH_Livetime Content: "
// 			<<_th_livetime_allphi->GetBinContent(ibinx,ibiny)<<endl;
		
// 		}
// 	      }
// 	    }

// 	  }

//  	  ras=l*D2R; dec=b*D2R; DEVFT_Gal2Equat(ras, dec, 1);                   // Galactic to J2000 conversion, Novas Library Implemented
// 	  Sph2Cart((ras*R2D), (dec*R2D), _skydir);                              // Spherical to Cartesian conversion
// 	  vector_dot_product(_skydir, _zen_eq_dir, &_mod, &_anglezen);          // scalar product and theta angle between the two vector

// 	  // if ras,dec position within specified theta range...
// 	  if ((_anglezen>=_minta) && (_anglezen<_maxta)) {
// 	    // ... add event and delt seconds to exposed time  	
// 	    _th_livetime_J2000->Fill(ras*R2D,dec*R2D,(_delt*_lt_sysb0));
// 	  }
// 	  k++;

// 	}
// 	kk++;
//    }
//     }
    
//   }

//   cout<<"[ltcube - INFO] End of the Loop"<<endl;
//   cout<<endl;
		
//   for(j=0; j<_nrastep; j++){
//     for( i=0; i<_ndestep; i++){
//       if(ASCIIDUMP) pi<<tot[j*_ndestep+i]<<" ";
//     }
//     if(ASCIIDUMP) pi<<endl; // new line for each longitude bin  
//   }

//   double _norun_frac = _norun_count/(double)(_Nsteps-1)*100.;
//   cout<<"[ltcube - INFO] Fraction of time with DAQ not running :"<<_norun_frac<<" %"<<endl;
//   double _bigzenangdist_frac = _bigzenangdist_count/(double)(_Nsteps-1)*100.;
//   cout<<"[ltcube - INFO] Fraction of time with bad AMS-02 attitude :"<<_bigzenangdist_frac<<" %"<<endl;
//   double _saa_frac = _saa_count/(double)(_Nsteps-1)*100.;
//   cout<<"[ltcube - INFO] Fraction of time in SAA :"<<_saa_frac<<" %"<<endl;
//   cout<<"[ltcube - INFO] Output written to ROOT file:"<<_rootfile_name<<endl;
//   if(ASCIIDUMP){
//     cout<<_asciifile_name<<" (ASCII) in seconds"<<endl;
//     cout<<_logfile_name<<endl;
//   }
//   if(ASCIIDUMP){
//     pi.close();	
//     pilog.close();
//   }

//   cout<<endl;

//   //============================================================/
//   //Save Histo&Plots
//   //============================================================/

//   _rootfile.cd("aux_hist");

//   for(int ith=0; ith<3; ith++){

//     if(_th_isspos[ith]->GetEntries()!=0) _th_isspos[ith]->Write();
//     if(_th_amspoi[ith]->GetEntries()!=0) _th_amspoi[ith]->Write();
//     if(_th_amspoi[ith+3]->GetEntries()!=0) _th_amspoi[ith+3]->Write();

//   }
//   if(_th_saapoi[0]->GetEntries()!=0) _th_saapoi[0]->Write();
//   if(_th_saapoi[1]->GetEntries()!=0) _th_saapoi[1]->Write();
//   _th_livetime_J2000->Write();

//   // Francesca
//   th_vs_time->Write();
//   phi_vs_time->Write();
//   for(int ire=0; ire<3; ire++) {
//     _th_returned[ire]->Write();
//   }

//   _rootfile.cd("aux_graph");
//   _gr_DAQStatus->Write();
//   _gr_LiveTime->Write();
//   _gr_ZenAngDist->Write();

//   _rootfile.cd();
//   _th_livetime_allphi->Write();
//   _th_livetime_allphi_oldm->Write();
//   for(int iphi=0; iphi<gPAR->LTCMapIncPhiBins; iphi++){
//     _th_livetime[iphi]->Write();
//   }

//   _rootfile.Close();

  return 0;

}

//!AnyOption Interface function.
/*!

Command line option parsing function.

*/
int GetUserOpt(int argc, char* argv[]){

  AnyOption *opt = new AnyOption();


  //opt->setVerbose();                          // print warnings about unknown options
  //opt->autoUsagePrint(false);                 // print usage for bad options

  opt->addUsage( "Usage: dbcon_test.exe [options] [arguments]" );
  opt->addUsage( "" );
  opt->addUsage( "Options: " );
  opt->addUsage( "  -h, --help               Print this help " );
  // opt->addUsage( "  -d, --dump               Enable output recording in ASCII format" );
  // opt->addUsage( "  -b, --angbin <binsize>   Insert lat.&long. binning size [deg.]" );
  // opt->addUsage( "  -c, --config <actcode>   Insert attitude-coordinate-time code" );
  // opt->addUsage( "  -v, --verbose <vlevel>   Change verbosity level" );
  opt->addUsage( "" );
  opt->addUsage( "Arguments: " );
  // opt->addUsage( "  DateTimeInt              UTC Date&Time Interval(format: YYMMDDhhmmss_YYMMDDhhmmss)" );
  // opt->addUsage( "  ThetaAngInt              Theta angle interval respect to AMS Zenith(format: lTheta_uTheta, Theta in deg.)" );
  opt->addUsage( "" );
  //   opt->addUsage( "Notes: ");
  //   opt->addUsage( "  - Pre-Scaling Factors: Enter a sequence of DT and Pre-Scaling factor using the char \"|\" to separate them");
  //   opt->addUsage( "                          (Example: \"0x05|10|0x07|0|\" get 1 SCI Blocks every 10 and No CFG)                ");
  opt->addUsage( "" );

  opt->setFlag( "help", 'h' );  
  // opt->setFlag( "dump", 'd' );
  // opt->setOption( "angbin", 'b' );
  // opt->setOption( "config", 'c' );
  // opt->setOption( "verbose", 'v' );

  opt->processCommandArgs( argc, argv );      // go through the command line and get the options 

  if( ! opt->hasOptions()) {                  // print usage if no options 
    cout<<"[dbcon_test.exe - WARNING] No options/arguments specified. Using default settings. Type 'dbcon.exe --help' for usage."<<endl;
    delete opt;
    return OK_STAT;
  }

  if( opt->getFlag( "help" ) || opt->getFlag( 'h' ) ){      //get the options and the flag
    opt->printUsage();
    return 2;
  }
  // if( opt->getFlag( "dump" ) || opt->getFlag( 'd' ) ){
  //   ASCIIDUMP=true; 
  // }

  // if( opt->getValue( "angbin" ) != NULL ){
  //   ANGBIN=atof(opt->getValue( "angbin" ));
    
  //   if(!(ANGBIN>=.5 && ANGBIN<=10.)){
  //     cout<<"[ltcube.exe - ERROR] The specified angular binning, "<<
  // 	ANGBIN<<" is outside the allowed interval [.5-10 deg.]."<<endl;
  //     return 3;
  //   }

  // }

  // if( opt->getValue( "config" ) != NULL ){
  //   ACTCFG=atof(opt->getValue( "config" ));

  //   if(ACTCFG>444){
  //     cout<<"[ltcube.exe - ERROR] The specified configuration, "<<
  // 	ACTCFG<<" is outside the allowed interval [....]."<<endl;
  //     return 3;
  //   }

  // }

  // if( opt->getValue( "verbose" ) != NULL ){
  //   VLEVEL=atoi(opt->getValue( "verbose" )); 
  // }

  switch(opt->getArgc()){                                   //get the actual arguments after the options

  // case 2: 
  //   DTINTERVAL = opt->getArgv(0);
  //   TAINTERVAL = opt->getArgv(1);
  //   break;
    
  // case 1: 
  //   DTINTERVAL = opt->getArgv(0);
  //   break;

  default:
    return BAD_STAT;
    break;
  }

  delete opt;
    
  return OK_STAT;

}
