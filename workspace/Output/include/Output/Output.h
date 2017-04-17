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
        NO_POS_BINARY
    };

    Output(const std::string & logfile = "", Format fmt = FULL_ASCII);
    ~Output();
    bool SetLogfile(const std::string & name);
    static void SetLogAll(bool val);
    static void SetLogPositron(bool val);
    static void SetBinary(bool val);
    void SetFormat(Format format);
    void LogInteraction(const Interaction & interact);
    bool GetLogPositron() const;
    static int GetFormat(const std::string & identifier, Format & fmt);

private:
    int MakeLogWord(Interaction::INTER_TYPE interaction, int color,
                    bool scatter, int det_mat, int src_id);

    std::ofstream log_file;
    static bool log_data;
    static bool log_positron;
    static bool log_all;

    Format format;
};

#endif /*OUTPUT_H_*/
