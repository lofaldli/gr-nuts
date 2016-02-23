#include "reed_solomon.h"
#include <stdint.h>
#include <stdio.h>
#include "ngham.h"

#include "fec-3.0.1/fec.h"
#include "fec-3.0.1/char.h"
#include "fec-3.0.1/rs-common.h"

#define RS_BITS_PER_SYM 8
#define RS_GFPOLY 0x187
#define RS_FCR 112
#define RS_PRIM 11
#define RS_PARITY_LEN 32

namespace gr {
    namespace nuts {
        reed_solomon::reed_solomon(uint8_t parity_len, uint8_t padding_len)
        {
            // TODO parity_len must be 16 or 32
            d_rs = init_rs_char(RS_BITS_PER_SYM, RS_GFPOLY, RS_FCR, RS_PRIM, parity_len, 0);
            ((struct rs*)d_rs)->pad = padding_len; // eww..
        }
        reed_solomon::~reed_solomon()
        {
            if (d_rs)
                free_rs_char(d_rs);
            d_rs = 0;
        }
        void reed_solomon::encode(uint8_t *data, uint8_t *parity)
        {
            encode_rs_char(d_rs, data, parity);
        }
        int16_t reed_solomon::decode(uint8_t *data)
        {
            return decode_rs_char(d_rs, data, 0, 0);
        }
    }
}
