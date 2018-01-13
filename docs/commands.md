# Gray Commands

Units are always cm for distance, uCi for activity, seconds for time, and
degrees for angle, unless specified otherwise.

## Reading and Writing

### hits_format
```
hits_format [format]
```
Specifies the output format for the hits, assuming a hit file is specified.
Valid options are var_ascii, var_binary, full_ascii, full_binary, or
no_pos_binary.  Default is var_ascii.

### singles_format
```
singles_format [format]
```
Specifies the output format for the singles, assuming a singles file is
specified.  See hits_format for valid options.

### coinc_format
```
coinc_format [format]
```
Specifies the output format for the coincidences, assuming a coinc file (or
coinc files) is/are specified.  See hits_format for valid options.

### hits_output
```
hits_output [filename]
```
Set the output filename for the hits file. This will cause hits to be written
out.

### singles_output
```
singles_output [filename]
```
Set the output filename for the singles file. This will cause singles to be
written out.  This is any event from the data acquisition model that has not yet
been run through a coincidence processor.

### coinc_output
```
coinc_output [filename]
```
Adds an output filename for the coincidences. This will cause coincidences to be
written out.  If any names are specified, then the number must match the number
of coincidence processors specified in the daq model.

### process_file
```
process_file [filename]
```
Pick a file to read in as the data acquisition model description.  This filename
is assumed to be a path relative to the file in which it is contained.  A
process file can be forgone by using the "process" command instead.

### mapping_file
```
mapping_file [filename]
```
Pick a file to read in as the data acquisition model mapping file.  This
filename is assumed to be a path relative to the file in which it is contained.

### hits_var_mask
```
hits_var_mask [mask of 15 0s or 1s]
```
If the format for hits is either var_ascii or var_binary, then this sets the
mask applied to the variable output.  The mask should be a series of 0 or 1s
corresponding to the following fields in order:

1. time
2. decay_id
3. color
4. type
5. pos
6. energy
7. det_id
8. src_id
9. mat_id
10. scatter_compton_phantom
11. scatter_compton_detector
12. scatter_rayleigh_phantom
13. scatter_rayleigh_detector
14. xray_flouresence
15. coinc_id

By default, all of them are on.

### singles_var_mask
```
singles_var_mask [mask of 15 0s or 1s]
```
See hits_var_mask.  Does the same for the singles output file.

### coinc_var_mask
```
coinc_var_mask [mask of 15 0s or 1s]
```
See hits_var_mask.  Does the same for all coincidence output files.

### log_positron
```
log_positron
```
Has no options.  Tells gray to log positron decays and their locations in the
hits file.

### log_nondepositing
```
log_nondepositing
```
Has no options.  Tells gray to interactions in sensitive detectors that do not
deposit energy.  This currently only causes Rayleigh scattering interactions
to be logged in the hits file.

### log_nonsensitive
```
log_nonsensitive
```
Has no options.  Tells gray to log hits in non-sensitive materials in the hits
file.

### log_nointeraction
```
log_nointeraction
```
Has no options.  Tells gray to log times a photon passes through a material
without interacting in the hits file.  Not really useful unless you want to see
the full photon path for some reason.

### log_errors
```
log_errors
```
Has no options.  Tells gray to log any errors that occur within the ray tracer
in the hits file.  Errors tend to occur because objects accidentally overlap or
share a parallel face.  Neither of these are allowed

### log_all
```
log_all
```
Has no options.  Equivalent to log_positron, log_nonsensitive,
log_nointeraction, log_errors.  Not recommended for a full simulation unless you
want to fill your disk very quickly.

### echo
```
echo [Any string]
```
Prints out a whatever follows.  Useful for debugging includes and repeats, or
surprising collaborators.

## Simulation Parameters

### process
```
process [process file line]
```
Equivalent to writing one line in a process file included by process_file.  If a
process_file is specified, these lines are ignored.

### time
```
time [simulation time in seconds]
```
Sets the amount of time simulated in seconds.

