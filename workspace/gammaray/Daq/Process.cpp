#include <Daq/Process.h>

/*!
 *
 */
Process::EventIter Process::process_events(EventIter begin, EventIter end) {
    const long start_count_dropped = count_dropped;
    auto ready_events = _process_events(begin, end);

    auto kept_func = [](const EventT & event) {return (!event.dropped);};
    const long kept = std::count_if(begin, ready_events, kept_func);
    count_kept += kept;
    count_events += kept + (count_dropped - start_count_dropped);
    return(ready_events);
}

/*!
 *
 */
void Process::stop(EventIter begin, EventIter end) {
    const long start_count_dropped = count_dropped;

    _stop(begin, end);

    auto kept_func = [](const EventT & event) {return (!event.dropped);};
    const long kept = std::count_if(begin, end, kept_func);
    count_kept += kept;
    count_events += kept + (count_dropped - start_count_dropped);
}

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
    return(count_events);
}

/*!
 * Number of events dropped or merged by the class
 */
long Process::no_dropped() const {
    return(count_dropped);
}

/*!
 * Number of events output by the class.
 */
long Process::no_kept() const {
    return(count_kept);
}

/*!
 *
 */
std::ostream& operator<< (std::ostream & os, const Process & p) {
    os << p.print_info();
    return(os);
}

void Process::inc_no_dropped() {
    count_dropped++;
}

void Process::inc_no_dropped(long val) {
    count_dropped += val;
}

Process::TimeT Process::get_time(const EventT& event) {
    return (event.time);
}

Process::TimeT Process::time_diff(const EventT& lhs, const EventT& rhs) {
    return (lhs.time - rhs.time);
}

Process::DetIdT Process::get_det_id(const EventT& event) {
    return (event.det_id);
}

std::string Process::print_info() const {
    return ("");
}
