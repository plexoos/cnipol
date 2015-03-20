//  RunDB Reader
//  file name :   RunDBReader.cc
//
//
//  Author    :   Itaru Nakagawa
//                Dmitri Smirnov
//  Creation  :   02/06/2006
//

/**
 *
 * 1 Nov, 2010 - Dmitri Smirnov
 *    - Cleaned up the code
 *
 */

#include "RunDBReader.h"

#include <iostream>

//#include "AsymDb.h"
#include "AsymDbSql.h"
#include "MeasInfo.h"

using namespace std;

int Usage(char *argv[])
{
  cout << "\n Usage:" << argv[0] << "[-h] [-x][-f <runID>]" << endl;
  cout << "\n Description: " << endl;
  cout << "\t Read configulations from run.db and print" << endl;
  cout << "\n Options:" << endl;
  cout << "\t -f <runID> " << endl;
  cout << "\t -h \t show this help    " << endl;
  cout << "\t -x \t show example    " << endl;
  cout << endl;
  exit(0);
}

int Example(char *argv[])
{
  cout << "\n Exapmle: " << endl;
  cout << "\t" << argv[0] << " -f 7279.005" << endl;
  cout << 
  cout << endl;
  exit(0);
}


int main(int argc, char *argv[])
{
   AsymDb *asymDbFile = new AsymDbFile();

   DbEntry *dbEntry = asymDbFile->Select("15335.209");

   dbEntry->Print();

   //AsymDb *asymDb = new AsymDbSql();

   //asymDb->Select();
   //asymDb->Insert(dbEntry);

   return 0;

   char * RunID;
   int opt;

   while (EOF != (opt = getopt(argc, argv, "f:hx?"))) {
      switch (opt) {
      case 'x':
         Example(argv);
         break;
      case 'f':
         RunID = optarg;
         break;
      case 'h':
      case '?':
      case '*':
         Usage(argv);
      }
   }

   // RunID 
   double RUNID = strtod(RunID, NULL);

   // Read Conditions from run.db
   readdb(RUNID);

   gMeasInfo->PrintConfig();

   return 0;
}
