// ngham specific constants etc.
//

#ifndef NGHAM_H
#define NGHAM_H

#define NGHAM_PREAMBLE_SIZE 4
#define NGHAM_SYNC_SIZE 4
#define NGHAM_SIZE_TAG_SIZE 3
#define NGHAM_MAX_CODEWORD_SIZE 255
#define NGHAM_MAX_TOTAL_SIZE (NGHAM_PREAMBLE_SIZE + NGHAM_SYNC_SIZE + NGHAM_SIZE_TAG_SIZE + NGHAM_MAX_CODEWORD_SIZE)

#define NGHAM_SIZES 7
namespace gr {
    namespace nuts {
        const unsigned char NGHAM_PL_SIZE[NGHAM_SIZES]          = {28, 60, 92, 124, 156, 188, 220};
        const unsigned char NGHAM_PL_SIZE_FULL[NGHAM_SIZES]     = {31, 63, 95, 127, 159, 191, 223};
        const unsigned char NGHAM_CODEWORD_SIZE[NGHAM_SIZES]    = {47, 79, 111, 159, 191, 223, 255};
        const unsigned char NGHAM_PAR_SIZE[NGHAM_SIZES]         = {16, 16, 16, 32, 32, 32, 32};

        const unsigned char NGHAM_PREAMBLE[NGHAM_PREAMBLE_SIZE] = {0xaa, 0xaa, 0xaa, 0xaa};
        const unsigned char NGHAM_SYNC[NGHAM_SYNC_SIZE]         = {0x5d, 0xe6, 0x2a, 0x7e};
        const unsigned char NGHAM_SIZE_TAG[NGHAM_SIZES][NGHAM_SIZE_TAG_SIZE] = {
            {0x3b, 0x49, 0xcd},
            {0x4d, 0xda, 0x57},
            {0x76, 0x93, 0x9a},
            {0x9b, 0xb4, 0xae},
            {0xa0, 0xfd, 0x63},
            {0xd6, 0x6e, 0xf9},
            {0xed, 0x27, 0x34}
        };


        
    }
}
#endif
 
