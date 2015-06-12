//////////////////////////////////////////////////////////
// e3RunDbConn.cpp 
//
// Created by F.Pilo on April 2015.
//
///////////////////////////////////////////////////////// 

#include "e3RunDbConn.h"

//! finish_with_error function.
/*!
  Return error, close db connection and exit

  \param MYSQL C api connection
*/
void e3RunDbConn::finish_with_error()
{

  cerr<<"[- ERROR ] "<<mysql_error(_mysqlCon)<<endl;
  mysql_close(_mysqlCon);
  exit(1);        

}

// E3RUNDBCONN constructor
//------------------------------------------------------------
e3RunDbConn::e3RunDbConn(const string hostName, const string dbUser, const string dbPwd,  const string dbName){

  _vLevel = 0;

  _hostName = hostName;
  _dbUser = dbUser;
  _dbPwd = dbPwd;
  _dbName = dbName;

  _mysqlCon = NULL;

  _e3Start.tm_hour = 0; _e3Start.tm_min = 0; _e3Start.tm_sec = 0; _e3Start.tm_isdst = 0;
  _e3Start.tm_year = 107; _e3Start.tm_mon = 0; _e3Start.tm_mday = 1;
  _e3StartTime = mktime(&_e3Start) - timezone; 

}

// e3RunDbConn Init member function
//------------------------------------------------------------
void e3RunDbConn::Init(){

  _mysqlCon = mysql_init(NULL);
  if (_mysqlCon == NULL) 
    {
      cerr<<"[e3RunDbConn::Init - ERROR] "<<mysql_error(_mysqlCon)<<endl;
      exit(1);
    }

  if (mysql_real_connect(_mysqlCon, _hostName.c_str(), "eee", "eee-monitoring","eee_rundb2",0, NULL, 0) == NULL) {
    finish_with_error();
  }  
  else if(_vLevel>1) cout<<"[e3RunDbConn::Init - INFO] DB "<<_dbName<<" on host "<<_hostName<<" connected."<<endl;


}

// e3RunDbConn CloseConn member function
//------------------------------------------------------------
void e3RunDbConn::CloseConn(){

  mysql_close(_mysqlCon);

}

// e3RunDbConn GetRunList member function
//------------------------------------------------------------
int e3RunDbConn::GetRunList(vector<string>& fileNameList, const string stationID, const string tWinLowStr, const string tWinUpStr, const string whereClausesStr, unsigned int outFormat){

  //***************************
  //Time window limit
  //***************************
  char dummy;
  stringstream lineStream;

  struct tm _tWinLow;
  memset(&_tWinLow, 0, sizeof _tWinLow);  // set all fields to 0
  lineStream.str(tWinLowStr);

  // Parse the input and store each value into the correct variables.
  lineStream >> _tWinLow.tm_year >> dummy >> _tWinLow.tm_mon >> dummy >> _tWinLow.tm_mday >> dummy >> _tWinLow.tm_hour >> dummy >> _tWinLow.tm_min >> dummy >> _tWinLow.tm_sec;
  if(!lineStream && (_tWinLow.tm_year==0 || _tWinLow.tm_mon==0 || _tWinLow.tm_mday==0)) {
    cerr << "[e3RunDbConn::GetRunList - ERROR] You entered an invalid date/time value." << endl;
    return 1;
  }

  _tWinLow.tm_year -= 1900;
  _tWinLow.tm_mon --;
  time_t _tWinLowTime = mktime(&_tWinLow) - timezone -_overtime; 
  double _tWinLowSec = difftime(_tWinLowTime,_e3StartTime);

  struct tm _tWinUp;
  memset(&_tWinUp, 0, sizeof _tWinUp);  // set all fields to 0
  lineStream.clear();
  lineStream.str(tWinUpStr);
  // Parse the input and store each value into the correct variables.
  lineStream >> _tWinUp.tm_year >> dummy >> _tWinUp.tm_mon >> dummy >> _tWinUp.tm_mday >> dummy >> _tWinUp.tm_hour >> dummy >> _tWinUp.tm_min >> dummy >> _tWinUp.tm_sec;
  if(!lineStream && (_tWinUp.tm_year==0 || _tWinUp.tm_mon==0 || _tWinUp.tm_mday==0)) {
    cerr << "[e3RunDbConn::GetRunList - ERROR] You entered an invalid date/time value." << endl;
    return 1;
  }

  _tWinUp.tm_year -= 1900;
  _tWinUp.tm_mon --;
  time_t _tWinUpTime = mktime(&_tWinUp) - timezone + _overtime; 
  if(tWinUpStr == tWinLowStr) _tWinUpTime += 24*60*60; //Whole day if low time and up time are the same
  double _tWinUpSec = difftime(_tWinUpTime,_e3StartTime);

  //***************************
  //WHERE clauses
  //***************************
  string _clause;
  vector<string> _whereClauses; 
  istringstream _iss(whereClausesStr);
  while(getline(_iss, _clause, ',')){
    if(!_clause.empty())
      _whereClauses.push_back(_clause);
  }

  //***************************
  //Output format options
  //***************************
  bool _fullPath=false;
  if(outFormat == 1) _fullPath=true;

  //***************************
  //Query string
  //***************************
  _queryStr.str("");
  _queryStr.clear();
  _queryStr.precision(0);
  _queryStr<<"SELECT * FROM runs ";
  _queryStr<<"WHERE station_name='"<<stationID<<"' ";
  _queryStr<<"AND run_start>="<<fixed<<_tWinLowSec<<" ";
  _queryStr<<"AND run_stop<"<<fixed<<_tWinUpSec<<" ";
  for (vector<string>::iterator it = _whereClauses.begin(); it!=_whereClauses.end(); ++it){
    _queryStr<<"AND "<<*it<<" ";
  }

  if(_vLevel>1) cout<<"[e3RunDbConn::GetRunList - INFO] Performing query: "<<_queryStr.str()<<endl;

  //***************************************
  //Perform query and store results
  //***************************************
  if (mysql_query(_mysqlCon,_queryStr.str().c_str()))
    {
      finish_with_error();
    }

  _queryRes = mysql_store_result(_mysqlCon);
  if (_queryRes == NULL) 
    {
      finish_with_error();
    }

  //*****************************************************
  //QUERY res Dump & Fill output string vector
  //*****************************************************
  if(_vLevel>5){

    _dbFields = mysql_fetch_fields(_queryRes);
    for(unsigned int ifield = 0; ifield <mysql_num_fields(_queryRes)-10; ifield++)
      cout<<"\t\t"<<_dbFields[ifield].name;
    cout<<endl;

  }

  unsigned int irow=0;
  while ((_dbRow = mysql_fetch_row(_queryRes))) 
    { 
  
      if(_vLevel>5){
	for(unsigned int ifield = 0; ifield <mysql_num_fields(_queryRes)-10; ifield++)
	  {

	    cout<<"\t\t"<<(_dbRow[ifield]  ? _dbRow[ifield]:"NULL");

	  }
	cout<<endl;
	irow++;
      }

      stringstream _fileName;
      _fileName.clear();

      if(_fullPath)
	{
	  _fileName<<"/recon/";
	  _fileName<<_dbRow[0]<<"/";
	  _fileName<<_dbRow[1]<<"/";
	}

      _fileName<<_dbRow[0]<<"-"<<_dbRow[1]<<"-";
      _fileName.fill('0'); _fileName.width(5); _fileName<<_dbRow[2];
      _fileName<<"_dst.root";  
      fileNameList.push_back(_fileName.str());

    }

  mysql_free_result(_queryRes);

  return 0;
}

