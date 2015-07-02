import os
import ROOT

from __common__ import *

OUTPUT_FILE_PATH = 'chisquare.root'

print 'Opening output file %s...' % OUTPUT_FILE_PATH
outputFile = ROOT.TFile(OUTPUT_FILE_PATH, 'RECREATE')
for station in STATIONS:
    filePath = os.path.join(DATA_FOLDER, '%s_full_upgoingmu.root' % station)
    f = ROOT.TFile(filePath)
    t = f.Get('Events')
    outputFile.cd()
    hname = 'hchisquare_%s' % station
    hchisquare = ROOT.TH1F(hname, station, 2000, 0, 10)
    hchisquare.SetXTitle('#chi^{2}')
    hchisquare.SetYTitle('Entries/bin')
    t.Project(hname, 'ChiSquare', CUT_EXPR)
    print 'Writing %s...' % hname
    hchisquare.Write()
    print 'Done.'
    f.Close()
