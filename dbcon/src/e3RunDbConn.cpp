//////////////////////////////////////////////////////////
// SAMSEventR.cpp 
//
// Created by F.Pilo on Nov 2012.
//
///////////////////////////////////////////////////////// 

#include "SAMSEventR.h"
#include "TParameters.h"

extern "C" int ISSLoad(const char *name, const char *line1, const char *line2);
extern "C" int ISSGTOD(float *r,float *t,float *p, float *v, float *vt, float *vp, float *grmedphi, double time);
extern "C" int ISSECI(double *x, double *y, double *z, double *vx, double *vy, double *vz, double time);

//! MemCpyISS2LE function.
/*!

Store ISS 2LE data in the AMSSetupR::fISSData map (time, AMSSetupR::ISSData) for
the time interval between t1 and t2.

\param setup AMSSetupR object containing the fISSData map 
\param t1 time interval lower limit, UTC time
\param t2 time interval upper limit, UTC time

Copied From gbatch: .... method.

*/
void MemCpyISS2LE(AMSSetupR *setup, unsigned int t1, unsigned int t2){

  char amsdatadir[]="/afs/cern.ch/ams/Offline/AMSDataDir";
  char *amsd=getenv("AMSDataDir");
  if(!amsd || !strlen(amsd))amsd=amsdatadir;
  string ifile=amsd;
  ifile+="/v5.00";   //<--- Messo a mano!!!!
  ifile+="/ISS_tlefile.txt";
  ifstream ifbin;
  ifbin.clear();
  ifbin.open(ifile.c_str());
  
  if(ifbin){
    
    char stime[80],namei[80], line1i[80], line2i[80];
    while(!ifbin.eof()){
      namei[0]=0;
      line1i[0]=0;
      line2i[0]=0;
      stime[0]=0;
      unsigned int tme;
      ifbin.getline(stime,75);
      tme=atol(stime);
      ifbin.getline(namei,75);
      ifbin.getline(line1i,75);
      ifbin.getline(line2i,75);
      if(!ifbin.eof() ){
	AMSSetupR::ISSData a;
 	a.TL1=line1i;
 	a.TL2=line2i;
 	a.Name=namei;
 	if(tme<t1)setup->fISSData.clear();
 	setup->fISSData.insert(make_pair(tme,a));
	if(tme>t2)break;
      }
    }
    ifbin.close();

    cout<<"[MemCpyISS2LE - INFO] 2LE DATA copied in memory from file: "<<ifile.c_str()<<endl; 

  }
}

//! LoadISS2LE function.
/*!

Copied From gbatch: AMSNtuple::LoadISS method

*/
bool LoadISS2LE(AMSSetupR *setup, time_t xtime){

  static unsigned int time=0;
  unsigned int tl=0;

  if(setup && setup->fISSData.size()>0){
    for(AMSSetupR::ISSData_i i=setup->fISSData.begin();i!=setup->fISSData.end();i++){
      if(fabs(i->first-xtime)<fabs(tl-xtime))tl=i->first;
    }
    if(tl!=time && tl){
      time=tl;
      AMSSetupR::ISSData_i i=setup->fISSData.find(time);
      if(i!=setup->fISSData.end() && ISSLoad((const char *)i->second.Name,(const char *)i->second.TL1,(const char*)i->second.TL2)){
	cout<<"[LoadISS2LE - INFO] Loading a new set of 2LE data"<<endl;
	return true;
      }
      else{
	static int print=0;
	time=0;
	if(print++<100)cerr<<"[LoadISS2LE - ERROR] Unable to load "<<i->second.Name<<" "<<i->first<<endl;
	return false ;
      }
    }
    return time!=0;
    
  }
  else{
    static int print=0;
    if(print++<100)cerr<<"[LoadISS2LE - ERROR] No Setup Found or ISSData Empty "<<setup<<endl;
    return false;
  }
}

// SAMSEventR default constructor
//------------------------------------------------------------
SAMSEventR::SAMSEventR(){

  _AMSChain = NULL;
  _AMSEvR = NULL;
  _AMSSetupR = NULL;
  _HeaderR = NULL;

  _vlevel = 0;
  _rootfile_name = "./data/amsevent.root";

}

// SAMSEventR Init member function
//------------------------------------------------------------
void SAMSEventR::Init(string rootfile_name){

  if(!rootfile_name.empty()) _rootfile_name = rootfile_name;
  Init();
}

// SAMSEventR Init member function
//------------------------------------------------------------
void SAMSEventR::Init(){

  _AMSChain = new AMSChain();
  _AMSChain->Add(_rootfile_name.c_str());

  int nentries=_AMSChain->GetEntries();

  _AMSEvR=(AMSEventR *)_AMSChain->GetEvent();
  if(_AMSEvR){ if(_vlevel>=1) cout<<"[SAMSEventR::Init - INFO] AMS event loaded ./data/amsevent.root file. AMSEventR Class correctly initialized."<<endl; }
  else{
    cerr<<"[SAMSEventR::Init - ERROR] AMS ROOT file "<<_rootfile_name<<" is missing or no AMSEventR inside."<<endl;
    exit(EXIT_FAILURE);
  }

  _HeaderR = &(_AMSEvR->fHeader);
  RstEventTime();
  _HeaderR->Run=0;

  _AMSSetupR = _AMSEvR->getsetup();
  if(!_AMSSetupR){
    cerr<<"[SAMSEventR::Init - ERROR] AMSEventR setup is not correctly initialized."<<endl;
    exit(EXIT_FAILURE);
  }
  _AMSSetupR->fHeader.FEvent = 0;
  _AMSSetupR->fHeader.LEvent = 0;

}

// SAMSEventR GetRunID member function
//------------------------------------------------------------
bool SAMSEventR::CheckNewRun(){

  unsigned int _ret;
  const unsigned int ISSOrbitT = 5566; //ISS Orbit Period [sec.]
  unsigned int RunT = (unsigned int) ISSOrbitT/4;

  unsigned int RunID;
  unsigned int FEvent; unsigned int FEventTime;
  unsigned int LEvent; unsigned int LEventTime;

  _ret = GetRunSum(RunID, FEvent, FEventTime, LEvent, LEventTime, JMDCEventTime());
  //cout<<"[SAMSEventR -- INFO] CheckNewRun -- "<<_ret<<" "<<_SAMSEventR->JMDCEventTime()<<" --- "<<RunID<<" "<<FEvent<<" "<<FEventTime<<" "<<LEvent<<" "<<LEventTime<<endl;

  if(_HeaderR->Run==0 || ((_HeaderR->Run+RunT)<_HeaderR->Time[0])){

    _HeaderR->Run = _HeaderR->Time[0];
    _AMSSetupR->Reset();
    _AMSSetupR->fHeader.Run = _HeaderR->Run; 
    _AMSSetupR->fHeader.FEvent = 0;
    _AMSSetupR->fHeader.LEvent = 0;

    MemCpyISS2LE(_AMSSetupR,_HeaderR->Run-60,_HeaderR->Run+3600);   

    return true;
  }
  else return false;

}

