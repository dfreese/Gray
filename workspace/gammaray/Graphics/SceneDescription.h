#ifndef SCENE_DESCRIPTION_H
#define SCENE_DESCRIPTION_H

#include <map>
#include <vector>
#include <VrMath/LinearR3.h>
#include <Graphics/CameraView.h>
#include <Graphics/Light.h>
#include <Graphics/Material.h>
#include <Graphics/ViewableBase.h>

class SceneDescription
{

public:

    SceneDescription();
    ~SceneDescription();

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
    Light* NewLight()
    {
        Light * light = new Light();
        LightArray.push_back(light);
        return(light);
    }
    int AddLight( Light* newLight );
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
    int AddMaterial( MaterialBase * newMaterial );
    MaterialBase& GetMaterial( int i )
    {
        return *MaterialArray[i];
    }
    const MaterialBase& GetMaterial( int i ) const
    {
        return *MaterialArray[i];
    }

    size_t NumViewables() const
    {
        return ViewableArray.size();
    }
    int AddViewable( ViewableBase* newViewable );
    ViewableBase& GetViewable(size_t i )
    {
        return *ViewableArray[i];
    }
    const ViewableBase& GetViewable(size_t i ) const
    {
        return *ViewableArray[i];
    }
    const std::vector<ViewableBase*> & GetViewableArray() const {
        return(ViewableArray);
    }
    int GetMaterialIndex(const std::string & name) {
        if (material_names_map.count(name)) {
            return(material_names_map[name]);
        } else {
            return(-1);
        }
    }

    AABB GetExtents() const;
    double GetMaxDistance() const;

    void DeleteAllLights();
    void DeleteAllMaterials();
    void DeleteAllViewables();
    void DeleteAll();

private:

    VectorR3 TheGlobalAmbientLight;
    VectorR3 TheBackgroundColor;

    CameraView CameraAndViewer;
    double RegisteredScreenWidth;
    double RegisteredScreenHeight;
    bool ScreenRegistered;

    std::vector<Light*> LightArray;
    std::vector<MaterialBase*> MaterialArray;
    std::vector<ViewableBase*> ViewableArray;
    std::map<std::string, int> material_names_map;
};

#endif // SCENE_DESCRIPTION_H
