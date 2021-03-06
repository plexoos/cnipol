#ifndef RawDataReader_h
#define RawDataReader_h

#include <errno.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymGlobals.h"
#include "MeasInfo.h"
#include "MseMeasInfo.h"


/**
 * This class loads a raw data file into memory, processes data structures with
 * information about the measurement, and loops over the events.
 */
class RawDataReader
{
public:

   RawDataReader(std::string fname);
   ~RawDataReader();

   void ReadRecBegin(MseMeasInfoX &mseMeasInfo);
   void ReadMeasInfo(MseMeasInfoX &mseMeasInfo);
   void ReadDataPassOne(MseMeasInfoX &mseMeasInfo);
   void ReadDataPassTwo(MseMeasInfoX &mseMeasInfo);

private:

   std::string      fFileName;
   FILE            *fFile;
   char            *fMem;
   UInt_t           fMemSize;
   std::ifstream    fFileStream;
   std::set<Int_t>  fSeenRecords;
};

void UpdateRunConst(TRecordConfigRhicStruct *ci);
void DecodeTargetID(const polDataStruct &poldat, MseMeasInfoX &run);
void ProcessRecordPCTarget(const pCTargetStruct &rec, MseMeasInfoX &run);

void ProcessRecord(const recordConfigRhicStruct &rec);
void ProcessRecord(const recordMeasTypeStruct &rec);
void ProcessRecord(const recordPolAdoStruct &rec, MseMeasInfoX &MeasInfo);
void ProcessRecord(const recordpCTagAdoStruct &rec, MseMeasInfoX &run);
void ProcessRecord(const recordWFDV8ArrayStruct &rec);
void ProcessRecord(const recordCountRate &rec);
void ProcessRecord(const recordWcmAdoStruct &rec);
void ProcessRecord(const RecordWcm &rec);
void ProcessRecord(const recordBeamAdoStruct &rec);
void ProcessRecord(const RecordMachineParams &rec);
void ProcessRecord(const RecordTargetParams &rec);

#endif
