#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "egelke_oled.h"

#define SSD1306OI2C_ADDR _u(0x3C)
#define SSD1306OI2C_WRITE_MODE _u(0xFE)
#define SSD1306OI2C_READ_MODE _u(0xFF)

#define SSD1306_HEIGHT _u(32)
#define SSD1306_WIDTH _u(128)
#define SSD1306_PAGE_HEIGHT _u(8)
#define SSD1306_NUM_PAGES SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT
#define SSD1306_BUF_LEN (SSD1306_NUM_PAGES * SSD1306_WIDTH)

// commands (see datasheet)
#define SSD1306_SET_CONTRAST _u(0x81)
#define SSD1306_SET_DISPLAY_SOURCE _u(0xA4)
#define SSD1306_SET_NORM_INV _u(0xA6)
#define SSD1306_SET_DISP _u(0xAE)
#define SSD1306_SET_MEM_ADDR _u(0x20)
#define SSD1306_SET_COL_ADDR _u(0x21)
#define SSD1306_SET_PAGE_ADDR _u(0x22)
#define SSD1306_SET_DISP_START_LINE _u(0x40)
#define SSD1306_SET_SEG_REMAP _u(0xA0)
#define SSD1306_SET_MUX_RATIO _u(0xA8)
#define SSD1306_SET_COM_OUT_DIR _u(0xC0)
#define SSD1306_SET_DISP_OFFSET _u(0xD3)
#define SSD1306_SET_COM_PIN_CFG _u(0xDA)
#define SSD1306_SET_DISP_CLK_DIV _u(0xD5)
#define SSD1306_SET_PRECHARGE _u(0xD9)
#define SSD1306_SET_VCOM_DESEL _u(0xDB)
#define SSD1306_SET_CHARGE_PUMP _u(0x8D)
#define SSD1306_SET_HORIZ_SCROLL _u(0x26)
#define SSD1306_SET_SCROLL _u(0x2E)

int ssd1306oI2c_send_cmd(oled_canvas_t *c, uint8_t cmd) {
    // I2C write process expects a control byte followed by data
    // this "data" can be a command or data to follow up a command

    // Co = 1, D/C = 0 => the driver expects a command
    uint8_t buf[2] = {0x80, cmd};
    return i2c_write_blocking(c->hw_addr, (SSD1306OI2C_ADDR & SSD1306OI2C_WRITE_MODE), buf, 2, false) == 2 ? PICO_OK : PICO_ERROR_GENERIC;
}

int ssd1306oI2c_send_buffer(oled_canvas_t *c) {
    return i2c_write_blocking(c->hw_addr, (SSD1306OI2C_ADDR & SSD1306OI2C_WRITE_MODE), c->buffer, c->buffer_len, false) == c->buffer_len ? PICO_OK : PICO_ERROR_GENERIC;
}

