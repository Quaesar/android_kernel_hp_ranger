
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "rt5636.h"
#include "wolouartdrv.h"

#define RTK_IOCTL
#ifdef RTK_IOCTL
#if defined(CONFIG_SND_HWDEP)
#include "rt56xx_ioctl.h"
#include "rt5636_ioctl.h"
#endif
#endif

#define RT5636_VERSION "0.02 alsa 1.0.25"
#define ALSA_SOC_VERSION "1.0.25"

struct rt5636_priv {
	struct snd_soc_codec *codec;
	unsigned int sysclk;
};

struct rt5636_init_reg{
	u8 reg_index;
	u16 reg_value;	
};

static struct rt5636_init_reg init_list[] = {
//b-{RT5636_MIC_INPUT_VOL, 0x0200},//Mic Input Volume,boost 30db
	{RT5636_MIC_INPUT_VOL, 0x0000},//Mic Input Volume,boost 0db
//b-{RT5636_ADC_CTRL1, 0x0008},//ADC digital volume boost 12db
	{RT5636_ADC_CTRL1, 0x0013},//ADC digital volume boost 28.5dB	
	{RT5636_GPIO_CTRL1, 0x0008},
	{RT5636_DIGITAL_MIC_CTRL, 0x0948},
};
#define RT5636_INIT_NUM ARRAY_SIZE(init_list)

static void rt5636_init_reg_set(struct snd_soc_codec *codec) 
{
	int i;
	
	for (i = 0; i < RT5636_INIT_NUM; i ++) {
		snd_soc_write(codec, init_list[i].reg_index, init_list[i].reg_value);
	}
}

static const u16 rt5636_reg[RT5636_VEND_ID_2 + 1] = {
	[0x00] = 0x8000,
	[0x12] = 0x8080,
	[0x14] = 0xffff,
	[0x24] = 0x0808,
	[0x34] = 0xc000,
	[0x38] = 0x2000,
	[0x3a] = 0x0004,
	[0x40] = 0x0100,
	[0x4a] = 0x8008,
	[0x4b] = 0x0008,
	[0x5c] = 0x0005,
	[0x64] = 0x0206,
	[0x66] = 0x0080,
	[0x7c] = 0x10ec,
	
};


/**
 * rt5636_index_write - Write private register.
 * @codec: SoC audio codec device.
 * @reg: Private register index.
 * @value: Private register Data.
 *
 * Modify private register for advanced setting. It can be written through
 * private index (0x6a) and data (0x6c) register.
 *
 * Returns 0 for success or negative error code.
 */
static int rt5636_index_write(struct snd_soc_codec *codec,
			      unsigned int reg, unsigned int value)
{
	int ret;

	ret = snd_soc_write(codec, 0x6a, reg);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set private addr: %d\n", ret);
		goto err;
	}
	ret = snd_soc_write(codec, 0x6c, value);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set private value: %d\n", ret);
		goto err;
	}
	return 0;

err:
	return ret;
}

/**
 * rt5636_index_read - Read private register.
 * @codec: SoC audio codec device.
 * @reg: Private register index.
 *
 * Read advanced setting from private register. It can be read through
 * private index (0x6a) and data (0x6c) register.
 *
 * Returns private register value or negative error code.
 */
static unsigned int rt5636_index_read(struct snd_soc_codec *codec,
				      unsigned int reg)
{
	int ret;

	ret = snd_soc_write(codec, 0x6a, reg);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set private addr: %d\n", ret);
		return ret;
	}
	return snd_soc_read(codec, 0x6c);
}

static int rt5636_index_update_bits(struct snd_soc_codec *codec,
				    unsigned int reg, unsigned int mask,
				    unsigned int value)
{
	unsigned int old, new;
	int change, ret;

	ret = rt5636_index_read(codec, reg);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to read private reg: %d\n", ret);
		goto err;
	}

	old = ret;
	new = (old & ~mask) | (value & mask);
	change = old != new;
	if (change) {
		ret = rt5636_index_write(codec, reg, new);
		if (ret < 0) {
			dev_err(codec->dev,
				"Failed to write private reg: %d\n", ret);
			goto err;
		}
	}
	return change;