// SAMSEventR GetRunInfo member function
//------------------------------------------------------------
int SAMSEventR::GetRunInfo(){

  unsigned int _ret;
  const unsigned int ISSOrbitT = 5566; //ISS Orbit Period [sec.]
  unsigned int RunT = (unsigned int) ISSOrbitT/4;

  unsigned int RunID;
  unsigned int FEvent; unsigned int FEventTime;
  unsigned int LEvent; unsigned int LEventTime;

  _ret = GetRunSum(RunID, FEvent, FEventTime, LEvent, LEventTime, JMDCEventTime());
  cout<<"[SAMSEventR -- INFO] GetRunInfo -- "<<_ret<<" "<<JMDCEventTime()<<" --- "<<RunID<<" "<<FEvent<<" "<<FEventTime<<" "<<LEvent<<" "<<LEventTime<<endl;

  switch(_ret){

  case 0:
    if(_HeaderR->Run!=RunID){

      //Nominal DAQ, New Run
      _HeaderR->Run = RunID;
      _AMSSetupR->Reset();
      _AMSSetupR->fHeader.Run = _HeaderR->Run; 
      _AMSSetupR->fHeader.FEvent = FEvent;
      _AMSSetupR->fHeader.FEventTime = FEventTime;
      _AMSSetupR->fHeader.LEvent = LEvent;
      _AMSSetupR->fHeader.LEventTime = LEventTime;
      
      MemCpyISS2LE(_AMSSetupR,_HeaderR->Run-60,_HeaderR->Run+3600);   
      
      return 1;

    }
    else return 0;

  case 1:
    //DAQ is stopped, no RunID 
    _HeaderR->Run = 0;
    _AMSSetupR->Reset();
    _AMSSetupR->fHeader.Run = _HeaderR->Run; 
    _AMSSetupR->fHeader.FEvent = 0;
    _AMSSetupR->fHeader.LEvent = 0;

    return 2;

  case 2:
  case 3:
    //DAQ is stopped, no RunID 
    _HeaderR->Run = 0;
    _AMSSetupR->Reset();
    _AMSSetupR->fHeader.Run = _HeaderR->Run; 
    _AMSSetupR->fHeader.FEvent = 0;
    _AMSSetupR->fHeader.LEvent = 0;
    return 3;

  case 4:
    if(_HeaderR->Run!=RunID){

      //Nominal DAQ, New Run
      _HeaderR->Run = RunID;
      _AMSSetupR->Reset();
      _AMSSetupR->fHeader.Run = _HeaderR->Run; 
      _AMSSetupR->fHeader.FEvent = FEvent;
      _AMSSetupR->fHeader.FEventTime = FEventTime;
      _AMSSetupR->fHeader.LEvent = LEvent;
      _AMSSetupR->fHeader.LEventTime = LEventTime;
      
      MemCpyISS2LE(_AMSSetupR,_HeaderR->Run-60,_HeaderR->Run+3600);   
      
      return 1;

    }
    else return 0;

  default:
    return 10;
  }

//OLD VERSION - before 28-01-2014
//Simulated RunID
//   else if(_ret>1){
  
//     if(_HeaderR->Run==0 || ((_HeaderR->Run+RunT)<_HeaderR->Time[0])){
      
//       _HeaderR->Run = _HeaderR->Time[0];
//       _AMSSetupR->Reset();
//       _AMSSetupR->fHeader.Run = _HeaderR->Run; 
//       _AMSSetupR->fHeader.FEvent = 0;
//       _AMSSetupR->fHeader.LEvent = 0;
      
//       MemCpyISS2LE(_AMSSetupR,_HeaderR->Run-60,_HeaderR->Run+3600);   
      
//       return 3;
//     }
//     else
//       return 0;

}

// SAMSEventR UpdISSPos member function
//------------------------------------------------------------
int SAMSEventR::UpdISSPos(){

  //Event UTC Time
  double xtime=_HeaderR->UTCTime();

  if(!LoadISS2LE(_AMSSetupR,xtime)) return 2;
  ISSGTOD(&_HeaderR->RadS, &_HeaderR->ThetaS, &_HeaderR->PhiS,
	  &_HeaderR->VelocityS, &_HeaderR->VelTheta, &_HeaderR->VelPhi,
	  &_grmedphi, xtime);
  
  //Subtract Greenwich Mean Sidereal Time (in rad, predict definition)
  _HeaderR->PhiS = fmod((_HeaderR->PhiS-_grmedphi)+PI2, PI2);
  _HeaderR->VelPhi = fmod((_HeaderR->VelPhi-_grmedphi)+PI2, PI2);

  //MY PREDICT: requires ISSECI function implemented in predict.c, FT_Equat2GTOD function from FrameTrans.C
  //     double w=0.;
  //     double x=0.,y=0.,z=0.;
  //     double vx=0.,vy=0.,vz=0.;
  //     ISSECI(&x, &y, &z, &vx, &vy, &vz, xtime); 
  //     cout<<" ECI   : x "<<x<<" km | y "<<y<<" km | z "<<z<<" km "<<endl;
  //     w=sqrt(pow(x,2)+pow(y,2)+pow(z,2));
  //     _iss_lat = (float) asin(z/w); _iss_lon = (float)atan2(y,x);
  //     cout<<" ECI   : theta "<<_iss_lat*R2D<<" deg. | phi "<<fmod(_iss_lon+PI2,PI2)*R2D<<" deg. "<<endl;
  //     FT_Equat2GTOD(x, y, z, xtime);
  //     w=sqrt(pow(x,2)+pow(y,2)+pow(z,2));
  //     _iss_lat = (float) asin(z/w); _iss_lon = (float)atan2(y,x);
  //     cout<<" GTOD  : theta "<<_iss_lat*R2D<<" deg. | phi "<<fmod(_iss_lon+PI2,PI2)*R2D<<" deg. "<<endl;
  
  return 0;

}

// SAMSEventR GetISSTRSPos member function
//------------------------------------------------------------
int SAMSEventR::GetISSTRSPos(unsigned int datasetID, float &r, float &theta, float &phi){
  
  int ret=0;

  //Event UTC Time
  double xtime=_HeaderR->UTCTime();

  switch(datasetID){

  case 0:
 
    r = _HeaderR->RadS;
    theta = _HeaderR->ThetaS;
    phi = _HeaderR->PhiS;

    break;

  default:

    cerr<<"[SAMSEventR::GetISSTRSPos - ERROR] Unknown datasetID, "<<datasetID<<endl;
    return -1;

    break;

  }

  if (phi>PI){ //if longitude is >180deg then substract 360deg
    phi+=-PI2;
  }

  return ret;

}

