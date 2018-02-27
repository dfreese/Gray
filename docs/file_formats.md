# File Formats in Gray
Gray currently supports two file formats: a variable ascii output, which is the
default, and a variable binary format.

## Output Fields
In version 1 of both the binary and ascii variable outputs, there are 15 fields
of data that can be represented.  These are:

* Time (seconds)
* Decay ID - representing the positron the event was from
* Color - representing the photon number from the decay.  511keV photons from
  a positron are 0, blue, and 1, red.  Prompt gammas from the decay are 2,
  yellow.
* Type - Specifies what type of interaction created the event. If this
  is negative, then it indicates an error.  These indicate problems
  with the ray-tracing geometry.  This can happen if materials are
  intersecting, gaps are left in materials, or rounding errors cause an object
  to not close completely.
  * 0: Compton
  * 1: Photoelectric
  * 2: Rayleigh
  * 3: Nuclear decay such as a positron
  * -1: indicates the ray-tracer found more exiting faces of materials than it
    was aware of.  This tends to indicate overlapping materials of the same
    type.
  * -2: indicates a trace depth error.  This means the photon was tracked
    through too many materials and non-interactions.  The trace depth is fixed
    at 500, so this should not be an issue for even extremely complicated
    geometries.  If you are running into this issue, check for overlapping
    geometry.
  * -3: indicates the material that the ray-tracer was trying to exit did not
    match the material it thought it was in, which is typically a result of
    overlapping geometries.
* Position - The x, y, and z coordinate the event was recorded at.  For events
  that have been processed by the DAQ model, this will typically represent the
  position of the largest event after interactions are merged, unless merge
  first is selected.
* Energy - The energy the interaction deposited in MeV
* Detector ID - The ID of the detector, typically a scintillation crystal, that
  the event interacted in, or -1 if the interaction was in a non-sensitive
  material.  Detector IDs are given in the order in which they are specified in
  the geometry file.
* Source ID - The source from which the positron was emitted.  These IDs are
  given in the order in which they are specified in the scene file.
* Material ID - The ID of the material based on the order in which it falls in
  the materials database file.
* Phantom Compton Scatters - The number of compton scatters a photon has
  undergone in a non-sensitive material.  If there is a merging process/step in
  the DAQ model, then this number will represent the total for all of the
  photons in the merge.  This is done for all scatter stats.
* Detector Compton Scatters - The number of Compton scatters undergone in a
  sensitive material, which is a detector.
* Phantom Rayleigh Scatters - The number of Rayleigh scatters undergone in a
  non-sensitive material.
* Detector Rayleigh Scatters - The number of Compton scatters undergone in a
  sensitive material, which is a detector.
* X-Ray Flouresences - a legacy field for x ray flourensence contributions that
  is no longer supported.
* Coincidence ID - The ID of the coincidence event the event was paired into.
  By convention, if multiples are enabled, all pairs of events are given the
  same coincidence id so they can be identified.

## Variable ASCII Format
The variable ASCII format is a space delimited file with a header prepended.
The header will list the version of the output on the first line.  In version 1
there will be two lines that follow that list the number of fields possible and
the number of fields that are active.  The names of the different columns and
if they are active "1" or inactive "0" will be listed on an individual line
below that.  The remainder of the file can be read as a space delimited file
for the active fields.

## Variable Binary Format.
The variable binary format is a list of events prepended by a header describing
the fields in each of the events.  For version 1 of the format, the header has
the following 20 fields listed as signed 32 bit integers:

1. The hex number 0xFFFB
2. The version number
3. The number of fields
4. The number of active fields
5. The size in bytes per event
6. time field active (0 or 1)
7. decay id field active (0 or 1)
8. color field active (0 or 1)
9. type field active (0 or 1)
10. position fields (x, y, and x) active (0 or 1)
11. energy field active (0 or 1)
12. detector id field active (0 or 1)
13. source id field active (0 or 1)
14. material id field active (0 or 1)
15. phantom compton scatter field active (0 or 1)
16. detector compton scatter field active (0 or 1)
17. phantom rayleigh scatter field active (0 or 1)
18. detector rayleigh scatter field active (0 or 1)
19. xray flouresence field active (0 or 1)
20. coincidence id field active (0 or 1)

The remaining data in the file correspond to the event data.  The fields, if
active, are then written in the following order with the given types for each
event.

1. time (float64)
2. decay id (int32)
3. color (int32)
4. type (int32)
5. position (3 x float64)
6. energy (float64)
7. detector id (int32)
8. source id (int32)
9. material id (int32)
10. phantom compton scatter (int32)
11. detector compton scatter (int32)
12. phantom rayleigh scatter (int32)
13. detector rayleigh scatter (int32)
14. xray flouresence field active (int32)
15. coincidence id field active (int32)

An example of how to read in this file using NumPy in Python is implemented in
the python/gray/io.py file.
As a note of caution, these sizes are based on typical x64 linux system, but
are technically platform dependent, which needs to be changed in the software
to not assume little endian byte order, that "int" is 32 bits, or that double
is a 64 bit IEEE double precision floating point number.
