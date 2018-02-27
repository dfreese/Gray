# Gray DAQ Model
Gray has two major components: a physics engine, and a data acquisition model.
The different DAQ components that can be modeled are listed out in the process
markdown file.  This document describes the use of the gray-daq executable that
is created when building Gray.

The DAQ model of is designed to be able to be run independently of the physics
model within gray.  Often a system's physical design is fixed, but different
choices, such as the amount of multiplexing done in a system, or the effect of
deadtime, need to be studied.  That is where gray-daq comes in.

## Basic Use
Assuming we have the following files:

* system.dff : the physical PET system description
* system_mux_1.pdc : the first configuration of multiplexing to be studied
* system_mux_2.pdc : the second configuration of multiplexing to be studied
* system.map : the mapping file for the system

we could run the following commands in order to get the output from the
different multiplexing models.

```
gray -f system.dff -m system.map -i output_hits.dat
gray-daq -f system.dff -m system.map -p system_mux_1.pdc -i output_hits.dat -c output_coinc_mux_1.dat
gray-daq -f system.dff -m system.map -p system_mux_2.pdc -i output_hits.dat -c output_coinc_mux_2.dat
```

This will generate the expected coincidences for each of the DAQ system
configurations, holding the input interaction data constant, so the effect of
the DAQ parameters can be studied. This also can speed up iteration on just the
DAQ model itself, as the physics simulation contributes to ~95-99% of the
computational time required for a simulation.

## Advanced Use
Now outputing the hits file from a simulation can cause a prohibitively large
data file size.  Often certain configurations of the system are known and much
of the data in the hits file is unnecessary.  In this case we can use the fact
that gray-daq can read in singles data as well as hits data.  This singles data
can be generated from any number of processes.

Working from our previous example, let's assume we have a system.pdc file that
describes the initial integration logic and triggering threshold.  This will
energy gate all events under 150keV out of the DAQ datastream.

```
gray -f system.dff -m system.map -p system.pdc -s output_singles.dat
```

We now have a singles data file describing all of the interactions that would
have summed up to 150keV inside of a detector.  Now, if we assume
system_mux_1.pdc and system_mux_2.pdc only describe the remainder of the DAQ
model after the integration and initial triggering threshold, the following
would generate the system's coincidence output:

```
gray-daq -f system.dff -m system.map -p system_mux_1.pdc -i output_singles.dat -c output_coinc_mux_1.dat
gray-daq -f system.dff -m system.map -p system_mux_2.pdc -i output_singles.dat -c output_coinc_mux_2.dat
```

This allows the user to reduce the output of data output from the physics
simulation to what he or she requires.  The input file format can be either the
variable ascii or binary format, described in another document.  As a warning,
if a photon interacts in two detectors, but within one detector is merged into
another event, the statistics on the number of scatters in the detector or
phantom will be incorrect if these events are written out as singles and then
the two singles are merged together later by gray-daq.  This is a fairly
unlikely event, but if you require exactly accurate statistics make sure you
output singles after the last merge process in your DAQ model, or output hits
data.
