#include <Physics/GammaStats.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

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

GammaStats::~GammaStats()
{
}

void GammaStats::SetName(char * n)
{
    name = n;
}

void GammaStats::SetMaterialType(int s)
{
    material=s;
}

void GammaStats::SetFileName(char * n)
{
    filename = n;
    name = n;
}

void GammaStats::SetSize(int s)
{
    size = s;
    energy = new double[s];
    mu = new double[s];
    sigma = new double[s];
    tau = new double[s];
    dsdom = new double[s];

    for (int i = 0; i < s; i++) {
        energy[i] = -1.0;
        mu[i] = -1.0;
        sigma[i] = -1.0;
        tau[i] = -1.0;
        dsdom[i] = -1.0;
    }
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

bool GammaStats::Load(void)
{
    FILE* infile;
    double e, m, t, s, d;
    m = 0.0;
    char tmpfile[400];
    char * pPath;
    pPath = getenv ("GRAY_INCLUDE");
    sprintf(tmpfile,"%s/%s",pPath,filename);
    infile = fopen( tmpfile , "rb" );
    if ( !infile ) {
        fprintf(stderr, " Unable to open file: %s\nExiting..\n",
                tmpfile);
        exit(0);
        return false;
    }
    int sz = 0;
    fscanf(infile, "%d", &sz);
    int line = 0;
    if (sz > 0) {
        SetSize(sz);
        cout << "Opening file: ";
        cout << tmpfile;
        cout << " with ";
        cout << sz;
        cout << " points\n";
        for (int i = 0; i < sz; i++) {
            if (EOF != fscanf(infile, "%lf%lf%lf%lf",&e,&t,&s,&d)) {
                line++;
                energy[i] = e;
                mu[i] = s+t;
                sigma[i] = s;
                tau[i] = t;
                dsdom[i] = d;
            } else {
                cout << "Error reading file: ";
                cout << filename;
                cout << " on line: ";
                cout << line;
                cout << "\n";
                fclose(infile);
                exit(1);
            }
        }
    } else {
        fclose(infile);
        return false;
    }
    fclose(infile);
    return true;
}


double GammaStats::GetSigma(double e, int idx) const
{
    double val;

    if (idx == -1) {
        idx = GetIndex(e);
    }
    if (idx == 0) {
        return sigma[0];
    } else {
        double delta = energy[idx] - energy[idx-1];
        double alpha = (e - energy[idx-1])/delta;
        val = (1.0-alpha) * sigma[idx-1] + alpha*sigma[idx];
        return val;
    }
}

double GammaStats::GetMu(double e, int idx) const
{
    return 0.0;
}

double GammaStats::GetTau(double e,int idx) const
{
    return 0.0;
}

double GammaStats::GetDsDom(double e) const
{
    int idx = GetIndex(e);
    if (idx == 0) {
        return dsdom[0];
    } else {
        double delta = energy[idx] - energy[idx-1];
        double alpha = (e - energy[idx-1])/delta;
        if (alpha > 1.0) {
            alpha = 1.0;
        }
        return (1.0-alpha) * dsdom[idx-1]		+ alpha*dsdom[idx];
    }
}

void GammaStats::GetPE(double e, double &m, double &s) const
{
    int idx = GetIndex(e);
    if (idx == 0) {
        m = mu[0];
        s = sigma[0];
    } else {
        double delta = energy[idx] - energy[idx-1];
        double alpha = (e - energy[idx-1])/delta;
        if (alpha > 1.0) {
            alpha = 1.0;
        }
        m = (1.0-alpha) * mu[idx-1]		+ alpha*mu[idx];
        s = (1.0-alpha) * sigma[idx-1]	+ alpha*sigma[idx];
    }
}

void GammaStats::AddEscape(double * xray_escapes,double * xray_probs, double * auger_probs, int num)
{
    num_escape = num;
    xray_escape = xray_escapes;
    xray_escape_probability = xray_probs;
    auger = auger_probs;
}
