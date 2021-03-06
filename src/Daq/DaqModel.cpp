/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Daq/DaqModel.h"
#include "Gray/Daq/DaqStats.h"
#include "Gray/Daq/CoincProcess.h"
#include "Gray/Daq/DeadtimeProcess.h"
#include "Gray/Daq/FilterProcess.h"
#include "Gray/Daq/Mapping.h"
#include "Gray/Daq/MergeProcess.h"
#include "Gray/Daq/ProcessFactory.h"
#include "Gray/Random/Random.h"

/*!
 * If the initial sort window is greater than zero, a sorting process is
 * added to the stream immediately with that wait window.  Necessary for
 * gray, where we need to guarantee sorting before the user's preferences
 * are added.
 */
DaqModel::DaqModel(TimeT initial_sort_window) {
    if (initial_sort_window > 0) {
        add_process(ProcessFactory::SortFactory(initial_sort_window), false);
    }

}

DaqModel::ContainerT& DaqModel::get_buffer() {
    return (input_events);
}

void DaqModel::consume(std::vector<Interaction> inters) {
    input_events.insert(input_events.end(), inters.begin(), inters.end());
}

int DaqModel::set_processes(const std::vector<std::string> & lines,
                            const Mapping::IdMappingT& mapping)
{
    std::vector<ProcessDescription> process_descriptions;
    int status = ProcessFactory::ProcessDescLines(lines, process_descriptions);
    if (status < 0) {
        return(-1);
    }

    return(set_processes(process_descriptions, mapping));
}

int DaqModel::load_processes(const std::string & filename,
                             const Mapping::IdMappingT& mapping)
{
    std::vector<ProcessDescription> process_descriptions;
    int status = ProcessFactory::ProcessDescFile(filename,
                                                 process_descriptions);
    if (status < 0) {
        return(-1);
    }
    return(set_processes(process_descriptions, mapping));
}

size_t DaqModel::no_processes() const {
    return(processes.size());
}

size_t DaqModel::no_coinc_processes() const {
    return(coinc_processes.size());
}

long DaqModel::no_events() const {
    if (!processes.empty()) {
        return (processes.front().second.no_events());
    } else if (!coinc_processes.empty()) {
        return (coinc_processes.front().second.no_events());
    } else {
        return (0);
    }
}

long DaqModel::no_kept() const {
    if (!processes.empty()) {
        return (processes.back().second.no_kept);
    } else {
        return (0);
    }
}

long DaqModel::no_dropped() const {
    long count = 0;
    for (const auto& p: processes) {
        count += p.second.no_dropped;
    }
    return(count);
}

long DaqModel::no_merged() const {
    long count = 0;
    for (const auto& p: processes) {
        if (dynamic_cast<const MergeProcess*>(p.first.get())) {
            count += p.second.no_dropped;
        }
    }
    return(count);
}

long DaqModel::no_filtered() const {
    long count = 0;
    for (const auto& p: processes) {
        if (dynamic_cast<const FilterProcess*>(p.first.get())) {
            count += p.second.no_dropped;
        }
    }
    return(count);
}

long DaqModel::no_deadtimed() const {
    long count = 0;
    for (const auto& p: processes) {
        if (dynamic_cast<const DeadtimeProcess*>(p.first.get())) {
            count += p.second.no_dropped;
        }
    }
    return(count);
}

DaqStats DaqModel::stats() const {
    DaqStats report;
    report.no_events = no_events();
    report.no_kept = no_kept();
    report.no_dropped = no_dropped();
    report.no_merged = no_merged();
    report.no_filtered = no_filtered();
    report.no_deadtimed = no_deadtimed();
    report.no_kept_per_proc = std::vector<long>(no_processes());
    report.no_dropped_per_proc = std::vector<long>(no_processes());
    for (size_t idx = 0; idx < no_processes(); ++idx) {
        report.no_kept_per_proc[idx] = processes[idx].second.no_kept;
        report.no_dropped_per_proc[idx] = processes[idx].second.no_dropped;
    }
    report.coinc_stats = std::vector<ProcessStats>(no_coinc_processes());
    for (size_t idx = 0; idx < no_coinc_processes(); ++idx) {
        report.coinc_stats[idx] = coinc_processes[idx].second;
    }
    report.print_info = print_info;
    return (report);
}

std::ostream & operator << (std::ostream & os, const DaqModel & s) {
    return(os << s.stats());
}

int DaqModel::set_processes(
        const std::vector<ProcessDescription> & process_descriptions,
        const Mapping::IdMappingT& mapping)
{
    for (const auto & desc: process_descriptions) {
        std::unique_ptr<Process> proc = ProcessFactory::ProcessFactory(desc,
                                                                       mapping);
        if (proc == nullptr) {
            return (-1);
        }
        add_process(std::move(proc), true);
        if ((desc.type == "blur") && (desc.subtype == "time")) {
            // If we blur time, we need to sort the data afterwards.
            // Since we could add the time blur, we know we can safely get
            // the time used.
            TimeT max_blur_time = 0;
            // TODO: place index for time blur in a reusable location so
            // this doesn't need to be updated if the format changes.
            desc.as_time(1, max_blur_time);
            max_blur_time *= 2 * ProcessFactory::default_max_time_blur;
            add_process(ProcessFactory::SortFactory(max_blur_time), false);
        }
    }
    return(0);
}

