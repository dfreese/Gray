/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef SCENE_DESCRIPTION_H
#define SCENE_DESCRIPTION_H

#include <map>
#include <memory>
#include <vector>
#include "Gray/VrMath/LinearR3.h"
#include "Gray/Graphics/CameraView.h"
#include "Gray/Graphics/Light.h"
#include "Gray/Graphics/Material.h"
#include "Gray/Graphics/ViewableBase.h"
#include "Gray/KdTree/KdTree.h"

class SceneDescription
{

public:
    SceneDescription() = default;

    void SetBackGroundColor( float* color )
    {
        TheBackgroundColor.Load( color );
    }
    void SetBackGroundColor( double* color )
    {
        TheBackgroundColor.Load( color );
    }
    void SetBackGroundColor( const VectorR3& color )
    {
        TheBackgroundColor = color;
    }
    void SetBackGroundColor( double r, double g, double b )
    {
        TheBackgroundColor.Set(r, g, b);
    }
    VectorR3& BackgroundColor()
    {
        return TheBackgroundColor;
    }
    const VectorR3& BackgroundColor() const
    {
        return TheBackgroundColor;
    }

    void SetGlobalAmbientLight( float* color )
    {
        TheGlobalAmbientLight.Load( color );
    }
    void SetGlobalAmbientLight( double* color )
    {
        TheGlobalAmbientLight.Load( color );
    }
    void SetGlobalAmbientLight( const VectorR3& color )
    {
        TheGlobalAmbientLight = color;
    }
    void SetGlobalAmbientLight( double r, double g, double b )
    {
        TheGlobalAmbientLight.Set(r, g, b);
    }
    VectorR3& GlobalAmbientLight()
    {
        return TheGlobalAmbientLight;
    }
    const VectorR3& GlobalAmbientLight() const
    {
        return TheGlobalAmbientLight;
    }

    CameraView& GetCameraView()
    {
        return CameraAndViewer;
    }
    const CameraView& GetCameraView() const
    {
        return CameraAndViewer;
    }

    // Once you have set up an initial CameraView, you can call RegisterCameraView.
    //  After that, you may call CalcNewSceenDims( aspectRatio ) to get
    //	a suggested width and height for the camera screen.
    void RegisterCameraView();
    void CalcNewScreenDims( float aspectRatio );

    size_t NumLights() const
    {
        return LightArray.size();
    }
    void AddLight( std::unique_ptr<Light> newLight );
    Light& GetLight(size_t i)
    {
        return *LightArray[i];
    }
    const Light& GetLight(size_t i) const
    {
        return *LightArray[i];
    }

    size_t NumMaterials() const
    {
        return MaterialArray.size();
    }
    void AddMaterial(std::unique_ptr<Material> newMaterial);
    Material& GetMaterial( int i )
    {
        return *MaterialArray[i];
    }
    const Material& GetMaterial( int i ) const
    {
        return *MaterialArray[i];
    }
    bool HasMaterial(const std::string & name) {
        return (material_names_map.count(name));
    }
    Material& GetMaterial(const std::string & name)
    {
        int idx = GetMaterialIndex(name);
        return *MaterialArray.at(idx);
    }
    const Material& GetMaterial(const std::string & name) const
    {
        int idx = GetMaterialIndex(name);
        return *MaterialArray.at(idx);
    }
    void SetDefaultMaterial(const std::string & name) {
        default_material = name;
    }
    Material& GetDefaultMaterial() {
        return (GetMaterial(default_material));
    }
    const Material& GetDefaultMaterial() const {
        return (GetMaterial(default_material));
    }
    size_t NumViewables() const
    {
        return ViewableArray.size();
    }
    void AddViewable(std::unique_ptr<ViewableBase> newViewable);
    ViewableBase& GetViewable(size_t i )
    {
        return *ViewableArray[i];
    }
    const ViewableBase& GetViewable(size_t i ) const
    {
        return *ViewableArray[i];
    }

    AABB GetExtents() const;
    double GetMaxDistance() const;

    void BuildTree(bool use_double_recurse_split, double object_cost);

    long SeekIntersection(const VectorR3& pos, const VectorR3& direction,
                          double & hitDist, VisiblePoint& returnedPoint) const;
    bool TestOverlap() const;
    static constexpr double ray_trace_epsilon = 1e-10;

private:

    int GetMaterialIndex(const std::string & name) const {
        auto iter = material_names_map.find(name);
        if (iter == material_names_map.end()) {
            return (-1);
        } else {
            return((*iter).second);
        }
    }
    bool TestOverlapSingle(VectorR3 & start, const VectorR3 & dir) const;
    bool intersection_callback(long objectNum, const VectorR3 & start_pos,
                               const VectorR3 & direction,
                               double & retStopDistance,
                               VisiblePoint & visible_point_return_ptr) const;
    VectorR3 TheGlobalAmbientLight = VectorR3(0, 0, 0);
    VectorR3 TheBackgroundColor = VectorR3(0, 0, 0);

    CameraView CameraAndViewer;
    double RegisteredScreenWidth;
    double RegisteredScreenHeight;
    bool ScreenRegistered = false;

    std::vector<std::unique_ptr<Light>> LightArray;
    std::vector<std::unique_ptr<Material>> MaterialArray;
    std::vector<std::unique_ptr<ViewableBase>> ViewableArray;
    std::map<std::string, int> material_names_map;
    KdTree kd_tree;
    std::string default_material;
};

#endif // SCENE_DESCRIPTION_H