// SAMSEventR GetISSTRSVel member function
//------------------------------------------------------------
int SAMSEventR::GetISSTRSVel(unsigned int datasetID, float &v, float &vtheta, float &vphi){
  
  int ret=0;

  //Event UTC Time
  double xtime=_HeaderR->UTCTime();

  switch(datasetID){

  case 0:
 
    v = _HeaderR->VelocityS;
    vtheta = _HeaderR->VelTheta;
    vphi = _HeaderR->VelPhi;

    break;

  default:

    cerr<<"[SAMSEventR::GetISSTRSVel - ERROR] Unknown datasetID, "<<datasetID<<endl;
    return -1;

    break;

  }

  return ret;

}

// LOCAL func - ISSAtt Class
//------------------------------------------------------------
class ISSAtt{
public:
  float Roll; ///< Roll in LVLH, Rad
  float Pitch; ///< Pitch in LVLH, Rad
  float Yaw; ///< Yaw in LVLH, Rad
};

typedef map <double,ISSAtt> ISSAtt_m;
typedef map <double,ISSAtt>::iterator ISSAtt_i;
ISSAtt_m fISSAtt;      ///< ISS Attitude angles map

// LOCAL func - LoadISSAtt function
//------------------------------------------------------------
int SAMSEventR::LoadISSAtt(unsigned int t1, unsigned int t2){

  string AMSISSlocal="/afs/cern.ch/ams/Offline/AMSDataDir";
  char postfix[]="/altec/";
  char * AMSDataDir=getenv("AMSDataDir");
  if (AMSDataDir && strlen(AMSDataDir)){
    AMSISSlocal=AMSDataDir;
  }
   
  AMSISSlocal+=postfix;
  const char * AMSISS=getenv("AMSISS");
  if (!AMSISS || strlen(AMSISS))AMSISS=AMSISSlocal.c_str();
  
  
  
  if(t1>t2){
    cerr<< "AMSSetupR::LoadISSAtt-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
    return 2;
  }
  else if(t2-t1>864000){
    cerr<< "AMSSetupR::LoadISSAtt-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<endl;
    t2=t1+864000;
  }
  const char fpatb[]="ISS_ATT_EULR_LVLH-";
  const char fpate[]="-24H.csv";
  const char fpate2[]="-24h.csv";
  
  // convert time to GMT format
  
  // check tz
  unsigned int tzd=0;
  tm tmf;
  {
    char tmp2[255];
    time_t tz=t1;
    strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
    strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadISSAtt-I-TZDSeconds "<<tzd<<endl;
    
  }
  {
    char tmp2[255];
    time_t tz=t1;
    strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
    strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadISSAtt-I-TZDSeconds "<<tzd<<endl;
    
  }
  
  char tmp[255];
  time_t utime=t1;
  strftime(tmp, 40, "%Y", gmtime(&utime));
  unsigned int yb=atol(tmp);
  strftime(tmp, 40, "%j", gmtime(&utime));
  unsigned int db=atol(tmp);
  utime=t2;
  strftime(tmp, 40, "%Y", gmtime(&utime));
  unsigned int ye=atol(tmp);
  strftime(tmp, 40, "%j", gmtime(&utime));
  unsigned int de=atol(tmp);
  
  unsigned int yc=yb;
  unsigned int dc=db;
  int bfound=0;
  int efound=0;
  while(yc<ye || dc<=de){
    string fname=AMSISS;
    fname+=fpatb;
    char utmp[80];
    sprintf(utmp,"%u-%03u",yc,dc);
    fname+=utmp;
    fname+=fpate;
    ifstream fbin;
    fbin.close();    
    fbin.clear();
    cout<<"Try opening file "<<fname.c_str()<<endl;
    fbin.open(fname.c_str());
    if(!fbin){
      // change 24H to 24h
      fname=AMSISS;
      fname+=fpatb;
      char utmp[80];
      sprintf(utmp,"%u-%03u",yc,dc);
      fname+=utmp;
      fname+=fpate2;
      fbin.close();    
      fbin.clear();
      fbin.open(fname.c_str());
    }
    if(fbin){
      while(fbin.good() && !fbin.eof()){
        char line[120];
        fbin.getline(line,119);
        
        if(isdigit(line[0])){
	  char *pch;
	  pch=strtok(line,".");
	  ISSAtt a;
	  if(pch){
	    strptime(pch,"%Y_%j:%H:%M:%S",&tmf);
	    time_t tf=mktime(&tmf)+tzd;
	    pch=strtok(NULL,",");
	    char tm1[80];
	    sprintf(tm1,".%s",pch);
	    double tc=tf+atof(tm1);
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    a.Yaw=atof(pch)*3.14159267/180;
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    a.Pitch=atof(pch)*3.14159267/180;
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    a.Roll=atof(pch)*3.14159267/180;
	    fISSAtt.insert(make_pair(tc,a));
          
	    if(tc>=t1 && tc<=t2){
	      if(abs(bfound)!=2){
		fISSAtt.clear();
		fISSAtt.insert(make_pair(tc,a));
		bfound=bfound?2:-2;
		//               cout <<" line "<<line<<" "<<tc<<endl;
	      }
	    }
	    else if(tc<t1)bfound=1;
	    else if(tc>t2){
	      efound=1;
	      break;
	    }
	  }   
	}
      }
    }
    else{
      cerr<<"AMSSetupR::LoadISSAtt-E-UnabletoOpenFile "<<fname<<endl;
    }
    dc++;
    if((dc>365 && (yc-2012)%4!=0) || (dc>366)){
      dc=1;
      yc++;
    }
  }


  int ret;
  if(bfound>0 &&efound)ret=0;
  else if(!bfound && !efound )ret=2;
  else ret=1;
  cout<< "AMSSetupR::LoadISSAtt-I- "<<fISSAtt.size()<<" Entries Loaded"<<endl;

  return ret;
}

