#!/bin/csh -f

set DATE = `date +%s`
set FILENAME = '/gray_'$DATE'.tar.gz'
set RELEASE_DIR = '/home/polcott/Gray'
set NAME = '/Gray'
set BIN_FILE = './Debug/Gray'
set FILES = {'./Debug/Gray','*.dat','detectors/*.dff','matlab/*.m'}
echo 'packaging '$FILENAME' for distribution'
if (-d $RELEASE_DIR$NAME) then
	echo 'moving old directory.'
mv $RELEASE_DIR$NAME $RELEASE_DIR'/old_'$DATE
endif
mkdir $RELEASE_DIR'/Gray'
foreach FILE ($FILES)
cp $FILE $RELEASE_DIR$NAME
end
cd $RELEASE_DIR
tar cvfz '.'$FILENAME '.'$NAME
