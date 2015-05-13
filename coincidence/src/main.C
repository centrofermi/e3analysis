//****************************************************************************************************
//
//							CoincAtCnaf.exe							   
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

bool CfrString(const char *str1,const char *str2);

int main(int argc,char *argv[]){

  char *date = NULL;
  char *sc1 = NULL;
  char *sc2 = NULL;
  char *path = NULL;

  printf("list of options to overwrite the config file infos:\n");
  printf("-d DATE = to pass the date from line command\n");
  printf("-s SCHOOL_1 SCHOOL_2 = to pass the schools from line command\n");
  printf("-p PATH = to pass the path of the reco dirs");

  int kNoConfigFile = 0;

  for(Int_t i=1;i < argc;i++){
    if(CfrString(argv[i],"-d")){
      if(i+1 > argc){
        printf("date is missing\n");
	return 1;
      }
       
      date = argv[i+1];
      i++;
      kNoConfigFile++;
    }
    if(CfrString(argv[i],"-s")){                       
      if(i+2 > argc){
        printf("Two schools name have to be provided\n");
        return 2;
      }
 
      sc1 = argv[i+1];
      sc2 = argv[i+2];

      i+=2;
      kNoConfigFile++;
    }
    if(CfrString(argv[i],"-p")){
      if(i+1 > argc){
        printf("path is missing\n");
        return 1;
      }

      path = argv[i+1];
      i++;  
      kNoConfigFile++;
    }

  }

  correlation_EEE(date,sc1,sc2,path,kNoConfigFile==3);

  return 0;
}

bool CfrString(const char *str1,const char *str2){
  int n=0;

  while(str1[n] == str2[n] && str1[n] != '\0'  && str2[n] != '\0' && n < 100){
    n++;
  }

  return (str1[n] == str2[n]);
}

