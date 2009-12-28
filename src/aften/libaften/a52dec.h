#ifndef A52DEC_H
#define A52DEC_H

#include "a52.h"

typedef struct {
	uint8_t *input_frame_buffer;
	int input_frame_buffer_size;
	
	int channel_mode;
	int fbw_channels;
	int lfe_on;
	int sample_rate;

	int bsid;
	int bsmod;
	
	int cmixlev;
	int surmixlev;
	int dsurmod;

	A52BitAllocParams bit_alloc_params;
} A52DecodeContext;

struct A52ThreadContext;

static inline void a52_decode_init(){}

static inline void a52_decode_init_thread(UNUSED(struct A52ThreadContext *tctx)){}

static inline int a52_decode_frame(UNUSED(struct A52ThreadContext *tctx)){return 0;}

static inline void a52_decode_deinit_thread(UNUSED(struct A52ThreadContext *tctx)){}

static inline int a52_get_frame_size(UNUSED(A52DecodeContext *s)){return 0;}

#endif /* A52DEC_H */
