#ifndef BYTEORDER_H_
#define BYTEORDER_H_

#include <cstdint>

#define LIT_ENDIAN 1

#if BIG_ENDIAN
#error endian not implemented
#elif LIT_ENDIAN
inline  int32_t lit2host(int32_t val) { return val; }
inline uint32_t lit2host(uint32_t val) { return val; }
inline  int16_t lit2host(int16_t val) { return val; }
inline uint16_t lit2host(uint16_t val) { return val; }
inline   int8_t lit2host(int8_t val) { return val; }
inline  uint8_t lit2host(uint8_t val) { return val; }
inline  float   lit2host(float val) { return val; }
inline  double  lit2host(double val) { return val; }
#else
#error endian not defined
#endif


#endif