err:
	return ret;
}


static int rt5636_reset(struct snd_soc_codec *codec)
{
	return snd_soc_write(codec, 0, 0);
}

static int rt5636_volatile_register(
	struct snd_soc_codec *codec, unsigned int reg)
{
	switch (reg) {
	case RT5636_GPIO_CTRL1:
	case RT5636_GPIO_CTRL2:
	case RT5636_PRIV_DATA:
	case RT5636_VEND_ID_1:
	case RT5636_VEND_ID_2:
		return 1;
	default:
		return 0;
	}
}

static int rt5636_readable_register(
	struct snd_soc_codec *codec, unsigned int reg)
{
	switch (reg) {
	case RT5636_RESET:
	case RT5636_MIC_INPUT_VOL:
	case RT5636_ADC_CTRL1:
	case RT5636_ADC_CTRL2:
	case RT5636_MIC_CTRL:
	case RT5636_DIGITAL_MIC_CTRL:
	case RT5636_AUD_DATA_PORT_CTRL:
	case RT5636_ADC_CLK_CTRL:
	case RT5636_PWR_MANAG_ADD1:
	case RT5636_GENE_PPS_CTRL:
	case RT5636_GLB_CLK_CTRL:
	case RT5636_PLL_CTRL:
	case RT5636_GPIO_CTRL1:
	case RT5636_GPIO_CTRL2:
	case RT5636_ZC_FUNC_CTRL:
	case RT5636_ALC_CTRL1:
	case RT5636_ALC_CTRL2:	
	case RT5636_ALC_CTRL3:
	case RT5636_VEND_ID_1:
	case RT5636_VEND_ID_2:
		return 1;
	default:
		return 0;
	}
}

int rt5636_headset_detect(struct snd_soc_codec *codec, int jack_insert)
{
	int i, headset = 0, headphone = 0;
	int jack_type;

	if(jack_insert) {
		printk("enter %s codec->dapm.bias_level=%d\n",__func__,codec->dapm.bias_level);
		snd_soc_update_bits(codec, RT5636_PWR_MANAG_ADD1,
			PWR_MAIN_BIAS | PWR_VREF | PWR_MICBIAS | PWR_MICBIAS_OVCD | PWR_LDO,
			PWR_MAIN_BIAS | PWR_VREF | PWR_MICBIAS | PWR_MICBIAS_OVCD | PWR_LDO);

		snd_soc_update_bits(codec,RT5636_MIC_CTRL,
				SEL_MIC_OVCD_TH_MASK,SEL_MIC_OVCD_1500);//b- SEL_MIC_OVCD_600);
		msleep(300);
		snd_soc_write(codec, RT5636_PRIV_INDEX, 0x55);
		for (i = 0 ; i < 10 ; i++) {
			if (snd_soc_read(codec, RT5636_PRIV_DATA) & 0x200)
				headphone++;
			else
				headset++;
			msleep(20);
		}
		if (headset < headphone)
			jack_type = RT5636_HEADPHO_DET;
		else
			jack_type = RT5636_HEADSET_DET;

		if (SND_SOC_BIAS_OFF == codec->dapm.bias_level)
			snd_soc_write(codec, RT5636_PWR_MANAG_ADD1, 0);
		else if (SND_SOC_BIAS_STANDBY == codec->dapm.bias_level)
			snd_soc_write(codec, RT5636_PWR_MANAG_ADD1,
				PWR_MAIN_BIAS | PWR_VREF | PWR_LDO);
		else
			snd_soc_update_bits(codec, RT5636_PWR_MANAG_ADD1,
				PWR_MICBIAS_OVCD, 0);
		K2U_CMD(K2U_JACK_TYPE,1);
	} else {
		jack_type = RT5636_NO_JACK;
		K2U_CMD(K2U_JACK_TYPE,0);
	}

	printk("jack_type=%d\n",jack_type);
	return jack_type;
}
EXPORT_SYMBOL(rt5636_headset_detect);

