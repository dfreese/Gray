#ifndef OUTPUT_H_
#define OUTPUT_H_
#include "../Graphics/MaterialBase.h"
#include "../Physics/Photon.h"
#include "../Physics/PositronDecay.h"
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <iostream>
#include <fstream>

class Output
{
public:
    Output();
    ~Output();
    bool SetLogfile(char * name);
    void LogPositron(PositronDecay &p);
    void SetLogAll(bool val);
    void LogCompton(Photon &p, double deposit, const MaterialBase & mat);
    void LogPhotoElectric(Photon &p, double deposit, const MaterialBase & mat);
private:
    ofstream log_file;
    bool log_data;
    bool log_positron;
    bool log_all;
    bool log_event;
};

#endif /*OUTPUT_H_*/

