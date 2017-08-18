
#ifndef MAIN_MB_TESTI2S_H_
#define MAIN_MB_TESTI2S_H_

#include "Arduino.h"

extern "C" {
#include "driver/i2s.h"
#include "freertos/ringbuf.h"
//#include "rom/ets_sys.h"
}

static RingbufHandle_t rb;

const int sample_rate = 44100;//22050;
const static char *TAG2 = "I2S";

void tryI2SRecord(void *p)
{
	ESP_LOGI(TAG2, "Opening file for recording");
	FILE *f = NULL;

	f = fopen("/spiflash/hello.wav", "wb");
	if (f == NULL) {
		ESP_LOGE(TAG2, "Failed to open file for writing");
		return;
	}

	i2s_config_t i2s_config_rx = {
		mode : (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
		sample_rate : 16000,//sample_rate,
		bits_per_sample : I2S_BITS_PER_SAMPLE_32BIT,
		channel_format : I2S_CHANNEL_FMT_RIGHT_LEFT,
		communication_format : (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
		intr_alloc_flags : ESP_INTR_FLAG_LEVEL1,
		dma_buf_count : 32,
		dma_buf_len : 64
	};

	i2s_config_t i2s_config_tx = {
		mode : (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
		sample_rate : 16000,//sample_rate,
		bits_per_sample : I2S_BITS_PER_SAMPLE_16BIT,
		channel_format : I2S_CHANNEL_FMT_RIGHT_LEFT,
		communication_format : (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
		intr_alloc_flags : ESP_INTR_FLAG_LEVEL1,
		dma_buf_count : 32,
		dma_buf_len : 64
	};

	i2s_pin_config_t pin_config_tx = {
		.bck_io_num = 26, //21,
		.ws_io_num = 25,
		.data_out_num = 22, //I2S_PIN_NO_CHANGE
		.data_in_num = I2S_PIN_NO_CHANGE
	};

	i2s_pin_config_t pin_config_rx = {
		.bck_io_num = GPIO_NUM_17, //GPIO_NUM_26, //GPIO_NUM_17
		.ws_io_num = GPIO_NUM_5, //GPIO_NUM_25, //GPIO_NUM_5
		.data_out_num = I2S_PIN_NO_CHANGE,
		.data_in_num = GPIO_NUM_16//GPIO_NUM_39,//GPIO_NUM_36
	};

    //for recording
    i2s_driver_install(I2S_NUM_1, &i2s_config_rx, 0, NULL);
    i2s_set_pin(I2S_NUM_1, &pin_config_rx);
    i2s_stop(I2S_NUM_1);

    //for playback
    i2s_driver_install(I2S_NUM_0, &i2s_config_tx, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config_tx);
    i2s_stop(I2S_NUM_0);

    int cnt = 0;
    uint64_t buffer = 0;
    char buf[512];

    unsigned elapsedTime = millis();

    i2s_start(I2S_NUM_1);
    while (1)
    {
        char *buf_ptr_read = buf;
        char *buf_ptr_write = buf;

        // read whole block of samples
        int bytes_read = 0;
        while(bytes_read == 0) {
        		bytes_read = i2s_read_bytes(I2S_NUM_1, buf, sizeof(buf), 0);
        }

        uint32_t samples_read = bytes_read / 2 / (I2S_BITS_PER_SAMPLE_32BIT / 8);

        //const char samp64[8] = {};
        for (int i = 0; i < samples_read; i++)
        {
            buf_ptr_write[0] = buf_ptr_read[2]; // mid
            buf_ptr_write[1] = buf_ptr_read[3]; // high

            buf_ptr_write += 1 * (I2S_BITS_PER_SAMPLE_16BIT / 8);
            buf_ptr_read += 2 * (I2S_BITS_PER_SAMPLE_32BIT / 8);
        }

        ssize_t readable_bytes = samples_read * (I2S_BITS_PER_SAMPLE_16BIT / 8);

        // // 2 * mono
//        i2s_write_bytes(I2S_NUM_0, (const char*)buf, readable_bytes, portMAX_DELAY);

        fwrite((const char*)buf, sizeof(char), readable_bytes, f);

        if (millis() - elapsedTime > 10000) {
        		printf("Recording done\n");
        		elapsedTime = millis();

        		break;
        }
    }

    fclose(f);

    //reopen
    // Open file for reading
	ESP_LOGI(TAG2, "Reading file");
	f = fopen("/spiflash/hello.wav", "rb");
	if (f == NULL) {
		ESP_LOGE(TAG2, "Failed to open file for reading");
		return;
	}

	//read
	i2s_stop(I2S_NUM_1);
	delay(100);

//	i2s_set_sample_rates(I2S_NUM_0, 11000); //set sample rates

	uint8_t buf_bytes_per_sample = (I2S_BITS_PER_SAMPLE_16BIT / 8);
	int buffer_len = -1;
	int buffer_index = 0;
	char buffer_read[512];
	int readed = 0;
	//bool i2s_full = false;
	//unsigned int sample_val;

	i2s_start(I2S_NUM_0);

	while(1) {

//		while (buffer_index < bufferlen) {
//			int16_t pcm = bufferRead[buffer_index];
//
////			int16_t left = pcm;
////			int16_t right = pcm;
////
////			left = (left >> 8) + 0x80;
////			right = (right >> 8) + 0x80;
////			unsigned short sample = (left << 8) | (right & 0xff);
//
////			int sample = right & 0xFFFF;
////			sample = sample << 16;
////			sample |= left & 0xFFFF;
//
//			sample_val = 0;
//			sample_val += (short) pcm;// >> attenuation;
//			sample_val = sample_val << 16;
//			sample_val += (short) pcm;// >> attenuation;
//
//			int ret = i2s_push_sample(I2S_NUM_0, (char *)&sample_val, portMAX_DELAY);
////			char sample_val = bufferRead[buffer_index];
////
////			int ret = i2s_push_sample(I2S_NUM_0, (char *)&sample_val, portMAX_DELAY);
//
////			Serial.printf("wavRead %d\r\n", ret);
//			//if (i2s_write_lr_nb(pcm, pcm)) {
//			if (ret) {
//				buffer_index++;
//			} else {
//				i2s_full = true;
//				break;
//			}
//			if ((buffer_index & 0x3F) == 0)
//				delay(1);
//		}
//
//		if (i2s_full)
//			break;

		readed = fread((char*)buffer_read, sizeof(char), sizeof(buffer_read), f);

		if (readed > 0) {

			buffer_len = readed / sizeof(buffer_read[0]);
			buffer_index = 0;

//			delay(1);

			char *ptr_l = buffer_read;
			char *ptr_r = ptr_l;
			uint8_t stride = buf_bytes_per_sample;
			uint32_t num_samples = buffer_len / buf_bytes_per_sample / 1;
			//printf("Read size: %d, num samples: %d\r\n", readed, num_samples);

			int bytes_pushed = 0;
			for (int i = 0; i < num_samples; i++) {

				const char samp32[4] = {ptr_l[0], ptr_l[1], ptr_r[0], ptr_r[1]};

				bytes_pushed = i2s_push_sample(I2S_NUM_0, (const char*) &samp32, portMAX_DELAY);

				// DMA buffer full - retry
				if (bytes_pushed == 0) {
					i--;
				} else {
					ptr_r += stride;
					ptr_l += stride;
				}
			}

		} else {
			printf("Stop playing\n");

			i2s_stop(I2S_NUM_0);
			break;
		}
	}

	fclose(f);

    vTaskDelete(NULL);
}

void tryI2SInput(void *p)
{
    // delay(50);

    i2s_config_t i2s_config_rx = {
        mode : (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        sample_rate : sample_rate,
        bits_per_sample : I2S_BITS_PER_SAMPLE_32BIT,
        channel_format : I2S_CHANNEL_FMT_RIGHT_LEFT,
        communication_format : (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        intr_alloc_flags : ESP_INTR_FLAG_LEVEL1,
        dma_buf_count : 32,
        dma_buf_len : 64
    };

    i2s_config_t i2s_config_tx = {
        mode : (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        sample_rate : sample_rate,
        bits_per_sample : I2S_BITS_PER_SAMPLE_32BIT,
        channel_format : I2S_CHANNEL_FMT_RIGHT_LEFT,
        communication_format : (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        intr_alloc_flags : ESP_INTR_FLAG_LEVEL1,
        dma_buf_count : 32,
        dma_buf_len : 64
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = 26, //21,
        .ws_io_num = 25,
        .data_out_num = 22, //I2S_PIN_NO_CHANGE
        .data_in_num = I2S_PIN_NO_CHANGE};

    i2s_pin_config_t pin_config_rx = {
    		.bck_io_num = GPIO_NUM_26, //GPIO_NUM_17
    		.ws_io_num = GPIO_NUM_25, //GPIO_NUM_5
    		.data_out_num = I2S_PIN_NO_CHANGE,
    		.data_in_num = GPIO_NUM_36};

    //Somehow it's needed. If not, noise!
    // pinMode(23, INPUT);
    // pinMode(39, INPUT);
    // pinMode(22, OUTPUT);

    //for recording
    i2s_driver_install(I2S_NUM_1, &i2s_config_rx, 0, NULL);
    i2s_set_pin(I2S_NUM_1, &pin_config_rx);

    //for playback
    i2s_driver_install(I2S_NUM_0, &i2s_config_tx, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);

    int cnt = 0;
    uint64_t buffer = 0;
    char buf[2048];
    // int32_t buf64[512];

    while (1)
    {
        cnt++;
        buffer = 0;

        // int bytes_read = i2s_read_bytes(I2S_NUM_1, buf, sizeof(buf), portMAX_DELAY);

        char *buf_ptr_read = buf;
        char *buf_ptr_write = buf;

        // read whole block of samples
        int bytes_read = 0;
        //while(bytes_read == 0) {
        bytes_read = i2s_read_bytes(I2S_NUM_1, buf, sizeof(buf), portMAX_DELAY);
        //}

        int samples_read = bytes_read / (I2S_BITS_PER_SAMPLE_32BIT / 8);

        //const char samp64[8] = {};
        for (int i = 0; i < samples_read; i++)
        {
            buf_ptr_write[0] = buf_ptr_read[2]; // mid
            buf_ptr_write[1] = buf_ptr_read[3]; // high

            // buf_ptr_write += 1 * (I2S_BITS_PER_SAMPLE_32BIT / 8);
            // buf_ptr_read += 2 * (I2S_BITS_PER_SAMPLE_32BIT / 8);

            // Serial.printf("%d -> %x\n", cnt, buf[i]);
        }

        int readable_bytes = samples_read * (I2S_BITS_PER_SAMPLE_32BIT / 8);

        // // 2 * mono
        i2s_write_bytes(I2S_NUM_0, (const char*)buf, readable_bytes, portMAX_DELAY);
        // // i2s_write_bytes(I2S_NUM_0, (const char*)buf, readable_bytes, portMAX_DELAY);

        // xRingbufferPrintInfo(rb);
        // int r = xRingbufferSend(rb, buf, readable_bytes + 1, 2000 / portTICK_PERIOD_MS);
        // if (!r)
        // {
        //     // printf("Timeout on send!\n");
        // }

        // --> For amplitude chart
        // delay(1);

        // float meanval = 0;
        // buf_ptr_read = &buf[0];
        // for (int i = 0; i < (512); i++)
        // {
        // 	uint32_t buf32 = *(uint32_t *)buf_ptr_read;
        // 	meanval += buf32;
        // 	buf_ptr_read += 4;
        // }

        // meanval /= 512;
        // //Serial.println(meanval);

        // float maxsample, minsample;
        // minsample = 10000;
        // maxsample = 0;

        // buf_ptr_read = &buf[0];
        // for (int i = 0; i < (512); i++)
        // {
        // 	uint32_t buf32 = *(uint32_t *)buf_ptr_read;
        // 	float sbuf32 = buf32 - meanval;

        // 	minsample = min(minsample, sbuf32);
        // 	maxsample = max(maxsample, sbuf32);

        // 	buf_ptr_read += 4;
        // }

        // Serial.println((maxsample - minsample) / 1000);

        // delay(50);
    }

    // uint32_t cnt = 0;
    // uint32_t buffer;
    // uint32_t buffer_out = 0;
    // while (1) {
    // 	cnt++;
    // 	buffer = 0;
    // 	int bytes_popped = i2s_pop_sample(I2S_NUM_1, (char*) &buffer,
    // 			portMAX_DELAY);

    // 	buffer_out = buffer << 5;

    // 	if (buffer_out == 0) {
    // 		//For debugging, if out is zero
    // 		// Serial.printf("%d -> %x\n", cnt, (int) buffer_out);
    // 		delay(1);
    // 	} else {
    // 		//Just playback for now
    // 		i2s_push_sample(I2S_NUM_0, (char*) &buffer_out, portMAX_DELAY);
    // 	}
    // }

    vTaskDelete(NULL);
}

static void init_i2s()
{
	// TX: I2S_NUM_0
	i2s_config_t i2s_config_tx = {
		.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
		.sample_rate = sample_rate,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, // 2-channels
		.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S_MSB),
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Interrupt level 1
		.dma_buf_count = 32,					 // number of buffers, 128 max.
		.dma_buf_len = 32 * 2					 // size of each buffer		
	};

	i2s_pin_config_t pin_config_tx = {
		.bck_io_num = GPIO_NUM_26,
		.ws_io_num = GPIO_NUM_25,
		.data_out_num = GPIO_NUM_22,
		.data_in_num = I2S_PIN_NO_CHANGE
	};

	i2s_driver_install(I2S_NUM_0, &i2s_config_tx, 0, NULL);
	i2s_set_pin(I2S_NUM_0, &pin_config_tx);

	// RX: I2S_NUM_1 
	i2s_config_t i2s_config_rx = {
		.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX), // Only TX
		.sample_rate = sample_rate,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, // Only 8-bit DAC support
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, // 2-channels
		.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S_MSB),
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Interrupt level 1
		.dma_buf_count = 32,					 // number of buffers, 128 max.
		.dma_buf_len = 32 * 2					 // size of each buffer
	};

	i2s_pin_config_t pin_config_rx = {
		.bck_io_num = GPIO_NUM_17, //GPIO_NUM_26,
		.ws_io_num = GPIO_NUM_5, //GPIO_NUM_25,
		.data_out_num = I2S_PIN_NO_CHANGE,
		.data_in_num = GPIO_NUM_39};

	i2s_driver_install(I2S_NUM_1, &i2s_config_rx, 0, NULL);
	i2s_set_pin(I2S_NUM_1, &pin_config_rx);

	//pinMode(39, INPUT);
	// pinMode(22, OUTPUT);
}

/*
void task_megaphone(void *pvParams)
{
	uint16_t buf_len = 1024;
	char *buf = (char*)calloc(buf_len, sizeof(char));

	struct timeval tv = {0};
	struct timezone *tz = {0};
	gettimeofday(&tv, &tz);
	uint64_t micros = tv.tv_usec + tv.tv_sec * 1000000;
	uint64_t micros_prev = micros;
	uint64_t delta = 0;

	init_i2s();

    ESP_LOGI(TAG2, "Opening file for recording");
	FILE *f;

    // Open file
    f = fopen("/spiflash/hello.wav", "wb");
	if (f == NULL) {
		ESP_LOGE(TAG2, "Failed to open file for writing");
        vTaskDelete(NULL);
		return;
	}

	int cnt = 0;
	int bytes_written = 0;
    unsigned elapsedTime = millis();

	while (1)
	{
		char *buf_ptr_read = buf;
		char *buf_ptr_write = buf;

		// read whole block of samples
		int bytes_read = 0;
		while (bytes_read == 0)
		{
			bytes_read = i2s_read_bytes(I2S_NUM_1, buf, buf_len, portMAX_DELAY);
		}

		uint32_t samples_read = bytes_read / 2 / (I2S_BITS_PER_SAMPLE_32BIT / 8);

		//  convert 2x 32 bit stereo -> 1 x 16 bit mono
		for (int i = 0; i < samples_read; i++)
		{
			// const char samp32[4] = {ptr_l[0], ptr_l[1], ptr_r[0], ptr_r[1]};

			// left
			buf_ptr_write[0] = buf_ptr_read[2]; // mid
			buf_ptr_write[1] = buf_ptr_read[3]; // high

			// right
			buf_ptr_write[2] = buf_ptr_write[0]; // mid
			buf_ptr_write[3] = buf_ptr_write[1]; // high

			buf_ptr_write += 2 * (I2S_BITS_PER_SAMPLE_16BIT / 8);
			buf_ptr_read += 2 * (I2S_BITS_PER_SAMPLE_32BIT / 8);
		}

        bytes_written = samples_read * 2 * (I2S_BITS_PER_SAMPLE_16BIT / 8);
        fwrite((const char*)buf, sizeof(char), bytes_written, f);

        cnt += samples_read;

		if (cnt >= sample_rate)
		{
			gettimeofday(&tv, &tz);
			micros = tv.tv_usec + tv.tv_sec * 1000000;
			delta = micros - micros_prev;
			micros_prev = micros;
			printf("%d samples in %" PRIu64 " usecs\n", cnt, delta);

			cnt = 0;
		}

        if (millis() - elapsedTime > 10000)
        {
            printf("Recording done\n");
            elapsedTime = millis();
            break;
        }
	}

    fclose(f);

    free(buf);
    // buf = (char*)calloc(buf_len, sizeof(char));
    
    //reopen
    // Open file for reading
	ESP_LOGI(TAG2, "Reading file");
	f = fopen("/spiflash/hello.wav", "rb");
	if (f == NULL) {
		ESP_LOGE(TAG2, "Failed to open file for reading");
        vTaskDelete(NULL);
		return;
	}

	i2s_stop(I2S_NUM_1);
	delay(100);

    int bufferlen = -1;
	int buffer_index = 0;
	int16_t bufferRead[256*5];
	int readed = 0;
	bool i2s_full = false;
	unsigned int sample_val;

    while (1)
	{
		readed = fread((char*)bufferRead, sizeof(char), sizeof(bufferRead), f);

		if (readed > 0) {
//			printf("wavRead %d\r\n", readed);
			// bufferlen = readed / sizeof(bufferRead[0]);
			// buffer_index = 0;

//			delay(1);
			i2s_write_bytes(I2S_NUM_0, (const char*)bufferRead, readed, portMAX_DELAY);

		} else {
			printf("Stop playing\n");

			i2s_stop(I2S_NUM_0);
			break;
		}

        // char *buf_ptr_read = buf;
		// char *buf_ptr_write = buf;

		// // read whole block of samples
		// int bytes_read = 0;

        // bytes_read = fread(buf, sizeof(char), buf_len, f);

		// if (bytes_read > 0) {

        //     uint32_t samples_read = bytes_read / 2 / (I2S_BITS_PER_SAMPLE_32BIT / 8);

        //     //  convert 2x 32 bit stereo -> 1 x 16 bit mono
        //     for (int i = 0; i < samples_read; i++)
        //     {
        //         // const char samp32[4] = {ptr_l[0], ptr_l[1], ptr_r[0], ptr_r[1]};

        //         // left
        //         buf_ptr_write[0] = buf_ptr_read[2]; // mid
        //         buf_ptr_write[1] = buf_ptr_read[3]; // high

        //         // right
        //         buf_ptr_write[2] = buf_ptr_write[0]; // mid
        //         buf_ptr_write[3] = buf_ptr_write[1]; // high

        //         buf_ptr_write += 2 * (I2S_BITS_PER_SAMPLE_16BIT / 8);
        //         buf_ptr_read += 2 * (I2S_BITS_PER_SAMPLE_32BIT / 8);
        //     }

        //     bytes_written = samples_read * 2 * (I2S_BITS_PER_SAMPLE_16BIT / 8);
        //     i2s_write_bytes(I2S_NUM_0, buf, bytes_written, portMAX_DELAY);

		// } else {
		// 	printf("Stop playing\n");

		// 	i2s_stop(I2S_NUM_0);
		// 	break;
		// }
	}    

    fclose(f);

    vTaskDelete(NULL);
}
*/

/*
void task_megaphone(void *pvParams)
{
	uint16_t buf_len = 1024;
	char *buf = (char*)calloc(buf_len, sizeof(char));

	struct timeval tv = {0};
	struct timezone *tz = {0};
	gettimeofday(&tv, &tz);
	uint64_t micros = tv.tv_usec + tv.tv_sec * 1000000;
	uint64_t micros_prev = micros;
	uint64_t delta = 0;

	init_i2s();

	int cnt = 0;
	int bytes_written = 0;

	while (1)
	{
		char *buf_ptr_read = buf;
		char *buf_ptr_write = buf;

		// read whole block of samples
		int bytes_read = 0;
		while (bytes_read == 0)
		{
			bytes_read = i2s_read_bytes(I2S_NUM_1, buf, buf_len, portMAX_DELAY);
		}

		uint32_t samples_read = bytes_read / 2 / (I2S_BITS_PER_SAMPLE_32BIT / 8);

		//  convert 2x 32 bit stereo -> 1 x 16 bit mono
		for (int i = 0; i < samples_read; i++)
		{
			// const char samp32[4] = {ptr_l[0], ptr_l[1], ptr_r[0], ptr_r[1]};

			// left
			buf_ptr_write[0] = buf_ptr_read[2]; // mid
			buf_ptr_write[1] = buf_ptr_read[3]; // high

			// right
			buf_ptr_write[2] = buf_ptr_write[0]; // mid
			buf_ptr_write[3] = buf_ptr_write[1]; // high

			buf_ptr_write += 2 * (I2S_BITS_PER_SAMPLE_16BIT / 8);
			buf_ptr_read += 2 * (I2S_BITS_PER_SAMPLE_32BIT / 8);
		}

		// local echo
		bytes_written = samples_read * 2 * (I2S_BITS_PER_SAMPLE_16BIT / 8);
		i2s_write_bytes(I2S_NUM_0, buf, bytes_written, portMAX_DELAY);

		cnt += samples_read;

		if (cnt >= sample_rate)
		{
			gettimeofday(&tv, &tz);
			micros = tv.tv_usec + tv.tv_sec * 1000000;
			delta = micros - micros_prev;
			micros_prev = micros;
			printf("%d samples in %" PRIu64 " usecs\n", cnt, delta);

			cnt = 0;
		}
	}
}
*/

#endif
