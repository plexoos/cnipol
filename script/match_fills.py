
import sys
import string

def main():

   fileAllMeas   = open("run13_phys")
   fileHJetFills = open("run13_phys_hjet")

   allMeass  = fileAllMeas.readlines()
   hJetFills = fileHJetFills.readlines()

   for meas in allMeass:
      meas = meas.strip()
      fillId, measId = meas.split('.')

      for hjFill in hJetFills:

         hjFill = hjFill.strip()

         if (fillId == hjFill):
            print meas


if __name__ == '__main__':
    main()
