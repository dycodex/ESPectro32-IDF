/*
 * audio_renderer.h
 *
 *  Created on: 12.03.2017
 *      Author: michaelboeckling
 */

#ifndef INCLUDE_AUDIO_RENDERER_H_
#define INCLUDE_AUDIO_RENDERER_H_

#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"
//#include "common_component.h"

#define AUDIO_OUTPUT_MODE 0

typedef enum {
    UNINITIALIZED=0, INITIALIZED=1, RUNNING=2, STOPPED=3
} component_status_t;

typedef enum {
    I2S, I2S_MERUS, DAC_BUILT_IN, PDM
} output_mode_t;


/* ESP32 is Little Endian, I2S is Big Endian.
 *
 * Samples produced by a decoder will probably be LE,
 * and I2S recordings BE.
 */
typedef enum
{
    PCM_INTERLEAVED, PCM_LEFT_RIGHT
} pcm_buffer_layout_t;

typedef enum
{
    PCM_BIG_ENDIAN, PCM_LITTLE_ENDIAN
} pcm_endianness_t;

typedef struct
{
    uint32_t sample_rate;
    i2s_bits_per_sample_t bit_depth;
    uint8_t num_channels;
    pcm_buffer_layout_t buffer_format;
    pcm_endianness_t endianness; // currently unused
} pcm_format_t;

class Renderer{
    private:
		output_mode_t output_mode;
		int sample_rate;
		float sample_rate_modifier;
		i2s_bits_per_sample_t bit_depth;
		i2s_port_t i2s_num;
		i2s_pin_config_t *i2s_pin_config = nullptr;

		void init_i2s();

    public:
		Renderer(i2s_pin_config_t *pin_config = nullptr);
		/* generic renderer interface */
		void render_samples(const char *buf, uint32_t len, pcm_format_t *format);

		void renderer_init();
		void renderer_start();
		void renderer_stop();
		void renderer_destroy();

		void renderer_zero_dma_buffer();

		i2s_bits_per_sample_t getBitDepth() const;
};




#endif /* INCLUDE_AUDIO_RENDERER_H_ */
