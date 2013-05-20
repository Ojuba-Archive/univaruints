/**
 * univaruints: variable length compact serialization of unsigned integers
 * Copyright 2013,
 **/
#include "univaruints.h"
#include <endian.h> // for be64toh
#include <string.h> // for memcpy


uint64_t shifts[]={0, 128, 16512, 2113664, 270549120, 34630287488, 4432676798592, 567382630219904, 72624976668147840};
uint64_t *shifts2=&shifts[2];
uchar_t n_by_chr[]="\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\4\4\4\4\4\4\4\4\5\5\5\5\6\6\7\10";

uchar_t bisect_right(const uint64_t *a, uint64_t x, uchar_t lo, uchar_t hi) {
	uchar_t mid;
	while(lo<hi) {
		mid = (lo+hi)>>1; // (lo+hi)/2
		if (x<a[mid]) hi = mid;
        else lo = mid+1;
	}
	return lo;
}

uint64_t univaruints_decode_single(const uchar_t *buf, uint64_t *used) {
  uchar_t o, n, mask;
  uint64_t pack=0;
  uchar_t *ptr=(uchar_t *)&pack;
  if (128>(o=*buf)) {
	  *used=1;
	  return (uint64_t)o;
  }
  n=n_by_chr[o];
  mask=127>>n;
  *used=n+1;
  memcpy(ptr+8-n, buf+1, n); // read from buf into pack, buf is big-endian
  return shifts[n] + ( (((uint64_t)(o & mask))<< (n<<3)) | be64toh(pack) );
}

/**
 * you need to allocate buf
 **/
uint64_t univaruints_encode_single(uchar_t *buf, uint64_t v) {
	uchar_t n;
	uint64_t offset, pack=0;
	uchar_t *ptr=(uchar_t *)&pack;
    if (v<128) {
		*buf=v;
		return 1;
	}
    n=bisect_right(shifts2, v, 0, 7)+1;
    offset=shifts[n];
    v-=offset;
    *buf=((65280>>n) & 255) | ( (127>>n) & (v>>(n<<3)) );
    pack=htobe64(v); // pack is big-endian
    memcpy(buf+1, ptr+8-n, n); // read from pack into buf
    return 1+n;
}
