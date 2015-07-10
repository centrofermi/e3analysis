import os
import ROOT

from __common__ import *

OUTPUT_FILE_PATH = 'dir_error_mc.root'

print 'Opening output file %s...' % OUTPUT_FILE_PATH
outputFile = ROOT.TFile(OUTPUT_FILE_PATH, 'RECREATE')
for station in STATIONS:
    for fitTool in ['ana', '2dw', '2dnw']:
        filePath = os.path.join(DATA_FOLDER, '%s_1_%s_mc.root' % \
                                (station, fitTool))
        f = ROOT.TFile(filePath)
        t = f.Get('Events')
        outputFile.cd()
        hname = 'hdirerr_%s_%s' % (station, fitTool)
        hdirerr = ROOT.TH1F(hname, station, 2000, 0, 10)
        #hdirerr.SetXTitle('#chi^{2}')
        #hdirerr.SetYTitle('Entries/bin')
        t.Project(hname, 'McDirErr')
        print 'Writing %s...' % hname
        hdirerr.Write()
        print 'Done.'
        f.Close()
