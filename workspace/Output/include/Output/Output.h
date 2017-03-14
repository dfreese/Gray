#ifndef OUTPUT_H_
#define OUTPUT_H_
#include <Graphics/MaterialBase.h>
#include <Physics/Photon.h>
#include <Physics/Interaction.h>
#include <Physics/NuclearDecay.h>
#include <Output/BinaryFormat.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <iostream>
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
    void LogNuclearDecay(NuclearDecay *p);
    void SetLogAll(bool val);
    void SetLogPositron(bool val);
    void LogInteraction(const Interaction & interact);
    void LogRayleigh(const Photon &p, const GammaStats & mat_gamma_prop);
    void LogCompton(const Photon &p, double deposit, const GammaStats & mat_gamma_prop);
    void LogPhotoElectric(const Photon &p, const GammaStats & mat_gamma_prop);
    void SetBinary(bool val);
    enum BinaryOutputFormat {
        FULL_OUTPUT,
        NO_POS
    };
    enum ErrorType {
        ERROR_EMPTY = -2,
        ERROR_TRACE_DEPTH = -3
    };
    void LogError(const Photon &p, ErrorType t, int det_mat);
    void SetBinaryFormat(BinaryOutputFormat format);

private:
    int MakeLogWord(int interaction, int color,bool scatter, int det_mat, int src_id);
    int MakeLogWord(Interaction::INTER_TYPE interaction, int color,
                    bool scatter, int det_mat, int src_id);
    void LogNuclearDecayBinary(NuclearDecay *p);
    void LogNuclearDecayASCII(NuclearDecay *p);
    void LogErrorASCII(const Photon &p, ErrorType t, int detmaterial);
    void LogErrorBinary(const Photon &p, ErrorType t, int detmaterial) ;
    void LogASCII(const Photon &p,
                  Interaction::INTER_TYPE type,
                  double deposit,
                  int material_id);
    void LogBinary(const Photon &p,
                   Interaction::INTER_TYPE type,
                   double deposit,
                   int material_id);

    ofstream log_file;
    bool log_data;
    bool log_positron;
    bool log_all;
    bool binary_output;

    void write(GRAY_BINARY& data);
    void write(BinaryDetectorOutput & data);
    
    BinaryOutputFormat binary_format;
    
    long counter_nuclear_decay;
    long counter_photoelectric;
    long counter_compton;
    long counter_rayleigh;
    long counter_error;

};

#endif /*OUTPUT_H_*/
