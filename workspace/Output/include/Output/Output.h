#ifndef OUTPUT_H_
#define OUTPUT_H_
#include <Physics/Interaction.h>
#include <Output/BinaryFormat.h>
#include <stdlib.h>
#include <fstream>
#include <string>

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

    Output(const std::string & logfile = "", Format fmt = FULL_ASCII);
    ~Output();
    bool SetLogfile(const std::string & name);
    void SetFormat(Format format);
    void LogInteraction(const Interaction & interact);
    static int GetFormat(const std::string & identifier, Format & fmt);

    static int header_start_magic_number;
    static int output_version_number;
    static bool write_header(std::ostream & output, bool binary);
    static bool read_header(std::istream & input, bool & binary,
                            int & version);

    struct WriteFlags {
        bool time;
        bool id;
        bool color;
        bool type;
        bool pos;
        bool energy;
        bool det_id;
        bool src_id;
        bool mat_id;
        bool scatter_compton_phantom;
        bool scatter_compton_detector;
        bool scatter_rayleigh_phantom;
        bool scatter_rayleigh_detector;
        bool xray_flouresence;
        bool sensitive_mat;

        WriteFlags() :
        time(true),
        id(true),
        color(true),
        type(true),
        pos(true),
        energy(true),
        det_id(true),
        src_id(true),
        mat_id(true),
        scatter_compton_phantom(true),
        scatter_compton_detector(true),
        scatter_rayleigh_phantom(true),
        scatter_rayleigh_detector(true),
        xray_flouresence(true),
        sensitive_mat(true)
        {}
    };
    static void write_flags_stats(const WriteFlags & flags, int & no_fields,
                                  int & no_active);
    static int event_size(const WriteFlags & flags);
    static bool write_write_flags(const WriteFlags & flags,
                                  std::ostream & output, bool binary);
    static bool read_write_flags(WriteFlags & flags,
                                 std::istream & input, bool binary);
    static bool parse_write_flags_mask(WriteFlags & flags,
                                       const std::string & mask);
    static bool write_interaction(const Interaction & inter,
                                  std::ostream & output,
                                  const WriteFlags & flags, bool binary);
    static bool read_interaction(Interaction & inter,
                                 std::istream & input,
                                 const WriteFlags & flags, bool binary);

    void SetVariableOutputMask(const WriteFlags & flags);

private:
    int MakeLogWord(Interaction::INTER_TYPE interaction, int color,
                    bool scatter, int det_mat, int src_id);

    std::ofstream log_file;
    Format format;
    WriteFlags var_format_write_flags;
};

#endif /*OUTPUT_H_*/
