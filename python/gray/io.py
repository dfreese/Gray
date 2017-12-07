import numpy as np
import gray

def variable_field_mask(data):
    '''
    Goes through every field in interaction_fields, and checks if it exists in
    in the data array that was passed.
    '''
    return [f in data.dtype.descr for f in interaction_all_dtype.descr]

def write_variable_binary(filename, data):
    with open(filename, 'wb') as fid:
        fields = np.array(variable_field_mask(data), dtype=np.int32)
        # Magic number, version number, no_fields, no_active, event size
        np.array((65531, 1, fields.size, fields.sum(), data.dtype.itemsize,),
                  dtype=np.int32).tofile(fid)
        fields.tofile(fid)
        data.tofile(fid)

def load_detector_output(filename, full=False, expand=False):
    if full:
        data = np.fromfile(filename, dtype=gray.standard_dtype)
    else:
        data = np.fromfile(filename, dtype=gray.no_position_dtype)

    if expand:
        return expand_detector_format(data, full)
    else:
        return data

def load_variable_binary(filename):
    with open(filename, 'r') as fid:
        magic_number = np.fromfile(fid, dtype=np.int32, count=1)
        if magic_number != 65531:
            RuntimeError('Invalid binary file start')
        version_number = np.fromfile(fid, dtype=np.int32, count=1)
        if version_number != 1:
            RuntimeError('Invalid version number')
        no_fields = np.fromfile(fid, dtype=np.int32, count=1)
        no_active = np.fromfile(fid, dtype=np.int32, count=1)
        event_size = np.fromfile(fid, dtype=np.int32, count=1)
        read_mask = np.fromfile(fid, dtype=np.int32, count=no_fields).astype(bool)
        if read_mask.sum() != no_active:
            RuntimeError('Number of active fields does not match header')
        cur_dtype = gray.create_variable_dtype(read_mask)
        events = np.fromfile(fid, dtype=cur_dtype)
    return events

def load_mapping_file(filename):
    '''
    Load a mapping file used for the gray daq process model into a numpy array
    with a custom dtype to reflect the mapping names.

    Parameters
    ----------
    filename : str,
        The file containing the mapping information

    Returns
    -------
    data : ndarray, dtype custom
        returns the mapping information contained within the file.  The array
        dtype is created based on the column headers and all values are int.

    '''
    with open(filename, 'r') as fid:
        # Read the column names from the first row
        col_names = fid.readline().splitlines()[0].split()
        # Generate a new dtype to represent the mapping file
        map_dtype = np.dtype([(n, int) for n in col_names])
        # and load in the remainder of the rows as the data
        mapping = np.loadtxt(fid, dtype=map_dtype)
    return mapping

def save_mapping_file(filename, mapping):
    '''
    Write a mapping file used for the gray daq process model from a numpy array
    with a custom dtype to reflect the mapping names.

    Parameters
    ----------
    filename : str,
        The filename to which to write the mapping file.
    mapping : ndarray, dtype custom
        The mapping information to be written within the file.  Each name in
        the dtype is used as a column header.  Assumes all types are integers.

    '''
    with open(filename, 'w') as fid:
        print >>fid, ' '.join(mapping.dtype.names)
        np.savetxt(fid, mapping, fmt='%d')
