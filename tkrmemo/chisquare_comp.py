import os
import ROOT

from __common__ import *

OUTPUT_FILE_PATH = 'chisquare_mc.root'

print 'Opening output file %s...' % OUTPUT_FILE_PATH
outputFile = ROOT.TFile(OUTPUT_FILE_PATH, 'RECREATE')
for station in STATIONS:
    for fitTool in ['ana', '2dw', '2dnw']:
        filePath = os.path.join(DATA_FOLDER, '%s_1_%s_mc.root' % \
                                (station, fitTool))
        f = ROOT.TFile(filePath)
        t = f.Get('Events')
        outputFile.cd()
        hname = 'hchisquare_%s_%s' % (station, fitTool)
        hchisquare = ROOT.TH1F(hname, station, 2000, 0, 10)
        hchisquare.SetXTitle('#chi^{2}')
        hchisquare.SetYTitle('Entries/bin')
        if fitTool == 'ana':
            t.Project(hname, 'ChiSquare**2')
        else:
            t.Project(hname, 'ChiSquare')
        print 'Writing %s...' % hname
        hchisquare.Write()
        print 'Done.'
        f.Close()