static ssize_t rt5636_codec_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct rt5636_priv *rt5636 = i2c_get_clientdata(client);
	struct snd_soc_codec *codec = rt5636->codec;
	unsigned int val;
	int cnt = 0, i;

	cnt += sprintf(buf, "RT5636 codec register\n");
	for (i = 0; i <= RT5636_VEND_ID_2; i++) {
		if (cnt + 23 >= PAGE_SIZE)
			break;
		val = snd_soc_read(codec, i);
		if (!val)
			continue;
		cnt += snprintf(buf + cnt, 23,
				"#rng%02x  #rv%04x  #rd0\n", i, val);
	}

	if (cnt >= PAGE_SIZE)
		cnt = PAGE_SIZE - 1;

	return cnt;
}

static ssize_t rt5636_codec_store(struct device *dev,struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct rt5636_priv *rt5636 = i2c_get_clientdata(client);
	struct snd_soc_codec *codec = rt5636->codec;
	unsigned int val=0,addr=0;
	int i;

	printk("register \"%s\" count=%d\n",buf,count);
	for(i=0;i<count;i++) //address
	{
		if(*(buf+i) <= '9' && *(buf+i)>='0')
		{
			addr = (addr << 4) | (*(buf+i)-'0');
		}
		else if(*(buf+i) <= 'f' && *(buf+i)>='a')
		{
			addr = (addr << 4) | ((*(buf+i)-'a')+0xa);
		}
		else if(*(buf+i) <= 'F' && *(buf+i)>='A')
		{
			addr = (addr << 4) | ((*(buf+i)-'A')+0xa);
		}
		else
		{
			break;
		}
	}
	 
	for(i=i+1 ;i<count;i++) //val
	{
		if(*(buf+i) <= '9' && *(buf+i)>='0')
		{
			val = (val << 4) | (*(buf+i)-'0');
		}
		else if(*(buf+i) <= 'f' && *(buf+i)>='a')
		{
			val = (val << 4) | ((*(buf+i)-'a')+0xa);
		}
		else if(*(buf+i) <= 'F' && *(buf+i)>='A')
		{
			val = (val << 4) | ((*(buf+i)-'A')+0xa);
			
		}
		else
		{
			break;
		}
	}
	printk("addr=0x%x val=0x%x\n",addr,val);
	if(addr > RT5636_VEND_ID_2 || val > 0xffff || val < 0)
		return count;

	if(i==count)
	{
		printk("0x%02x = 0x%04x\n",addr,snd_soc_read(codec, addr));
	}
	else
	{
		snd_soc_write(codec, addr, val);
	}
	

	return count;
}
 	
static DEVICE_ATTR(codec_reg, 0666, rt5636_codec_show, rt5636_codec_store);

static int rt5636_adc_volume_get(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int val = snd_soc_read(codec, mc->reg);

	ucontrol->value.integer.value[0] = val & 0x1f;

	return 0;
}

static int rt5636_adc_volume_put(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int val_t, val;
	int ret;

	val_t = snd_soc_read(codec, RT5636_PWR_MANAG_ADD1);
	snd_soc_update_bits(codec, RT5636_PWR_MANAG_ADD1,
			PWR_I2S | PWR_ADC, PWR_I2S | PWR_ADC);
	val = ucontrol->value.integer.value[0];
	ret = snd_soc_update_bits_locked(codec, mc->reg, 0x1f, val);

	snd_soc_update_bits(codec, RT5636_PWR_MANAG_ADD1,
			PWR_I2S | PWR_ADC, val_t);
	return ret;
}

