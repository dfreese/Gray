//
//  blur.h
//  graypipeline
//
//  Created by David Freese on 2/5/17.
//
//

#ifndef blur_h
#define blur_h

#include <cmath>
#include <Random/Random.h>

class Blur {
public:
    template<class T>
    static void blur_energy(T & event, double eres) {
        event.energy *= 1.0 + (eres * fwhm_to_sigma * Random::Gaussian());
    }

    template<class T>
    static void blur_time(T & event, double tres) {
        event.time += tres * fwhm_to_sigma * Random::Gaussian();
    }

    template<class T>
    static void blur_time_capped(T & event, double tres, double max_blur) {
        double time_blur;
        do {
            time_blur = tres * fwhm_to_sigma * Random::Gaussian();
        } while(std::abs(time_blur) > max_blur);
        event.time += time_blur;
    }

private:
    static const double sigma_to_fwhm;
    static const double fwhm_to_sigma;
};

const double Blur::sigma_to_fwhm = 2.0 * std::sqrt(2.0 * std::log(2.0));
const double Blur::fwhm_to_sigma = 1.0 / Blur::sigma_to_fwhm;


#endif /* blur_h */