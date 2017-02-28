#include <string>
#include <climits>

#include "AnaFillExternResult.h"
#include "AnaGlobResult.h"
#include "BiasCurrentUtil.h"
#include "CachingLogReader.h"
#include "SshLogReader.h"
#include "Target.h"

ClassImp(AnaFillExternResult)

using namespace std;


/** */
AnaFillExternResult::AnaFillExternResult(UInt_t fillId) : TObject(),
   fBluIntensGraph(0), fYelIntensGraph(0),
   fBluRotCurStarGraph(0), fYelRotCurStarGraph(0), fBluRotCurPhenixGraph(0),
   fYelRotCurPhenixGraph(0), fBluSnakeCurGraph(0), fYelSnakeCurGraph(0),
   fTimeEventLumiOn(LONG_MAX), fTimeEventLumiOff(LONG_MIN)
{
   LoadInfo(fillId);
}


/** */
AnaFillExternResult::~AnaFillExternResult() { }


TGraphErrors* AnaFillExternResult::GetGrBluIntens() const { return fBluIntensGraph; }
TGraphErrors* AnaFillExternResult::GetGrYelIntens() const { return fYelIntensGraph; }


/** */
TGraphErrors* AnaFillExternResult::MakeGraph(const std::map<opencdev::cdev_time_t, double> &values, int thin_out_factor)
{
   TGraphErrors *gr = new TGraphErrors(values.size() / thin_out_factor);
   int i = 0;

   for(std::map<opencdev::cdev_time_t, double>::const_iterator it = values.begin(); it != values.end(); it++)
   {
      opencdev::cdev_time_t time = it->first;
      double value = it->second;

      if ((i % thin_out_factor) == 0)
      {
         gr->SetPoint(i / thin_out_factor, time, value);
      }
      i++;
   }

   return gr;
}


/** */
void AnaFillExternResult::LoadInfo(UInt_t fillId)
{
   if (fillId == 0)
   {
      Error("LoadInfo", "fillId == 0");
      return;
   }

   CachingLogReader<SshLogReader> log_reader;
   log_reader.set_additional_args(" -start \"ev-lumi\" -stop \"softev-physics-off\"");
   opencdev::result_t result;
   log_reader.query_fill("RHIC/BeamIons", fillId, &result);
   log_reader.query_fill("RHIC/PowerSupplies/rot-ps", fillId, &result);
   log_reader.query_fill("RHIC/PowerSupplies/snake-ps", fillId, &result);

   fBluIntensGraph = MakeGraph(result["bluDCCTtotal"], 100);
   fYelIntensGraph = MakeGraph(result["yelDCCTtotal"], 100);
   fBluRotCurStarGraph = MakeGraph(result["bi5-rot3-outer"]);
   fYelRotCurStarGraph = MakeGraph(result["yo5-rot3-outer"]);
   fBluRotCurPhenixGraph = MakeGraph(result["yi7-rot3-outer"]);
   fYelRotCurPhenixGraph = MakeGraph(result["bo7-rot3-outer"]);
   fBluSnakeCurGraph = MakeGraph(result["bo3-snk7-outer"]);
   fYelSnakeCurGraph = MakeGraph(result["yi3-snk7-outer"]);

   {
      const std::map<opencdev::cdev_time_t, double> &values = result["bluDCCTtotal"];
      if(values.empty())
	{
	  fTimeEventLumiOn = -2211753600;
	  fTimeEventLumiOff = -2211753600;
	}
      for(std::map<opencdev::cdev_time_t, double>::const_iterator it = values.begin(); it != values.end(); it++)
      {
         opencdev::cdev_time_t time = it->first;
         // Determine the lumi on/off events

         if (time < fTimeEventLumiOn)  fTimeEventLumiOn  = (time_t) time;
         if (time > fTimeEventLumiOff) fTimeEventLumiOff = (time_t) time;
      }
      if(fillId == 17443){fTimeEventLumiOn = (time_t)1367362617;}
      if(fillId == 17699){fTimeEventLumiOn = (time_t)1370651445;}
   }

   opencdev::result_t bc_result;
   log_reader.set_additional_args("");
   log_reader.query_fill("RHIC/Polarimeter/Blue/biasReadbacks", fillId, &bc_result);
   log_reader.query_fill("RHIC/Polarimeter/Yellow/biasReadbacks", fillId, &bc_result);
   for(opencdev::result_t::const_iterator it = bc_result.begin(); it != bc_result.end(); it++)
   {
      const string &key = it->first;
      const map<opencdev::cdev_time_t, double> &values = it->second;

      EPolarimeterId polId = BiasCurrentUtil::ParseLoggerPolId(key);
      TGraphErrors *gr = MakeGraph(values);
      fBCCurGraph[polId].push_back(gr);
   }
}