int init_ssd1306(oled_canvas_t *c) {
    int ret;

    ret = c->send_cmd(c, SSD1306_SET_DISP | 0x00); // set display off

    /* memory mapping */
    ret = ret || c->send_cmd(c, SSD1306_SET_MEM_ADDR); // set memory address mode
    ret = ret || c->send_cmd(c, 0x00); // horizontal addressing mode

    /* resolution and layout */
    ret = ret || c->send_cmd(c, SSD1306_SET_DISP_START_LINE); // set display start line to 0

    ret = ret || c->send_cmd(c, SSD1306_SET_SEG_REMAP | 0x01); // set segment re-map
    // column address 127 is mapped to SEG0

    ret = ret || c->send_cmd(c, SSD1306_SET_MUX_RATIO); // set multiplex ratio
    ret = ret || c->send_cmd(c, SSD1306_HEIGHT - 1); // our display is only 32 pixels high

    ret = ret || c->send_cmd(c, SSD1306_SET_COM_OUT_DIR | 0x08); // set COM (common) output scan direction
    // scan from bottom up, COM[N-1] to COM0

    ret = ret || c->send_cmd(c, SSD1306_SET_DISP_OFFSET); // set display offset
    ret = ret || c->send_cmd(c, 0x00); // no offset

    ret = ret || c->send_cmd(c, SSD1306_SET_COM_PIN_CFG); // set COM (common) pins hardware configuration
    ret = ret || c->send_cmd(c, 0x02); // manufacturer magic number

    /* timing and driving scheme */
    ret = ret || c->send_cmd(c, SSD1306_SET_DISP_CLK_DIV); // set display clock divide ratio
    ret = ret || c->send_cmd(c, 0x80); // div ratio of 1, standard freq

    ret = ret || c->send_cmd(c, SSD1306_SET_PRECHARGE); // set pre-charge period
    ret = ret || c->send_cmd(c, 0xF1); // Vcc internally generated on our board

    ret = ret || c->send_cmd(c, SSD1306_SET_VCOM_DESEL); // set VCOMH deselect level
    ret = ret || c->send_cmd(c, 0x30); // 0.83xVcc

    /* display */
    ret = ret || c->send_cmd(c, SSD1306_SET_CONTRAST); // set contrast control
    ret = ret || c->send_cmd(c, 0xFF);

    ret = ret || c->send_cmd(c, SSD1306_SET_DISPLAY_SOURCE | source_gddram); // set entire display on to follow RAM content

    ret = ret || c->send_cmd(c, SSD1306_SET_NORM_INV); // set normal (not inverted) display

    ret = ret || c->send_cmd(c, SSD1306_SET_CHARGE_PUMP); // set charge pump
    ret = ret || c->send_cmd(c, 0x14); // Vcc internally generated on our board

    ret = ret || c->send_cmd(c, SSD1306_SET_SCROLL | 0x00); // deactivate horizontal scrolling if set
    // this is necessary as memory writes will corrupt if scrolling was enabled

    //prep the buffer (prefix the right command) and display ram (set the drawing area)
    c->buffer[0] = 0x40;
    ret = ret || c->send_cmd(c, SSD1306_SET_COL_ADDR);
    ret = ret || c->send_cmd(c, 0);
    ret = ret || c->send_cmd(c, SSD1306_WIDTH-1);
    ret = ret || c->send_cmd(c, SSD1306_SET_PAGE_ADDR);
    ret = ret || c->send_cmd(c, 0);
    ret = ret || c->send_cmd(c, SSD1306_NUM_PAGES-1);

    //clear the buffer and the graphical display ram
    memset(c->buffer+1, 0x00, SSD1306_BUF_LEN);
    ret = ret || c->update_gddram(c);

    ret = ret || c->send_cmd(c, SSD1306_SET_DISP | 0x01); // turn display on

    return ret;
}

int ssd1306_set_source(oled_canvas_t *c, display_source_t source) {
    return c->send_cmd(c, SSD1306_SET_DISPLAY_SOURCE | source);
}

int ssd1306_update_gddram(oled_canvas_t *c) {
    return c->send_buffer(c);
}

int ssd1306_draw_hline(oled_canvas_t *c, uint8_t x1, uint8_t y1, uint8_t x2) {
    uint8_t page_index = y1 >> 3;
    uint8_t page_mask = 0x01 << (y1 & 0x03);

    int offset = 1 + page_index * SSD1306_WIDTH;
    for(int i=x1 + offset; i<=x2 + offset; i++) {
        c->buffer[i] |= page_mask;
    }
    return PICO_OK;
}

oled_canvas_t oled_create_ssd1306oI2c(i2c_inst_t *hw_addr) {
    oled_canvas_t canvas;

    canvas.hw_addr = hw_addr;
    canvas.buffer = malloc(SSD1306_BUF_LEN + 1);
    canvas.buffer_len = SSD1306_BUF_LEN + 1;
    canvas.init = init_ssd1306;
    canvas.set_source = ssd1306_set_source;
    canvas.update_gddram = ssd1306_update_gddram;
    canvas.draw_hline = ssd1306_draw_hline;
    canvas.send_cmd = ssd1306oI2c_send_cmd;
    canvas.send_buffer = ssd1306oI2c_send_buffer;

    return canvas;
}

int oled_init(oled_canvas_t *c) {
    return c->init(c);
}

int oled_set_source(oled_canvas_t *c, display_source_t source) {
    return c->set_source(c, source);
}

int oled_draw_hline(oled_canvas_t *c, uint8_t x1, uint8_t y1, uint8_t x2) {
    return c->draw_hline(c, x1, y1, x2);
}

int oled_update_gddram(oled_canvas_t *c) {
    return c->update_gddram(c);
}

int oled_destroy(oled_canvas_t *c) {
    free(c->buffer);
    return PICO_OK;
}



