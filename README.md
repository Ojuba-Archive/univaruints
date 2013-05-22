univaruints
===========

compact serialization of variable length unsigned integers

this C libary provides needed API to encode/decode such integers
and it can be used to serialize more complex structures

usage
-----

We have file-based API which can be used like this

    int i=123456, error;
    f=fopen("out.tmp", "wb+");
    univaruints_write_single(f, 123456);
    fseek(f, 0, SEEK_SET);
    i=univaruints_read_single(f, &error)

We also provide another set of API calls for in-memory operations

idea
----

based on idea from google's varint in its protocolbuffers http://code.google.com/apis/protocolbuffers/docs/encoding.html
and from UTF-8 variable length encoding of Unicode
I made this more compact format which is also supposed to be faster too
and have extra useful properties.

a single integer in univaruints can be something like

    0xxx-xxxx
    10xx-xxxx xxxx-xxxx
    110x-xxxx xxxx-xxxx xxxx-xxxx 

the number of leading most significant set bits (ie. till the first zero) of first byte
indicates the number of extra bytes needed to encode a single integer


    0xxx-xxxx is 0-127
    10xx-xxxx xxxx-xxxx  is 128-16511 (as 0b1000-0000-0000-0000 => 128 and 0b1011-1111-1111-1111 => 16511)
    110x-xxxx xxxx-xxxx xxxx-xxxx is 16512-2113663 


features
--------

1. simple, fast, unit-tested and have predetermined length (from first byte)
2. it preserve order (unlike protocolbuffers) eg. can be used to sort nested thread comments
3. several convinient interfaces (eg. encode_signle/decode_single)
4. more compact serialization can be made for increasing lists

Language Support
----------------

This algorithm is also implemented in many computer languages

1. [univaruints](https://github.com/ojuba-org/univaruints/) library implemented in C programming language
2. [python-univaruints](https://github.com/ojuba-org/python-univaruints/) Pure python implementation
3. [js-univaruints](https://github.com/ojuba-org/js-univaruints/) Implementation in Javascript

