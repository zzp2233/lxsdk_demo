from libs.Khmer import s_cvt_map_km
from libs.Hindi import s_cvt_map_hi


def km_deformation(byte_array: bytearray):
    index = 0
    bHasDeformation = False
    bDeformated = False
    while index < len(byte_array):
        for i in range(len(s_cvt_map_km)):
            if byte_array[index:index + len(s_cvt_map_km[i].p_key)] == s_cvt_map_km[i].p_key:
                byte_array[index:index + len(s_cvt_map_km[i].p_key)] = s_cvt_map_km[i].p_value
                index += len(s_cvt_map_km[i].p_value)
                bHasDeformation = True
                if not bDeformated:
                    bDeformated = True
                break

        if bHasDeformation:
            bHasDeformation = False
        else:
            index += 1

    if bDeformated:
        return bytes(byte_array) + b'\x17\xF0'
    else:
        return bytes(byte_array)


def hi_deformation(byte_array: bytearray):
    index = 0
    bHasDeformation = False
    bDeformated = False
    while index < len(byte_array):
        for i in range(len(s_cvt_map_hi)):
            if byte_array[index:index + len(s_cvt_map_hi[i].p_key)] == s_cvt_map_hi[i].p_key:
                byte_array[index:index + len(s_cvt_map_hi[i].p_key)] = s_cvt_map_hi[i].p_value
                index += len(s_cvt_map_hi[i].p_value)
                bHasDeformation = True
                if not bDeformated:
                    bDeformated = True
                break

        if bHasDeformation:
            bHasDeformation = False
        else:
            index += 1

    if bDeformated:
        return bytes(byte_array) + b'\xE0\x00'
    else:
        return bytes(byte_array)
