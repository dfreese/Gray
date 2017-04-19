#include <Physics/GammaStats.h>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

GammaStats::GammaStats()
{
    size = 0;
    cache_idx = -1;
    cache_energy = -1.0;
    log_material = false;
    enable_interactions = true;
    num_escape = 0;
    material = -1;
}

void GammaStats::SetName(const std::string & n)
{
    name = n;
}

void GammaStats::SetMaterialType(int s)
{
    material=s;
}

void GammaStats::SetFileName(const std::string & n)
{
    filename = n;
    name = n;
}

int GammaStats::GetMaterial() const
{
    return material;
}

std::string GammaStats::GetName() const
{
    return name;
}

void GammaStats::DisableInteractions()
{
    enable_interactions = false;
}

int GammaStats::GetNumEscape() const
{
    return num_escape;
}

double * GammaStats::GetXrayEscape() const
{
    return xray_escape;
}

double * GammaStats::GetXrayEscapeProb() const
{
    return xray_escape_probability;
}

double GammaStats::GetAugerProb(int i) const
{
    return auger[i];
}

void GammaStats::SetSize(int s)
{
    size = s;
    energy.clear();
    energy.resize(s, -1);
    mu.clear();
    mu.resize(s, -1);
    photoelectric.clear();
    photoelectric.resize(s, -1);
    compton.clear();
    compton.resize(s, -1);
    rayleigh.clear();
    rayleigh.resize(s, -1);
}

int GammaStats::search(double e, int b_idx, int s_idx) const
{
    if (b_idx == s_idx) {
        return b_idx;
    }
    int idx = (int)(((b_idx) + (s_idx))/2);
    if (energy[idx] < e) {
        return search(e,idx+1,s_idx);
    } else {
        return search(e,b_idx,idx);
    }
}

int GammaStats::GetIndex(double e) const
{

    if (cache_energy == e) {
        return cache_idx;
    }

    // binary search the sorted list of energies for the index
    // And override the const of the function to store the cache of the value.
    const_cast<GammaStats*>(this)->cache_idx = search(e, 0, size-1);
    const_cast<GammaStats*>(this)->cache_energy = e;
    return cache_idx;
}

bool GammaStats::Load()
{
    ifstream infile(filename);
    if (!infile) {
        cerr << " Unable to open file: " << filename << endl;
        return false;
    }
    string line;
    getline(infile, line);
    stringstream line_stream(line);
    int no_points;
    if ((line_stream >> no_points).fail()) {
        return(false);
    }
    if (0 >= no_points) {
        return(false);
    }

    SetSize(no_points);
    for (int i = 0; i < no_points; i++) {
        string pt_line;
        getline(infile, pt_line);
        stringstream pt_stream(pt_line);
        bool line_fail = false;
        line_fail |= (pt_stream >> energy[i]).fail();
        line_fail |= (pt_stream >> photoelectric[i]).fail();
        line_fail |= (pt_stream >> compton[i]).fail();
        line_fail |= (pt_stream >> rayleigh[i]).fail();

        if (line_fail) {
            cerr << "Error reading file: " << filename << " on line: "
                 << (i + 1) << endl;
            return(false);
        }
        mu[i] = compton[i] + photoelectric[i] + rayleigh[i];
    }
    return(true);
}

double GammaStats::GetMu(double e) const {
    int idx = GetIndex(e);
    if (idx == 0) {
        return(mu[0]);
    } else {
        double delta = energy[idx] - energy[idx-1];
        double alpha = (e - energy[idx-1])/delta;
        if (alpha > 1.0) {
            alpha = 1.0;
        }
        return((1.0 - alpha) * mu[idx - 1] + alpha * mu[idx]);
    }
}

void GammaStats::GetInteractionProbs(double e, double & pe, double & comp,
                                     double & ray) const
{
    int idx = GetIndex(e);
    if (idx == 0) {
        pe = photoelectric[0];
        comp = compton[0];
        ray = rayleigh[0];
    } else {
        double delta = energy[idx] - energy[idx-1];
        double alpha = (e - energy[idx-1])/delta;
        if (alpha > 1.0) {
            alpha = 1.0;
        }
        pe = (1.0 - alpha) * photoelectric[idx - 1] + alpha * photoelectric[idx];
        comp = (1.0 - alpha) * compton[idx - 1] + alpha * compton[idx];
        ray = (1.0 - alpha) * rayleigh[idx - 1] + alpha * rayleigh[idx];
    }
}

void GammaStats::AddEscape(double * xray_escapes,double * xray_probs, double * auger_probs, int num)
{
    num_escape = num;
    xray_escape = xray_escapes;
    xray_escape_probability = xray_probs;
    auger = auger_probs;
}
