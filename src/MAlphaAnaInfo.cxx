#include <cstdlib>

#include <getopt.h>

#include "MAlphaAnaInfo.h"

#include "TString.h"
#include "TSystem.h"

ClassImp(MAlphaAnaInfo)

using namespace std;


/** */
MAlphaAnaInfo::MAlphaAnaInfo() : AnaInfo(),
   fMListFileName(""),
   fUseSsh(false),
   fOutputFileName("")
{
   Init();
}


void MAlphaAnaInfo::SetMListFileName(std::string listName)
{
   fMListFileName = listName;
}

string MAlphaAnaInfo::GetMListFileName()
{
   return fMListFileName;
}

string MAlphaAnaInfo::GetMListFullPath()
{
   return GetResultsDir() + "/runXX/lists/" + fMListFileName;
}

string MAlphaAnaInfo::GetExternInfoPath()
{
   return GetResultsDir() + "/runXX/";
}


/** */
void MAlphaAnaInfo::ProcessOptions(int argc, char **argv)
{
   int option_index = 0;

   static struct option long_options[] = {
      {"log",                 optional_argument,   0,   'l'},
      {"graph",               no_argument,         0,   AnaInfo::MODE_GRAPH},

      {"meas-list",           required_argument,   0,   MAlphaAnaInfo::OPTION_MLIST},
      {"output-file",         required_argument,   0,   MAlphaAnaInfo::OPTION_OFILE},
      {"use-ssh",             optional_argument,   0,   MAlphaAnaInfo::OPTION_USE_SSH},
      {0, 0, 0, 0}
   };

   int c;

   while ((c = getopt_long(argc, argv, "?hlg::m:o:", long_options, &option_index)) != -1) {
      switch (c) {

         case '?':
         case 'h':
            PrintUsage();
            exit(EXIT_SUCCESS);

         case 'l':
            fFileStdLogName = (optarg != 0 ? optarg : "");
            break;

         case 'g':
         case AnaInfo::MODE_GRAPH:
            fModes |= AnaInfo::MODE_GRAPH;
            break;

         case 'm':
         case MAlphaAnaInfo::OPTION_MLIST:
            SetMListFileName(optarg);
            break;

         case 'o':
         case MAlphaAnaInfo::OPTION_OFILE:
            fOutputFileName = optarg;
            break;

         case MAlphaAnaInfo::OPTION_USE_SSH:
            fUseSsh = true;
            break;

         default:
            Error("ProcessOptions", "Invalid option provided");
            PrintUsage();
            exit(EXIT_FAILURE);
      }
   }
}


/** */
void MAlphaAnaInfo::VerifyOptions()
{
   // The run name must be specified
   if (fMListFileName.empty()) {
      Error("VerifyOptions", "List file name has to be specified");
      PrintUsage();
      exit(EXIT_FAILURE);
   }

   if (fOutputFileName.empty()) {
      Error("VerifyOptions", "Output file name has to be specified");
      PrintUsage();
      exit(EXIT_FAILURE);
   }

   TString tmpFileName(fMListFileName.c_str());

   // file exists?
   if ( !gSystem->FindFile( (GetResultsDir() + "/runXX/lists/").c_str(), tmpFileName ) ) {
      Error("VerifyOptions",
            "File list \"%s\" not found\n", GetMListFullPath().c_str());
      exit(EXIT_FAILURE);
   }

   fOutputName = fMListFileName;
}


/** */
void MAlphaAnaInfo::Print(const Option_t* opt) const
{
   Info("Print", "Print members:");
   PrintAsPhp();
}


/** */
void MAlphaAnaInfo::PrintAsPhp(FILE *f) const
{
   AnaInfo::PrintAsPhp(f);
   fprintf(f, "$rc['fMListFileName']               = \"%s\";\n", fMListFileName.c_str());
   fprintf(f, "\n");
}


/** */
void MAlphaAnaInfo::PrintUsage()
{
   cout << endl;
   cout << "Options:" << endl;
   cout << " -h, -?                               : Print this help" << endl;
   cout << " -g, --graph                          : Plot graphs" << endl;
   cout << " -m, --meas-list <file_name>          : Name of run with raw data in $CNIPOL_RESULTS_DIR directory" << endl;
   cout << " -o, --output-file <file_name>        : Output file name" << endl;
   cout << " --use-ssh                            : Use ssh method to access CDEV log data" << endl;
   cout << endl;
}