// LOCAL func - getISSAtt function
//------------------------------------------------------------
int SAMSEventR::getISSAtt(float &roll, float&pitch,float &yaw,double xtime){

#ifdef __ROOTSHAREDLIBRARY__
  static unsigned int ssize=0;
  static unsigned int stime[2]={0,0};
#pragma omp threadprivate (stime)
#pragma omp threadprivate (ssize)
  if(stime[0] && stime[1] && (xtime<stime[0] || xtime>stime[1]))fISSAtt.clear();
  if(fISSAtt.size()==0){
    const int dt=120;
    if(xtime<stime[0] || xtime>stime[1] || ssize){
      stime[0]=_runid?_runid-dt:xtime-dt;
      stime[1]=_runid?_runid+3600:xtime+3600;

      if(xtime<stime[0])stime[0]=xtime-dt;
      if(xtime>stime[1])stime[1]=xtime+dt;
      LoadISSAtt(stime[0],stime[1]);
      ssize=fISSAtt.size();

    }
  }
#endif 




  if(fISSAtt.size()==0)return 2;

  ISSAtt_i k=fISSAtt.lower_bound(xtime);
  if(k==fISSAtt.begin()){
    roll=k->second.Roll;
    pitch=k->second.Pitch;
    yaw=k->second.Yaw;
    return 1;
  }
  if(k==fISSAtt.end()){
    k--;
    roll=k->second.Roll;
    pitch=k->second.Pitch;
    yaw=k->second.Yaw;
    return 1;
  }
  if(xtime==k->first){
    roll=k->second.Roll;
    pitch=k->second.Pitch;
    yaw=k->second.Yaw;
    return 0;
  }
  k--;
  float s0[2]={-1.,-1};
  double tme[2]={0,0};
  tme[0]=k->first;
  ISSAtt_i l=k;
  l++;
  tme[1]=l->first;
  s0[0]=k->second.Roll;
  s0[1]=l->second.Roll;
  roll=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=k->second.Pitch;
  s0[1]=l->second.Pitch;
  pitch=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=k->second.Yaw;
  s0[1]=l->second.Yaw;
  yaw=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  const float dmax=301;
  if(fabs(tme[1]-tme[0])>dmax)return 3;
  else return 0;


}

// SAMSEventR UpdISSAtt member function
//------------------------------------------------------------
int SAMSEventR::UpdISSAtt(){

  int ret=0;
  streamsize ss = cout.precision();

  //Event UTC Time
  double xtime=_HeaderR->UTCTime();

  //     if(VLEVEL==0) capture_cmd_prepare();
#ifdef LOCALFUN
  ret=getISSAtt(_HeaderR->Roll, _HeaderR->Pitch, _HeaderR->Yaw, xtime);
#else
  ret=_AMSSetupR->getISSAtt(_HeaderR->Roll, _HeaderR->Pitch, _HeaderR->Yaw, xtime);
#endif
  //     if(VLEVEL==0) capture_cmd_output();
  if(ret==3){
    cout<<"[SAMSEventR::UpdISSAtt - WARNING] getISSAtt function returned "<<ret;
    cout<<" - bad extrapolation (gap in csv DATA file > 300 sec)";
    cout<<" for UTC time "<<setprecision(15)<<xtime<<setprecision(ss);
    cout<<" and RUNID "<<GetRunID()<<endl;
  }
  else if(ret==2){
    cerr<<"[SAMSEventR::UpdISSAtt - WARNING] getISSAtt function returned "<<ret;
    cerr<<" - no data in ISSAtt map";
    cerr<<" for UTC time "<<setprecision(15)<<xtime<<setprecision(ss);
    cerr<<" and RUNID "<<GetRunID()<<endl;
    exit(EXIT_FAILURE);
  }
  
  return ret;

}

// SAMSEventR GetISSAtt member function
//------------------------------------------------------------
void SAMSEventR::GetISSAtt(float &roll, float &pitch, float &yaw){
  
  roll=_HeaderR->Roll;  
  pitch=_HeaderR->Pitch;
  yaw=_HeaderR->Yaw;

}

// SAMSEventR GetAMSJ2000Point member function
//------------------------------------------------------------
int SAMSEventR::GetAMSJ2000Point(double &ras, double &dec, 
				 unsigned int use_att, 
				 unsigned int use_coo, 
				 unsigned int  use_time){

  int ret=0, result=0;
  int dt=0;
  int out_type=2;

  //Local function defined in astroutils.h
  //   ret=GetEqCoo(_AMSEvR, result, ras, dec, false, false, false, false);  //Star Tracker No, GPS No, GTOD No, CTRS No
  //   //     cout<<"ret: "<<_ret<<" res GetEqCoo: "<<((result>>4)&0x1)<<"|"<<((result>>3)&0x1)<<"|"<<((result>>2)&0x1)<<"|"<<((result>>1)&0x1)<<"|"<<(result&0x1);
  //   //     cout<<endl;

  //gbatch - Old version, bef. 3-11-12
  //   ret=_AMSEvR->GetGalCoo(result, lon, lat, false, false, false, false);  //Star Tracker No, GPS No, GTOD No, CTRS No
  //   ras=lon*D2R; dec=lat*D2R;
  //   FT_Gal2Equat(ras, dec); 
  //   ras=*R2D; dec=*R2D;
  //   //     cout<<"ret: "<<_ret<<" res GetGalCoo: "<<((result>>4)&0x1)<<"|"<<((result>>3)&0x1)<<"|"<<((result>>2)&0x1)<<"|"<<((result>>1)&0x1)<<"|"<<(result&0x1);
  //   //     cout<<endl;

  //gbatch - New Haino version after 3-11-12, 
  //  -- theta in ams coo system = 0 - up-going .. was PI!!! BAH!!!
  //  -- out_type = 2 - Equatorial coord. (R.A. and Dec.)
  ret=_AMSEvR->GetGalCoo(result, ras, dec, PI, 0., use_att, use_coo, use_time, dt, out_type);
  if(_vlevel>=2){
    cout<<"[SAMSEventR::GetAMSJ2000Point - INFO] AMSEventR::GetGalCoo function returned "<<ret<<", result: "<<
      "|"<<((result>>6)&0x1)<<"|"<<((result>>5)&0x1)<<"|"<<((result>>4)&0x1)<<
      "|"<<((result>>3)&0x1)<<"|"<<((result>>2)&0x1)<<"|"<<((result>>1)&0x1)<<"|"<<(result&0x1);
    cout<<endl;
  }

  return ret;

}

// SAMSEventR GetAMSGalPoint member function
//------------------------------------------------------------
int SAMSEventR::GetAMSGalPoint(double &lon, double &lat, 
				 unsigned int use_att, 
				 unsigned int use_coo, 
				 unsigned int  use_time){

  int ret=0, result=0;
  int dt=0;
  int out_type=1;

  //gbatch - New Haino version after 3-11-12, 
  //  -- theta in ams coo system = 0 - up-going
  //  -- out_type = 1 - Galactic coord. (Lon. and Lat.)
  ret=_AMSEvR->GetGalCoo(result, lon, lat, PI, 0., use_att, use_coo, use_time, dt, out_type);
  if(_vlevel>=2){
    cout<<"[SAMSEventR::GetAMSGalPoint - INFO] AMSEventR::GetGalCoo function returned "<<ret<<", result: "<<
      "|"<<((result>>6)&0x1)<<"|"<<((result>>5)&0x1)<<"|"<<((result>>4)&0x1)<<
      "|"<<((result>>3)&0x1)<<"|"<<((result>>2)&0x1)<<"|"<<((result>>1)&0x1)<<"|"<<(result&0x1);
    cout<<endl;
  }

  return ret;

}

