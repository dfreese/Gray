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
    Output(const std::string & logfile = "");
    ~Output();
    bool SetLogfile(const std::string & name);
    static void SetLogAll(bool val);
    static void SetLogPositron(bool val);
    static void SetBinary(bool val);
    enum BinaryOutputFormat {
        FULL_OUTPUT,
        NO_POS
    };
    static void SetBinaryFormat(BinaryOutputFormat format);

    void LogInteraction(const Interaction & interact);
    bool GetLogPositron() const;

private:
    int MakeLogWord(Interaction::INTER_TYPE interaction, int color,
                    bool scatter, int det_mat, int src_id);

    std::ofstream log_file;
    static bool log_data;
    static bool log_positron;
    static bool log_all;
    static bool binary_output;

    static BinaryOutputFormat binary_format;
};

#endif /*OUTPUT_H_*/
