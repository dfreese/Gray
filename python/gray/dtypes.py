
import numpy as np

# Don't align this dtype as #pragma pack(push, 1) was used for the struct
standard_dtype = np.dtype([
    ('time', np.float64),
    ('energy', np.float32),
    ('pos', np.float32, (3,)),
    ('log', np.int32),
    ('id', np.int32),
    ('det', np.int32)], align=False)

standard_expanded_dtype = np.dtype([
    ('time', np.float64),
    ('energy', np.float32),
    ('pos', np.float32, (3,)),
    ('interaction', np.int32),
    ('color', np.int32),
    ('scatter', np.int32),
    ('det_mat', np.int32),
    ('src_id', np.int32),
    ('id', np.int32),
    ('det', np.int32)], align=True)

no_position_dtype = np.dtype([
    ('time', np.float64),
    ('energy', np.float32),
    ('log', np.int32),
    ('id', np.int32),
    ('det', np.int32)], align=True)

no_position_expanded_dtype = np.dtype([
    ('time', np.float64),
    ('energy', np.float32),
    ('interaction', np.int32),
    ('color', np.int32),
    ('scatter', np.int32),
    ('det_mat', np.int32),
    ('src_id', np.int32),
    ('id', np.int32),
    ('det', np.int32)], align=True)

minimum_dtype = np.dtype([
    ('time', np.float64),
    ('energy', np.float32),
    ('det', np.int32)], align=True)

interaction_fields = (
    ('time', np.float64),
    ('id', np.int32),
    ('color', np.int32),
    ('type', np.int32),
    ('pos', np.float64, (3,)),
    ('energy', np.float64),
    ('det_id', np.int32),
    ('src_id', np.int32),
    ('mat_id', np.int32),
    ('scatter_compton_phantom', np.int32),
    ('scatter_compton_detector', np.int32),
    ('scatter_rayleigh_phantom', np.int32),
    ('scatter_rayleigh_detector', np.int32),
    ('xray_flouresence', np.int32),
    ('coinc_id', np.int32),
    )

def interaction_all_dtype():
    return np.dtype(list(interaction_fields))

def create_variable_dtype(write_mask):
    vals = []
    for field, mask in zip(interaction_fields, write_mask):
        if mask:
            vals.append(field)
    return np.dtype(vals)