// SAMSEventR GetAMSCoo member function
//------------------------------------------------------------
int SAMSEventR::GetAMSCoo(int &result, double &AMS_phi, double &AMS_theta, 
			 double glon, double glat, 
			 int use_att, int use_coo, int use_time, 
			 double dt, int in_type){

  enum { bSTK  = 0, bINTL = 1, bTLE  = 2, bGTOD = 3, bCTRS = 4, bGPSW = 5,
	 bGPST = 6, bGPSC = 7 };

  enum { rNAtt = 1, rNCoo = 2, rBCoo = 3, rNTim = 4 };

  enum { uLVLH = 1, uINTL = 2, uSTK  = 3,
	 uTLE  = 1, uCTRS = 2, uGTOD = 3, uGPSW = 4,
	 uUTCT = 1, uGPST = 2, uGPSC = 3 };

  int ret = result = 0;

  bool gal_coo = (in_type == 1) ? true : false;
  if (in_type == 3) use_att = uLVLH;

  double xtime = _HeaderR->UTCTime();

  // Check and use AMS GPS Time if OK
  if (use_time == uGPST) {
    unsigned int time,nanotime;
    if (_AMSEvR->GetGPSTime(time, nanotime)) ret = rNTim;
    else {
      xtime = double(time)+double(nanotime)/1.e9-AMSEventR::gpsdiff(time);
      result |= (1<<bGPST);
    }
  }

  // Check and use AMS GPS Time if OK
  else if (use_time == uGPSC){
    double err;
    if (_AMSEvR->UTCTime(xtime, err)) ret = rNTim;
    else result |= (1<<bGPSC);
  }

  // Check and use AMS STK if OK
  if (use_att == uSTK) {

//TO BE IMPLEMENTED!!!
//     AMSSetupR::AMSSTK stk;
//     if (_AMSSetupR && !_AMSSetupR->getAMSSTK(stk, xtime+dt)) {
//       result |= (1<<bSTK);
//       int ret2 = _HeaderR->get_gal_coo(glon, glat, theta, phi, stk.cam_id,
// 				     stk.cam_ra, stk.cam_dec, stk.cam_or,
// 				     gal_coo);
//       return (ret2 == 0) ? ret : ret2;
//     }
//     else ret = rNAtt;

    ret = rNAtt;
  }

  // Check and use INTL if OK, INTL is radiant!!!
  else if (use_att == uINTL) {
    float Roll, Pitch, Yaw;
    if (_AMSSetupR && !_AMSSetupR->getISSINTL(Roll, Pitch, Yaw, xtime+dt)) {
      double YPR[3];
      YPR[0] = Yaw*R2D;
      YPR[1] = Pitch*R2D;
      YPR[2] = Roll*R2D;
      result |= (1<<bINTL);
      int ret2 = DEVget_AMS_coo(AMS_phi, AMS_theta, glon, glat, YPR, xtime, gal_coo);
      return (ret2 == 0) ? ret : ret2;
    }
    else ret = rNAtt;
  }

  double YPR[3] = { _HeaderR->Yaw,       _HeaderR->Pitch,  _HeaderR->Roll };
  double RPT[3] = { _HeaderR->RadS,      _HeaderR->PhiS,   _HeaderR->ThetaS };
  double VPT[3] = { _HeaderR->VelocityS, _HeaderR->VelPhi, _HeaderR->VelTheta };
  float RTP[3], VTP[3];
  if (_AMSSetupR && _AMSSetupR->getISSTLE(RTP, VTP, xtime) == 0) {
    RPT[0] = RTP[0];
    RPT[1] = RTP[2];
    RPT[2] = RTP[1];
    VPT[0] = VTP[0];
    VPT[1] = VTP[2];
    VPT[2] = VTP[1];
  }
   
  bool _gtod = true;

  float Roll, Pitch, Yaw;
  if (_AMSSetupR && !_AMSSetupR->getISSAtt(Roll,Pitch,Yaw,xtime)){
    YPR[0] = Yaw*R2D;
    YPR[1] = Pitch*R2D;
    YPR[2] = Roll*R2D;
  }

  static int mprint = 0;
  double prec = 80e5;

  // Check and use GTOD if OK
  if (use_coo == uGTOD) {
    AMSSetupR::ISSGTOD gtod;
    if (_AMSSetupR && !_AMSSetupR->getISSGTOD(gtod, xtime+dt)) {
      double diff = _AMSEvR->get_coo_diff(RPT, gtod.r, gtod.theta, gtod.phi);
      if (diff < prec) {
	RPT[0] = gtod.r; RPT[1] = gtod.phi;  RPT[2] = gtod.theta;
	VPT[0] = gtod.v; VPT[1] = gtod.vphi; VPT[2] = gtod.vtheta;
	result |= (1<<bGTOD);
      }
      else {
	if (mprint++ <= 10) {
	  cerr << "AMSEventR::GetGalCoo-E-GTODCooTooDistantFrom2Ele "
	       << diff << " km";
	  if (mprint == 10) cerr << " (LastMessage)";
	  cerr << endl;
	}
	result |= (1<<bTLE);
	ret = 3;
      }
    }
    else {
      result |= (1<<bTLE);
      ret = 2;
    }
  }

  // Check and use CTRS if OK
  else if (use_coo == uCTRS) {
    AMSSetupR::ISSCTRSR ctrs; 
    if (_AMSSetupR && !_AMSSetupR->getISSCTRS(ctrs, xtime+dt)) {
      double diff = _AMSEvR->get_coo_diff(RPT, ctrs.r, ctrs.theta, ctrs.phi);
      if (diff < prec) {
	RPT[0] = ctrs.r; RPT[1] = ctrs.phi;  RPT[2] = ctrs.theta;
	VPT[0] = ctrs.v; VPT[1] = ctrs.vphi; VPT[2] = ctrs.vtheta;
	result |= (1<<bCTRS);
	_gtod = false;
      }
      else {
	if (mprint++ <= 10) {
	  cerr << "AMSEventR::GetGalCoo-E-GTODCooTooDistantFrom2Ele "
	       << diff << " km";
	  if (mprint == 10) cerr << " (LastMessage)";
	  cerr << endl;
	}
	result |= (1<<bTLE);
	ret = 3;
      }
    }
    else {
      result |= (1<<bTLE);
      ret = 2;
    }
  }

  // Check and use GPS if OK
  else if (use_coo == uGPSW) {
    AMSSetupR::GPSWGS84R gpsw;
    if (_AMSSetupR && !_AMSSetupR->getGPSWGS84(gpsw, xtime+dt)) {
      double diff = _AMSEvR->get_coo_diff(RPT, gpsw.r, gpsw.theta, gpsw.phi);
      if (diff < prec) {
	RPT[0] = gpsw.r; RPT[1] = gpsw.phi;  RPT[2] = gpsw.theta;
	VPT[0] = gpsw.v; VPT[1] = gpsw.vphi; VPT[2] = gpsw.vtheta;
	result |= (1<<bGPSW);
	_gtod = false;
      }
      else {
	if (mprint++ <= 10) {
	  cerr << "AMSEventR::GetGalCoo-E-GTODCooTooDistantFrom2Ele "
	       << diff << " km";
	  if (mprint == 10) cerr << " (LastMessage)";
	  cerr << endl;
	}
	result |= (1<<bTLE);
	ret = 3;
      }
    }
    else {
      result |= (1<<bTLE);
      ret = 2;
    }
  }
  else result |= (1<<bTLE);

  // timer.Stop();
//   cout << " Cpu time fo GetCoo: " <<  setprecision(10) << timer.CpuTime() << endl; 

  int ret2 = (in_type == 3)
    ? _HeaderR->get_gtod_coo(glon, glat, AMS_theta, AMS_phi, RPT, VPT, YPR, xtime, _gtod)
    : DEVget_AMS_coo(AMS_phi, AMS_theta, glon, glat, RPT, VPT, YPR, xtime, gal_coo);
  return (ret2 == 0) ? ret : ret2;

}

