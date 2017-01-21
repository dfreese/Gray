#ifndef OUTPUT_H_
#define OUTPUT_H_
#include <Graphics/MaterialBase.h>
#include <Physics/Photon.h>
#include <Physics/NuclearDecay.h>
#include <Output/BinaryFormat.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <string>

class GammaStats;

#define ERROR_EMPTY (int)-2
#define ERROR_TRACE_DEPTH (int)-3

class Output
{
public:
    Output();
    ~Output();
    bool SetLogfile(const std::string & name);
    void LogNuclearDecay(NuclearDecay *p);
    void SetLogAll(bool val);
    void SetLogPositron(bool val);
    void LogCompton(const Photon &p, double deposit, const GammaStats & mat_gamma_prop);
    void LogPhotoElectric(const Photon &p, const GammaStats & mat_gamma_prop);
    void LogError(const Photon &p, int t, int det_mat);
    void SetBinary(bool val);
    enum BinaryOutputFormat {
        FULL_OUTPUT,
        NO_POS
    };
    void SetBinaryFormat(BinaryOutputFormat format);

private:
    int MakeLogWord(int interaction, int color,bool scatter, int det_mat, int src_id);
    void LogNuclearDecayBinary(NuclearDecay *p);
    void LogNuclearDecayASCII(NuclearDecay *p);
    void LogErrorASCII(const Photon &p, int t, int detmaterial);
    void LogErrorBinary(const Photon &p, int t, int detmaterial) ;
    void LogASCII(const Photon &p, INTER_TYPE type, double deposit, const GammaStats & mat_gamma_prop);
    void LogBinary(const Photon &p, INTER_TYPE type, double deposit, const GammaStats & mat_gamma_prop);

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
    long counter_error;

};

#endif /*OUTPUT_H_*/
