/**
 * univaruints: variable length compact serialization of unsigned integers
 * Copyright 2013,
 **/
#ifndef UNIVARUINTS_H
#define UNIVARUINTS_H

#include<stdint.h>
#define uchar_t unsigned char

typedef struct univaruints_reader_s {
    uint64_t offset;
} univaruints_reader;

uint64_t univaruints_decode_single(const uchar_t *buf, uint64_t *used);
uint64_t univaruints_encode_single(uchar_t *buf, uint64_t v);

#endif