static int rt5636_amic_volume_get(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int val = snd_soc_read(codec, RT5636_MIC_INPUT_VOL);

	printk("val = 0x%x\n", val);
	ucontrol->value.integer.value[0] = ((val >> 8) & 0x7);
	printk("ucontrol->value.integer.value[0] = %d\n", ucontrol->value.integer.value[0]);

	return 0;
}

static int rt5636_amic_volume_put(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int val_t, val;
	int ret;

	val_t = snd_soc_read(codec, RT5636_PWR_MANAG_ADD1);
	snd_soc_update_bits(codec, RT5636_PWR_MANAG_ADD1,
			PWR_I2S | PWR_ADC, PWR_I2S | PWR_ADC);
	val = ucontrol->value.integer.value[0];
	ret = snd_soc_update_bits_locked(codec, RT5636_MIC_INPUT_VOL, 0x0700, val << 8);

	snd_soc_update_bits(codec, RT5636_PWR_MANAG_ADD1,
			PWR_I2S | PWR_ADC, val_t);
	return ret;
}

static const char *rt5636_mic_boost_sel[] = {"bypass", "+20dB", "+30dB", "+35dB", "+40dB", 
						"+44dB", "+50dB", "+52dB"};

static const SOC_ENUM_SINGLE_DECL(
	rt5636_amic_boost_enum, RT5636_MIC_INPUT_VOL,
	8, rt5636_mic_boost_sel);

static const struct snd_kcontrol_new rt5636_snd_controls[] = { 

SOC_SINGLE("AMIC Capture Switch", RT5636_DIGITAL_MIC_CTRL, 11, 1, 1),	//enable Analog Mic to digital ADC mixer	
SOC_DOUBLE("DMIC Capture Switch", RT5636_DIGITAL_MIC_CTRL, 13, 12, 1, 1),//enable Digital Mic to digital ADC mixer
SOC_SINGLE_EXT("ADC_Boost Capture Volume", RT5636_ADC_CTRL1, 0, 19, 0,
		rt5636_adc_volume_get, rt5636_adc_volume_put),		//from 0~28.5DB(one step is 1.5db)
SOC_SINGLE("ADC Capture Volume", RT5636_ADC_CTRL2, 8, 255, 0),			//from 0~-95.625dB(one step is -0.375db)	
SOC_ENUM_EXT("AMIC Boost", rt5636_amic_boost_enum,
		rt5636_amic_volume_get, rt5636_amic_volume_put),		//Analog Mic Boost control
};

/*PLL divisors*/
struct _pll_div {
	u32 pll_in;
	u32 pll_out;
	u16 regvalue;
};

static const struct _pll_div codec_master_pll_div[] = {
	
	{  2048000,  8192000,	0x0ea0},		
	{  3686400,  8192000,	0x4e27},	
	{ 12000000,  8192000,	0x456b},   
	{ 13000000,  8192000,	0x495f},
	{ 13100000,  8192000,	0x0320},	
	{  2048000,  11289600,	0xf637},
	{  3686400,  11289600,	0x2f22},	
	{ 12000000,  11289600,	0x3e2f},   
	{ 13000000,  11289600,	0x4d5b},
	{ 13100000,  11289600,	0x363b},	
	{  2048000,  16384000,	0x1ea0},
	{  3686400,  16384000,	0x9e27},	
	{ 12000000,  16384000,	0x452b},   
	{ 13000000,  16384000,	0x542f},
	{ 13100000,  16384000,	0x03a0},	
	{  2048000,  16934400,	0xe625},
	{  3686400,  16934400,	0x9126},	
	{ 12000000,  16934400,	0x4d2c},   
	{ 13000000,  16934400,	0x742f},
	{ 13100000,  16934400,	0x3c27},			
	{  2048000,  22579200,	0x2aa0},
	{  3686400,  22579200,	0x2f20},	
	{ 12000000,  22579200,	0x7e2f},   
	{ 13000000,  22579200,	0x742f},
	{ 13100000,  22579200,	0x3c27},		
	{  2048000,  24576000,	0x2ea0},
	{  3686400,  24576000,	0xee27},	
	{ 12000000,  24576000,	0x2915},   
	{ 13000000,  24576000,	0x772e},
	{ 13100000,  24576000,	0x0d20},	
};


