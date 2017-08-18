
#ifndef MAIN_MB_TESTI2S_H_
#define MAIN_MB_TESTI2S_H_

#include "Arduino.h"
#include <SPIFFS.h>
#include <FS.h>

extern "C" {
#include "driver/i2s.h"
#include "esp_log.h"
}


const int sample_rate = 44100;//22050;
const static char *TAG2 = "I2S";

void tryI2SRecord(void *p)
{
	ESP_LOGI(TAG2, "Opening file for recording");

	fs::FS fs = SPIFFS;

	File file = fs.open("/hello.wav", FILE_WRITE);
	if (!file) {
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

        size_t readable_bytes = samples_read * (I2S_BITS_PER_SAMPLE_16BIT / 8);

        // // 2 * mono
//        i2s_write_bytes(I2S_NUM_0, (const char*)buf, readable_bytes, portMAX_DELAY);

        //fwrite((const char*)buf, sizeof(char), readable_bytes, f);
        file.write((uint8_t*)buf, readable_bytes);

        if (millis() - elapsedTime > 10000) {
        		printf("Recording done\n");
        		elapsedTime = millis();

        		break;
        }
    }

    //fclose(f);
    file.close();

    //reopen
    // Open file for reading
	ESP_LOGI(TAG2, "Reading file");
	//f = fopen("/spiflash/hello.wav", "rb");
	file = fs.open("/hello.wav", FILE_READ);

	if (!file) {
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

	i2s_start(I2S_NUM_0);

	while(1) {

		//readed = fread((char*)buffer_read, sizeof(char), sizeof(buffer_read), f);
		readed = file.read((uint8_t*)buffer_read, sizeof(buffer_read));

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

	//fclose(f);
	file.close();

    vTaskDelete(NULL);
}

void tryI2SPlay(void *p)
{
	ESP_LOGI(TAG2, "Opening file for playing");

	fs::FS fs = SPIFFS;

	i2s_config_t i2s_config_tx = {
		mode : (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
		sample_rate : 22050,//sample_rate,
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

	//for playback
    i2s_driver_install(I2S_NUM_0, &i2s_config_tx, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config_tx);
    i2s_stop(I2S_NUM_0);

    // Open file for reading
	ESP_LOGI(TAG2, "Reading file");
	File file = fs.open("/indonesia.wav", FILE_READ);

	if (!file) {
		ESP_LOGE(TAG2, "Failed to open file for reading");
		return;
	}

//	i2s_set_sample_rates(I2S_NUM_0, 11000); //set sample rates

	uint8_t buf_bytes_per_sample = (I2S_BITS_PER_SAMPLE_16BIT / 8);
	int buffer_len = -1;
	int buffer_index = 0;
	char buffer_read[512];
	int readed = 0;

	i2s_start(I2S_NUM_0);

	while(1) {

		//readed = fread((char*)buffer_read, sizeof(char), sizeof(buffer_read), f);
		readed = file.read((uint8_t*)buffer_read, sizeof(buffer_read));

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

	file.close();
	return;
    vTaskDelete(NULL);
}

#endif
