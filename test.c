#include <stdlib.h>
#include <stdio.h> 
#include <univaruints.h>


int main() {
	uint64_t i,v, used=0,n=8, list_i[]={0, 1, 127, 128, 129, 16512, 270549121, 34630287487};
	uchar_t *list_s[]={"\0", "\1", "\177", "\200\0", "\200\1", "\xc0\x00\x00", "\360\0\0\0\1", "\367\377\377\377\377"};
	uchar_t *buf=(uchar_t *)malloc(9);
	for(i=0;i<n;++i) {
		printf("%d. decoding: ", i+1);
		v=univaruints_decode_single(list_s[i], &used);
		printf("%d bytes used, value=%lld ", used, v);
		if (list_i[i]==v) printf(" pass.\n");
		else printf(" failed!! expected value=%lld\n", list_i[i]);
	}
	for(i=0;i<n;++i) {
		printf("%d. encoding: value=%lld, ", i+1, list_i[i]);
		used=univaruints_encode_single(buf, list_i[i]);
		printf("%d bytes used, ", used);
		v=univaruints_decode_single(buf, &used);
		if (list_i[i]==v) printf(" pass.\n");
		else printf(" failed!! expected value=%lld\n", list_i[i]);
	}
	return 0;
}
