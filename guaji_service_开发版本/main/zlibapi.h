#ifndef ZLIBAPI_H
#define ZLIBAPI_H

#pragma once

#include "zlib.h"

#define BEST_SPEED             Z_BEST_SPEED	//(default)
#define BEST_COMPRESSION       Z_BEST_COMPRESSION
#define DEFAULT_COMPRESSION	   Z_DEFAULT_COMPRESSION

//#define Compress(dst, dst_size, src, src_size, level) compress2((Bytef*)(dst), (uLongf*)(dst_size), (const Bytef*)(src), (uLong)(src_size), (level))
#define Compress(dst, dst_size, src, src_size) compress2((Bytef*)(dst), (uLongf*)(dst_size), (const Bytef*)(src), (uLong)(src_size), DEFAULT_COMPRESSION)
#define UnCompress(dst, dst_size, src, src_size) uncompress((Bytef*)(dst), (uLongf*)(dst_size), (const Bytef*)(src), (uLong)(src_size))


//destLen is the total  size of the destination buffer, which must be large enough to hold the
//   entire uncompressed data.

#endif // ZLIBAPI_H
