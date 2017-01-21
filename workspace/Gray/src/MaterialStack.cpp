#include <Gray/MaterialStack.h>

MaterialStack::MaterialStack()
{
    defaultMaterial = NULL;
}

void MaterialStack::PushMaterial(GammaMaterial *mat)
{
    MatStack.Push(mat);
}

GammaMaterial* MaterialStack::PopMaterial()
{
    if (NumMaterials() <= 0) {
        cerr << "Poping empty\n";
        return NULL;
    }
    GammaMaterial * ret = MatStack.Pop();
    return ret;
}

GammaMaterial * MaterialStack::curMaterial()
{
    if (NumMaterials() <= 0) {
        return NULL;
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

int MaterialStack::NumMaterials() const
{
    return MatStack.Size();
}
