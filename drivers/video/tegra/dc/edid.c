/*
 * drivers/video/tegra/dc/edid.c
 *
 * Copyright (C) 2010 Google, Inc.
 * Author: Erik Gilling <konkers@android.com>
 *
 * Copyright (c) 2010-2013, NVIDIA CORPORATION, All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */


#include <linux/debugfs.h>
#include <linux/fb.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>
#include "foxconn.h"

#include "edid.h"

struct tegra_edid_pvt {
	struct kref			refcnt;
	struct tegra_edid_hdmi_eld	eld;
	bool				support_stereo;
	bool				support_underscan;
	bool				support_audio;
	int			        hdmi_vic_len;
	u8			        hdmi_vic[7];
	/* Note: dc_edid must remain the last member */
	struct tegra_dc_edid		dc_edid;
};

struct tegra_edid {
	struct i2c_client	*client;
	struct i2c_board_info	info;
	int			bus;

	struct tegra_edid_pvt	*data;

	struct mutex		lock;
};
#define USE_DEFAULT_EDID /* define this for using default EDID */

#ifdef USE_DEFAULT_EDID
#if Foxconn_1600x900	
u8 default_edid_data[] = {
  0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x30, 0xE4, 0xCA, 0x01, 0x00, 0x00, 0x00, 0x00,	
  0x00, 0x12, 0x01, 0x03, 0x80, 0x26, 0x15, 0x78, 0x0A, 0xA8, 0xC0, 0x9D, 0x58, 0x50, 0x9A, 0x26,	
  0x1C, 0x50, 0x54, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,	
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x2F, 0x26, 0x40, 0xB8, 0x60, 0x84, 0x0C, 0x30, 0x30, 0x30,	
  0x23, 0x00, 0x7E, 0xD7, 0x10, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00,	
  0x00, 0x00, 0x4C, 0x47, 0x44, 0x69, 0x73, 0x70, 0x6C, 0x61, 0x79, 0x0A, 0x00, 0x00, 0x00, 0xFE,	
  0x00, 0x4C, 0x50, 0x31, 0x37, 0x33, 0x57, 0x44, 0x31, 0x2D, 0x54, 0x4C, 0x41, 0x31, 0x00, 0x78,	

	// extension blocks
	0x02, 0x03, 0x2f, 0x72, 0x4c, 0x20, 0x13, 0x84, 0x14, 0x05, 0x90, 0x03, 0x02, 0x01, 0x12, 0x1f,
	0x11, 0x83, 0x01, 0x00, 0x00, 0x75, 0x03, 0x0c, 0x00, 0x10, 0x00, 0x00, 0x21, 0x20, 0xc0, 0x0b,
	0x01, 0x01, 0x00, 0x07, 0x00, 0x10, 0x20, 0x38, 0x10, 0x48, 0x10, 0x23, 0x09, 0x7f, 0x07, 0x02,
	0x3a, 0x80, 0x18, 0x71, 0x38, 0x2d, 0x40, 0x58, 0x2c, 0x45, 0x00, 0x56, 0x50, 0x21, 0x00, 0x00,
	0x1e, 0x01, 0x1d, 0x00, 0x72, 0x51, 0xd0, 0x1e, 0x20, 0x6e, 0x28, 0x55, 0x00, 0x56, 0x50, 0x21,
	0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60,
};
#else
u8 default_edid_data[] = {
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x3a, 0xc4, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x20, 0x17, 0x01, 0x03, 0x80, 0x3c, 0x22, 0x78, 0x2a, 0x7b, 0x65, 0xa4, 0x55, 0x4a, 0xa2, 0x27,
	0x0c, 0x50, 0x54, 0xbf, 0xef, 0x80, 0x71, 0x4f, 0x81, 0x00, 0x81, 0x80, 0x95, 0x00, 0x95, 0x0f,
	0xb3, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3a, 0x80, 0x18, 0x71, 0x38, 0x2d, 0x40, 0x58, 0x2c,
	0x45, 0x00, 0x56, 0x50, 0x21, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x38, 0x4b, 0x1e,
	0x53, 0x11, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x4e,
	0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x53, 0x41, 0x4d, 0x50, 0x4c, 0x45, 0x00, 0x00, 0x00, 0xff,
	0x00, 0x30, 0x30, 0x30, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x01, 0xb5,
	// extension blocks
	0x02, 0x03, 0x2f, 0x72, 0x4c, 0x20, 0x13, 0x84, 0x14, 0x05, 0x90, 0x03, 0x02, 0x01, 0x12, 0x1f,
	0x11, 0x83, 0x01, 0x00, 0x00, 0x75, 0x03, 0x0c, 0x00, 0x10, 0x00, 0x00, 0x21, 0x20, 0xc0, 0x0b,
	0x01, 0x01, 0x00, 0x07, 0x00, 0x10, 0x20, 0x38, 0x10, 0x48, 0x10, 0x23, 0x09, 0x7f, 0x07, 0x02,
	0x3a, 0x80, 0x18, 0x71, 0x38, 0x2d, 0x40, 0x58, 0x2c, 0x45, 0x00, 0x56, 0x50, 0x21, 0x00, 0x00,
	0x1e, 0x01, 0x1d, 0x00, 0x72, 0x51, 0xd0, 0x1e, 0x20, 0x6e, 0x28, 0x55, 0x00, 0x56, 0x50, 0x21,
	0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60,
};
#endif
#endif

