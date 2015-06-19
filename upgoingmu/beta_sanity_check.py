
import os
import ROOT

DATA_FOLDER = '/data/work/EEE/data/upgoingmu'
STATIONS = ['FRAS-02', 'SAVO-01', 'SAVO-02', 'SAVO-03']
BETA_EXPR = 'TrackLength/TimeOfFlight/29979245800.e-9'
CUT_EXPR = 'ChiSquare <= 10 && StatusCode == 0 && TimeOfFlight > 0'
OUTPUT_FILE_PATH = 'beta_sanity_check.root'

print 'Opening output file %s...' % OUTPUT_FILE_PATH
outputFile = ROOT.TFile(OUTPUT_FILE_PATH, 'RECREATE')
for station in STATIONS:
    filePath = os.path.join(DATA_FOLDER, '%s_full_upgoingmu.root' % station)
    f = ROOT.TFile(filePath)
    t = f.Get('Events')
    outputFile.cd()
    hname = 'hbeta_%s' % station
    hbeta = ROOT.TH1F(hname, station, 200, 0, 3)
    hbeta.SetXTitle('#beta')
    hbeta.SetYTitle('Entries/bin')
    t.Project(hname, BETA_EXPR, CUT_EXPR)
    print 'Writing %s...' % hname
    hbeta.Write()
    print 'Done.'
    hname = 'hbetainv_%s' % station
    hbetainv = ROOT.TH1F(hname, station, 200, 0, 3)
    hbetainv.SetXTitle('1/#beta')
    hbetainv.SetYTitle('Entries/bin')
    t.Project(hname, '1./(%s)' % BETA_EXPR, CUT_EXPR)
    print 'Writing %s...' % hname
    hbetainv.Write()
    print 'Done.'
    hname = 'hbetazdir_%s' % station
    hbetazdir = ROOT.TH2F(hname, station, 100, 0, 1, 100, 0, 3)
    hbetazdir.SetXTitle('cos(#theta)')
    hbetazdir.SetYTitle('#beta')
    t.Project(hname, '(%s):ZDir' % BETA_EXPR, CUT_EXPR)
    print 'Writing %s...' % hname
    hbetazdir.Write()
    print 'Done.'
    hname = 'hbetazdirinv_%s' % station
    hbetazdirinv = ROOT.TH2F(hname, station, 100, 0, 1, 100, 0, 3)
    hbetazdirinv.SetXTitle('cos(#theta)')
    hbetazdirinv.SetYTitle('1/#beta')
    t.Project(hname, '1./(%s):ZDir' % BETA_EXPR, CUT_EXPR)
    print 'Writing %s...' % hname
    hbetazdirinv.Write()
    print 'Done.'
    f.Close()

print 'Closing output file...'
outputFile.Close()
print 'Done, bye.'


