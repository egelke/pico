#include "lcd.h"
#include "device.h"
#include "interface.h"

#define SSD1306_I2C_ADDRESS     0x3C

#define SSD1306_HEIGHT          32
#define SSD1306_WIDTH           128
#define SSD1306_PAGE_HEIGHT     8
#define SSD1306_NUM_PAGES       SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT
#define SSD1306_BUF_LEN         (SSD1306_NUM_PAGES * SSD1306_WIDTH)

// commands (see datasheet)
#define SSD1306_SET_CONTRAST        0x81
#define SSD1306_SET_DISPLAY_SOURCE  0xA4
#define SSD1306_SET_NORM_INV        0xA6
#define SSD1306_SET_DISP            0xAE
#define SSD1306_SET_MEM_ADDR        0x20
#define SSD1306_SET_COL_ADDR        0x21
#define SSD1306_SET_PAGE_ADDR       0x22
#define SSD1306_SET_DISP_START_LINE 0x40
#define SSD1306_SET_SEG_REMAP       0xA0
#define SSD1306_SET_MUX_RATIO       0xA8
#define SSD1306_SET_COM_OUT_DIR     0xC0
#define SSD1306_SET_DISP_OFFSET     0xD3
#define SSD1306_SET_COM_PIN_CFG     0xDA
#define SSD1306_SET_DISP_CLK_DIV    0xD5
#define SSD1306_SET_PRECHARGE       0xD9
#define SSD1306_SET_VCOM_DESEL      0xDB
#define SSD1306_SET_CHARGE_PUMP     0x8D
#define SSD1306_SET_HORIZ_SCROLL    0x26
#define SSD1306_SET_SCROLL          0x2E

int ssd1306_init(lcd_device_t *d) {
    int ret = LCD_OK;
    lcd_device_priv_t *dp = lcd_convert_device(d);
    lcd_intf_t *intf = *(dp->intf);
    lcd_intf_meta_t *meta = &(dp->intf_meta);

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_DISP | 0x00); // set display off

    /* memory mapping */
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_MEM_ADDR); // set memory address mode
    ret |= intf->send_cmd(intf, meta, 0x00); // horizontal addressing mode

    /* resolution and layout */
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_DISP_START_LINE); // set display start line to 0

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_SEG_REMAP | 0x01); // set segment re-map
    // column address 127 is mapped to SEG0

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_MUX_RATIO); // set multiplex ratio
    ret |= intf->send_cmd(intf, meta, SSD1306_HEIGHT - 1); // our display is only 32 pixels high

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_COM_OUT_DIR | 0x08); // set COM (common) output scan direction
    // scan from bottom up, COM[N-1] to COM0

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_DISP_OFFSET); // set display offset
    ret |= intf->send_cmd(intf, meta, 0x00); // no offset

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_COM_PIN_CFG); // set COM (common) pins hardware configuration
    ret |= intf->send_cmd(intf, meta, 0x02); // manufacturer magic number

    /* timing and driving scheme */
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_DISP_CLK_DIV); // set display clock divide ratio
    ret |= intf->send_cmd(intf, meta, 0x80); // div ratio of 1, standard freq

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_PRECHARGE); // set pre-charge period
    ret |= intf->send_cmd(intf, meta, 0xF1); // Vcc internally generated on our board

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_VCOM_DESEL); // set VCOMH deselect level
    ret |= intf->send_cmd(intf, meta, 0x30); // 0.83xVcc

    /* display */
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_CONTRAST); // set contrast control
    ret |= intf->send_cmd(intf, meta, 0xFF);

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_DISPLAY_SOURCE | 0x00); // set entire display on to follow RAM content

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_NORM_INV); // set normal (not inverted) display

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_CHARGE_PUMP); // set charge pump
    ret |= intf->send_cmd(intf, meta, 0x14); // Vcc internally generated on our board

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_SCROLL | 0x00); // deactivate horizontal scrolling if set
    // this is necessary as memory writes will corrupt if scrolling was enabled

    //prep the buffer (prefix the right command) and display ram (set the drawing area)
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_COL_ADDR);
    ret |= intf->send_cmd(intf, meta, 0);
    ret |= intf->send_cmd(intf, meta, SSD1306_WIDTH-1);
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_PAGE_ADDR);
    ret |= intf->send_cmd(intf, meta, 0);
    ret |= intf->send_cmd(intf, meta, SSD1306_NUM_PAGES-1);

    //todo::repeat to clear the entire buffer
    memset(intf->dbuf, 0, intf->dbuf_len);
    int buf_len = SSD1306_BUF_LEN;
    while (buf_len > 0) {
        int tx_size = intf->dbuf_len < buf_len ? intf->dbuf_len : buf_len;
        ret |= intf->send_dbuf(intf, meta, tx_size);
        buf_len -= tx_size;
    }

    ret |= intf->send_cmd(intf, meta, SSD1306_SET_DISP | 0x01); // set display on

    return ret;
}


lcd_device_t *lcd_create_ssd1306_emul(lcd_intf_t **intf) {
    lcd_device_priv_t *d = lcd_create_device(intf, MAX_STR_LEN * EMUL_FONT_WITH);

    d->emul = true;
    d->size.columns = 21;
    d->size.lines = 4;
    d->view.columns = 21;
    d->view.lines = 4;

    d->intf_meta.i2c_addr = SSD1306_I2C_ADDRESS;
    d->init = ssd1306_init;
    d->shift_view = NULL;

    return  (lcd_device_t *)d;
}