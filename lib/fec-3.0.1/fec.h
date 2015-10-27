/* User include file for libfec
 * Copyright 2004, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */

// NOTE this file is slimmed down to only include the functions needed for reed solomon codec

#ifndef _FEC_H_
#define _FEC_H_

extern "C" {

/* General purpose RS codec, 8-bit symbols */
void encode_rs_char(void *rs,unsigned char *data,unsigned char *parity);
int decode_rs_char(void *rs,unsigned char *data,int *eras_pos,
		   int no_eras);
void *init_rs_char(int symsize,int gfpoly,
		   int fcr,int prim,int nroots,
		   int pad);
void free_rs_char(void *rs);



}
#endif /* _FEC_H_ */



