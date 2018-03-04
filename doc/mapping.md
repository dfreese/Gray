# The Mapping File in Gray
In the design of Gray, we have tried to not to force one system architecture
convention.  Those familiar with GATE will know that there are different PET
system types, such as cylindricalPET, CPET, Ecat, etc. that define how a
particular detector, such as a scintillation crystal element, is referenced.
In order to not enforce a particular heirarchy, the mapping file was created.
This tells gray, or gray-daq, to what parts of the system each detector
belongs.  The mapping file is only used by the DAQ model, and has no effect on
the behavior of the physics within the simulation.

## Format
Gray expects the mapping file to start with a line listing the names of all of
the fields that can be refernced by the DAQ model.  It then expects one line in
the file for every detector in the simulation.  The detector line lists the
detectors id or "mapping" under each of the field names at the top of the file.
For example, lets say we have a system with 16 detector crystals.  These are
placed in 2x2 blocks at 90 degree rotations from each other.  To map those
detectors to those places, our file would look like the following:

```
detector block
0        0
1        0
2        0
3        0
4        1
5        1
6        1
7        1
8        2
9        2
10       2
11       2 # Anything after a pound sign is ignored.
12       3
13       3
# Blank lines are also ignored

14       3
15       3
```

Note: only a single space is required.

This tells gray-daq that detectors 8, 9, 10, and 11 are in the same block of
crystals.  We can now say in the DAQ model process file:

```
merge block 40e-9 max
```

This will merge all interactions in the individual blocks together into the
interaction with the maximum energy, as long as they happen within 40ns of each
other.

## Anger Logic
Now, often within PET system, we will use some sort of multiplexing to readout
a block that will cause the event to be positioned using an energy-weighted
mean of the interactions.  To do this within Gray, we now need a bit more
information in our mapping file.  Here's an updated version of the previous
example.

```
detector block bx by bz
0        0     0  0  0
1        0     0  0  1
2        0     0  1  0
3        0     0  1  1
4        1     0  0  0
5        1     0  0  1
6        1     0  1  0
7        1     0  1  1
8        2     0  0  0
9        2     0  0  1
10       2     0  1  0
11       2     0  1  1 # Anything after a pound sign is ignored.
12       3     0  0  0
13       3     0  0  1
14       3     0  1  0
15       3     0  1  1
```

This tells us, again, detectors 8, 9, 10, and 11 are in the same block 2, but
now we know that detector 9 is in the first row in y and the second column in z
spatially within the array.  This gives gray and gray-daq the spatial
information it needs to assign a detector id to an event.  This is not
particularly intersting in a 2x2 block, but for larger blocks, this allows an
event to be assigned to a detector that did not take part in the interactions
being merged, but that is still at the energy-weighted centroid of the
interactions.  This is useful to approximate Anger logic.  To describe that in
the DAQ process file, you could change the merge command given as an example
previously to the following.

```
merge block 40e-9 anger bx by bz
```

## Creation
It'll be obvious to the reader now that the mapping file would be annoying to
create manually for a system with thousands of crystals.  That is why gray
would automatically create the above mapping file when passed the "--write_map"
flag.  Let's say the previous example is described in the system.dff file, that
has the contents:

```
begin_repeat rotate 4 90.0 0.0 0.0 1.0
    # This indentation is optional
    array 20.0 0.0 0.0  1 2 2  2.0 1.1 1.1  2.0 1.0 1.0
end_repeat
```

Then the following command would generate the above mapping file.  This can
be modified via a script or something else to add more complexity.

```
gray -f system.dff --write_map system.map
```

Examples of reading and writing mapping files can be seen in
detectors/create_gate_benchmark_map.py.
