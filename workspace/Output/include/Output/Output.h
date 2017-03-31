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
    Output();
    ~Output();
    bool SetLogfile(const std::string & name);
    void SetLogAll(bool val);
    void SetLogPositron(bool val);
    void SetBinary(bool val);
    enum BinaryOutputFormat {
        FULL_OUTPUT,
        NO_POS
    };
    void SetBinaryFormat(BinaryOutputFormat format);

    void LogInteraction(const Interaction & interact);
    bool GetLogPositron() const;

private:
    int MakeLogWord(Interaction::INTER_TYPE interaction, int color,
                    bool scatter, int det_mat, int src_id);

    std::ofstream log_file;
    bool log_data;
    bool log_positron;
    bool log_all;
    bool binary_output;

    BinaryOutputFormat binary_format;
    
    long counter_nuclear_decay;
    long counter_photoelectric;
    long counter_compton;
    long counter_rayleigh;
    long counter_error;

};

#endif /*OUTPUT_H_*/
