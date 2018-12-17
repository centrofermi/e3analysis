from pylab import *

from datetime import datetime, timedelta
import datetime

import sys

import tables

from array import array

from ROOT import TFile, TTree

from sapphire import download_data, CoincidencesESD

daystart=int(sys.argv[3])
monthstart=int(sys.argv[2])
yearstart=int(sys.argv[1])

STATIONS = [501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512, 513, 514, 521, 522]
START = datetime.datetime(yearstart, monthstart, daystart)
END = START  + timedelta(hours=24)

if __name__ == '__main__':
    station_groups = ['/s%d' % u for u in STATIONS]
    data = tables.open_file('data.h5', 'w')
    for station, group in zip(STATIONS, station_groups):
        download_data(data, group, station, START, END)

print(data)
coincidences = CoincidencesESD(data, '/coincidences', station_groups)
coincidences.search_and_store_coincidences(station_numbers=STATIONS)
print(data)

events = data.root.coincidences.coincidences

label = '{:04d}'.format(yearstart) + '-' + '{:02d}'.format(monthstart) + '-' + '{:02d}'.format(daystart)

f = TFile( "hisparc500-"+label+".root", 'recreate' )
t = TTree( 't1', 'tree with histos' )
sec = array( 'i', [ 0 ] )
ns = array( 'i', [ 0 ] )
num = array( 'i', [ 0 ] )
indexst = array( 'i', 16*[ 0 ] )
timest = array( 'f', 16*[ 0. ] )

t.Branch( 'Seconds',sec, 'Seconds/I')
t.Branch( 'NanoSeconds',ns,'NanoSeconds/I')
t.Branch( 'NumStations',num,'NumStations/I')
t.Branch( 'IndexStation',indexst,'IndexStation[NumStations]/I')
t.Branch( 'TimeStation',timest,'TimeStation[NumStations]/F')

for i in range(size(events)):
   if events[i][4] > 2:
      sec[0] = events[i][1]
      ns[0] = events[i][2]
      num[0] = events[i][4]
      indexing = data.root.coincidences.c_index[i]
      for j in range(num[0]):
         indexst[j] = indexing[j][0]
         event = indexing[j][1]
         group = data.root.coincidences.s_index[indexst[j]]
         timest[j] = (data.get_node(group, 'events')[event][1] - sec[0])*1000000000 + data.get_node(group, 'events')[event][2] - ns[0]
      sec[0]-=1167609600
      t.Fill()

f.Write()
f.Close()