static const struct _pll_div codec_slave_pll_div[] = {
	
	{  1024000,  16384000,  0x3ea0},	
	{  1411200,  22579200,	0x3ea0},
	{  1536000,  24576000,	0x3ea0},	
	{  2048000,  16384000,  0x1ea0},	
	{  2822400,  22579200,	0x1ea0},
	{  3072000,  24576000,	0x1ea0},
	{  705600,   11289600,  0x3ea0},
	{  705600,   8467200,   0x3ab0},
			
};

struct _coeff_div{
	u32 mclk;
	u32 rate;
	u16 fs;
	u16 regvalue;
};


static const struct _coeff_div coeff_div[] = {

	/* 8k */
	{ 8192000,  8000, 256*4, 0x4000},
	{12288000,  8000, 384*4, 0x4004},

	/* 11.025k */
	{11289600, 11025, 256*4, 0x4000},
	{16934400, 11025, 384*4, 0x4004},

	/* 16k */
	{16384000, 16000, 256*4, 0x4000},
	{24576000, 16000, 384*4, 0x4004},
	/* 22.05k */
	{11289600, 22050, 256*2, 0x2000},
	{16934400, 22050, 384*2, 0x2004},
	{8467200, 22050, 384*1, 0x0004},

	/* 32k */
	{16384000, 32000, 256*2, 0x2000},
	{24576000, 32000, 384*2, 0x2004},

	/* 44.1k */
	{22579200, 44100, 256*2, 0x2000},
	/* 48k */
	{12288000, 48000, 256*1, 0x0000}, //bard 8-30
	{24576000, 48000, 256*2, 0x2000},

};


static int get_coeff(int mclk, int rate)
{
	int i;

	printk("get_coeff mclk=%d,rate=%d\n",mclk,rate);

	for (i = 0; i < ARRAY_SIZE(coeff_div); i++) {
		if (coeff_div[i].rate == rate && coeff_div[i].mclk == mclk)
			return i;
	}
	
	return -EINVAL;
}


struct _coeff_dmic{
	unsigned int rate;
	u16 reg_value;
};

struct _coeff_dmic coeff_dmic[] = {

	{8000,  0x0000},
	{11025, 0x0000},
	{16000, 0x0000},
	{22050, 0x0020},
	{32000, 0x0020},
	{44100, 0x0040},
	{48000, 0x0040},

};

static int get_coeff_dmic(unsigned int rate)
{
	int i;
	
	for (i = 0; i < ARRAY_SIZE(coeff_dmic); i ++) {
		if (rate == coeff_dmic[i].rate)
			return i;
	}
	
	return -1;
}

#define RT5636_RATES SNDRV_PCM_RATE_8000_48000
#define RT5636_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
	SNDRV_PCM_FMTBIT_S24_LE)
