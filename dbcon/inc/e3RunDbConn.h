#ifndef E3RUNDBCONN_H
#define E3RUNDBCONN_H

#include <string>
#include <cmath>
#include <iostream>
#include <fstream> 
#include <sstream> 
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <climits> //for INT_MAX
#include <map>
#include <vector>

#include <my_global.h>
#include <mysql.h>

using namespace std;

//! e3RunDbConn class.
/*!

  EEEE rundb connector, a C++ wrapper for the MySQL C API library.

*/
class e3RunDbConn{

 private:

  int  _vLevel;                       //!< Verbosity Level

  string _hostName;            //!< Database hostname (or IP address)
  string _dbUser;                 //!< Database user
  string _dbPwd;                  //!< Database user password
  string _dbName;               //!< Database name
  MYSQL *_mysqlCon;

  stringstream _queryStr;
  MYSQL_RES *_queryRes;
  MYSQL_FIELD *_dbFields;
  MYSQL_ROW _dbRow;

  struct tm _e3Start;
  time_t _e3StartTime;
  int _overtime;

  void finish_with_error();

 protected:

 public:

  //! Default constructor.
  /*!
    Description.
  */
  e3RunDbConn(){ _vLevel=0; _mysqlCon = NULL; };

  //! Constructor.
  /*!
    \param[in] hostName
    \param[in] dbUser
    \param[in] dbPwd
    \param[in] dbName
  */
  e3RunDbConn(const string, const string, const string, const string);

  //! Default destructor.
  /*!
    A more elaborate description of the destructor.
  */
  ~e3RunDbConn(){};

  //! Init mysql structure and open a db connection according to the db parameters
  /*!

    Correctly initialize the mysql connection.

   */
  void Init();

  //! Close db connection
  /*!

    Correctly close the mysql connection  and free MYSQL structure.

   */
  void CloseConn();

  //! Get run list
  /*!

    Get run list in a spiecified time window.

   */
  int GetRunList(vector<string>&, const string, const string, const string, const string, unsigned int);

  //! Get daq configuration
  /*!

    Get the daq parameter of a specific telescope for a specific date-time.

   */
  int GetDaqConf(vector<string>&, const string, const string, const string);

  inline void SetOvertime(unsigned int overtime){ _overtime = overtime; _overtime*=(60*60); };      //!< Set overtime for timewindow search (in hours)
  inline void SetVerbosity(unsigned int vLevel){ _vLevel = vLevel; };                //!< Set verbosity level

};

#endif
