# Gray DAQ Model Processes

gray, and gray-daq model the data acquisition system as a series of processes
that can be performed on the hits or singles events, sequentially.  At the end
of these processes, the singles events can then be passed to one or more
coincidence sorters.  The options for these processes are either described in a
process file, or in the system description file, by starting the line with the
command, "process"

The processes model currently requires a mapping file.  This mapping file tells
how "component" in the processes below maps to a particular function of the
system.  By default the individual crystals created by the command are called
"detector" and "array" depending if they were created with the "k" or "array"
command respectively.


## Process Type

### Merging

merge [component] [time window] (type/options)

Merge two, or more events that occur within the same window.  How the events are
merged is dictated by the merge type.  Currenlty allowed types are "max", which
is default, "first", and "anger".  First merges the events into the interaction
that occurs first in time.  Max merges the events into the one with maximum
energy.  Anger requires six more options.  [component name x] [name y] [name z]
[array size x] [sy] [sz].  These dictate the size of the array and how each of
the crystals maps to it's location within the array.  This currently requires
that the x, y, and z components are specified in the mapping file using C index
order for [X] [Y] [Z], otherwise the positioning will fail.  Currently, only
energy, statistics, and detector id are effected from the merging process


### Deadtime

deadtime [component] [time window] (type)

Simulates a deadtime process at a particular component level.  The type is
non-paralyzable by default, but type can be "paralyzable or "nonparalyzable".
This removes any subsequent events following in the datastream.


### Filtering or Energy Gating

filter [type] [level]

Valid filter types are "egate_high" and "egate_low", with the value specifying
the given energy in MeV.  Filters are inclusive of that value.

### Energy or Time Blurring

blur [type] [level] (options)

Valid blur types are "time" and "energy".  For time the value describes the
FWHM, in seconds of a truncated gaussian.  The gaussian is currently truncated
at 3 FWHMs from either direction of the mean, to make resorting the events after
blurring reasonable.  For energy, the value is specified as a gaussian FWHM in
percent.  If no other options are given, this value is constant across all
energies.  If a reference is given by supplying the option "at 0.511" or any
other energy in MeV, then an inverse square-root model will be used from that
reference.


### Sorting in Time

sort time [max]

Sorts the events in time.  This process assumes that events are generally time
sorted, but have local deviations, which is generally true for gray simulations.
The process will wait until it sees a time "current time + max" later before it
assumes an event is sorted.  Max is in seconds.  A sorting process is added
automatically after a time blurring process.  gray also adds a sorting process
to sort individual photon interactions that may be out of order because of
geometry.  gray-daq has an option to add a sorting process initially as well,
but will otherwise assume the events are sorted in time.  The value of max
should be kept minimal, as events are buffered as long as required, which could
be memory-expensive if the events are extremely out of order.

### Coincidence Sorting

coinc [type] [window] (options)

Performs coincidence sorting on the singles output.  Each "coinc" command adds
another coincidence sorter.  Valid types are "window" and "delay".  Delay
requires the first option to be the offset of delayed window in seconds.  Other
options allowed for all windows are keep_multiples, paralyzable, and
nonparalyzable.  If more than two events occur within the window width, they are
all rejected by default.  With keep_multiples on, they are saved and tagged with
the same coinc_id.  With the paralyzable option, the coincidence window is
extended if any event occurs within the currently open window.  The default
behavior is nonparalyzable, where the end of the window is fixed, regardless of
what happens in the window.
