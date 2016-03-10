// ngham specific constants etc.
//

#ifndef NGHAM_H
#define NGHAM_H

#define NGHAM_PREAMBLE_SIZE 4
#define NGHAM_SYNC_SIZE 4
#define NGHAM_SIZE_TAG_SIZE 3
#define NGHAM_HEADER_SIZE (NGHAM_PREAMBLE_SIZE + NGHAM_SYNC_SIZE + NGHAM_SIZE_TAG_SIZE)
#define NGHAM_RS_HEADER_SIZE 1
#define NGHAM_CRC_SIZE 2
#define NGHAM_MAX_CODEWORD_SIZE 255
#define NGHAM_MAX_TOTAL_SIZE (NGHAM_HEADER_SIZE + NGHAM_MAX_CODEWORD_SIZE)


#define NGHAM_FLAGS_MASK 0xe0
#define NGHAM_PADDING_SIZE_MASK 0x1f

#define NGHAM_SIZES 7
#define NGHAM_SIZE_TAG_MAX_ERROR 6
namespace gr {
    namespace nuts {

        struct ngham_tx_pkt {
            uint8_t preamble[NGHAM_PREAMBLE_SIZE];
            uint8_t sync_word[NGHAM_SYNC_SIZE];
            uint8_t size_tag[NGHAM_SIZE_TAG_SIZE];
            uint8_t rs_codeword[NGHAM_MAX_CODEWORD_SIZE];
        }__attribute__((packed));

        const uint8_t NGHAM_RS_DATA_SIZE[NGHAM_SIZES]          = {28, 60, 92, 124, 156, 188, 220};
        const uint8_t NGHAM_RS_DATA_SIZE_FULL[NGHAM_SIZES]     = {31, 63, 95, 127, 159, 191, 223};
        const uint8_t NGHAM_RS_PARITY_SIZE[NGHAM_SIZES]        = {16, 16, 16, 32, 32, 32, 32};
        const uint8_t NGHAM_RS_CODEWORD_SIZE[NGHAM_SIZES]      = {47, 79, 111, 159, 191, 223, 255};

        const uint8_t NGHAM_PREAMBLE[NGHAM_PREAMBLE_SIZE] = {0xaa, 0xaa, 0xaa, 0xaa};
        const uint8_t NGHAM_SYNC[NGHAM_SYNC_SIZE]         = {0x5d, 0xe6, 0x2a, 0x7e};
        const uint8_t NGHAM_SIZE_TAG[NGHAM_SIZES][NGHAM_SIZE_TAG_SIZE] = {
            {0x3b, 0x49, 0xcd},
            {0x4d, 0xda, 0x57},
            {0x76, 0x93, 0x9a},
            {0x9b, 0xb4, 0xae},
            {0xa0, 0xfd, 0x63},
            {0xd6, 0x6e, 0xf9},
            {0xed, 0x27, 0x34}
        };

