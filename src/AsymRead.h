#ifndef ASYM_READ_H
#define ASYM_READ_H

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
 * Efficiently reads the raw data file and processes data structures.
 */
class RawDataProcessor
{
public:

   std::string    fFileName;
   FILE          *fFile;
   char          *fMem;
   UInt_t         fMemSize;
   std::ifstream  fFileStream;

   RawDataProcessor();
   RawDataProcessor(std::string fname);
   ~RawDataProcessor();

   void ReadRecBegin(MseMeasInfoX &mseMeasInfo);
   void ReadMeasInfo(MseMeasInfoX &mseMeasInfo);
   void ReadDataPassOne(MseMeasInfoX &mseMeasInfo);
   void ReadDataPassTwo(MseMeasInfoX &mseMeasInfo);

private:

   std::set<Int_t> fSeenRecords;
};

void UpdateRunConst(TRecordConfigRhicStruct *ci);
void DecodeTargetID(const polDataStruct &poldat, MseMeasInfoX &run);
void ProcessRecordPCTarget(const pCTargetStruct &rec, MseMeasInfoX &run);
void PrepareCollidingBunchPattern();

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
