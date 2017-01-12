#ifndef OUTPUT_H_
#define OUTPUT_H_
#include "../Graphics/MaterialBase.h"
#include "../Physics/Photon.h"
#include "../Physics/NuclearDecay.h"
#include "DetectorArray.h"
#include "BinaryFormat.h"
#include "EventBuffer.h"
#include "Coincidence.h"
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <string>

#ifdef ROOT_OUTPUT
// If root is available on the machine, we can use it in the includes here
#endif

#define ERROR_EMPTY (int)-2
#define ERROR_TRACE_DEPTH (int)-3

class Output
{
public:
    Output();
    ~Output();
    bool SetLogfile(char * name);
    void LogNuclearDecay(NuclearDecay *p);
    void SetLogAll(bool val);
    void LogCompton(const Photon &p, double deposit, const MaterialBase & mat);
    void LogPhotoElectric(const Photon &p, const MaterialBase & mat);
    void LogError(const Photon &p, int t, int det_mat);
    void SetBinary(bool val);
    void SetBinning(bool val);
    void SetLogDetId(bool val);
    void SetLogDetCoord(bool val);
    void LogNEC(const Photon &p, double deposit, const MaterialBase & mat);

    // helper class for reading binary data
    void read(const std::string& file_name, GRAY_BINARY &data);
    EventBuffer eb;
    DetectorArray d;
    Coincidence c;

private:
    int MakeLogWord(int interaction, int color,bool scatter, int det_mat, int src_id);
    void LogNuclearDecayBinary(NuclearDecay *p);
    void LogNuclearDecayASCII(NuclearDecay *p);
    //	void LogComptonBinary(const Photon &p, double deposit, const MaterialBase & mat);
    //	void LogComptonASCII(const Photon &p, double deposit, const MaterialBase & mat);
    //	void LogPhotoElectricASCII(const Photon &p, const MaterialBase & mat);
    //	void LogPhotoElectricBinary(const Photon &p, const MaterialBase & mat);
    void LogErrorASCII(const Photon &p, int t, int detmaterial);
    void LogErrorBinary(const Photon &p, int t, int detmaterial) ;
    void LogASCII(const Photon &p, INTER_TYPE type, double deposit, const MaterialBase & mat);
    void LogBinary(const Photon &p, INTER_TYPE type, double deposit, const MaterialBase & mat);
#ifdef ROOT_OUTPUT
    void LogNuclearDecayRoot(NuclearDecay *p);
    void LogComptonRoot(const Photon &p, double deposit, const MaterialBase & mat);
    void LogPhotoRoot(const Photon &p, const MaterialBase & mat);
#endif

    // TODO: Binning not implemented
    void BinEvent(GRAY_BINARY &data, const Photon &p, const Detector &d);
    ofstream log_file;
    bool log_data;
    bool log_positron;
    bool log_all;
    bool log_event;
    bool log_det_id;
    bool log_det_coord;
    bool binary;
    bool binning;

    void write(GRAY_BINARY& data);

};

#endif /*OUTPUT_H_*/

