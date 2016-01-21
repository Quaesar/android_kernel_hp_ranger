/*
 * rt5636_ioctl.h  --  RT5636 ALSA SoC audio driver IO control
 *
 * Copyright 2012 Realtek Microelectronics
 * Author: Bard <bardliao@realtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/spi/spi.h>
#include <sound/soc.h>
#include "rt56xx_ioctl.h"
#include "rt5636_ioctl.h"
#include "rt5636.h"

int rt5636_ioctl_common(struct snd_hwdep *hw, struct file *file,
			unsigned int cmd, unsigned long arg)
{
	return 0;
}
EXPORT_SYMBOL_GPL(rt5636_ioctl_common);