### start_time
```
start_time [start time in seconds]
```
Sets the amount time at which the simulation starts.  This is useful for
modeling the decay of isotopes from a particular starting activity.

### seed
```
seed [unsigned int (0 to 4294967295)]
```
Sets the seed for the random number generator for the simulation.  If a seed is
not set, then the unix time in seconds is used.  Useful for repeating a
simulation with the exact same output.  A mt19937 generator from the c++
standard library is used.

### disable_half_life
```
disable_half_life
```
Has no options.  No isotopes experience decay in activity over time, regardless
of their settings

### disable_rayleigh
```
disable_rayleigh
```
Has no options.  Disables rayleigh scattering globally within all materials.


## Movement and Orientation

### t
```
t [x] [y] [z]
```
Move the origin of the world, in the current matrix, by x, y, z cm.

### push
```
push
```
Has no options.  Saves the current coordinate matrix, so that any future
commands can be undone by calling pop.  Multiple push commands can be layered.

### pop
```
pop
```
Has no options.  Recalls a previous state of the coordinate matrix (origin and
rotation).

### raxis
```
raxis [x] [y] [z] [degrees]
```
Rotates the current world around the direction indicated by x, y, and z by the
specified angle.  The current origin is used as the center of rotation.

### include
```
include [filename]
```
Pulls in the specified file.  Behaves equivalent to copy and pasting that file
in place of this line.

### begin_repeat
```
begin_repeat [n]

begin_repeat rotate [n] [degrees] [x] [y] [z]

begin_repeat grid [nx] [ny] [nz] [x] [y] [z]
```
Equivalent to copy and pasting the section between begin_repeat and end_repeat n
times.  The rotate option calls "raxis" after each repeat with the specified
Parameters.  The grid option cals "t" after every repeat, stepping out a grid,
centered at the origin.  X steps are made first, then Y, then, Z.  These
commands can be layered.

### end_repeat
```
end_repeat
```
Has no options.  Must be paired with begin_repeat.

## Geometry

### m
```
m [material name]
```

Sets the current material to the given name.  This must match, exactly, the name
given to the material in the materials file.

### sphere
```
sphere [x] [y] [z] [radius]
```
Creates a sphere of given radius, centered at x, y, z.

### cyl
```
cyl [center xyz] [axis xyz] [radius] [height]
```
Creates a cylinder of given radius and center, with the top circle facing
towards axis.

### ann_cyl
```
ann_cyl [center xyz] [axis xyz] [inner radius] [outer radius] [height]
```
Creates an annulus cylinder (hollow) of given inner and outer radius and center,
with the top circle facing towards axis.

### k
```
k [center xyz] [size xyz]
```
Creates a box, oriented to the current axes with a given size.  If the current
material is sensitive, then this will be made into a detector.

### array
```
array [center xyz] [number xyz] [step xyz] [size xyz]
```
Effectively calls:
```
begin_repeat [number xyz] [step xyz]
    k [center xyz] [size xyz]
end_repeat
```

### p
```
p [order]
[x] [y] [z] (repeat this line [order] times)
```
Specifies a polygon of generic order.  Each vertex of the polygon must then be
specified in the following lines.  The outer face of the polygon is specified by
the right hand rule, starting with the first vertex.  Vertices must then be
specified in a circular order around the edge of the polygon.

If the polygon is apart of a detector, increment must be called before this is
specified.

If the polygon is apart of a source, start_vecsrc and stop_vecsrc must be called
before and after all of the polygons in the source.

### scale
```
scale [factor]
```
Scale all of the distances from here on out in the file by this amount.
Subsequent calls will overwrite this value.

### ellipsoid
```
ellipsoid [center xyz] [axis1 xyz] [axis2 xyz] [radius1] [r2] [r3]
```
Creates an ellipsoid with the given parameters

### elliptic_cyl
```
elliptic_cyl [center xyz] [axis xyz] [radius1] [radius2] [height]
```
Creates an elliptical cylinder, similar to "cyl", but with radius1 and radius2
oriented towards x and y in the current frame.

