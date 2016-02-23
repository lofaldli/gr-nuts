#ifndef INCLUDED_REED_SOLOMON_H
#define INCLUDED_REED_SOLOMON_H

#include <nuts/api.h>
#include <stdint.h>

namespace gr {
    namespace nuts {

        class NUTS_API reed_solomon {
            private:
                void *d_rs;
            public:
                reed_solomon(uint8_t parity_len, uint8_t padding_len);
                ~reed_solomon();

                void encode(uint8_t *data, uint8_t *parity);
                int16_t decode(uint8_t *data);
        };
    }
}

#endif /* INCLUDED_REED_SOLOMON_H */
