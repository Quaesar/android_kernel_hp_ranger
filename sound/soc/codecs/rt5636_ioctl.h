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

#ifndef __RT5636_IOCTL_H__
#define __RT5636_IOCTL_H__

#include <sound/hwdep.h>
#include <linux/ioctl.h>

int rt5636_ioctl_common(struct snd_hwdep *hw, struct file *file,
			unsigned int cmd, unsigned long arg);
#endif /* __RT5636_IOCTL_H__ */
