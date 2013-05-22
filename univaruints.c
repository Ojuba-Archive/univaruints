/**
 * univaruints: variable length compact serialization of unsigned integers
 * Copyright 2013, Muayyad Alsadi <alsadi@gmail.com>
 * 
 * we have API insterface for dealing with files
 * 
 * - uint64_t univaruints_read_single(FILE *file, int *error);
 * - uint64_t univaruints_write_single(FILE *file, uint64_t v);
 *
 * and another set for dealing with in-memory buffers
 *
 * - uint64_t univaruints_decode_single(const uchar_t *buf, uint64_t *used);
 * - uint64_t univaruints_encode_single(uchar_t *buf, uint64_t v);
 * - uint64_t univaruints_decode(const uchar_t *buf, uint64_t size, uint64_t *v);
 * - uint64_t univaruints_encode(uchar_t *buf, const uint64_t *v, uint64_t count);
 *
 **/
#include "univaruints.h"
#include <stdio.h>
#include <endian.h> // for be64toh
#include <string.h> // for memcpy


uint64_t shifts[]={0, 128, 16512, 2113664, 270549120, 34630287488, 4432676798592, 567382630219904, 72624976668147840};
uint64_t *shifts2=&shifts[2];
uchar_t n_by_chr[]="\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\4\4\4\4\4\4\4\4\5\5\5\5\6\6\7\10";

// generic bisection method (was used internally)
uchar_t bisect_right(const uint64_t *a, uint64_t x, uchar_t lo, uchar_t hi) {
	uchar_t mid;
	while(lo<hi) {
		mid = (lo+hi)>>1; // (lo+hi)/2
		if (x<a[mid]) hi = mid;
        else lo = mid+1;
	}
	return lo;
}

// optimized special case utility function (used internally)
uchar_t bisect_right7(const uint64_t *a, uint64_t x) {
	if (x<a[3]) {
		if (x<a[1]) {
			if (x<a[0]) {
				return 0;
			} else {
				return 1;
			}
		} else {
			if (x<a[2]) {
				return 2;
			} else {
				return 3;
			}			
		}
	} else {
		if (x<a[5]) {
			if (x<a[4]) {
				return 4;
			} else {
				return 5;
			}
		} else {
			if (x<a[6]) {
				return 6;
			} else {
				return 7;
			}			
		}
	}
}

/**
 * univaruints_read_single
 * params:
 * 		file:
 *      error: a pointer to an integer to be set in case of error
 * return:
 * 		the decoded usigned integer value read
 *
 * NOTE: you can use ftell to know how many bytes used
 **/
uint64_t univaruints_read_single(FILE *file, int *error) {
    uint64_t pack=0;
    uchar_t *ptr=(uchar_t *)&pack;
    uchar_t o, n, mask;
    int ch=getc(file);
    if (ch==EOF && error) {
		*error=1;
		return 0;
	}
    o=ch;
    if (128>o) {
		return (uint64_t)o;
	}
	n=n_by_chr[o];
	mask=127>>n;
	if (1>fread(ptr+8-n, n, 1, file) && error) {
		*error=1;
		return 0;
	}
	return shifts[n] + ( (((uint64_t)(o & mask))<< (n<<3)) | be64toh(pack) );
}

/**
 * univaruints_write_single
 * params:
 * 		file
 *      v: the usinged integer value to write
 * return:
 * 		number of bytes used, 0 if failed
 * 
 **/
uint64_t univaruints_write_single(FILE *file, uint64_t v) {
	uint64_t offset, pack=0;
	uchar_t *ptr=(uchar_t *)&pack;
	uchar_t n;
    if (v<128) {
		if (EOF==putc(v, file)) return 0;
		return 1;
	}
    n=bisect_right7(shifts2, v)+1; // n=bisect_right(shifts2, v, 0, 7)+1;
    offset=shifts[n];
    v-=offset;
    if (EOF==putc(((65280>>n) & 255) | ( (127>>n) & (v>>(n<<3)) ), file)) return 0;
    pack=htobe64(v); // pack is big-endian
    if (1>fwrite(ptr+8-n, n, 1, file)) return 0;
    return 1+n;
}


/**
 * univaruints_decode_single
 * params:
 * 		buf: pointer to the buffer
 *      used: pointer to be set with number of actually used bytes
 * return:
 * 		unsigned 64-bit integer hold the decoded value.
 * 
 * TODO: buffer overflow risk
 * if buffer is from untrusted source
 * then we need size parameter to be safe
 * (one can send malformed buffer with '\xff' and size=1)
 **/
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
 * univaruints_encode_single
 * params:
 * 		buf: pointer to previously allocated buffer (to be safe it should be able to take 9 bytes)
 *      v: the unsigned integer value to encode
 * return:
 * 		number of used bytes
 **/
uint64_t univaruints_encode_single(uchar_t *buf, uint64_t v) {
	uchar_t n;
	uint64_t offset, pack=0;
	uchar_t *ptr=(uchar_t *)&pack;
    if (v<128) {
		*buf=v;
		return 1;
	}
    n=bisect_right7(shifts2, v)+1; // n=bisect_right(shifts2, v, 0, 7)+1;
    offset=shifts[n];
    v-=offset;
    *buf=((65280>>n) & 255) | ( (127>>n) & (v>>(n<<3)) );
    pack=htobe64(v); // pack is big-endian
    memcpy(buf+1, ptr+8-n, n); // read from pack into buf
    return 1+n;
}
/**
 * return: number of decoded integers
 **/
uint64_t univaruints_decode(const uchar_t *buf, uint64_t size, uint64_t *v /*, uint64_t *used */) {
    // we will add used when we add limit
    const uchar_t *end=buf+size;
    uint64_t used_i=0, used_sum=0, i=0;
    while(buf<end) {
        *v=univaruints_decode_single(buf, &used_i);
        ++v;
        buf+=used_i;
        used_sum+=used_i;
        ++i;
    }
    /* *used=used_sum; */
    return i;
}

/**
 * return: number of used bytes
 **/
uint64_t univaruints_encode(uchar_t *buf, const uint64_t *v, uint64_t count) {
    const uint64_t *end=v+count;
    uint64_t used=0, used_sum=0;
    while(v<end) {
        used=univaruints_encode_single(buf, *v++);
        used_sum+=used;
        buf+=used;
    }
    return used_sum;
}