bool pnpoly(int npol, float *xp, float *yp, float x, float y){

  int i, j;
  bool c=false;
  for (i = 0, j = npol-1; i < npol; j = i++) {
    if ((((yp[i] <= y) && (y < yp[j])) ||
	 ((yp[j] <= y) && (y < yp[i]))) &&
	(x < (xp[j] - xp[i]) * (y - yp[i]) / (yp[j] - yp[i]) + xp[i]))
      c = !c;
  }
  return c;

}

// SAMSEventR InsideSAA member function
//------------------------------------------------------------
bool SAMSEventR::InsideSAA(float &theta, float &phi){

  bool in_SAA = false;

  float alpha=0;
  float Phi_SAA[5] = {-83.-alpha, -38.+0.35*alpha, 0.+alpha, 0.+alpha, -83.-alpha};
  float Theta_SAA[5] = {-1., -1., -22., -60., -60.};    
  float ThetaS = theta*R2D;
  float PhiS = phi*R2D;
  
  switch(gPAR->SAAStrategy){
  case 0:
    in_SAA=false;
    break;
    
  case 1:
    if(PhiS>180 && PhiS<360) PhiS = -360+PhiS;
    in_SAA = pnpoly(5, Phi_SAA, Theta_SAA, PhiS, ThetaS);
    break;
    
  default:
    cout<<"SAMSEventR - WARNING] In function InsideSAA, not identified SAA Strategy ID: "<<gPAR->SAAStrategy<<endl;
    in_SAA=false;
    break;
  }

  return in_SAA;

}

// LOCAL func - LiveTime Class
//------------------------------------------------------------
class LiveTimes{
public:
  float lt_sysb0; ///< Live-Time with system busy 0, %
  float lt_sysb1; ///< Live-Time with system busy 1 (only FE), %
};

typedef map <double,LiveTimes> LiveTimes_m;
typedef map <double,LiveTimes>::iterator LiveTimes_i;
LiveTimes_m fLiveTimes;      ///< Live-Times map

// SAMSEventR LoadLiveTime member function
//------------------------------------------------------------
int SAMSEventR::LoadLiveTime(unsigned int t1, unsigned int t2){

  int ret;

  string AMSDATAlocal;
  try {
    AMSDATAlocal = gSystem->Getenv("LTCUBE");
    if(AMSDATAlocal==NULL) {
      cerr<<"[SAMSEventR - ERROR] In SAMSEvenetR::LoadLiveTime";
      cerr<<", please set the environment variable $LTCUBE.";
      cerr<<" It must point on your LTCUBE working directory."<<endl;
      throw 1;
    }
  }
  catch (Int_t e)
  {
    throw e;
  }
  AMSDATAlocal+="/data";

  char * DATAPath = getenv("DATAPath");
  if (DATAPath && strlen(DATAPath)){
    AMSDATAlocal=DATAPath;
  }
  
  char postfix[]="/DAQ/";
  AMSDATAlocal+=postfix;

  if(t1>t2){
    cerr<<"[SAMSEventR::LoadLiveTime - ERROR] Begin Time is not less than End Time: "<<t1<<" "<<t2<<endl;
    return 2;
  }
  else if(t2-t1>864000){
    cerr<<"[SAMSEventR::LoadLiveTime - ERROR] End Time / Begin Time Difference Too Big (Max864000): "<<t2-t1<<endl;
    t2=t1+864000;
  }
  const char fpatb[]="JLV1_Livetimes_";
  const char fpate[]="-24H.csv";

  // convert time to GMT format
  // check tz
  unsigned int tzd=0;
  tm tmf;
  {
    char tmp2[255];
    time_t tz=t1;
    strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
    strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    //     cout<< "AMSSetupR::LoadISSAtt-I-TZDSeconds "<<tzd<<endl;

  }
  {
    char tmp2[255];
    time_t tz=t1;
    strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
    strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    //     cout<< "AMSSetupR::LoadISSAtt-I-TZDSeconds "<<tzd<<endl;

  }

  char tmp[255];
  time_t utime=t1;
  strftime(tmp, 40, "%Y", gmtime(&utime));
  unsigned int yb=atol(tmp);
  strftime(tmp, 40, "%j", gmtime(&utime));
  unsigned int db=atol(tmp);
  utime=t2;
  strftime(tmp, 40, "%Y", gmtime(&utime));
  unsigned int ye=atol(tmp);
  strftime(tmp, 40, "%j", gmtime(&utime));
  unsigned int de=atol(tmp);
    
  unsigned int yc=yb;
  unsigned int dc=db;
  int bfound=0;
  int efound=0;
  while(yc<ye || dc<=de){
    string fname=AMSDATAlocal.c_str();
    fname+=fpatb;
    char utmp[80];
    sprintf(utmp,"%u-%03u",yc,dc);
    fname+=utmp;
    fname+=fpate;
    ifstream fbin;
    fbin.close();    
    fbin.clear();
    fbin.open(fname.c_str());
    if(fbin){
      while(fbin.good() && !fbin.eof()){
        char line[120];
        fbin.getline(line,119);
        
        if(isdigit(line[0])){
	  char *pch;
	  pch=strtok(line,".");
	  LiveTimes a;
	  if(pch){
	    strptime(pch,"%Y_%j:%H:%M:%S",&tmf);
	    time_t tf=mktime(&tmf)+tzd;
	    pch=strtok(NULL,",");
	    char tm1[80];
	    sprintf(tm1,".%s",pch);
	    double tc=tf+atof(tm1);
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    a.lt_sysb0=atof(pch);
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    a.lt_sysb1=atof(pch);
	    pch=strtok(NULL,",");

	    fLiveTimes.insert(make_pair(tc,a));
	    if(tc>=t1 && tc<=t2){
	      if(abs(bfound)!=2){
		fLiveTimes.clear();
		fLiveTimes.insert(make_pair(tc,a));
		bfound=bfound?2:-2;
		//cout <<" line "<<line<<" "<<tc<<endl;
	      }
	    }
	    else if(tc<t1) bfound=1;
	    else if(tc>t2){
	      efound=1;
	      break;
	    }
	  }   
	}
      }
    }
    else{
      cerr<<"[AMSSetupR::LoadLiveTime - ERROR] Unable to open the file "<<fname<<endl;
    }
    dc++;
    if((dc>365 && (yc-2012)%4!=0) || (dc>366)){
      dc=1;
      yc++;
    }
  }
  
  if(bfound>0 && efound) ret=0;
  else if(!bfound && !efound ) ret=2;
  else ret=1;

  cout<<"[AMSSetupR::LoadLiveTime - INFO] "<<fLiveTimes.size()<<" Entries Loaded"<<endl;
  
  return ret;

}