### increment
```
increment
```
Has no options.  Advances the detector id assigned to sensitive polygons meshes.

## Sources

### isotope
```
isotope [isotope name]
```
Sets the current isotope to the given name.  This must match, exactly, the name
given to the isotope in the isotopes file.

```
isotope beam [direction xyz] [angle width (deg fwhm)] [energy] [activity]
```
Creates a monoenergetic source with a given direction.  The angle should be
non-negative.  If it is greater than zero, an angle offset from the direction
axis will randomly be selected, along with a random azimuthal coordinate around
the axis.  This is typically paired with pt_src.  Gamma rays are emitted in
both direction and (-direction).

### cyl_src
```
cyl_src ["cyl" options] [activity]
```
Creates a cylindrical source.

### sp_src
```
sp_src ["sphere" options] [activity]
```
Creates a spherical source.

### rect_src
```
rect_src ["k" options] [activity]
```
Creates a rectangular source.

### start_vecsrc
```
start_vecsrc [activity]
```
Indicates the start of a vectorized source, made of a number of polygons.  Any
polygons until end_vecsrc will be assigned to the source.  The polygons should
encapsulate a closed volume.  None of the polygons will be used in the main
simulation geometry, so if a material geometry is supposed to mimic the source
geometry, then the full set of polygons should be specified again before
start_vecsrc or after end_vecsrc.

### end_vecsrc
```
end_vecsrc
```
Indicates the end of a vector source started by start_vecsrc

### pt_src
```
pt_src [position xyz] [activity]
```
Creates a source where decays are emitted precisely from the position given.
If the current isotope is a positron, the positron range will be applied from
this position.  This can commonly be paired with a "isotope beam ..."  command
to create a gamma ray beam of a particular energy.

### voxel_src
```
voxel_src [filename] [no voxels xyz] [voxel size xyz] [activity]
```
Creates a voxelized phantom at 0, 0, 0.  It then loads in a file with a series
of values listed as doubles in text.  Currently does not support being apart of
multiple materials.

### ellipsoid_src
```
ellipsoid_src ["ellipsoid" options] [activity]
```
Creates an ellipsoid source

### elliptic_cyl_src
```
elliptic_cyl_src ["elliptic_cyl" options] [activity]
```
Creates an elliptical cylinder source.

### annulus_ell_src
```
annulus_ell_src ["ellipsoid_src" options]
```
Creates an ellipsoid source, but activity only occurs on the outer rim of the
source volume

### annulus_cyl_src
```
annulus_cyl_src ["cyl_src" options]
```
Creates an cylinder source, but activity only occurs on the outer rim of the
source volume.

### scale_act
```
scale_act [factor]
```
Scales all subsequent activity by this factor.  Overwrites any previous scale.

## Viewing

These commands are only useful when using the OpenGL viewer gray-view.

### from
```
from [position xyz]
```
View the point specified by "at" from this point.

### at
```
at [position xyz]
```
Look at this point in the geometry.

### up
```
up [position xyz]
```
Define the up direction in the viewer

### angle
```
angle [degrees]
```
Set how large of an angle to view through the viewer

### hither
```
hither [distance]
```
Set the hither distance to dictate how perspective is handled.  Currently, this
value is ignored.

### resolution
```
resolution [x] [y]
```
Set the width in pixels of the viewer.

### color
```
color [ambient rgb] [diffuse rgb] [spec rgb] [trans rgb] [reflec rgb] [index of
refraction]
```
Set the color of the current material.  RGB values should be [0, 1].  Overwrites
any previous setting for all objects of that material.

### l
```
l [position xyz] [color rgb]
```
Add a light to the scene at a given position with a particular color.  RGB
values should be [0, 1].

### b
```
b [color rgb]
```
Sets the background color of the scene.  RGB values should be [0, 1].

### v
```
v
```
Legacy.  Does nothing and can safely be removed.
