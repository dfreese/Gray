#include "MaterialStack.h"

MaterialStack::MaterialStack() {
	defaultMaterial = NULL;
	errorMaterial = ERROR_MATERIAL;
}

void MaterialStack::PushMaterial(MaterialBase *mat) {
	MatStack.Push(mat);
}

MaterialBase* MaterialStack::PopMaterial() {
	if (NumMaterials() <= 0) {
		cerr << "Poping empty\n";
		return errorMaterial;
	}
	MaterialBase* ret = MatStack.Pop();
	return ret;
}

MaterialBase * MaterialStack::curMaterial() {
	if (NumMaterials() <= 0) {
		return errorMaterial;
	}
	return MatStack.Top();
}

void MaterialStack::ResetMaterial() {
	long i;
	MatStack.Reset();
	MatStack.Push(defaultMaterial);
}
void MaterialStack::SetDefault(MaterialBase * mat) {
	defaultMaterial = mat;
}
