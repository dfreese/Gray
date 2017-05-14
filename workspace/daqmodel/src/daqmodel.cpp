#include <fstream>
#include <iostream>
#include <Output/BinaryFormat.h>
#include <Output/Input.h>
#include <Output/Output.h>
#include <Physics/Interaction.h>
#include <Pipeline/InteractionStream.h>
#include <Random/Random.h>

using namespace std;

void usage() {
    cout << "gray-daq [-vh] -f [filename] -m [map file] -p [process list] ..\n"
         << "  -s [name] : singles output filename\n"
         << "  -c [name] : add coinc output filename (order matters)\n"
         << "  -t [type] : input format, (i.e \"full_ascii\")\n"
         << "  --singles_format [type] : default: input type\n"
         << "  --coinc_format [type] : default: input type\n"
         << "  --singles_mask [type] : default: input mask\n"
         << "  --coinc_mask [type] : default: input mask\n"
         << "  --sort [time] : sort the incoming events, assuming this max out of order time\n"
         << "  --seed [val] : set the random number generator seed for blur processes\n"
         << endl;
}

int main(int argc, char ** argv) {
    if (argc == 1) {
        usage();
        return(0);
    }

    bool verbose = false;
    string filename;
    string filename_output;
    vector<string> filenames_coinc;
    string filename_map;
    string filename_process;
    string format_str;
    string singles_format_str;
    string coinc_format_str;
    string singles_var_mask_str;
    string coinc_var_mask_str;
    // by default indicate to SinglesStream that no initial sorting process
    // should be applied to the incoming data
    double initial_sorting_window = -1;
    bool seed_specified = false;
    unsigned int seed;

    // Arguments not requiring input
    for (int ix = 1; ix < argc; ix++) {
        string argument(argv[ix]);
        if (argument == "-v") {
            verbose = true;
            cout << "Running in verbose mode " << endl;
        }
        if (argument == "-h" || argument == "--help") {
            usage();
            return(0);
        }
    }
    // Arguments requiring input
    for (int ix = 1; ix < (argc - 1); ix++) {
        string argument(argv[ix]);
        string following_argument(argv[ix + 1]);
        stringstream follow_arg_ss;
        follow_arg_ss << following_argument;
        if (argument == "-f") {
            filename = following_argument;
        }
        if (argument == "-s") {
            filename_output = following_argument;
        }
        if (argument == "-c") {
            filenames_coinc.push_back(following_argument);
        }
        if (argument == "-m") {
            filename_map = following_argument;
        }
        if (argument == "-p") {
            filename_process = following_argument;
        }
        if (argument == "-t") {
            format_str = following_argument;
        }
        if (argument == "--singles_format") {
            singles_format_str = following_argument;
        }
        if (argument == "--coinc_format") {
            coinc_format_str = following_argument;
        }
        if (argument == "--singles_mask") {
            singles_var_mask_str = following_argument;
        }
        if (argument == "--coinc_mask") {
            coinc_var_mask_str = following_argument;
        }
        if (argument == "--sort") {
            if ((follow_arg_ss >> initial_sorting_window).fail()) {
                cerr << "Unable to parse initial sorting time: "
                     << following_argument << endl;
                return(1);
            }
        }
        if (argument == "--seed") {
            if ((follow_arg_ss >> seed).fail()) {
                cerr << "Unable to parse seed value: "
                     << following_argument << endl;
                return(1);
            }
            seed_specified = true;
        }
    }

    if (seed_specified) {
        Random::Seed(seed);
    } else {
        Random::Seed();
    }

    if (filename.empty()) {
        cerr << "Filename not specified" << endl;
        return(2);
    }

    if (format_str.empty()) {
        cerr << "Input format not specified" << endl;
        return(2);
    }

    if (singles_format_str.empty()) {
        singles_format_str = format_str;
    }
    if (coinc_format_str.empty()) {
        coinc_format_str = format_str;
    }

    if (verbose) {
        cout << "input filename   : " << filename << endl;
        cout << "output filename  : " << filename_output << endl;
        cout << "map filename     : " << filename_map << endl;
        cout << "process filename : " << filename_process << endl;
    }

    Output::Format input_format;
    if (Output::ParseFormat(format_str, input_format) < 0) {
        cerr << "unable to parse format type: " << format_str << endl;
        return(3);
    }

    Output::Format singles_format;
    if (Output::ParseFormat(singles_format_str, singles_format) < 0) {
        cerr << "unable to parse format type: " << singles_format_str << endl;
        return(3);
    }

    Output::Format coinc_format;
    if (Output::ParseFormat(coinc_format_str, coinc_format) < 0) {
        cerr << "unable to parse format type: " << coinc_format_str << endl;
        return(3);
    }

    Input input;
    input.set_format(input_format);
    // Assume the variable format mask will be picked up from the file header.
    if (!input.set_logfile(filename)) {
        cerr << "Opening input failed" << endl;
        return(4);
    }

    Output::WriteFlags singles_mask = input.get_write_flags();
    if (!singles_var_mask_str.empty()) {
        if (!Output::parse_write_flags_mask(singles_mask, singles_var_mask_str)) {
            cerr << "unable to parse mask: " << singles_var_mask_str << endl;
            return(3);
        }
    }

    Output::WriteFlags coinc_mask = input.get_write_flags();
    if (!coinc_var_mask_str.empty()) {
        if (!Output::parse_write_flags_mask(coinc_mask, coinc_var_mask_str)) {
            cerr << "unable to parse mask: " << coinc_var_mask_str << endl;
            return(3);
        }
    }

    Output output;
    output.SetFormat(singles_format);
    output.SetVariableOutputMask(singles_mask);
    if (!output.SetLogfile(filename_output)) {
        cerr << "Opening output failed" << endl;
        return(5);
    }

    InteractionStream singles_stream(initial_sorting_window);
    int no_detectors = singles_stream.load_mappings(filename_map);
    if (no_detectors < 0) {
        cerr << "Loading mapping file failed" << endl;
        return(2);
    }

    int proc_load_status = singles_stream.load_processes(filename_process);
    if (proc_load_status < 0) {
        cerr << "Loading process file failed" << endl;
        return(2);
    }

    bool log_coinc = !filenames_coinc.empty();
    bool log_singles = !filename_output.empty();

    if (!log_coinc && !log_singles) {
        cerr << "No output specified" << endl;
        return(7);
    }

    std::vector<Output> outputs_coinc(singles_stream.no_coinc_processes());
    if (log_coinc) {
        if (singles_stream.no_coinc_processes() != filenames_coinc.size()) {
            cerr << "Incorrect number of filenames specified for coinc outputs"
            << endl;
            return(4);
        }
        for (size_t idx = 0; idx < singles_stream.no_coinc_processes(); idx++) {
            Output & output_coinc = outputs_coinc[idx];
            output_coinc.SetFormat(coinc_format);
            output_coinc.SetVariableOutputMask(coinc_mask);
            output_coinc.SetLogfile(filenames_coinc.at(idx));
        }
    }

    vector<Interaction> input_events;
    while (input.read_interactions(input_events, 100000)) {
        vector<Interaction> singles_events = singles_stream.add_events(input_events);
        if (log_singles) {
            for (const auto & event: singles_events) {
                output.LogInteraction(event);
            }
        }
        for (size_t idx = 0; idx < singles_stream.no_coinc_processes(); idx++) {
            // We need to make sure that we clear the coinc buffers every
            // so often (every round here) otherwise, they will build up
            // data.  A singles_stream.clear(), or a get_coinc_buffer call
            // to each buffer is required.
            auto coinc_events = singles_stream.get_coinc_buffer(idx);
            if (log_coinc) {
                for (const auto & interact: coinc_events) {
                    outputs_coinc[idx].LogInteraction(interact);
                }
            }
        }
    }
    vector<Interaction> events = singles_stream.stop();
    if (log_singles) {
        for (const auto & event: events) {
            output.LogInteraction(event);
        }
    }
    for (size_t idx = 0; idx < singles_stream.no_coinc_processes(); idx++) {
        auto coinc_events = singles_stream.get_coinc_buffer(idx);
        if (log_coinc) {
            for (const auto & interact: coinc_events) {
                outputs_coinc[idx].LogInteraction(interact);
            }
        }
    }

    if (verbose) {
        cout << "______________\n DAQ Stats\n______________\n"
             << singles_stream << endl;
    }
    return(0);
}