// SAMSEventR GetLiveTime member function
//------------------------------------------------------------
int SAMSEventR::GetLiveTime(float &lt_sysb0, float &lt_sysb1, double xtime){

  int _ret=0;
  static unsigned int ssize=0;
  static unsigned int stime[2]={0,0};
  
  if(stime[0] && stime[1] && (xtime<stime[0] || xtime>stime[1]))fLiveTimes.clear();
  if(fLiveTimes.size()==0){
    const int dt=300;
    if(xtime<stime[0] || xtime>stime[1] || ssize){
      stime[0]=_HeaderR->Run?_HeaderR->Run-dt:xtime-dt;
      stime[1]=_HeaderR->Run?_HeaderR->Run+3600:xtime+3600;
      //        if(_HeaderR->FEventTime-dt<_HeaderR->Run && _HeaderR->LEventTime+1>_HeaderR->Run && _HeaderR->Run!=0){
      //  	stime[0]=_HeaderR->FEventTime-dt;
      //  	stime[1]=_HeaderR->LEventTime+dt;
      //        }
      if(xtime<stime[0])stime[0]=xtime-dt;
      if(xtime>stime[1])stime[1]=xtime+dt;
      _ret=LoadLiveTime(stime[0],stime[1]);
      ssize=fLiveTimes.size();
    
    }
  }

  if(_ret==1) return 4;
  if(fLiveTimes.size()==0 || _ret==2) return 2;
  
  LiveTimes_i k=fLiveTimes.lower_bound(xtime);

  if(k==fLiveTimes.begin() && xtime<k->first){
    lt_sysb0=k->second.lt_sysb0;
    lt_sysb1=k->second.lt_sysb1;
    return 3;
  }
  if(k==fLiveTimes.end()){
    k--;
    lt_sysb0=k->second.lt_sysb0;
    lt_sysb1=k->second.lt_sysb1;
    return 1;
  }
  if(xtime==k->first){
    lt_sysb0=k->second.lt_sysb0;
    lt_sysb1=k->second.lt_sysb1;
    return 1;
  }

  k--;
  float s0[2]={-1.,-1};
  double tme[2]={0,0};
  tme[0]=k->first;
  LiveTimes_i l=k;
  l++;
  tme[1]=l->first;
  s0[0]=k->second.lt_sysb0;
  s0[1]=l->second.lt_sysb0;
  lt_sysb0=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=k->second.lt_sysb1;
  s0[1]=l->second.lt_sysb1;
  lt_sysb1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  const float dmax=301;
  if(fabs(tme[1]-tme[0])>dmax) return 5;
  else return 0;

}

// LOCAL func - RunSum Class
//------------------------------------------------------------
class RunSum{
public:
  unsigned int RunID;      ///< Run ID
  unsigned int FEvent;     ///< First Event in run
  unsigned int FEventTime; ///< JMDC Time-stamp of the First Event
  unsigned int LEvent;     ///< Last Event in run
  unsigned int LEventTime; ///< JMDC Time-stamp of the Last Event
};

typedef map <double,RunSum> RunSum_m;
typedef map <double,RunSum>::iterator RunSum_i;
RunSum_m fRunSum;      ///< Live-Times map

