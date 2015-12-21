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
#include <sstream>
#include <iterator>

#include "anyoption.h"
#include "e3RunDbConn.h"

using namespace std;

enum STATUS {
  OK_STAT=0x01,
  BAD_STAT=0x00
};

int VLEVEL = 0;
string STATIONID;
string DATETIMEWIN;

int GetUserOpt(int, char**);

//! Main routine
/*!
  Main routine.
*/
int getQuello(int argc, char *argv[]) {
  
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

  string _dtWinLow, _dtWinUp;
  istringstream _iss(DATETIMEWIN);
  getline(_iss, _dtWinLow, '/');
  getline(_iss, _dtWinUp);
  vector<string> _fileNameList;
  if(_mysqlCon->GetRunList(_fileNameList,STATIONID,_dtWinLow,_dtWinUp,1)!=0)
    exit(EXIT_FAILURE);

  cout<<"[e3RunDbConn::GetRunList - INFO] Query results:"<<endl;
  /* Print _fileNameList vector to console */
  copy(_fileNameList.begin(), _fileNameList.end(), ostream_iterator<string>(cout, "\n"));

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
  opt->addUsage( "  -v, --verbose <vlevel>   Change verbosity level" );
  opt->addUsage( "" );
  opt->addUsage( "Arguments: " );
  opt->addUsage( "  StationID               Telescope name" );
  opt->addUsage( "  DateTimeWindows              UTC Date&Time Interval(format: YY-MM-DD_hh:mm:ss/YY- MM-DD_hh:mm:ss)" );
  opt->addUsage( "" );
  //opt->addUsage( "Notes: ");
  //opt->addUsage( "" );

  opt->setFlag( "help", 'h' );  
  opt->setOption( "host", 's' );
  opt->setOption( "verbose", 'v' );

  opt->processCommandArgs( argc, argv );      // go through the command line and get the options 

  if( ! opt->hasOptions()) {                  // print usage if no options 
    cout<<"[dbcon_test.exe - WARNING] No options/arguments specified. Using default settings. Type 'dbcon.exe --help' for usage."<<endl;
    delete opt;
    return BAD_STAT;
  }

  if( opt->getFlag( "help" ) || opt->getFlag( 'h' ) ){      //get the options and the flag
    opt->printUsage();
    return 2;
  }

  if( opt->getValue( "verbose" ) != NULL ){
    VLEVEL=atoi(opt->getValue( "verbose" )); 
  }

  switch(opt->getArgc()){                                   //get the actual arguments after the options

  case 2: 
    STATIONID = opt->getArgv(0);
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
