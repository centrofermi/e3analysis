import os
import numpy
import ROOT

from __common__ import *

OUTPUT_FILE_PATH = 'mu_lifetime.root'
CUT_EXPR = '%s && TimeOfFlight < 0' % BASE_CUT_EXPR

print 'Opening output file %s...' % OUTPUT_FILE_PATH
outputFile = ROOT.TFile(OUTPUT_FILE_PATH, 'RECREATE')
for station in STATIONS:
    filePath = os.path.join(DATA_FOLDER, '%s_full_upgoingmu.root' % station)
    f = ROOT.TFile(filePath)
    t = f.Get('Events')
    outputFile.cd()
    hname = 'hmutau_%s' % station
    binning = numpy.logspace(-7, 0, 101)
    hmutau = ROOT.TH1F(hname, station, 100, binning)
    hmutau.SetXTitle('TDP [2]')
    hmutau.SetYTitle('Entries/bin')
    t.Project(hname, 'DeltaTime', CUT_EXPR)
    print 'Writing %s...' % hname
    hmutau.Write()
    print 'Done.'
    f.Close()
