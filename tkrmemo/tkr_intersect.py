import os
import ROOT

from __common__ import *

OUTPUT_FILE_PATH = 'tkr_intersect_mc.root'

print 'Opening output file %s...' % OUTPUT_FILE_PATH
outputFile = ROOT.TFile(OUTPUT_FILE_PATH, 'RECREATE')
for station in STATIONS:
    for fitTool in ['ana', '2dw', '2dnw']:
        filePath = os.path.join(DATA_FOLDER, '%s_1_%s_mc.root' % \
                                (station, fitTool))
        f = ROOT.TFile(filePath)
        t = f.Get('Events')
        outputFile.cd()
        hname = 'hposerr_%s_%s' % (station, fitTool)
        hposerr = ROOT.TH2F(hname, station, 200, -5., 5., 200, -5., 5.)
        hposerr.SetXTitle('Track intersect error x [cm]')
        hposerr.SetYTitle('Track intersect error y [cm]')
        t.Project(hname, 'IntersectYMid-McPosYMid+1.6:IntersectXMid-McPosXMid')
        print 'Writing %s...' % hname
        hposerr.Write()
        print 'Done.'
        f.Close()
