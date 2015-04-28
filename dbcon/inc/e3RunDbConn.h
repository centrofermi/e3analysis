#ifndef E3RUNDBCONN_H
#define E3RUNDBCONN_H

#include <string>
#include <cmath>
#include <iostream>
#include <fstream> 
#include <sstream> 
#include <cstdlib>
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
  void GetRunList(const string, const string, unsigned int);


  inline void SetVerbosity(unsigned int vLevel){ _vLevel = vLevel; };




/*  private: */
  
/*   unsigned int _vlevel;   //!< Verbosity Level */
/*   string _rootfile_name;  //!< default name of the AMS ROOT file needed by the Init member */
/*   unsigned int _runid; */

/*   float _grmedphi;   */
/*   TStopwatch timer; */

/*   int LoadISSAtt(unsigned int t1, unsigned int t2); */
/*   int getISSAtt(float &roll, float &pitch, float &yaw, double xtime); */

/*   //! SAMSEventR LoadLiveTime member function */
/*   /\*! */
/*     \brief Load data from Live-Time files created analysing HK data stream */

/*     \retval 0 if all entries in the specified time interval have been correctly loaded */
/*     \retval 1 if the end of the specified time interval has not been found */
/*     \retval 2 if no entries for the specified time interval has been loaded */
/*    *\/ */
/*   int LoadLiveTime(unsigned int t1, unsigned int t2); */
  
/*   //! SAMSEventR LoadRunSum member function */
/*   /\*! */
/*     \retval 0 if all entries in the specified time interval have been correctly loaded */
/*     \retval 1 if the end of the specified time interval has not been found */
/*     \retval 2 if no entries for the specified time interval has been loaded */
/*    *\/ */
/*   int LoadRunSum(unsigned int t1, unsigned int t2); */

};

#endif
