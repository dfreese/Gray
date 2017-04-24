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
    void SetFormat(Format format);
    void LogInteraction(const Interaction & interact);
    static int GetFormat(const std::string & identifier, Format & fmt);

private:
    int MakeLogWord(Interaction::INTER_TYPE interaction, int color,
                    bool scatter, int det_mat, int src_id);

    std::ofstream log_file;
    Format format;
};

#endif /*OUTPUT_H_*/