// e3RunDbConn GetDaqConf member function
//------------------------------------------------------------
int e3RunDbConn::GetDaqConf(vector<string>& parValueList, const string stationID, const string dateTimeStr, const string colNameList){

  //***************************
  //Date Time in sec
  //***************************
  char dummy;
  stringstream lineStream;

  struct tm _dateTime;
  memset(&_dateTime, 0, sizeof _dateTime);  // set all fields to 0
  lineStream.str(dateTimeStr);

  // Parse the input and store each value into the correct variables.
  lineStream >> _dateTime.tm_year >> dummy >> _dateTime.tm_mon >> dummy >> _dateTime.tm_mday >> dummy >> _dateTime.tm_hour >> dummy >> _dateTime.tm_min >> dummy >> _dateTime.tm_sec;
  if(!lineStream && (_dateTime.tm_year==0 || _dateTime.tm_mon==0 || _dateTime.tm_mday==0)) {
    cerr << "[e3RunDbConn::GetDaqConf - ERROR] You entered an invalid date/time value." << endl;
    return 1;
  }

  _dateTime.tm_year -= 1900;
  _dateTime.tm_mon --;
  time_t _dateTimeTime = mktime(&_dateTime) - timezone -_overtime; 
  double _dateTimeSec = difftime(_dateTimeTime,_e3StartTime);

  //***************************
  //Query string
  //***************************
  _queryStr.str("");
  _queryStr.clear();
  _queryStr.precision(0);
  _queryStr<<"SELECT "<<colNameList<<" FROM daq_configurations ";
  _queryStr<<"JOIN telescopes ";
  _queryStr<<"ON daq_configurations.telescope_id = telescopes.id  ";
  _queryStr<<"WHERE telescopes.name='"<<stationID<<"' ";
  _queryStr<<"AND '"<<dateTimeStr<<"' between valid_from and valid_until ";

  if(_vLevel>1) cout<<"[e3RunDbConn::GetDaqConf - INFO] Performing query: "<<_queryStr.str()<<endl;

  //***************************************
  //Perform query and store results
  //***************************************
  if (mysql_query(_mysqlCon,_queryStr.str().c_str()))
    {
      finish_with_error();
    }

  _queryRes = mysql_store_result(_mysqlCon);
  if (_queryRes == NULL) 
    {
      finish_with_error();
    }

  //*****************************************************
  //QUERY res Dump & Fill output string vector
  //*****************************************************
  if(_vLevel>5){

    _dbFields = mysql_fetch_fields(_queryRes);
    for(unsigned int ifield = 0; ifield <mysql_num_fields(_queryRes); ifield++)
      cout<<"\t\t"<<_dbFields[ifield].name;
    cout<<endl;
  }

  unsigned int irow=0;
  while ((_dbRow = mysql_fetch_row(_queryRes))) 
    { 
      
      if(_vLevel>5){
	for(unsigned int ifield = 0; ifield <mysql_num_fields(_queryRes); ifield++)
	  {
	    
	    cout<<"\t\t"<<(_dbRow[ifield]  ? _dbRow[ifield]:"NULL");
	    
	  }
	cout<<endl;
	irow++;
      }

      stringstream _parValue;
      _parValue.clear();

      for(unsigned int ifield = 0; ifield <mysql_num_fields(_queryRes); ifield++)
	{
	  _parValue<<(_dbRow[ifield]  ? _dbRow[ifield]:"NULL");
	  if(ifield <mysql_num_fields(_queryRes)-1) _parValue<<",";
	}
      parValueList.push_back(_parValue.str());

      
    }
  
  mysql_free_result(_queryRes);

  return 0;

}