static int rt5636_pcm_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;
	u16 iface = snd_soc_read(codec, RT5636_AUD_DATA_PORT_CTRL) & ~SEL_I2S_LEN_MASK;	
	struct rt5636_priv *rt5636 = snd_soc_codec_get_drvdata(codec);
	int coeff =  get_coeff(rt5636->sysclk, params_rate(params));
	int local_coeff_dmic = get_coeff_dmic(params_rate(params));

	switch(params_format(params)) {

	case SNDRV_PCM_FORMAT_S16_LE:
		break;

	case SNDRV_PCM_FORMAT_S20_3LE:

		iface |= SEL_I2S_LEN_20;
		break;

	case SNDRV_PCM_FORMAT_S24_LE:

		iface |= SEL_I2S_LEN_24;
		break;

	default:
		return -EINVAL;		
	}

	snd_soc_write(codec, RT5636_AUD_DATA_PORT_CTRL, iface);


		if (local_coeff_dmic == -1) {
			printk(KERN_WARNING "***dmic not configure right, because sample rate not match\n");
		} else {
			snd_soc_update_bits(codec, RT5636_DIGITAL_MIC_CTRL, SEL_DMIC_CLK_MASK, coeff_dmic[local_coeff_dmic].reg_value);
		}

	if (coeff >= 0) 
		snd_soc_write(codec, RT5636_ADC_CLK_CTRL, coeff_div[coeff].regvalue);
	else 
		printk(KERN_WARNING "***adc clk not configure right, because samplerate and sysclk not match");	
	
	return 0;
}
static int rt5636_set_dai_pll(struct snd_soc_dai *dai, int pll_id, int source,
			unsigned int freq_in, unsigned int freq_out)
{
	int i;
	int ret = -EINVAL;
	struct snd_soc_codec *codec = dai->codec;

	if (pll_id < RT5636_PLL_FR_MCLK || pll_id > RT5636_PLL_FR_BCLK)
		return -EINVAL;
	
	if (!freq_in || !freq_out)
		return 0;

	if (RT5636_PLL_FR_MCLK == pll_id) {

		for (i = 0; i < ARRAY_SIZE(codec_master_pll_div); i ++) {

			if ((freq_in == codec_master_pll_div[i].pll_in)
			    && (freq_out == codec_master_pll_div[i].pll_out)) {

				snd_soc_write(codec, RT5636_PLL_CTRL, codec_master_pll_div[i].regvalue);

				snd_soc_update_bits(codec, RT5636_PWR_MANAG_ADD1, PWR_PLL, PWR_PLL);

				mdelay(10);

				snd_soc_update_bits(codec, RT5636_GLB_CLK_CTRL, SEL_PLL_SOUR_MASK, SEL_PLL_SOUR_MCLK);

				ret = 0;
			}
		}
	} else {

		for (i = 0; i < ARRAY_SIZE(codec_slave_pll_div); i++) {

			if ((freq_in == codec_slave_pll_div[i].pll_in)
			    && (freq_out == codec_slave_pll_div[i].pll_out)) {

				snd_soc_write(codec, RT5636_PLL_CTRL, codec_slave_pll_div[i].regvalue);

				snd_soc_update_bits(codec, RT5636_PWR_MANAG_ADD1, PWR_PLL, PWR_PLL);

				mdelay(10);

				snd_soc_update_bits(codec, RT5636_GLB_CLK_CTRL, SEL_PLL_SOUR_MASK, SEL_PLL_SOUR_BCLK);

				ret = 0;
			}
		}
	}
		
	snd_soc_update_bits(codec, RT5636_GLB_CLK_CTRL, SEL_SYSCLK_MASK, SEL_SYSCLK_PLL);
	return ret;
}

static int rt5636_pcm_hw_prepare(struct snd_pcm_substream *substream,
				struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;

	snd_soc_update_bits(codec, RT5636_PWR_MANAG_ADD1, PWR_MAIN_BIAS | PWR_VREF | PWR_LDO | PWR_I2S | PWR_ADC | PWR_MICBIAS | PWR_MIC_PRE_BOOST
							, PWR_MAIN_BIAS | PWR_VREF | PWR_LDO | PWR_I2S | PWR_ADC | PWR_MICBIAS | PWR_MIC_PRE_BOOST);

	return 0;
}

static int rt5636_set_dai_sysclk(struct snd_soc_dai *dai,
		int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5636_priv *rt5636 = snd_soc_codec_get_drvdata(codec);
	
	printk(KERN_DEBUG "enter %s\n", __func__);
	
	if ((freq >= 256 * 8000) && (freq <= 512 *48000)) {
		rt5636->sysclk = freq;
		return 0;
	}

	printk(KERN_ERR "%s failed because of unsupported sysclk\n", __func__);
	return -EINVAL;
}

