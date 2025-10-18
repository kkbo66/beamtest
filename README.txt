Notice!!!:

First, please ssh lunon to find the data files.
ssh lunon
/data/DAQ/Beam_2510/

Then, open another terminal (Unbuntu) to work on the analysis.
go to /home/kkbo/beamtest/

data.dat in /home/kkbo/beamtest/data
data.root in /home/kkbo/beamtest/root
Figure.root in /home/kkbo/beamtest/draw/figureroot

Decode, Reconstruction script in /home/kkbo/beamtest/2025/ECAL/
Draw script in /home/kkbo/beamtest/draw

Run!!!:
For Decode and Draw
./ana1.sh DATE DATADATNAME

For ElecCalibration
./elecali.sh ROOTLIST  ELECLIST (in directory ./list/)
