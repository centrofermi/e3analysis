
DATA_FOLDER = '/data/work/EEE/data/upgoingmu'
STATIONS = ['FRAS-02', 'SAVO-01', 'SAVO-02', 'SAVO-03']
BETA_EXPR = 'TrackLength/TimeOfFlight/29979245800.e-9'
BASE_CUT_EXPR = 'ChiSquare <= 10 && StatusCode == 0'
CUT_EXPR = '%s && TimeOfFlight > 0' % BASE_CUT_EXPR 