static int rt5636_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	u16 iface = 0;

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		iface = 0x0000;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		iface = SEL_MS_SLAV;	 	
		break;
	default:
		return -EINVAL;
	}
	
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		iface |= SEL_I2S_FORMAT_I2S;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		iface |= SEL_I2S_FORMAT_LEFT;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		iface |= SEL_I2S_FORMAT_PCM_A;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		iface |= SEL_I2S_FORMAT_PCM_B;
		break;
	default:
		return -EINVAL;
	}
	
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_IB_NF:
		iface |= INV_BCLK;
		break;
	default:
		return -EINVAL;
	}

	iface |= 0x4000; /* Copy Left Channel to Right Channel */

	snd_soc_write(codec, RT5636_AUD_DATA_PORT_CTRL, iface);

	return 0;
}

static int rt5636_set_bias_level(struct snd_soc_codec *codec, enum snd_soc_bias_level level)
{
	switch(level) {

	case SND_SOC_BIAS_ON:
		snd_soc_update_bits(codec, RT5636_PWR_MANAG_ADD1, PWR_MAIN_BIAS | PWR_VREF | PWR_LDO | PWR_I2S | PWR_ADC | PWR_MICBIAS | PWR_MIC_PRE_BOOST
							, PWR_MAIN_BIAS | PWR_VREF | PWR_LDO | PWR_I2S | PWR_ADC | PWR_MICBIAS | PWR_MIC_PRE_BOOST);
		break;
	case SND_SOC_BIAS_PREPARE:
		break;

	case SND_SOC_BIAS_STANDBY:
		break;

	case SND_SOC_BIAS_OFF:
		snd_soc_write(codec, RT5636_PWR_MANAG_ADD1, 0);
		break;
	}

	codec->dapm.bias_level = level;

	return 0;
}


static int rt5636_probe(struct snd_soc_codec *codec)
{
	struct rt5636_priv *rt5636 = snd_soc_codec_get_drvdata(codec);
	struct rt56xx_ops *ioctl_ops;
	int ret;

	pr_info("RT5636 Audio Codec %s", RT5636_VERSION);

	codec->dapm.idle_bias_off = 1;

	ret = snd_soc_codec_set_cache_io(codec, 8, 16, SND_SOC_I2C);
	if (ret != 0) {
		dev_err(codec->dev, "Failed to set cache I/O: %d\n", ret);
		return ret;
	}
	
	rt5636_reset(codec);

	snd_soc_write(codec, RT5636_PWR_MANAG_ADD1, PWR_MAIN_BIAS | PWR_VREF | PWR_LDO | PWR_VREF_FASTB);
	
	codec->dapm.bias_level = SND_SOC_BIAS_STANDBY;
	rt5636->codec = codec;

	snd_soc_update_bits(codec, RT5636_PWR_MANAG_ADD1, PWR_VREF_FASTB | PWR_I2S | PWR_ADC, PWR_I2S | PWR_ADC);
	rt5636_init_reg_set(codec);
	snd_soc_update_bits(codec, RT5636_PWR_MANAG_ADD1, PWR_I2S | PWR_ADC, 0);

	snd_soc_add_codec_controls(codec, rt5636_snd_controls,
			ARRAY_SIZE(rt5636_snd_controls));

#ifdef RTK_IOCTL
#if defined(CONFIG_SND_HWDEP)
	ioctl_ops = rt56xx_get_ioctl_ops();
	ioctl_ops->index_write = rt5636_index_write;
	ioctl_ops->index_read = rt5636_index_read;
	ioctl_ops->index_update_bits = rt5636_index_update_bits;
	ioctl_ops->ioctl_common = rt5636_ioctl_common;
	realtek_ce_init_hwdep(codec);
#endif
#endif

	ret = device_create_file(codec->dev, &dev_attr_codec_reg);
	if (ret != 0) {
		dev_err(codec->dev,
			"Failed to create codex_reg sysfs files: %d\n", ret);
		return ret;
	}

	return 0;
}

