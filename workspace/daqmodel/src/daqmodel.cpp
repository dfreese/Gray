#include <fstream>
#include <iostream>
#include <Output/BinaryFormat.h>
#include <Output/Output.h>
#include <Physics/Interaction.h>
#include <Pipeline/singlesstream.h>

using namespace std;

void usage() {
    cout << "daqmodel [-vh] -f [filename] -m [map file] -p [process list] ..\n"
         << "  -o [name] : output filename, default: filename + \".dm\"\n"
         << "  -t [type] : event type, default: gray binary output 0\n"
         << endl;
}

template<class EventT>
int process_file(const std::string & filename_map,
                 const std::string & filename_process,
                 std::ifstream & input,
                 std::ofstream & output,
                 bool verbose)
{
    SinglesStream<EventT> singles_stream;
    int no_detectors = singles_stream.load_mappings(filename_map);
    if (no_detectors < 0) {
        cerr << "Loading mapping file failed" << endl;
        return(-2);
    }

    int proc_load_status = singles_stream.load_processes(filename_process);
    if (proc_load_status < 0) {
        cerr << "Loading process file failed" << endl;
        return(-2);
    }

    vector<EventT> input_events(100000);
    do {
        input.read(reinterpret_cast<char *>(input_events.data()),
                   input_events.size() * sizeof(EventT));
        size_t no_events = input.gcount() / sizeof(EventT);
        singles_stream.add_events(vector<EventT>(input_events.begin(),
                                                 input_events.begin() +
                                                 no_events));
        const vector<EventT> & events = singles_stream.get_ready();
        output.write(reinterpret_cast<const char *>(events.data()),
                     events.size() * sizeof(EventT));
        singles_stream.clear();
    } while(input);

    input.close();
    singles_stream.stop();
    const vector<EventT> & events = singles_stream.get_ready();
    output.write(reinterpret_cast<const char *>(events.data()),
                 events.size() * sizeof(EventT));
    singles_stream.clear();

    output.close();


    if (verbose) {
        cout << "______________\n DAQ Stats\n______________\n"
             << singles_stream << endl;
    }
    return(0);
}


template<>
int process_file<Interaction>(const std::string & filename_map,
                              const std::string & filename_process,
                              std::ifstream & input,
                              std::ofstream & output,
                              bool verbose)
{
    SinglesStream<Interaction> singles_stream(-1,
                                              Interaction::merge_interactions);
    int no_detectors = singles_stream.load_mappings(filename_map);
    if (no_detectors < 0) {
        cerr << "Loading mapping file failed" << endl;
        return(-2);
    }

    int proc_load_status = singles_stream.load_processes(filename_process);
    if (proc_load_status < 0) {
        cerr << "Loading process file failed" << endl;
        return(-2);
    }

    bool binary;
    int version;
    Output::read_header(input, binary, version);

    Output::WriteFlags flags;
    Output::read_write_flags(flags, input, binary);

    Output::write_header(output, binary);
    Output::write_write_flags(flags, output, binary);

    Interaction input_event;
    while (Output::read_interaction(input_event, input, flags, binary)) {
        singles_stream.add_event(input_event);
        if (singles_stream.no_ready() > 100000) {
            const vector<Interaction> & events = singles_stream.get_ready();
            for (const auto & event: events) {
                Output::write_interaction(event, output, flags, binary);
            }
            singles_stream.clear();
        }
    }

    input.close();
    singles_stream.stop();
    const vector<Interaction> & events = singles_stream.get_ready();
    for (const auto & event: events) {
        Output::write_interaction(event, output, flags, binary);
    }
    singles_stream.clear();

    output.close();


    if (verbose) {
        cout << "______________\n DAQ Stats\n______________\n"
        << singles_stream << endl;
    }
    return(0);
}


int main(int argc, char ** argv) {
    if (argc == 1) {
        usage();
        return(0);
    }

    bool verbose = false;
    string filename;
    string filename_output;
    string filename_map;
    string filename_process;
    int filetype = 0;

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
        if (argument == "-o") {
            filename_output = following_argument;
        }
        if (argument == "-m") {
            filename_map = following_argument;
        }
        if (argument == "-p") {
            filename_process = following_argument;
        }
        if (argument == "-t") {
            if ((follow_arg_ss >> filetype).fail()) {
                cerr << "Invalid filetype: " << following_argument << endl;
                return(1);
            }
        }
    }

    if (filename.empty()) {
        cerr << "Filename not specified" << endl;
        return(2);
    }
    if (filename_output == "") {
        filename_output = filename + ".dm";
    }

    if (verbose) {
        cout << "input filename   : " << filename << endl;
        cout << "output filename  : " << filename_output << endl;
        cout << "map filename     : " << filename_map << endl;
        cout << "process filename : " << filename_process << endl;
    }

    ifstream input(filename.c_str(), ios::binary);
    if (!input) {
        cerr << "Opening input failed" << endl;
        return(3);
    }

    ofstream output(filename_output.c_str(), ios::binary);
    if (!output) {
        cerr << "Opening output failed" << endl;
        return(4);
    }

    int status = 0;
    if (filetype == 0) {
        status = process_file<GrayBinaryStandard>(
                filename_map, filename_process, input, output, verbose);
    } else if (filetype == 1) {
        status = process_file<GrayBinaryNoPosition>(
                filename_map, filename_process, input, output, verbose);
    } else if (filetype == 2) {
        status = process_file<Interaction>(filename_map, filename_process,
                                           input, output, verbose);
    }
    if (status < 0) {
        return(5);
    }
    return(0);
}
