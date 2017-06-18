#include <Gray/Mpi.h>
#include <Gray/Config.h>
#include <Random/Random.h>
#include <Output/Output.h>
#include <Sources/SourceList.h>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

#ifdef MPI_COMPILE_ENABLED /////////////////////////////////////////////////////
#include <mpi.h>
void Mpi::Init(int argc, char **argv) {
    enabled = false;
    if (requested) {
        enabled = true;
    }
    if (Enabled()) {
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
    }
}

void Mpi::Barrier() {
    if (Enabled()) {
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

void Mpi::Finalize() {
    if (Enabled()) {
        MPI_Finalize();
    }
}

#else //////////////////////////////////////////////////////////////////////////
void Mpi::Init(int argc, char **argv) {
    enabled = false;
    rank = 0;
    size = 1;
}

void Mpi::Barrier() {
}

void Mpi::Finalize() {
}

#endif //MPI_COMPILE_ENABLED ///////////////////////////////////////////////////

bool Mpi::requested;
bool Mpi::enabled;
int Mpi::rank;
int Mpi::size;

void Mpi::Request() {
    requested = true;
}

bool Mpi::Enabled() {
    return (enabled);
}

int Mpi::Rank() {
    return (rank);
}

int Mpi::Size() {
    return (size);
}

void Mpi::ReSeed() {
    if (Enabled()) {
        Random::RankReseed(rank);
    }
}

void Mpi::ConfigOutputFileHeaders() {
    if (Enabled()) {
        if (rank > 0) {
            Output::DisableHeader();
        }
    }
}

std::string Mpi::OutputAppend(int mpi_rank) {
    if (!Enabled()) {
        return("");
    }
    std::stringstream mpi_append_ss;
    mpi_append_ss << ".mpi_rank_" << mpi_rank;
    std::string mpi_output_append = mpi_append_ss.str();
    return(mpi_output_append);
}

std::string Mpi::OutputAppend() {
    return(OutputAppend(rank));
}

void Mpi::AdjustSimTime(SourceList & sources) {
    if (Enabled()) {
        sources.AdjustTimeForSplit(rank, size);
    }
}


void Mpi::CombineFiles(const Config & config, Output & output_hits,
                       Output & output_singles,
                       std::vector<Output> & outputs_coinc)
{
    if (!Enabled()) {
        return;
    }
    Barrier();
    if (rank == 0) {
        ofstream hits;
        ofstream singles;
        // Unclear why g++ was complaining about emplace_back with ofstream,
        // but this works...
        vector<unique_ptr<ofstream>> coincs;
        if (config.get_log_hits()) {
            hits.open(config.get_filename_hits());
        }
        if (config.get_log_singles()) {
            singles.open(config.get_filename_singles());
        }
        if (config.get_log_coinc()) {
            for (string name: config.get_filenames_coinc()) {
                coincs.emplace_back(new ofstream(name));
            }
        }
        for (int idx = 0; idx < size; idx++) {
            const string mpi_output_append = OutputAppend(idx);
            if (config.get_log_hits()) {
                string hits_seg_name = config.get_filename_hits() + mpi_output_append;
                ifstream hits_seg(hits_seg_name);
                hits << hits_seg.rdbuf();
                hits_seg.close();
                remove(hits_seg_name.c_str());
            }
            if (config.get_log_singles()) {
                string singles_seg_name = config.get_filename_singles() + mpi_output_append;
                ifstream singles_seg(singles_seg_name);
                singles << singles_seg.rdbuf();
                singles_seg.close();
                remove(singles_seg_name.c_str());
            }
            if (config.get_log_coinc()) {
                for (string name: config.get_filenames_coinc()) {
                    string coinc_seg_name = name + mpi_output_append;
                    ifstream coinc_seg(coinc_seg_name);
                    (*coincs[idx]) << coinc_seg.rdbuf();
                    coinc_seg.close();
                    remove(coinc_seg_name.c_str());
                }
            }
        }
    }
}
