#include <Gray/MaterialStack.h>

MaterialStack::MaterialStack()
{
    defaultMaterial = NULL;
}

void MaterialStack::PushMaterial(GammaMaterial *mat)
{
    MatStack.push(mat);
}

GammaMaterial* MaterialStack::PopMaterial()
{
    if (NumMaterials() <= 0) {
        cerr << "Poping empty\n";
        return NULL;
    }
    GammaMaterial * ret = MatStack.top();
    MatStack.pop();
    return ret;
}

GammaMaterial * MaterialStack::curMaterial()
{
    if (NumMaterials() <= 0) {
        return NULL;
    }
    return MatStack.top();
}

void MaterialStack::ResetMaterial()
{
    while (!MatStack.empty()) {
        MatStack.pop();
    }
    MatStack.push(defaultMaterial);
}
void MaterialStack::SetDefault(GammaMaterial * mat)
{
    defaultMaterial = mat;
}

int MaterialStack::NumMaterials() const
{
    return MatStack.size();
}
