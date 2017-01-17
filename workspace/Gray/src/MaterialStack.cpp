#include <Gray/MaterialStack.h>

MaterialStack::MaterialStack()
{
    defaultMaterial = NULL;
    errorMaterial = ERROR_MATERIAL;
}

void MaterialStack::PushMaterial(GammaMaterial *mat)
{
    MatStack.Push(mat);
}

GammaMaterial* MaterialStack::PopMaterial()
{
    if (NumMaterials() <= 0) {
        cerr << "Poping empty\n";
        return errorMaterial;
    }
    GammaMaterial * ret = MatStack.Pop();
    return ret;
}

GammaMaterial * MaterialStack::curMaterial()
{
    if (NumMaterials() <= 0) {
        return errorMaterial;
    }
    return MatStack.Top();
}

void MaterialStack::ResetMaterial()
{
    MatStack.Reset();
    MatStack.Push(defaultMaterial);
}
void MaterialStack::SetDefault(GammaMaterial * mat)
{
    defaultMaterial = mat;
}