#if defined(DEBUG) || defined(CONFIG_DEBUG_FS)
static int tegra_edid_show(struct seq_file *s, void *unused)
{
	struct tegra_edid *edid = s->private;
	struct tegra_dc_edid *data;
	u8 *buf;
	int i;

	data = tegra_edid_get_data(edid);
	if (!data) {
		seq_printf(s, "No EDID\n");
		return 0;
	}

	buf = data->buf;

	for (i = 0; i < data->len; i++) {
		if (i % 16 == 0)
			seq_printf(s, "edid[%03x] =", i);

		seq_printf(s, " %02x", buf[i]);

		if (i % 16 == 15)
			seq_printf(s, "\n");
	}

	tegra_edid_put_data(data);

	return 0;
}
#endif

#ifdef CONFIG_DEBUG_FS
static int tegra_edid_debug_open(struct inode *inode, struct file *file)
{
	return single_open(file, tegra_edid_show, inode->i_private);
}

static const struct file_operations tegra_edid_debug_fops = {
	.open		= tegra_edid_debug_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

void tegra_edid_debug_add(struct tegra_edid *edid)
{
	char name[] = "edidX";

	snprintf(name, sizeof(name), "edid%1d", edid->bus);
	debugfs_create_file(name, S_IRUGO, NULL, edid, &tegra_edid_debug_fops);
}
#else
void tegra_edid_debug_add(struct tegra_edid *edid)
{
}
#endif

#ifdef DEBUG
static char tegra_edid_dump_buff[16 * 1024];

static void tegra_edid_dump(struct tegra_edid *edid)
{
	struct seq_file s;
	int i;
	char c;

	memset(&s, 0x0, sizeof(s));

	s.buf = tegra_edid_dump_buff;
	s.size = sizeof(tegra_edid_dump_buff);
	s.private = edid;

	tegra_edid_show(&s, NULL);

	i = 0;
	while (i < s.count ) {
		if ((s.count - i) > 256) {
			c = s.buf[i + 256];
			s.buf[i + 256] = 0;
			printk("%s", s.buf + i);
			s.buf[i + 256] = c;
		} else {
			printk("%s", s.buf + i);
		}
		i += 256;
	}
}
#else
static void tegra_edid_dump(struct tegra_edid *edid)
{
}
#endif

 u8 DataA[ ] = {0x0,0xff,0xff,0xff,0xff,0xff,0xff,0x0
  					,0x22,0xf0,0xa9,0x26,0x1,0x1,0x1,0x1
 						,0x2a,0x11,0x1,0x3 ,0x80 ,0x2f ,0x1e ,0x78
 						,0xee ,0xb5 ,0x35 ,0xa5 ,0x56 ,0x4a ,0x9a ,0x25
 						,0x10 ,0x50 ,0x54 ,0xa5 ,0x6b ,0x80 ,0x71 ,0x0
 						,0x81 ,0x40 ,0x81 ,0x80 ,0x95 ,0x0 ,0xa9 ,0x0
 						,0xb3 ,0x0 ,0x1 ,0x1 ,0x1 ,0x1 ,0x21 ,0x39
 						,0x90 ,0x30 ,0x62 ,0x1a ,0x27 ,0x40 ,0x68 ,0xb0
 						,0x36	,0x0 ,0xd9 ,0x28 ,0x11 ,0x0 ,0x0 ,0x1c
 						,0x0 ,0x0 ,0x0 ,0xfd ,0x0 ,0x30 ,0x4c ,0x18
 						,0x53 ,0x10 ,0x0 ,0xa ,0x20 ,0x20 ,0x20 ,0x20
 						,0x20 ,0x20 ,0x0 ,0x0 ,0x0 ,0xfc ,0x0 ,0x48
 						,0x50 ,0x20 ,0x52 ,0x41 ,0x4e ,0x47 ,0x45 ,0x52
 						,0x32 ,0x31 ,0x49 ,0x4e ,0x0 ,0x0 ,0x0 ,0xff
 						,0x0 ,0x48 ,0x57 ,0x50 ,0x33 ,0x31 ,0x33 ,0x35
 						,0x20 ,0x20 ,0x20 ,0xa ,0x20 ,0x20 ,0x1 ,0x4c};
 						
 u8 DataB[ ] = {0x2,0x3,0x1f,0xf1,0x4c,0x84,0x2,0x1
						,0x7,0x16,0x11,0x10,0x1f,0x3,0x12,0x5
						,0x13,0x65,0x3,0xc,0x0,0x10,0x0,0x23
						,0x9,0x7,0x7,0x83,0x1,0x0,0x0,0x8c
						,0xa,0xd0,0x8a,0x20,0xe0,0x2d,0x10,0x10
						,0x3e,0x96,0x0,0xd9,0x28,0x11,0x0,0x0
						,0x18,0x8c,0xa,0xd0,0x90,0x20,0x40,0x31
						,0x20,0xc,0x40,0x55,0x0,0xd9,0x28,0x11
						,0x0,0x0,0x19,0x1,0x1d,0x80,0x18,0x71
						,0x1c,0x16,0x20,0x58,0x2c,0x25,0x0,0xd9
						,0x28,0x11,0x0,0x0,0x9f,0x1,0x1d,0x0
						,0xbc,0x52,0xd0,0x1e,0x20,0xb8,0x28,0x55
						,0x40,0xd9,0x28,0x11,0x0,0x0,0x1f,0x1
						,0x1d,0x80,0xd0,0x72,0x1c,0x16,0x20,0x10
						,0x2c,0x25,0x80,0xd9,0x28,0x11,0x0,0x0
						,0x9e,0x0,0x0,0x0,0x0,0x0,0x0,0x28}; 		

int tegra_edid_read_block(struct tegra_edid *edid, int block, u8 *data)
{
	u8 block_buf[] = {block >> 1};
	u8 cmd_buf[] = {(block & 0x1) * 128};
	int status;
	u8 checksum = 0;
	u8 i;
	int t,k;
	k = 2;
if(k==2){
	for (t = 0; t < 128; t++) {
		if(block==0){
			data[t]=DataA[t];
		}else{
			data[t]=DataB[t];
		}	
	}

}else{		
	struct i2c_msg msg[] = {
		{
			.addr = 0x30,
			.flags = 0,
			.len = 1,
			.buf = block_buf,
		},
		{
			.addr = 0x50,
			.flags = 0,
			.len = 1,
			.buf = cmd_buf,
		},
		{
			.addr = 0x50,
			.flags = I2C_M_RD,
			.len = 128,
			.buf = data,
		}};
	struct i2c_msg *m;
	int msg_len;

	if (block > 1) {
		msg_len = 3;
		m = msg;
	} else {
		msg_len = 2;
		m = &msg[1];
	}

	status = i2c_transfer(edid->client->adapter, m, msg_len);
	
	printk("Block:%d \r\n",block);
	for (t = 0; t < 128; t++) {
		printk("Data[t]   :0x%x \r\n",data[t]);
	}

	if (status < 0)
		return status;

	if (status != msg_len)
		return -EIO;

	for (i = 0; i < 128; i++)
		checksum += data[i];
	if (checksum != 0) {
		pr_err("%s: checksum failed\n", __func__);
		return -EIO;
	}
}
	return 0;
}

int tegra_edid_parse_ext_block(const u8 *raw, int idx,
			       struct tegra_edid_pvt *edid)
{
	const u8 *ptr;
	u8 tmp;
	u8 code;
	int len;
	int i;
	bool basic_audio = false;

	edid->support_audio = 0;
	ptr = &raw[0];

	/* If CEA 861 block get info for eld struct */
	if (edid && ptr) {
		if (*ptr <= 3)
			edid->eld.eld_ver = 0x02;
		edid->eld.cea_edid_ver = ptr[1];

		/* check for basic audio support in CEA 861 block */
		if(raw[3] & (1<<6)) {
			/* For basic audio, set spk_alloc to Left+Right.
			 * If there is a Speaker Alloc block this will
			 * get over written with that value */
			basic_audio = true;
			edid->support_audio = 1;
		}
	}

	if (raw[3] & 0x80)
		edid->support_underscan = 1;
	else
		edid->support_underscan = 0;

	ptr = &raw[4];

	while (ptr < &raw[idx]) {
		tmp = *ptr;
		len = tmp & 0x1f;

		/* HDMI Specification v1.4a, section 8.3.2:
		 * see Table 8-16 for HDMI VSDB format.
		 * data blocks have tags in top 3 bits:
		 * tag code 2: video data block
		 * tag code 3: vendor specific data block
		 */
		code = (tmp >> 5) & 0x7;
		switch (code) {
		case 1:
		{
			edid->eld.sad_count = len;
			edid->eld.conn_type = 0x00;
			edid->eld.support_hdcp = 0x00;
			for (i = 0; (i < len) && (i < ELD_MAX_SAD); i ++)
				edid->eld.sad[i] = ptr[i + 1];
			len++;
			ptr += len; /* adding the header */
			/* Got an audio data block so enable audio */
			if(basic_audio == true)
				edid->eld.spk_alloc = 1;
			break;
		}
		/* case 2 is commented out for now */
		case 3:
		{
			int j = 0;

			if ((ptr[1] == 0x03) &&
				(ptr[2] == 0x0c) &&
				(ptr[3] == 0)) {
				edid->eld.port_id[0] = ptr[4];
				edid->eld.port_id[1] = ptr[5];
			}
			if ((len >= 8) &&
				(ptr[1] == 0x03) &&
				(ptr[2] == 0x0c) &&
				(ptr[3] == 0)) {
				j = 8;
				tmp = ptr[j++];
				/* HDMI_Video_present? */
				if (tmp & 0x20) {
					/* Latency_Fields_present? */
					if (tmp & 0x80)
						j += 2;
					/* I_Latency_Fields_present? */
					if (tmp & 0x40)
						j += 2;
					/* 3D_present? */
					if (j <= len && (ptr[j] & 0x80))
						edid->support_stereo = 1;
					/* HDMI_VIC_LEN */
					if (++j <= len && (ptr[j] & 0xe0)) {
						int k = 0;
						edid->hdmi_vic_len = ptr[j] >> 5;
						for (k = 0; k < edid->hdmi_vic_len; k++)
						    edid->hdmi_vic[k] = ptr[j+k+1];
					}
				}
			}
			if ((len > 5) &&
				(ptr[1] == 0x03) &&
				(ptr[2] == 0x0c) &&
				(ptr[3] == 0)) {

				edid->eld.support_ai = (ptr[6] & 0x80);
			}

			if ((len > 9) &&
				(ptr[1] == 0x03) &&
				(ptr[2] == 0x0c) &&
				(ptr[3] == 0)) {

				edid->eld.aud_synch_delay = ptr[10];
			}
			len++;
			ptr += len; /* adding the header */
			break;
		}
		case 4:
		{
			edid->eld.spk_alloc = ptr[1];
			len++;
			ptr += len; /* adding the header */
			break;
		}
		default:
			len++; /* len does not include header */
			ptr += len;
			break;
		}
	}

	return 0;
}

int tegra_edid_mode_support_stereo(struct fb_videomode *mode)
{
	if (!mode)
		return 0;

	if (mode->xres == 1280 &&
		mode->yres == 720 &&
		((mode->refresh == 60) || (mode->refresh == 50)))
		return 1;

	if (mode->xres == 1920 && mode->yres == 1080 && mode->refresh == 24)
		return 1;

	return 0;
}

static void data_release(struct kref *ref)
{
	struct tegra_edid_pvt *data =
		container_of(ref, struct tegra_edid_pvt, refcnt);
	vfree(data);
}

int tegra_edid_get_monspecs_test(struct tegra_edid *edid,
			struct fb_monspecs *specs, unsigned char *edid_ptr)
{
	int i, j, ret;
	int extension_blocks;
	struct tegra_edid_pvt *new_data, *old_data;
	u8 *data;

	new_data = vmalloc(SZ_32K + sizeof(struct tegra_edid_pvt));
	if (!new_data)
		return -ENOMEM;

	kref_init(&new_data->refcnt);

	new_data->support_stereo = 0;
	new_data->support_underscan = 0;

	data = new_data->dc_edid.buf;
	memcpy(data, edid_ptr, 128);

	memset(specs, 0x0, sizeof(struct fb_monspecs));
	memset(&new_data->eld, 0x0, sizeof(new_data->eld));
	fb_edid_to_monspecs(data, specs);
	if (specs->modedb == NULL) {
		ret = -EINVAL;
		goto fail;
	}

	memcpy(new_data->eld.monitor_name, specs->monitor,
					sizeof(specs->monitor));

	new_data->eld.mnl = strlen(new_data->eld.monitor_name) + 1;
	new_data->eld.product_id[0] = data[0x8];
	new_data->eld.product_id[1] = data[0x9];
	new_data->eld.manufacture_id[0] = data[0xA];
	new_data->eld.manufacture_id[1] = data[0xB];

	extension_blocks = data[0x7e];
	for (i = 1; i <= extension_blocks; i++) {
		memcpy(data+128, edid_ptr+128, 128);

		if (data[i * 128] == 0x2) {
			fb_edid_add_monspecs(data + i * 128, specs);

			tegra_edid_parse_ext_block(data + i * 128,
					data[i * 128 + 2], new_data);

			if (new_data->support_stereo) {
				for (j = 0; j < specs->modedb_len; j++) {
					if (tegra_edid_mode_support_stereo(
						&specs->modedb[j]))
						specs->modedb[j].vmode |=
#ifndef CONFIG_TEGRA_HDMI_74MHZ_LIMIT
						FB_VMODE_STEREO_FRAME_PACK;
#else
						FB_VMODE_STEREO_LEFT_RIGHT;
#endif
				}
			}
		}
	}

	new_data->dc_edid.len = i * 128;

	mutex_lock(&edid->lock);
	old_data = edid->data;
	edid->data = new_data;
	mutex_unlock(&edid->lock);

	if (old_data)
		kref_put(&old_data->refcnt, data_release);

	tegra_edid_dump(edid);
	return 0;
fail:
	vfree(new_data);
	return ret;
}

int tegra_edid_get_monspecs(struct tegra_edid *edid, struct fb_monspecs *specs)
{
	int i;
	int j;
	int ret;
	int extension_blocks;
	struct tegra_edid_pvt *new_data, *old_data;
	u8 *data;

	new_data = vmalloc(SZ_32K + sizeof(struct tegra_edid_pvt));
	if (!new_data)
		return -ENOMEM;

	kref_init(&new_data->refcnt);

	new_data->support_stereo = 0;

	data = new_data->dc_edid.buf;

#ifndef USE_DEFAULT_EDID
	ret = tegra_edid_read_block(edid, 0, data);
	if (ret)
		goto fail;
#else
	memcpy(data, default_edid_data, sizeof(default_edid_data));
#endif

	memset(specs, 0x0, sizeof(struct fb_monspecs));
	memset(&new_data->eld, 0x0, sizeof(new_data->eld));
	fb_edid_to_monspecs(data, specs);
	if (specs->modedb == NULL) {
		ret = -EINVAL;
		goto fail;
	}
	memcpy(new_data->eld.monitor_name, specs->monitor, sizeof(specs->monitor));
	new_data->eld.mnl = strlen(new_data->eld.monitor_name) + 1;
	new_data->eld.product_id[0] = data[0x8];
	new_data->eld.product_id[1] = data[0x9];
	new_data->eld.manufacture_id[0] = data[0xA];
	new_data->eld.manufacture_id[1] = data[0xB];

	extension_blocks = data[0x7e];

	for (i = 1; i <= extension_blocks; i++) {
		ret = tegra_edid_read_block(edid, i, data + i * 128);
		if (ret < 0)
			goto fail;

		if (data[i * 128] == 0x2) {
			fb_edid_add_monspecs(data + i * 128, specs);

			tegra_edid_parse_ext_block(data + i * 128,
					data[i * 128 + 2], new_data);

			if (new_data->support_stereo) {
				for (j = 0; j < specs->modedb_len; j++) {
					if (tegra_edid_mode_support_stereo(
						&specs->modedb[j]))
						specs->modedb[j].vmode |=
#ifndef CONFIG_TEGRA_HDMI_74MHZ_LIMIT
						FB_VMODE_STEREO_FRAME_PACK;
#else
						FB_VMODE_STEREO_LEFT_RIGHT;
#endif
				}
			}

			if (new_data->hdmi_vic_len > 0) {
				int k;
				int l = specs->modedb_len;
				struct fb_videomode *m;
				m = kzalloc((specs->modedb_len + new_data->hdmi_vic_len) *
				    sizeof(struct fb_videomode), GFP_KERNEL);
				if (!m)
				    break;
				memcpy(m, specs->modedb, specs->modedb_len *
				        sizeof(struct fb_videomode));
				for (k = 0; k < new_data->hdmi_vic_len; k++) {
				    unsigned vic = new_data->hdmi_vic[k];
				    if (vic >= HDMI_EXT_MODEDB_SIZE) {
				        pr_warning("Unsupported HDMI VIC %d, ignoring\n", vic);
				        continue;
				    }
				    memcpy(&m[l], &hdmi_ext_modes[vic], sizeof(m[l]));
				    l++;
				}
				kfree(specs->modedb);
				specs->modedb = m;
				specs->modedb_len = specs->modedb_len + new_data->hdmi_vic_len;
			}
		}
	}

	new_data->dc_edid.len = i * 128;

	mutex_lock(&edid->lock);
	old_data = edid->data;
	edid->data = new_data;
	mutex_unlock(&edid->lock);

	if (old_data)
		kref_put(&old_data->refcnt, data_release);

	tegra_edid_dump(edid);
	return 0;

fail:
	vfree(new_data);
	return ret;
}

int tegra_edid_audio_supported(struct tegra_edid *edid)
{
	if ((!edid) || (!edid->data))
		return 0;

	return edid->data->support_audio;
}

int tegra_edid_underscan_supported(struct tegra_edid *edid)
{
	if ((!edid) || (!edid->data))
		return 0;

	return edid->data->support_underscan;
}

int tegra_edid_get_eld(struct tegra_edid *edid, struct tegra_edid_hdmi_eld *elddata)
{
	if (!elddata || !edid->data)
		return -EFAULT;

	memcpy(elddata,&edid->data->eld,sizeof(struct tegra_edid_hdmi_eld));

	return 0;
}

struct tegra_edid *tegra_edid_create(int bus)
{
	struct tegra_edid *edid;
	struct i2c_adapter *adapter;
	int err;

	edid = kzalloc(sizeof(struct tegra_edid), GFP_KERNEL);
	if (!edid)
		return ERR_PTR(-ENOMEM);

	mutex_init(&edid->lock);
	strlcpy(edid->info.type, "tegra_edid", sizeof(edid->info.type));
	edid->bus = bus;
	edid->info.addr = 0x50;
	edid->info.platform_data = edid;

	adapter = i2c_get_adapter(bus);
	if (!adapter) {
		pr_err("can't get adpater for bus %d\n", bus);
		err = -EBUSY;
		goto free_edid;
	}

	edid->client = i2c_new_device(adapter, &edid->info);
	i2c_put_adapter(adapter);

	if (!edid->client) {
		pr_err("can't create new device\n");
		err = -EBUSY;
		goto free_edid;
	}

	tegra_edid_debug_add(edid);

	return edid;

free_edid:
	kfree(edid);

	return ERR_PTR(err);
}

void tegra_edid_destroy(struct tegra_edid *edid)
{
	i2c_release_client(edid->client);
	if (edid->data)
		kref_put(&edid->data->refcnt, data_release);
	kfree(edid);
}

struct tegra_dc_edid *tegra_edid_get_data(struct tegra_edid *edid)
{
	struct tegra_edid_pvt *data;

	mutex_lock(&edid->lock);
	data = edid->data;
	if (data)
		kref_get(&data->refcnt);
	mutex_unlock(&edid->lock);

	return data ? &data->dc_edid : NULL;
}

void tegra_edid_put_data(struct tegra_dc_edid *data)
{
	struct tegra_edid_pvt *pvt;

	if (!data)
		return;

	pvt = container_of(data, struct tegra_edid_pvt, dc_edid);

	kref_put(&pvt->refcnt, data_release);
}

static const struct i2c_device_id tegra_edid_id[] = {
        { "tegra_edid", 0 },
        { }
};

MODULE_DEVICE_TABLE(i2c, tegra_edid_id);

static struct i2c_driver tegra_edid_driver = {
        .id_table = tegra_edid_id,
        .driver = {
                .name = "tegra_edid",
        },
};

static int __init tegra_edid_init(void)
{
        return i2c_add_driver(&tegra_edid_driver);
}

static void __exit tegra_edid_exit(void)
{
        i2c_del_driver(&tegra_edid_driver);
}

module_init(tegra_edid_init);
module_exit(tegra_edid_exit);
