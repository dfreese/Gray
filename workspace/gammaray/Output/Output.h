#ifndef OUTPUT_H_
#define OUTPUT_H_
#include <Physics/Interaction.h>
#include <Output/BinaryFormat.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

class GammaStats;
class Interaction;

class Output
{
public:
    enum Format {
        FULL_ASCII,
        FULL_BINARY,
        NO_POS_BINARY,
        VARIABLE_ASCII,
        VARIABLE_BINARY,
    };

    Output();
    Output(Output&&) = default;
    ~Output();
    bool SetLogfile(const std::string & name, bool write_header);
    void SetFormat(Format format);
    void LogInteraction(const Interaction & interact);
    void LogInteractions(const std::vector<Interaction> & interactions);
    void Close();
    void LogHits(const std::vector<Interaction>::const_iterator & begin,
                 const std::vector<Interaction>::const_iterator & end);
    void LogSingles(const std::vector<Interaction>::const_iterator & begin,
                    const std::vector<Interaction>::const_iterator & end);
    void LogCoinc(const std::vector<Interaction>::const_iterator & begin,
                  const std::vector<Interaction>::const_iterator & end,
                  bool pair_all);
    std::string GetFilename() const;

    static int ParseFormat(const std::string & identifier, Format & fmt);
    static void DisableHeader();

    static int header_start_magic_number;
    static int output_version_number;
    static bool write_header(std::ostream & output, bool binary);

    struct WriteFlags {
        bool time = true;
        bool decay_id = true;
        bool color = true;
        bool type = true;
        bool pos = true;
        bool energy = true;
        bool det_id = true;
        bool src_id = true;
        bool mat_id = true;
        bool scatter_compton_phantom = true;
        bool scatter_compton_detector = true;
        bool scatter_rayleigh_phantom = true;
        bool scatter_rayleigh_detector = true;
        bool xray_flouresence = true;
        bool coinc_id = true;
    };

    struct WriteOffsets {
        int time = -1;
        int decay_id = -1;
        int color = -1;
        int type = -1;
        int pos = -1;
        int energy = -1;
        int det_id = -1;
        int src_id = -1;
        int mat_id = -1;
        int scatter_compton_phantom = -1;
        int scatter_compton_detector = -1;
        int scatter_rayleigh_phantom = -1;
        int scatter_rayleigh_detector = -1;
        int xray_flouresence = -1;
        int coinc_id = -1;
    };

    static void write_flags_stats(const WriteFlags & flags, int & no_fields,
                                  int & no_active);
    static int event_size(const WriteFlags & flags);
    static WriteOffsets event_offsets(const WriteFlags & flags);
    static bool write_write_flags(const WriteFlags & flags,
                                  std::ostream & output, bool binary);
    static bool parse_write_flags_mask(WriteFlags & flags,
                                       const std::string & mask);
    static bool write_variable_ascii(const Interaction & inter,
                                     std::ostream & output,
                                     const WriteFlags & flags);
    static bool write_variable_binary(const Interaction & inter,
                                      std::ostream & output,
                                      const WriteFlags & flags);

    void SetVariableOutputMask(const WriteFlags & flags);

private:
    int MakeLogWord(int interaction, int color, int scatter, int det_mat,
                    int src_id);

    std::ofstream log_file;
    Format format;
    WriteFlags var_format_write_flags;
    static bool write_header_flag;
    std::string log_filename;
};

#endif /*OUTPUT_H_*/