static int rt5636_remove(struct snd_soc_codec *codec)
{
	rt5636_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}
#ifdef CONFIG_PM
static int rt5636_suspend(struct snd_soc_codec *codec, pm_message_t state)
{
	rt5636_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static int rt5636_resume(struct snd_soc_codec *codec)
{
	rt5636_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	return 0;
}
#else
#define rt5636_suspend NULL
#define rt5636_resume NULL
#endif

struct snd_soc_dai_ops rt5636_aif_dai_ops = {
	.hw_params = rt5636_pcm_hw_params,
	.set_pll = rt5636_set_dai_pll,
	.set_fmt = rt5636_set_dai_fmt,
	.prepare = rt5636_pcm_hw_prepare,
	.set_sysclk = rt5636_set_dai_sysclk,
};

struct snd_soc_dai_driver rt5636_dai[] = {
	{
		.name = "rt5636-aif1",
		.id = 0,
		.playback = {
			.stream_name = "AIF1 Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5636_RATES,
			.formats = RT5636_FORMATS,
		},
		.capture = {
			.stream_name = "Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5636_RATES,
			.formats = RT5636_FORMATS,
		},
		.ops = &rt5636_aif_dai_ops,
	}
};



static struct snd_soc_codec_driver soc_codec_dev_rt5636 = {
	.probe = rt5636_probe,
	.remove = rt5636_remove, 
	.suspend = rt5636_suspend,
	.resume = rt5636_resume,
	.set_bias_level = rt5636_set_bias_level,
	.reg_cache_size = RT5636_VEND_ID_2 + 1,
	.reg_word_size = sizeof(u16),
	.reg_cache_default = rt5636_reg,
	.volatile_register = rt5636_volatile_register,
	.readable_register = rt5636_readable_register,
	.reg_cache_step = 1,
};

static const struct i2c_device_id rt5636_i2c_id[] = {
	{"rt5636", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, rt5636_i2c_id);

static int rt5636_i2c_probe(struct i2c_client *i2c, const struct i2c_device_id *id)
{
	struct rt5636_priv *rt5636;
	int ret;
	printk("enter %s\n",__func__);

	rt5636 = kzalloc(sizeof(struct rt5636_priv), GFP_KERNEL);
	if (NULL == rt5636)
		return -ENOMEM;

	i2c_set_clientdata(i2c, rt5636);

	ret = snd_soc_register_codec(&i2c->dev, &soc_codec_dev_rt5636,
			rt5636_dai, ARRAY_SIZE(rt5636_dai));
	if (ret < 0)
		kfree(rt5636);

	return ret;
}

static int rt5636_i2c_remove(struct i2c_client *i2c)
{
	snd_soc_unregister_codec(&i2c->dev);
	kfree(i2c_get_clientdata(i2c));
	return 0;
}
struct i2c_driver rt5636_i2c_driver = {
	.driver = {
		.name = "rt5636",
		.owner = THIS_MODULE,
	} ,
	.probe = rt5636_i2c_probe,
	.remove = rt5636_i2c_remove,
	.id_table = rt5636_i2c_id,
};

static int __init rt5636_modinit(void)
{
	printk("enter %s\n",__func__);
	return i2c_add_driver(&rt5636_i2c_driver);
}
module_init(rt5636_modinit);

static void __exit rt5636_modexit(void)
{
	i2c_del_driver(&rt5636_i2c_driver);
}
module_exit(rt5636_modexit);

MODULE_DESCRIPTION("ASoC RT5636 driver");
MODULE_AUTHOR("Bard Liao <bardliao@realtek.com>");
MODULE_LICENSE("GPL");
