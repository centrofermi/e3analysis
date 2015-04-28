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

int VLEVEL = 0;

int GetUserOpt(int, char**);

//! Main routine
/*!
  Main routine.
*/
int main(int argc, char *argv[]) {
  
  int _status=0;        ///< Process status
  int _ret=0;          
  e3RunDbConn *_mysqlCon = NULL;

  //======================================== 
  // Parse inline command options
  //========================================  

  _status=GetUserOpt(argc, argv);              
  if(_status!=OK_STAT){ 
    if(_status==0) cerr<<"[dbcon_test.exe - ERROR] GetUserOpt returned error code "
		       <<_status<<" - too few arguments. "<<endl;
    exit(EXIT_FAILURE);
  }

  //======================================== 
  // Init db connection
  //========================================  

  _mysqlCon = new e3RunDbConn("131.154.96.193","eee","eee-monitoring","eee_rundb2");
  _mysqlCon->SetVerbosity(VLEVEL);
  _mysqlCon->Init();

  //======================================== 
  // Get run list
  //========================================  

  _mysqlCon->GetRunList("2015-03-01","2015-03-05",1);

  //======================================== 
  // Close db connection
  //========================================  

  _mysqlCon->CloseConn();

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
  opt->addUsage( "  -s, --host <hostname>   Insert mysql server hostname or ip address" );
  // opt->addUsage( "  -d, --dump               Enable output recording in ASCII format" );
  // opt->addUsage( "  -b, --angbin <binsize>   Insert lat.&long. binning size [deg.]" );
  // opt->addUsage( "  -c, --config <actcode>   Insert attitude-coordinate-time code" );
  opt->addUsage( "  -v, --verbose <vlevel>   Change verbosity level" );
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
  opt->setOption( "host", 's' );
  opt->setOption( "verbose", 'v' );

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

  if( opt->getValue( "verbose" ) != NULL ){
    VLEVEL=atoi(opt->getValue( "verbose" )); 
  }

  switch(opt->getArgc()){                                   //get the actual arguments after the options

  // case 2: 
  //   DTINTERVAL = opt->getArgv(0);
  //   TAINTERVAL = opt->getArgv(1);
  //   break;
    
  // case 1: 
  //   DTINTERVAL = opt->getArgv(0);
  //   break;

  default:
    return OK_STAT;
    break;
  }

  delete opt;
    
  return OK_STAT;

}
