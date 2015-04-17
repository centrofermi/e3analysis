#ifndef E3RUNDBCONN_H
#define E3RUNDBCONN_H

#include <string>
#include <cmath>
#include <iostream>
#include <fstream> 
#include <cstdlib>
#include <climits> //for INT_MAX
#include <map>

#include <my_global.h>
#include <mysql.h>

using namespace std;


//! e3RunDbConn class.
/*!

  EEEE rundb connector, a C++ wrapper for the MySQL C API library.

*/
class e3RunDbConn{

 private:

  MYSQL *_mysqlCon;

  void finish_with_error();

 protected:

 public:

  //! Default constructor.
  /*!
    Description.
  */
  e3RunDbConn();

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

  //! Close db connection and free MYSQL structure
  /*!

    Correctly close the mysql connection.

   */
  void CloseConn();

/*   //! CheckNewRun member function */
/*   /\*! */
/*     \return true if a new "simulated" run has been started. */
/*    *\/ */
/*   bool CheckNewRun(); */

/*   //! GetRunInfo member function */
/*   /\*! */
/*     \brief Update Run Info (DAQ running/stopped etc.) */
/*     When DAQ is OFF, RUNSummary files are not filled in.  */
/*     If there are no RUNSummary data or data are newer than requested time, DAQ is assumed to be stopped. */

/*     \retvel 0 if DAQ is running */
/*     \retval 1 if a new run has been started (new RUNID) since last check */
/*     \retval 2 when the specified EventTime is outside any LEventTime-FEventTime intervals (RUNID set to 0) */
/*     \retval 3 if GetRunSum returned =2,3 (DAQ data missing or newer) */
 
/*   *\/ */
/*   int GetRunInfo(); */

/*   //! GetRunID member function */
/*   /\*! */
/*     \return current run ID */
/*    *\/ */
/*   inline unsigned int GetRunID(){ return _HeaderR->Run; } */

/*   //! UpdISSPos member function */
/*   /\*! */

/*     Update the ISS position contained in the AMSEventR, AMSSetupR header according to the 2LE simulation. */

/*     \retval -2 no 2LE data */

/*     \note All angles in rads. Distance in km. Velocity in km/s. */
/*           The longitude is returned in the 0,360 angular interval. */

/*   *\/ */
/*   int UpdISSPos(); */

/*   //! GetISSTRSPos member function */
/*   /\*! */

/*     Get the ISS position in a specific Terrestrial Reference Frame (TRS) in spherical coordinates (altitude, latitude, longitude). */

/*     \param datasetID data set ID: 0 - 2LE GTOD, 1 - NORAD CTRS, 2 - ALTEC GTOD, 3 - AMS GPS */

/*     \retval -1  if wrong datasetID was specified */
/*     \retval 0   ok (interpolation) */
/*     \retval 1   ok  (extrapolation) */
/*     \retval 2   no data */
/*     \retval 3   bad extrapolation ( gap > 60 sec) */

/*     \note All angles in rads. Distance in km. Velocity in km/s. */
/*           The longitude is returned in the -180,180 angular interval. */
 
/*   *\/ */
/*   int GetISSTRSPos(unsigned int datasetID, float &alt, float &lat, float &lon); */

/*   //! GetISSTRSVel member function */
/*   /\*! */

/*     Get the ISS velocity in a specific Terrestrial Reference Frame (TRS) in spherical coordinates (angvel, veltheta, velphi). */

/*     \param datasetID data set ID: 0 - 2LE GTOD, 1 - NORAD CTRS, 2 - ALTEC GTOD, 3 - AMS GPS */

/*     \retval -1  if wrong datasetID was specified */
/*     \retval 0   ok (interpolation) */
/*     \retval 1   ok  (extrapolation) */
/*     \retval 2   no data */
/*     \retval 3   bad extrapolation ( gap > 60 sec) */

/*     \note All angles in rads. Distance in km. Velocity in km/s. */
/*           The longitude is returned in the -180,180 angular interval. */
 
/*   *\/ */
/*   int GetISSTRSVel(unsigned int datasetID, float &angvel, float &veltheta, float &lvelphi); */

/*   //! UpdISSAtt member function */
/*   /\*! */

/*     Update the ISS attitude contained in the AMSEventR, AMSSetupR header according to the 2LE simulation. */

/*     \retval 2   no attitude data */

/*     \note All angles in rads. */

/*   *\/ */
/*   int UpdISSAtt(); */

/*   //! GetISSAtt member function */
/*   /\*! */

/*     Get the ISS Attitude */

/*     \note All angles in rads. */
/*    *\/ */
/*   void GetISSAtt(float &roll, float &pitch, float &yaw); */


/*   //! SAMSEventR GetAMSJ2000Point member function */
/*   /\*! */

/*     \brief Get the AMS Pointing direction (declination, right ascension) in J2000 celestial reference system. */

/*     \param[out] ras       J2000 Right Ascension (degree) */
/*     \param[out] dec        J2000 Declination  (degree) */

/*     \param[in]  use_att     1:Use LVLH, 2:Use INTL, 3: Use STK */
/*     \param[in]  use_coo     1:Use TLE,  2:Use CTRS, 3: Use GTOD, 4: Use AMS-GPS */
/*     \param[in]  use_time    1:UTCTime(), 2:AMS GPS time, 3: AMS GPS Time corrected */

/*     \retval     0 success */
/*     \retval    -1 failure */
/*     \retval     1 specified use_att  data not available; instead used TLE+LVLH */
/*     \retval     2 specified use_coo  data not available; instead used TLE */
/*     \retval     3 specified use_coo  data not reliable;  instead used TLE */
/*     \retval     4 specified use_time data not available; instead used UTCTime() */

/*     \note The right ascension is returned in the -180,180 angular interval. */
 
/*   *\/ */
/*   int GetAMSJ2000Point(double &ras, double &dec, unsigned int use_att, unsigned int use_coo,  */
/* 		       unsigned int use_time); */

/*   //! SAMSEventR GetAMSGalPoint member function */
/*   /\*! */

/*     \brief Get the AMS Pointing direction (longitude, latidude) in galactic reference system. */

/*     \param[out] lon         Galactic longitude (degree) */
/*     \param[out] lat         Galactic latitude (degree) */

/*     \param[in]  use_att     1:Use LVLH, 2:Use INTL, 3: Use STK */
/*     \param[in]  use_coo     1:Use TLE,  2:Use CTRS, 3: Use GTOD, 4: Use AMS-GPS */
/*     \param[in]  use_time    1:UTCTime(), 2:AMS GPS time, 3: AMS GPS Time corrected */

/*     \retval     0 success */
/*     \retval    -1 failure */
/*     \retval     1 specified use_att  data not available; instead used TLE+LVLH */
/*     \retval     2 specified use_coo  data not available; instead used TLE */
/*     \retval     3 specified use_coo  data not reliable;  instead used TLE */
/*     \retval     4 specified use_time data not available; instead used UTCTime() */

/*     \note The right ascension is returned in the -180,180 angular interval. */
 
/*   *\/ */
/*   int GetAMSGalPoint(double &lon, double &lat, unsigned int use_att, unsigned int use_coo,  */
/* 		     unsigned int use_time); */
  
/*   //! SAMSEventR GetAMSCoo member function */
/*   /\*! */
/*   \brief Convert direction in Galactic coordinates into AMS coordinates */
/* RIGUARDARE TUTTO!!!! */
/*   \param[out] result bits (1<<0 =  1): STK used, (1<<1 = 2):INTL used, */
/* 			  (1<<2 =  4): TLE used, (1<<3 = 8):GTOD used, */
/* 			  (1<<4 = 16):CTRS used, (1<<5 =32):AMS-GPS coo.used, */
/* 			  (1<<6 = 64):GPS time used, */
/* 			  (1<<7 =128):GPS time corrected used */
/*   \param[out] glong       Galactic longitude (degree) */
/*   \param[out] glat        Galactic latitude  (degree) */

/*   \param[in]  theta (rad) in ams coo system (pi: down-going 0: up-going) */
/*   \param[in]  phi   (rad) in ams coo system */
/*   \param[in]  use_att     1:Use LVLH,  2:Use INTL, 3: Use STK */
/*   \param[in]  use_coo     1:Use TLE,   2:Use CTRS, 3: Use GTOD, 4: Use AMS-GPS */
/*   \param[in]  use_time    1:UTCTime(), 2:AMSGPS time, 3:AMSGPS Time corrected */
/*   \param[in]  dt          time jitter (sec) for coordinates input */
/*   \param[in]  out_type    1:Galactic coord. 2:Equatorial coord.(R.A. and Dec.) */
/*                           3:GTOD coord.(use_att forced to 1) */

/*   \retval     0 success */
/*   \retval    -1 failure */
/*   \retval     1 specified use_att  data not available; instead used TLE+LVLH */
/*   \retval     2 specified use_coo  data not available; instead used TLE */
/*   \retval     3 specified use_coo  data not reliable;  instead used TLE */
/*   \retval     4 specified use_time data not available; instead used UTCTime() */
/*   *\/ */
/*   int GetAMSCoo(int &result, double &phi, double &theta,  */
/* 		double glon, double glat,  */
/* 		int use_att, int use_coo, int use_time,  */
/* 		double dt, int in_type); */

/*   //! SAMSEventR InSAA member function */
/*   /\*! */
/*     Difference strategy can be selected using gPar->GetSAAStrategy() */
/*       0 - Do not skip periods inside SAA */
/*       1 - Skip periods inside SAA and SSA contour defined  */
/*           in terms of geomagnetic field */
/*       2 - Skip periods inside SAA and SSA contour defined  */
/*           in terms of live-time of the AMS DAQ (<=0.65) */
    
/*     \param[in] theta ISS latitude in GTOD reference frame */
/*     \param[in] phi   ISS longitude in GTOD reference frame */

/*     \return true if the ISS is inside the SAA */

/*    *\/ */
/*   bool InsideSAA(float &theta, float &phi); */

/*   //! SAMSEventR GetLiveTime member function */
/*   /\*! */
/*     \brief sysb0 = Total Live-Time, sysb1 = FEE Live-Time */

/*     \retval 0 LiveTime interpolated from data */
/*     \retval 1 LiveTime corresponds to the first or the last entry in the database */
/*     \retval 2 No entries in the data map loaded from csv files */
/*     \retval 3 The first entry in the data map is newer than xtime */
/*     \retval 4 The entries loaded in the data map don't cover all the requested time interval */
/*     \retval 5 THe interpolation cannot be performed because there is a gap (>300 sec) in the data */

/*    *\/ */
/*   int GetLiveTime(float &lt_sysb0, float &lt_sysb1, double xtime); */

/*   //! SAMSEventR GetRunSum member function */
/*   /\*! */
/*     \retval 0 RunSum found, xtime is inside a run interval */
/*     \retval 1 RunSum found, xtime is outside run interval */
/*     \retval 2 No entries in the data map loaded from csv files */
/*     \retval 3 The first entry in the data map is newer than xtime */
/*     \retval 4 The entries loaded in the data map don't cover all the requested time interval */
/*    *\/ */
/*   int GetRunSum(unsigned int &RunID, */
/* 		  unsigned int &FEvent, unsigned int &FEventTime,  */
/* 		  unsigned int &LEvent, unsigned int &LEventTime,  */
/* 		  double xtime); */

/*   //! SAMSEventR GetZenAngDist member function */
/*   /\*! */
/*     Different computation method can be selected using gPar->GetZenithCompMethod() */
/*       0 - Zenith from RTI */
/*       1 - Zenith from HeaderR */
    
/*     \return Angular distance between AMS Zenith and the geocentric radius vector to the ISS center of mass */

/*    *\/ */
/*   double GetZenAngDist(); */

/*   //! SAMSEventR GetRTIInfo member function */
/*   /\*! */
/*     .... */
/*    *\/ */
/*   void GetRTIInfo(); */

/*   inline void SetVerbosity(unsigned int vlevel){ _vlevel = vlevel; }; */

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
