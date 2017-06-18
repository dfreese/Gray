#ifndef MPI_H_
#define MPI_H_

#include <string>
#include <vector>

class SourceList;
class Config;
class Output;

class Mpi {
public:
    static void Request();
    static void Init(int argc, char **argv);
    static void Barrier();
    static void Finalize();
    static bool Enabled();
    static int Rank();
    static int Size();
    static void ReSeed();
    static void ConfigOutputFileHeaders();
    static std::string OutputAppend();
    static void AdjustSimTime(SourceList & sources);
    static void CombineFiles(const Config & config, Output & output_hits,
                             Output & output_singles,
                             std::vector<Output> & outputs_coinc);

private:
    static bool requested;
    static bool enabled;
    static int rank;
    static int size;
    static std::string OutputAppend(int mpi_rank);
};

#endif //MPI_H_
