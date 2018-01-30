/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <Graphics/Light.h>
#include <Graphics/Material.h>

// This Material works with the Phong illumination model.

const Material Material::Default;

Material::Material(
        int id,
        const std::string & name,
        bool sensitive,
        bool interactive) :
    id(id),
    name(name),
    sensitive(sensitive),
    interactive(interactive)
{
}

bool Material::IsReflective() const {
    return ReflectiveFlag;
}

bool Material::IsTransmissive() const {
    return TransmissiveFlag;
}

bool Material::CalcRefractDir(
        const VectorR3& normal,
        const VectorR3& indir,
        VectorR3& outdir) const
{
    return Material::CalcRefractDir(
            IndexOfRefraction, IndexOfRefractionInv,
            normal, indir, outdir);
}

VectorR3 Material::GetReflectionColor(
        const VisiblePoint& visPoint,
        const VectorR3& outDir,
        const VectorR3& fromDir) const
{
    return ColorReflective;
}

VectorR3 Material::GetTransmissionColor(
        const VisiblePoint& visPoint,
        const VectorR3& outDir,
        const VectorR3& fromDir) const
{
    return ColorTransmissive;
}

void Material::SetColorAmbient(const VectorR3& color) {
    ColorAmbient = color;
}

void Material::SetColorDiffuse(const VectorR3& color) {
    ColorDiffuse = color;
}

void Material::SetColorAmbientDiffuse(const VectorR3& color) {
    ColorAmbient = color;
    ColorDiffuse = color;
}

void Material::SetColorSpecular(const VectorR3& color) {
    ColorSpecular = color;
}

void Material::SetColorEmissive(const VectorR3& color) {
    ColorEmissive = color;
}

void Material::SetColorTransmissive(const VectorR3& color) {
    TransmissiveFlag = (color.NormSq() == 0.0);
    ColorTransmissive = color;
}

void Material::SetColorReflective(const VectorR3& color) {
    ReflectiveFlag = (color.NormSq() != 0.0);
    ColorReflective = color;
}


const VectorR3& Material::GetColorAmbient() const {
    return ColorAmbient;
}

const VectorR3& Material::GetColorDiffuse() const {
    return ColorDiffuse;
}

const VectorR3& Material::GetColorSpecular() const {
    return ColorSpecular;
}

const VectorR3& Material::GetColorEmissive() const {
    return ColorEmissive;
}

const VectorR3& Material::GetColorTransmissive() const {
    return ColorTransmissive;
}

const VectorR3& Material::GetColorReflective() const {
    return ColorReflective;
}

void Material::SetShininess(double exponent) {
    Shininess = exponent;
}

double Material::GetPhongShininess() const {
    return Shininess;
}

double Material::GetShininess()  const {
    return Shininess;
}

int Material::GetId() const {
    return id;
}

std::string Material::GetName() const {
    return name;
}

void Material::SetName(const std::string & mat_name) {
    name = mat_name;
}

void Material::SetIndexOfRefraction(double x) {
    IndexOfRefraction = x;
    IndexOfRefractionInv = 1.0/x;
}

double Material::GetIndexOfRefraction() const {
    return IndexOfRefraction;
}

bool Material::IsSensitive() const {
    return (sensitive);
}

bool Material::InteractionsEnabled() const {
    return (interactive);
}

// General purpose calculation of refraction direction.
// Return false if "total internal reflection".
bool Material::CalcRefractDir(
        double indexOfRefraction, double indexOfRefractionInv,
        const VectorR3& normal, const VectorR3& indir, VectorR3& outdir)
{
    if ( indexOfRefraction==1.0 ) {
        outdir = indir;
        return true;
    }
    double ip = normal^indir;	// cosine of incident angle
    bool fromoutside = ( ip<0.0 );
    double etaInv;		// index of refraction from indir to outdir
    etaInv = fromoutside ? indexOfRefractionInv : indexOfRefraction;
    VectorR3 Tlat = normal;
    Tlat *= -ip;
    Tlat += indir;				// Lateral part of indir (parallel to surface)

    Tlat *= etaInv;				// Now lateral part of outdir

    // TsinSq = sine(transmission angle) squared
    double TsinSq = Tlat.NormSq();
    if (TsinSq >= 1.0 ) {
        return false;		// No transmission
    }
    outdir = normal;
    if ( fromoutside ) {
        outdir.Negate();
    }
    outdir *= sqrt(1.0-TsinSq);	// mult by Cosine(transmission angle)
    outdir += Tlat;
    outdir.ReNormalize();
    assert(std::abs(outdir.NormSq()-1.0) < 0.000001);
    return true;
}

void Material::CalcLocalLighting(
    VectorR3& colorReturned, const Light& light,
    const VectorR3& percentLit, double lightAttenuation,
    const VectorR3& N, const VectorR3& V,
    const VectorR3& L, const VectorR3* H ) const
{
    VectorR3 LightValue;	// Used to hold light level components

    if ( percentLit.NearZero(1.0e-6) ) {
        colorReturned.SetZero();	// Light entirely hidden
    } else {
        bool facingViewer = ( (N^V)>=0.0 );
        bool facingLight = ( (N^L)>=0.0 );
        if ( (facingLight^facingViewer) && !this->IsTransmissive() ) {
            // Light and viewer on opposite sides and no transmission.
            colorReturned.SetZero();
        } else {
            VectorR3 facingNormal(N);	// The normal facing the light
            if (!facingLight) {
                facingNormal.Negate();
            }

            // Diffuse light
            colorReturned = this->GetColorDiffuse();
            colorReturned.ArrayProd(light.GetColorDiffuse());
            colorReturned *= (L^facingNormal);

            // Specular light
            LightValue = this->GetColorSpecular();
            LightValue.ArrayProd(light.GetColorSpecular());
            double specularFactor;
            if ( !(facingLight^facingViewer) ) {	// If view and light on same side
                if ( H ) {
                    specularFactor = ((*H)^facingNormal);
                } else {
                    // R = -(L-(L^N)N) + (L^N)N = 2(L^N)N - L  // reflected direction
                    // R^V = 2(L^N)(V^N) - V^L;
                    specularFactor = 2.0*(L^N)*(V^N) - (V^L);	// R^V.
                }
            } else { // If viewer and light on opposite sides
                VectorR3 T;		// Transmission direction
                facingNormal = V;
                facingNormal.Negate();		// Use temporarily as incoming vector
                this->CalcRefractDir(N, facingNormal, T);
                specularFactor = (T^L);
            }
            if ( specularFactor>0.0 ) {
                if ( this->GetShininess() != 0.0) {
                    specularFactor = pow(specularFactor,this->GetShininess());
                }
                LightValue *= specularFactor;
                colorReturned += LightValue;
            }

            // Non-ambient light reduced by percentLit
            colorReturned.ArrayProd(percentLit);

            if ( (facingLight^facingViewer) ) {		// ^ is "exclusive or"
                // If on opposite sides
                colorReturned.ArrayProd(this->GetColorTransmissive());
            }
        }
    }

    // Ambient light
    LightValue = this->GetColorAmbient();
    LightValue.ArrayProd(light.GetColorAmbient());
    colorReturned += LightValue;

    // Scale by attenuation (the ambient part too)
    colorReturned *= lightAttenuation;
}
