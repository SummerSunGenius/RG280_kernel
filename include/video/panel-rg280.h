#ifndef __PANEL_RG280_H
#define __PANEL_RG280_H

struct rg280_platform_data {
	int gpio_reset;
	int gpio_clock;
	int gpio_enable;
	int gpio_data;
};

extern struct panel_ops rg280_panel_ops;
#endif
