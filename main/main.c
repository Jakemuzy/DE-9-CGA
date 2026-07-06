#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "esp_sleep.h"
#include "soc/clk_tree_defs.h"

#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"

// Measured in Hz
// TODO: this is actually a range, account for it 
#define VSYNC_RATE 60
#define HSYNC_RATE 15750

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 200

#define GROUND_ONE 1
#define GROUND_TWO 2
#define RED GPIO_NUM_13
#define GREEN GPIO_NUM_15
#define BLUE GPIO_NUM_2
#define INTENSITY GPI_NUM_12
// Pin 7 not in use for CGA 
#define H_SYNC GPIO_NUM_14
#define V_SYNC GPIO_NUM_4

/* 
   FOREWORD:
	The Samsung Santron SC-452C was a 1989 CRT monitor.
	   It features 16-bit color depth, and sports 
	    a legacy DB-9 CGA video input connector.
	 For purposes of this project, we aim to recreate
	     this video output via modern hardware.

   NOTES:
	- 
*/


const uint64_t DESIRED_CLOCK_RATE = SCREEN_WIDTH * SCREEN_HEIGHT * VSYNC_RATE;
const uint32_t CLOCK_RATE = 0;
static const char* TAG = "ESP32";

void app_main()
{

}

void Db9Clock()
{
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_cfg = {
	.clk_src = LCD_CLK_SRC_DEFAULT,
	.data_gpio_nums = {
	    RED,
	    GREEN,
	    BLUE,
	    INTENSITY,
	    H_SYNC,
	    V_SYNC,
	    GPIO_NUM_23, // Empty
	    GPIO_NUM_22, // Empty
	},
	.bus_width = 8,
	.max_transfer_bytes = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint8_t),
	// Let the chip choose dma_burst_size
    };

    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_cfg, &i80_bus));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_cfg = {
        .pclk_hz = DESIRED_CLOCK_RATE,
	.lcd_cmd_bits = 8,	// No clue, best guess
	.lcd_param_bits = 8,
	.trans_queue_depth = 10, // 10 seems reasonable, or could do 640 since that would allow us to write a whole scanline
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_cfg, &io_handle));

    //esp_lcd_panel_dev_handle_t = NULL;
    esp_lcd_panel_dev_config_t panel_cfg = {
	.bits_per_pixel = 4,
	.reset_gpio_num = -1,
	.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(io_handle, &panel_cfg, &panel_handle);
}

/*
void Db9Clock()
{
    //i2s_clock_src_t::I2S_CLK_SRC_APLL 
    // Can only properly pinout using parallel mode
    static i2s_chan_handle_t

    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &tx_chan, NULL));
    i2s_chan_config_t rx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&rx_chan_cfg, NULL, &rx_chan));

    i2s_std_config_t tx_std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,    
            .bclk = EXAMPLE_STD_BCLK_IO1,
            .ws   = EXAMPLE_STD_WS_IO1,
            .dout = EXAMPLE_STD_DOUT_IO1,
            .din  = EXAMPLE_STD_DIN_IO1,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_chan, &tx_std_cfg));

    #include "driver/i2s.h"

// Define your custom I2S parallel configuration
    i2s_parallel_config_t cfg = {
        .gpio_bus = {
            22, // D0 -> R
            19, // D1 -> G
            21, // D2 -> B
            5,  // D3 -> I
            18, // D4 -> HSync
            23, // D5 -> VSync
            -1, // D6 -> Unused
            -1  // D7 -> Unused
        },
    .gpio_clk = -1,           // No external pixel clock pin needed for DB-9/CGA
    .bits = I2S_PARALLEL_BITS_8,
    .clkspeed_hz = 14318180,  // Target video pixel clock (e.g., 14.318 MHz for NTSC/CGA)
    .bufa = dma_buffer_a,     // DMA buffer pointers containing pixel + sync data
    .bufb = dma_buffer_b
    };

    // REMEMBER: hsync and vsync are typically pulled high when writing. When pulled low it tells
    // the hardware to switch scanlines. Vsync is pulled low when it hits the bottom right of the screen
    // indicating that a new frame is about to be drawn and to move the ray to the top left of the screen.
    // Hsync is pulled low when it hits the right of the screen, allowing for hte rays to move to the left
    // of the screen on the next scanline. We can encode this information via the memory, since each distance
    // in memory equates to consistent timing. So we would load the entire dma buffer with our image and 
    // let the parallel i2s send out the data constantly
}
*/
