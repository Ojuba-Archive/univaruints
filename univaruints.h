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
#ifndef UNIVARUINTS_H
#define UNIVARUINTS_H
#include<stdio.h>
#include<stdint.h>
#define uchar_t unsigned char

/*
// we might have an interface the initialize a reader then fetch from it
typedef struct univaruints_reader_s {
    uint64_t offset;
} univaruints_reader;
*/

uint64_t univaruints_read_single(FILE *file, int *error);
uint64_t univaruints_write_single(FILE *file, uint64_t v);

uint64_t univaruints_decode_single(const uchar_t *buf, uint64_t *used);
uint64_t univaruints_encode_single(uchar_t *buf, uint64_t v);
uint64_t univaruints_decode(const uchar_t *buf, uint64_t size, uint64_t *v);
uint64_t univaruints_encode(uchar_t *buf, const uint64_t *v, uint64_t count);

#endif
