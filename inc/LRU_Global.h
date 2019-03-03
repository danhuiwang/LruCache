
#ifndef _LRUCACHE_INCLUDE_GLOBAL_H
#define _LRUCACHE_INCLUDE_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
// Define C99 equivalent types.
/*
typedef signed char           int8_t;
typedef signed short          int16_t;
typedef signed int            int32_t;
typedef signed long long      int64_t;
typedef unsigned char         uint8_t;
typedef unsigned short        uint16_t;
typedef unsigned int          uint32_t;
typedef unsigned long long    uint64_t;
*/
#define  BYTE16             16
#define  BYTE32             32
#define  BYTE64             64

#define  BYTE128            128
#define  BYTE256            256
#define  BYTE512            512

#define  KBYTES1            1024
#define  KBYTES2            2048
#define  KBYTES4            4096
#define  KBYTES8            8192

#define  BUFFER             512
#define  PACKET             1024

/*system error number*/
#define STATUS_SUCCEED         0
#define STATUS_FAILED          -1

/*net socket error number*/
#define STATUS_TIMEOUT          -2      //timeout
#define STATUS_PROTERR          -3      //protocol error
#define STATUS_CNNTERR          -4      //connect error

#define	LRU_INFO(format, args...)	do {	\
	fprintf(stdout, "[INFO] %s %04d: ", __FILE__, __LINE__);	\
	fprintf(stdout, format, ##args);				\
} while (0)

#define	LRU_WARN(format, args...)	do {				\
	fprintf(stdout, "[WARN] %s %04d: ", __FILE__, __LINE__);	\
	fprintf(stdout, format, ##args);				\
} while (0)

#define	LRU_PRINT(format, args...)	do {				\
	fprintf(stdout, format, ##args);				\
	fflush(stdout);							\
} while (0)

#define	LRU_ERROR(format, args...)	do {				\
	fprintf(stderr, "[ERROR] %s %04d: ", __FILE__, __LINE__);	\
	fprintf(stderr, format, ##args);				\
} while (0)

#define	LRU_DEBUG(format, args...)	do {				\
	fprintf(stdout, "[DEBUG] %s %04d: ", __FILE__, __LINE__);	\
	fprintf(stdout, format, ##args);				\
	fflush(stdout);							\
} while (0)

static const bool kLittleEndian = (__BYTE_ORDER == __LITTLE_ENDIAN);
#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED do { } while (0)
#endif
inline uint32_t DecodeFixed32(const char* ptr) {
    if (kLittleEndian) {
        // Load the raw bytes
        uint32_t result;
        memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
        return result;
    } else {
        return ((static_cast<uint32_t>(static_cast<unsigned char>(ptr[0])))
             |  (static_cast<uint32_t>(static_cast<unsigned char>(ptr[1])) << 8)
             |  (static_cast<uint32_t>(static_cast<unsigned char>(ptr[2])) << 16)
             |  (static_cast<uint32_t>(static_cast<unsigned char>(ptr[3])) << 24));
    }
}

// The FALLTHROUGH_INTENDED macro can be used to annotate implicit fall-through
// // between switch labels. The real definition should be provided externally.
// // This one is a fallback version for unsupported compilers.
#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED do { } while (0)
#endif
/*
static uint32_t LRU_Hash(const char* data, size_t n, uint32_t seed) {
    // Similar to murmur hash
    const uint32_t m = 0xc6a4a793;
    const uint32_t r = 24;
    const char* limit = data + n;
    uint32_t h = seed ^ (n * m);

    // Pick up four bytes at a time
    while (data + 4 <= limit) {
        uint32_t w = DecodeFixed32(data);
        data += 4;
        h += w;
        h *= m;
        h ^= (h >> 16);
    }

    // Pick up remaining bytes
    switch (limit - data) {
        case 3:
            h += static_cast<unsigned char>(data[2]) << 16;
            FALLTHROUGH_INTENDED;
        case 2:
            h += static_cast<unsigned char>(data[1]) << 8;
            FALLTHROUGH_INTENDED;
        case 1:
            h += static_cast<unsigned char>(data[0]);
            h *= m;
            h ^= (h >> r);
            break;
    }
    return h;
}
*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LRUCACHE_INCLUDE_GLOBAL_H */