        const uint8_t ccsds_poly[255] = {0xFF, 0x48, 0x0E, 0xC0, 0x9A, 0x0D, 0x70, 0xBC,
            0x8E, 0x2C, 0x93, 0xAD, 0xA7, 0xB7, 0x46, 0xCE, 0x5A, 0x97, 0x7D, 0xCC, 0x32,
            0xA2, 0xBF, 0x3E, 0x0A, 0x10, 0xF1, 0x88, 0x94, 0xCD, 0xEA, 0xB1, 0xFE, 0x90,
            0x1D, 0x81, 0x34, 0x1A, 0xE1, 0x79, 0x1C, 0x59, 0x27, 0x5B, 0x4F, 0x6E, 0x8D,
            0x9C, 0xB5, 0x2E, 0xFB, 0x98, 0x65, 0x45, 0x7E, 0x7C, 0x14, 0x21, 0xE3, 0x11,
            0x29, 0x9B, 0xD5, 0x63, 0xFD, 0x20, 0x3B, 0x02, 0x68, 0x35, 0xC2, 0xF2, 0x38,
            0xB2, 0x4E, 0xB6, 0x9E, 0xDD, 0x1B, 0x39, 0x6A, 0x5D, 0xF7, 0x30, 0xCA, 0x8A,
            0xFC, 0xF8, 0x28, 0x43, 0xC6, 0x22, 0x53, 0x37, 0xAA, 0xC7, 0xFA, 0x40, 0x76,
            0x04, 0xD0, 0x6B, 0x85, 0xE4, 0x71, 0x64, 0x9D, 0x6D, 0x3D, 0xBA, 0x36, 0x72,
            0xD4, 0xBB, 0xEE, 0x61, 0x95, 0x15, 0xF9, 0xF0, 0x50, 0x87, 0x8C, 0x44, 0xA6,
            0x6F, 0x55, 0x8F, 0xF4, 0x80, 0xEC, 0x09, 0xA0, 0xD7, 0x0B, 0xC8, 0xE2, 0xC9,
            0x3A, 0xDA, 0x7B, 0x74, 0x6C, 0xE5, 0xA9, 0x77, 0xDC, 0xC3, 0x2A, 0x2B, 0xF3,
            0xE0, 0xA1, 0x0F, 0x18, 0x89, 0x4C, 0xDE, 0xAB, 0x1F, 0xE9, 0x01, 0xD8, 0x13,
            0x41, 0xAE, 0x17, 0x91, 0xC5, 0x92, 0x75, 0xB4, 0xF6, 0xE8, 0xD9, 0xCB, 0x52,
            0xEF, 0xB9, 0x86, 0x54, 0x57, 0xE7, 0xC1, 0x42, 0x1E, 0x31, 0x12, 0x99, 0xBD,
            0x56, 0x3F, 0xD2, 0x03, 0xB0, 0x26, 0x83, 0x5C, 0x2F, 0x23, 0x8B, 0x24, 0xEB,
            0x69, 0xED, 0xD1, 0xB3, 0x96, 0xA5, 0xDF, 0x73, 0x0C, 0xA8, 0xAF, 0xCF, 0x82,
            0x84, 0x3C, 0x62, 0x25, 0x33, 0x7A, 0xAC, 0x7F, 0xA4, 0x07, 0x60, 0x4D, 0x06,
            0xB8, 0x5E, 0x47, 0x16, 0x49, 0xD6, 0xD3, 0xDB, 0xA3, 0x67, 0x2D, 0x4B, 0xBE,
            0xE6, 0x19, 0x51, 0x5F, 0x9F, 0x05, 0x08, 0x78, 0xC4, 0x4A, 0x66, 0xF5, 0x58
        };

        const uint16_t crc_ccitt_table[] = {
            0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
            0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
            0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
            0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
            0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
            0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
            0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
            0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
            0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
            0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
            0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
            0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
            0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
            0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
            0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
            0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
            0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
            0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
            0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
            0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
            0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
            0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
            0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
            0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
            0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
            0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
            0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
            0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
            0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
            0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
            0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
            0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
        };

        inline void print_bytes(uint8_t *data, uint32_t length) {
            const uint8_t items_per_row = 10;
            for (uint32_t i=0; i<length; i+=items_per_row) {
                // print hex...
                printf("    %4i: ", i);
                for (uint8_t j=0; j<items_per_row; j++) {
                    if (i+j < length) {
                        printf("%x%x ", data[i+j] >> 4 & 0x0f, data[i+j] & 0x0f);
                    } else {
                        printf("    ");
                    }
                }
                // ...and ascii
                printf("  ");
                for (uint8_t j=0; j<items_per_row && i+j<length; j++) {
                    if (data[i+j] > 31 && data[i+j] < 127) {
                        printf("%c", data[i+j]);
                    } else {
                        printf(".");
                    }
                }
                printf("\n");
            }
        }
        inline uint16_t calc_crc(uint8_t *data, uint8_t length) {
            uint16_t crc = 0xffff;
            for (uint8_t i=0; i<length; i++){
                crc = ((crc >> 8) & 0xff) ^ crc_ccitt_table[(crc ^ data[i]) & 0xff];
            }
            crc ^= 0xffff;
            return crc;
        }

        inline void scramble(uint8_t *data, uint8_t length) {
            for (int i=0; i<length; i++)
                data[i] ^= ccsds_poly[i];
        }
    }
}
#endif

