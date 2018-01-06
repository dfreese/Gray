#include <Daq/Process.h>

/*!
 * Resets the map and clears out all of the events.
 */
void Process::reset() {
    _reset();
    count_kept = 0;
    count_dropped = 0;
    count_events = 0;
}

/*!
 * Number of events processed by the class
 */
long Process::no_events() const {
    return (count_kept + count_dropped);
}

/*!
 * Number of events dropped or merged by the class
 */
long Process::no_dropped() const {
    return (count_dropped);
}

/*!
 * Number of events output by the class.
 */
long Process::no_kept() const {
    return (count_kept);
}

/*!
 *
 */
std::ostream& operator<< (std::ostream & os, const Process & p) {
    os << p.print_info();
    return (os);
}

void Process::inc_no_kept() {
    ++count_kept;
}

void Process::inc_no_kept(long val) {
    count_kept += val;
}

void Process::inc_no_dropped() {
    count_dropped++;
}

void Process::inc_no_dropped(long val) {
    count_dropped += val;
}

std::string Process::print_info() const {
    return ("");
}
