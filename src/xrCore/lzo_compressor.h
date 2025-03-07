#pragma once

#include "lzo/lzo1.h"

XRCORE_API int lzo_compress_dict(
    const lzo_bytep in , lzo_uint  in_len,
    lzo_bytep out, lzo_uintp out_len,
    lzo_voidp wrkmem,
    const lzo_bytep dict, lzo_uint dict_len
);

XRCORE_API int lzo_decompress_dict(
    const lzo_bytep in,  lzo_uint  in_len,
    lzo_bytep out, lzo_uintp out_len,
    lzo_voidp wrkmem /* NOT USED */,
    const lzo_bytep dict, lzo_uint dict_len
);

XRCORE_API int lzo_initialize();