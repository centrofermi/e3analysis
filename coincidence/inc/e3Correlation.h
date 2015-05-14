//****************************************************************************************************
//
// e3COrrelation.h
//
// Created by F.Pilo on May 2015
//
//****************************************************************************************************

#ifndef _E3CORRELATION_H
#define _E3CORRELATION_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>

//!  calculateThetaPhi function.
/*!
  Calculate Theta and Phi
 */
void calculateThetaPhi(Float_t &cx, Float_t &cy, Float_t &cz, Float_t &teta, Float_t &phi);

//! correlation_EEE function.
/*!

  This macro correlates the events measured by two EEE telescopes according to their GPS time
  within a time window = DiffCut

  Data are read from the two ROOT trees created for each telescope
*/
void correlation_EEE(const char *mydata=NULL,const char *mysc1=NULL,const char *mysc2=NULL,const char *mypath=NULL,bool kNoConfigFile=kFALSE,Double_t DiffCut = 0.1);

//! correlation_EEE3 function.
/*!

  This macro correlates the events measured by three EEE telescopes according to their GPS time
  within a time window = DiffCut

  Data are read from the two ROOT trees created for each telescope
*/
void correlation_EEE3(const char *mydata=NULL, Double_t DiffCut = 0.1);

//! correlation_EEEAC function.
/*!

  This macro AUTO-correlates the events measured by one EEE telescope according to their GPS time
  within a time window = DiffCut

  Data are read from the two ROOT trees created for each telescope
*/
void correlation_EEEAC(const char *mydata=NULL, Double_t DiffCut = 0.1);

#endif // _E3_CORRELATION_H