// SAMSEventR LoadRunSum member function
//------------------------------------------------------------
int SAMSEventR::LoadRunSum(unsigned int t1, unsigned int t2){

  int _ret;

  string AMSDATAlocal;
  try {
    AMSDATAlocal = gSystem->Getenv("LTCUBE");
    if(AMSDATAlocal==NULL) {
      cerr<<"[SAMSEventR - ERROR] In SAMSEvenetR::LoadRunSum";
      cerr<<", please set the environment variable $LTCUBE.";
      cerr<<" It must point on your LTCUBE working directory."<<endl;
      throw 1;
    }
  }
  catch (Int_t e)
  {
    throw e;
  }
  AMSDATAlocal+="/data";

  char * DATAPath = getenv("DATAPath");
  if (DATAPath && strlen(DATAPath)){
    AMSDATAlocal=DATAPath;
  }
  
  char postfix[]="/DAQ/";
  AMSDATAlocal+=postfix;

  if(t1>t2){
    cerr<<"[SAMSEventR::LoadRunSum - ERROR] Begin Time is not less than End Time: "<<t1<<" "<<t2<<endl;
    return 2;
  }
  else if(t2-t1>864000){
    cerr<<"[SAMSEventR::LoadRunSum - ERROR] End Time / Begin Time Difference Too Big (Max864000): "<<t2-t1<<endl;
    t2=t1+864000;
  }
  const char fpatb[]="RUN_Summary_";
  const char fpate[]="-24H.csv";

  // convert time to GMT format
  // check tz
  unsigned int tzd=0;
  tm tmf;
  {
    char tmp2[255];
    time_t tz=t1;
    strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
    strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    //     cout<< "AMSSetupR::LoadISSAtt-I-TZDSeconds "<<tzd<<endl;

  }
  {
    char tmp2[255];
    time_t tz=t1;
    strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
    strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    //     cout<< "AMSSetupR::LoadISSAtt-I-TZDSeconds "<<tzd<<endl;

  }

  char tmp[255];
  time_t utime=t1;
  strftime(tmp, 40, "%Y", gmtime(&utime));
  unsigned int yb=atol(tmp);
  strftime(tmp, 40, "%j", gmtime(&utime));
  unsigned int db=atol(tmp);
  utime=t2;
  strftime(tmp, 40, "%Y", gmtime(&utime));
  unsigned int ye=atol(tmp);
  strftime(tmp, 40, "%j", gmtime(&utime));
  unsigned int de=atol(tmp);
    
  unsigned int yc=yb;
  unsigned int dc=db;
  int bfound=0;
  int efound=0;
  while(yc<ye || dc<=de){
    string fname=AMSDATAlocal.c_str();
    fname+=fpatb;
    char utmp[80];
    sprintf(utmp,"%u-%03u",yc,dc);
    fname+=utmp;
    fname+=fpate;
    ifstream fbin;
    fbin.close();    
    fbin.clear();
    fbin.open(fname.c_str());
    if(_vlevel>=0) cout <<"[AMSSetupR::LoadRunSum - INFO] Opening file "<<fname.c_str()<<endl;
    if(fbin){
      while(fbin.good() && !fbin.eof()){
        char line[120];
        fbin.getline(line,119);
        
        if(isdigit(line[0])){
	  char *pch;
	  pch=strtok(line,".");
	  RunSum a;
	  if(pch){
	    strptime(pch,"%Y_%j:%H:%M:%S",&tmf);
	    time_t tf=mktime(&tmf)+tzd;
	    pch=strtok(NULL,",");
	    char tm1[80];
	    sprintf(tm1,".%s",pch);
	    double tc=tf+atof(tm1);
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    a.RunID=atoi(pch);
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    a.FEvent=atoi(pch);
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    a.FEventTime=atoi(pch);
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    a.LEvent=atoi(pch);
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    a.LEventTime=atoi(pch);
	    pch=strtok(NULL,",");
	    fRunSum.insert(make_pair(tc,a));
	    if(bfound==2) 
	      if(_vlevel>=5) cout <<"[AMSSetupR::LoadRunSum - INFO]  Loaded line "<<line<<" "<<tc<<" "<<a.RunID<<" -- "<<a.FEventTime<<" - "<<a.LEventTime<<endl;
	    if(tc>=t1 && tc<=t2){
	      if(abs(bfound)!=2){
		fRunSum.clear();
		fRunSum.insert(make_pair(tc,a));
		if(_vlevel>=5) cout <<"[AMSSetupR::LoadRunSum - INFO]  Loaded line "<<line<<" "<<tc<<" "<<a.RunID<<" -- "<<a.FEventTime<<" - "<<a.LEventTime<<endl;
		bfound=bfound?2:-2;
	      }
	    }
	    else if(tc<t1) bfound=1;
	    else if(tc>t2){
	      efound=1;
	      break;
	    }
	  }   
	}
      }
    }
    else{
      cerr<<"[SAMSEventR:::LoadRunSum - ERROR] Unable to open the file "<<fname<<endl;
    }
    dc++;
    if((dc>365 && (yc-2012)%4!=0) || (dc>366)){
      dc=1;
      yc++;
    }
  }

  if(bfound>0 && efound) _ret=0;
  else if(!bfound && !efound) _ret=2;
  else _ret=1;

  if(_vlevel>=0){
    cout<<"[AMSSetupR::LoadRunSum - INFO] "<<fRunSum.size()<<" Entries Loaded"<<endl;
  }

  return _ret;
}

// SAMSEventR GetRunSum member function
//------------------------------------------------------------
int SAMSEventR::GetRunSum(unsigned int &RunID,
			  unsigned int &FEvent, unsigned int &FEventTime, 
			  unsigned int &LEvent, unsigned int &LEventTime, 
			  double xtime){
  
  int _ret=0;
  static unsigned int ssize=0;
  static unsigned int stime[2]={0,0};
  
  if(stime[0] && stime[1] && (xtime<stime[0] || xtime>stime[1])) fRunSum.clear();
  if(fRunSum.size()==0){
    const int dt=3600;
    if(xtime<stime[0] || xtime>stime[1] || ssize){
      stime[0]=_HeaderR->Run?_HeaderR->Run-dt:xtime-dt;
      stime[1]=_HeaderR->Run?_HeaderR->Run+dt:xtime+dt;
      
      if(xtime<stime[0]) stime[0]=xtime-dt;
      if(xtime>stime[1]) stime[1]=xtime+dt;
      _ret=LoadRunSum(stime[0],stime[1]);
      ssize=fRunSum.size();
      if(_vlevel>=0) cout<<"[AMSSetupR::GetRunSum - INFO] Load ret: "<<_ret<<" ssize: "<<ssize<<" stime "<<stime[0]<<" - "<<stime[1]<<endl;    
    }
  }
  
  if(fRunSum.size()==0 || _ret==2) return 2;

  RunSum_i k=fRunSum.lower_bound(xtime);

  if(k==fRunSum.begin() && xtime<k->first){
    return 3;
  }
  if(xtime==k->first){
    RunID=k->second.RunID;
    FEvent=k->second.FEvent;
    FEventTime=k->second.FEventTime;
    LEvent=k->second.LEvent;
    LEventTime=k->second.LEventTime;

    if(_ret==1) return 4;
    else return 0;
  }

  k--;
  if(xtime<=k->second.LEventTime){
    RunID=k->second.RunID;
    FEvent=k->second.FEvent;
    FEventTime=k->second.FEventTime;
    LEvent=k->second.LEvent;
    LEventTime=k->second.LEventTime;

    if(_ret==1) return 4;
    else return 0;

  }
  else{
    RunID=0;
    FEvent=0;
    FEventTime=0;
    LEvent=0;
    LEventTime=0;
  } 

  cout<<"k: "<<k->first<<" xtime: "<<xtime<<" lower_bound: "<<k->second.LEventTime<<endl;

  return 1;
  
}

// SAMSEventR GetRTIInfo member function
//------------------------------------------------------------
void SAMSEventR::GetRTIInfo(){

    AMSSetupR::RTI a;
    _AMSSetupR->getRTI(a, (unsigned int)  JMDCEventTime());
    float _lt_sysb0, _lt_sysb1;         ///< AMS Total and FEE only Live-Time
    GetLiveTime(_lt_sysb0, _lt_sysb1, JMDCEventTime());

    cout<<"RUNID: "<<a.run<<" (vs. my RUNID "<<_HeaderR->Run<<")"<<endl;
    cout<<"GOOD: "<<a.good<<endl;
    cout<<"NEV: "<<a.nev<<endl;
    cout<<"Live-Time: "<<a.lf<<" (vs. my Live-Time: "<<_lt_sysb0/100.<<endl;
    cout<<"ZENITH: "<<a.zenith<<" degree (vs. my Zenith "<<_HeaderR->Zenith()<<" degree)"<<endl;

}

// SAMSEventR GetZenAngDist member function
//------------------------------------------------------------
double SAMSEventR::GetZenAngDist(){

  double _zenangdist=0;
  AMSSetupR::RTI a;

  switch(gPAR->ZenithCompMethod){

  case 0:

    _AMSSetupR->getRTI(a, (unsigned int) JMDCEventTime());
    _zenangdist = a.zenith;
    break;

  case 1:

    _zenangdist = _HeaderR->Zenith();
    break;

  default:
    break;

  }

  return _zenangdist;

}
