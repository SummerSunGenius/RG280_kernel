
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/gfp.h>
#include <linux/gpio.h>

#include <video/jzpanel.h>
#include <video/panel-rg280.h>


struct rg280 {
	struct rg280_platform_data *pdata;
};

static void SPI_WriteComm(struct rg280_platform_data *pdata, u8 data)
{
  int bit;
 
  gpio_direction_output(pdata->gpio_enable, 0);
  gpio_direction_output(pdata->gpio_clock, 0);
  gpio_direction_output(pdata->gpio_data, 0);
  udelay(10);
  gpio_direction_output(pdata->gpio_clock, 1);
  udelay(10);
  for(bit=7; bit>=0; bit--){
    gpio_direction_output(pdata->gpio_clock, 0);
    gpio_direction_output(pdata->gpio_data, (data >> bit) & 1);
    udelay(10);
    gpio_direction_output(pdata->gpio_clock, 1);
    udelay(10);
  }
  gpio_direction_output(pdata->gpio_enable, 1);
}
 
static void SPI_WriteData(struct rg280_platform_data *pdata, u8 data)
{
  int bit;
 
  gpio_direction_output(pdata->gpio_enable, 0);
  gpio_direction_output(pdata->gpio_clock, 0);
  gpio_direction_output(pdata->gpio_data, 1);
  udelay(10);
  gpio_direction_output(pdata->gpio_clock, 1);
  udelay(10);
  for(bit=7; bit>=0; bit--){
    gpio_direction_output(pdata->gpio_clock, 0);
    gpio_direction_output(pdata->gpio_data, (data >> bit) & 1);
    udelay(10);
    gpio_direction_output(pdata->gpio_clock, 1);
    udelay(10);
  }
  gpio_direction_output(pdata->gpio_enable, 1);
}


static int rg280_panel_init(void **out_panel, struct device *dev,
			      void *panel_pdata)
{
	struct rg280_platform_data *pdata = panel_pdata;
	struct rg280 *panel;
	int ret;

	panel = devm_kzalloc(dev, sizeof(*panel), GFP_KERNEL);
	if (!panel) {
		dev_err(dev, "Failed to alloc panel data\n");
		return -ENOMEM;
	}

	panel->pdata = pdata;

	*out_panel = panel;

	/* Reserve GPIO pins. */

	ret = devm_gpio_request(dev, pdata->gpio_reset, "LCD panel reset");
	if (ret) {
		dev_err(dev,
			"Failed to request LCD panel reset pin: %d\n", ret);
		return ret;
	}

	ret = devm_gpio_request(dev, pdata->gpio_clock, "LCD 3-wire clock");
	if (ret) {
		dev_err(dev,
			"Failed to request LCD panel 3-wire clock pin: %d\n",
			ret);
		return ret;
	}

	ret = devm_gpio_request(dev, pdata->gpio_enable, "LCD 3-wire enable");
	if (ret) {
		dev_err(dev,
			"Failed to request LCD panel 3-wire enable pin: %d\n",
			ret);
		return ret;
	}

	ret = devm_gpio_request(dev, pdata->gpio_data, "LCD 3-wire data");
	if (ret) {
		dev_err(dev,
			"Failed to request LCD panel 3-wire data pin: %d\n",
			ret);
		return ret;
	}

	/* Set initial GPIO pin directions and value. */

	gpio_direction_output(pdata->gpio_clock,  1);
	gpio_direction_output(pdata->gpio_enable, 1);
	gpio_direction_output(pdata->gpio_data,   0);

	return 0;
}

static void rg280_panel_exit(void *panel)
{
}

static void rg280_panel_enable(void *panel)
{
	struct rg280_platform_data *pdata = ((struct rg280 *)panel)->pdata;
	int i;

	/* Reset LCD panel. */
	gpio_direction_output(pdata->gpio_reset, 0);
	udelay(1000);
	gpio_direction_output(pdata->gpio_reset, 1);
	udelay(1000);
#if 0
	/* Init panel registers. */
	SPI_WriteComm(pdata, 0x02);SPI_WriteData(pdata, 0x7f);
	SPI_WriteComm(pdata, 0x03);SPI_WriteData(pdata, 0x0A);
	SPI_WriteComm(pdata, 0x04);SPI_WriteData(pdata, 0x80);  
	SPI_WriteComm(pdata, 0x06);SPI_WriteData(pdata, 0x90);  
	SPI_WriteComm(pdata, 0x08);SPI_WriteData(pdata, 0x28);
	SPI_WriteComm(pdata, 0x09);SPI_WriteData(pdata, 0x20);
	SPI_WriteComm(pdata, 0x0a);SPI_WriteData(pdata, 0x20);
	SPI_WriteComm(pdata, 0x0c);SPI_WriteData(pdata, 0x10);
	SPI_WriteComm(pdata, 0x0d);SPI_WriteData(pdata, 0x10);
	SPI_WriteComm(pdata, 0x0e);SPI_WriteData(pdata, 0x10);
	SPI_WriteComm(pdata, 0x10);SPI_WriteData(pdata, 0x7F);
	SPI_WriteComm(pdata, 0x11);SPI_WriteData(pdata, 0x3F);
#endif
}

static void rg280_panel_disable(void *panel)
{
	struct rg280_platform_data *pdata = ((struct rg280 *)panel)->pdata;

}

struct panel_ops rg280_panel_ops = {
	.init		= rg280_panel_init,
	.exit		= rg280_panel_exit,
	.enable		= rg280_panel_enable,
	.disable	= rg280_panel_disable,
};