void DaqModel::add_process(std::unique_ptr<Process> process,
                           bool proc_print_info)
{
    if (dynamic_cast<const CoincProcess*>(process.get())) {
        coinc_processes.emplace_back(std::move(process), ProcessStats());
    } else {
        processes.emplace_back(std::move(process), ProcessStats());
        print_info.push_back(proc_print_info);
        process_ready_distance.push_back(0);
    }
}

DaqModel::EventIter DaqModel::begin() {
    return(input_events.begin());
}

DaqModel::EventIter DaqModel::end() {
    return(input_events.end());
}

DaqModel::EventIter DaqModel::hits_begin() {
    if (hits_stopped) {
        if (process_ready_distance.empty()) {
            return(begin());
        } else {
            return(std::next(begin(), process_ready_distance.front()));
        }
    } else {
        return(begin());
    }
}

DaqModel::EventIter DaqModel::hits_end() {
    if (hits_stopped) {
        return(end());
    } else {
        if (process_ready_distance.empty()) {
            return(begin());
        } else {
            return(std::next(begin(), process_ready_distance.front()));
        }
    }
}

DaqModel::EventIter DaqModel::singles_begin() {
    if (singles_stopped) {
        return(singles_ready);
    } else {
        return(begin());
    }
}

DaqModel::EventIter DaqModel::singles_end() {
    if (singles_stopped) {
        return(end());
    } else {
        return(singles_ready);
    }
}

DaqModel::EventIter DaqModel::coinc_begin() {
    return(begin());
}

DaqModel::EventIter DaqModel::coinc_end() {
    if (coinc_stopped) {
        return(end());
    } else {
        return(coinc_ready);
    }
}

/*!
 * Only run the first process, which is always a sorting process in gray.  This
 * should not be called if initial_sort_window was not specified.
 */
void DaqModel::process_hits() {
    singles_stopped = false;
    singles_ready = input_events.end();
    if (!processes.empty()) {
        const auto begin = std::next(input_events.begin(),
                                     process_ready_distance.front());
        auto& proc_pair = processes.front();
        singles_ready = proc_pair.first->process(begin, singles_ready, proc_pair.second);
        process_ready_distance.front() = std::distance(input_events.begin(), singles_ready);
    }
    min_coinc_ready_dist = std::distance(input_events.begin(), singles_ready);
}

void DaqModel::process_singles() {
    singles_stopped = false;
    // We might have left some singles that we processed in the buffer from
    // previously, so start where we left off.
    singles_ready = input_events.end();
    for (size_t ii = 0; ii < processes.size(); ii++) {
        const auto begin = std::next(input_events.begin(),
                                     process_ready_distance[ii]);
        auto& proc_pair = processes[ii];
        singles_ready = proc_pair.first->process(begin, singles_ready, proc_pair.second);
        process_ready_distance[ii] = std::distance(input_events.begin(), singles_ready);
    }
    min_coinc_ready_dist = std::distance(input_events.begin(), singles_ready);
}

void DaqModel::process_coinc(size_t idx) {
    coinc_stopped = false;
    auto& proc_pair = coinc_processes[idx];
    coinc_ready = proc_pair.first->process(
            input_events.begin(), singles_ready, proc_pair.second);
    min_coinc_ready_dist = std::min(
            min_coinc_ready_dist,
            std::distance(input_events.begin(), coinc_ready));
}

void DaqModel::stop_hits() {
    hits_stopped = true;
    if (!processes.empty()) {
        auto p_beg = std::next(begin(), process_ready_distance.front());
        auto& proc_pair = processes.front();
        proc_pair.first->stop(p_beg, end(), proc_pair.second);
        process_ready_distance.front() = std::distance(begin(), end());
    }
}

void DaqModel::stop_singles() {
    singles_stopped = true;
    for (size_t ii = 0; ii < process_ready_distance.size(); ii++) {
        auto p_beg = std::next(begin(), process_ready_distance[ii]);
        auto& proc_pair = processes[ii];
        proc_pair.first->stop(p_beg, end(), proc_pair.second);
        process_ready_distance[ii] = std::distance(begin(), end());
    }
}

void DaqModel::stop_coinc(size_t idx) {
    coinc_stopped = true;
    auto& proc_pair = coinc_processes[idx];
    proc_pair.first->stop(begin(), end(), proc_pair.second);
}

void DaqModel::clear_complete() {
    auto singles_dist = std::distance(input_events.begin(), singles_ready);
    input_events.erase(begin(), std::next(begin(), min_coinc_ready_dist));
    for (auto & dist: process_ready_distance) {
        dist -= min_coinc_ready_dist;
    }
    singles_ready = std::next(input_events.begin(),
                              singles_dist - min_coinc_ready_dist);
}
