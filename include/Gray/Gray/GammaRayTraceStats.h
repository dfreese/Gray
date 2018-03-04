#ifndef GAMMARAYTRACESTATS_H
#define GAMMARAYTRACESTATS_H

struct GammaRayTraceStats {
    long decays = 0;
    long photons = 0;
    long no_interaction = 0;
    long photoelectric = 0;
    long xray_escape = 0;
    long compton = 0;
    long rayleigh = 0;
    long photoelectric_sensitive = 0;
    long xray_escape_sensitive = 0;
    long compton_sensitive = 0;
    long rayleigh_sensitive = 0;
    long error = 0;

    GammaRayTraceStats& operator+=(const GammaRayTraceStats& rhs) {
        decays += rhs.decays;
        photons += rhs.photons;
        no_interaction += rhs.no_interaction;
        photoelectric += rhs.photoelectric;
        xray_escape += rhs.xray_escape;
        compton += rhs.compton;
        rayleigh += rhs.rayleigh;
        photoelectric_sensitive += rhs.photoelectric_sensitive;
        xray_escape_sensitive += rhs.xray_escape_sensitive;
        compton_sensitive += rhs.compton_sensitive;
        rayleigh_sensitive += rhs.rayleigh_sensitive;
        error += rhs.error;
        return (*this);
    }

    GammaRayTraceStats operator+(const GammaRayTraceStats& rhs) {
        GammaRayTraceStats result(*this);
        result += rhs;
        return (result);
    }

    friend std::ostream & operator<< (std::ostream & os,
                                      const GammaRayTraceStats& s)
    {
        os << "decays: " << s.decays << "\n"
           << "photons: " << s.photons << "\n"
           << "no_interaction: " << s.no_interaction << "\n"
           << "photoelectric: " << s.photoelectric << "\n"
           << "xray_escape: " << s.xray_escape << "\n"
           << "compton: " << s.compton << "\n"
           << "rayleigh: " << s.rayleigh << "\n"
           << "photoelectric_sensitive: " << s.photoelectric_sensitive << "\n"
           << "xray_escape_sensitive: " << s.xray_escape_sensitive << "\n"
           << "compton_sensitive: " << s.compton_sensitive << "\n"
           << "rayleigh_sensitive: " << s.rayleigh_sensitive << "\n"
           << "error: " << s.error << "\n";
        return os;
    }
};

#endif // GAMMARAYTRACESTATS_H
