//****************************************************************************************************
//
//							coincAtCnaf.exe							   
//                                                                                                                                             
//   This program looks for the coincicidences between n-EEE telescopes. It uses a        
//   configuration file to to choose the two  telescopes under investigation and specify 
//   the input data directory.The output file contains a tree with basic information     
//   about the tracks and the time difference between the events collected by the    
//   telescopes.                                
//
//   Last updates by: F.Noferini, F.Pilo
//                                              
//   Original version by: P.La Rocca, F.Riggi               
//   Contacts: paola.larocca@ct.infn.it, francesco.riggi@ct.infn.it       
//
//****************************************************************************************************

#include <string>
#include <cmath>
#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>

#include <TFile.h>
#include <TTree.h>
#include <Riostream.h>
#include <TMath.h>
#include <stdio.h>
#include <TString.h>
#include <TArrayI.h>
#include <TChain.h>
#include "TRandom.h"
#include <TH1I.h>
#include <TH1F.h>

#include "e3Correlation.h"
#include "anyoption.h"

using namespace std;

enum STATUS {
  OK_STAT=0x01,
  BAD_STAT=0x00
};

bool USEDB;
string STATIONIDS;
string DATETIMEWIN;
string DSTPATH;

int GetUserOpt(int, char**);

//! Main routine
/*!
  Main routine.
*/
int main(int argc,char *argv[]){

  int _status=0;        ///< Process status
  int _ret=0;          

  //======================================== 
  // Set default values
  //========================================  

  USEDB = kTRUE;
  DSTPATH = "/recon";

  //======================================== 
  // Parse inline command options
  //========================================  

  _status=GetUserOpt(argc, argv);              
  if(_status!=OK_STAT){ 
    if(_status==0) cerr<<"[CoincAtCnaf.exe - ERROR] GetUserOpt returned error code "
		       <<_status<<" - too few arguments. "<<endl;
    exit(EXIT_FAILURE);
  }

  //Telescope list
  string _tel1,  _tel2;
  istringstream _iss(STATIONIDS);
  getline(_iss, _tel1, ',');
  getline(_iss, _tel2);

  if(_tel2.empty() || _tel1.empty()){
    cerr<<"[CoincAtCnaf.exe - ERROR] The telescope list doesn't contain 2 IDs"<<endl;
    exit(EXIT_FAILURE);
  }

  // //Time window
  // string _dtWinLow, _dtWinUp;
  // istringstream _iss(DATETIMEWIN);
  // getline(_iss, _dtWinLow, '/');
  // getline(_iss, _dtWinUp);

  // if(__dtWinUp.empty()){
  //   cout<<"[CoincAtCnaf.exe - WARNING] Only 1 date specified. All the runs collected in the corresponding day will be processed."<<endl;
  // }

  // printf("list of options to overwrite the config file infos:\n");
  // printf("-d DATE = to pass the date from line command\n");
  // printf("-s SCHOOL_1 SCHOOL_2 = to pass the schools from line command\n");
  // printf("-p PATH = to pass the path of the reco dirs");

  int kNoConfigFile = 0;

  // for(Int_t i=1;i < argc;i++){
  //   if(CfrString(argv[i],"-d")){
  //     if(i+1 > argc){
  //       printf("date is missing\n");
  // 	return 1;
  //     }
       
  //     date = argv[i+1];
  //     i++;
  //     kNoConfigFile++;
  //   }
  //   if(CfrString(argv[i],"-s")){                       
  //     if(i+2 > argc){
  //       printf("Two schools name have to be provided\n");
  //       return 2;
  //     }
 
  //     sc1 = argv[i+1];
  //     sc2 = argv[i+2];

  //     i+=2;
  //     kNoConfigFile++;
  //   }
  //   if(CfrString(argv[i],"-p")){
  //     if(i+1 > argc){
  //       printf("path is missing\n");
  //       return 1;
  //     }

  //     path = argv[i+1];
  //     i++;  
  //     kNoConfigFile++;
  //   }

  // }

  correlation_EEE(DATETIMEWIN.c_str(),_tel1.c_str(),_tel2.c_str(),DSTPATH.c_str(),USEDB);

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

  opt->addUsage( "Usage: coincAtCnaf.exe [options] [arguments]" );
  opt->addUsage( "" );
  opt->addUsage( "Options: " );
  opt->addUsage( "  -h, --help               Print this help " );
  opt->addUsage( "  -l, --file_list               Read DST file list from external file " );
  // opt->addUsage( "  -s, --host <hostname>   Insert mysql server hostname or ip address" );
  // opt->addUsage( "  -v, --verbose <vlevel>   Change verbosity level" );
  opt->addUsage( "" );
  opt->addUsage( "Arguments: " );
  opt->addUsage( "  StationIDs               Telescope list (comma separated, 2 IDs required)" );
  opt->addUsage( "  DateTimeWindows          UTC Date&Time Interval(format: YYYY-MM-DD_hh:mm:ss/YYYY- MM-DD_hh:mm:ss)" );
  opt->addUsage( "  DstPath          DST files absolute path" );
  opt->addUsage( "" );
  //opt->addUsage( "Notes: ");
  //opt->addUsage( "" );

  opt->setFlag( "help", 'h' );  
  opt->setFlag( "file_list", 'l' );  
  // opt->setOption( "host", 's' );
  // opt->setOption( "verbose", 'v' );

  opt->processCommandArgs( argc, argv );      // go through the command line and get the options 

  if( ! opt->hasOptions()) {                  // print usage if no options 
    cout<<"[coincAtCnaf.exe - WARNING] No options/arguments specified. Using default settings. Type 'coincAtCnaf.exe --help' for usage."<<endl;
    delete opt;
    return OK_STAT;
  }

  if( opt->getFlag( "help" ) || opt->getFlag( 'h' ) ){      //get the options and the flag
    opt->printUsage();
    return 2;
  }

  if( opt->getFlag( "file_list" ) || opt->getFlag( 'l' ) ){      //get the options and the flag
    USEDB=kFALSE;
  }

  // if( opt->getValue( "verbose" ) != NULL ){
  //   VLEVEL=atoi(opt->getValue( "verbose" )); 
  // }

  switch(opt->getArgc()){                                   //get the actual arguments after the options

  case 3: 
    STATIONIDS = opt->getArgv(0);
    DATETIMEWIN = opt->getArgv(1);
    DSTPATH = opt->getArgv(2);
    break;

  case 2: 
    STATIONIDS = opt->getArgv(0);
    DATETIMEWIN = opt->getArgv(1);
    break;
    
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
