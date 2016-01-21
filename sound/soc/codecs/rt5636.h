#ifndef __RT5636_H_
#define __RT5636_H_

#define RT5636_RESET 				(0x00)   //SFOTWARE RESET  
#define RT5636_MIC_INPUT_VOL 		(0x10)	 //MICROPHONE INPUT VOLUME
#define RT5636_ADC_CTRL1			(0x12)	 //ADC CONTROL 1
#define RT5636_ADC_CTRL2			(0x14)	 //ADC CONTROL 2
#define RT5636_MIC_CTRL				(0x22)	 //MICROPHONE CONTROL	
#define RT5636_DIGITAL_MIC_CTRL		(0x24)	 //DIGIAL MICROPHONE CONTROL
#define RT5636_AUD_DATA_PORT_CTRL	(0x34)	 //AUDIO SERIAL DATA PORT CONTROL	
#define RT5636_ADC_CLK_CTRL			(0x38)	 //ADC CLOCK CONTROL	
#define RT5636_PWR_MANAG_ADD1		(0x3a)	 //POWER MANAGEMENT 1	
#define RT5636_GENE_PPS_CTRL		(0x40)	 //GENERAL PURPOSE CONTROL	
#define RT5636_GLB_CLK_CTRL			(0x42)	 //GOOBAL CLOCK CONTROL
#define RT5636_PLL_CTRL				(0x44)	 //PLL CONTROL
#define RT5636_GPIO_CTRL1			(0x4a)	 //GPIO CONTROL 1
#define RT5636_GPIO_CTRL2 			(0x4b)	 //GPIO CONTROL 2	
#define RT5636_ZC_FUNC_CTRL			(0x5c)	 //ZERO CROSS FUNCTION CONTROL
#define RT5636_ALC_CTRL1			(0x64)	 //ALC CONTROL 1
#define RT5636_ALC_CTRL2			(0x65)	 //ALC CONTROL 2	
#define RT5636_ALC_CTRL3			(0x66)	 //ALC CONTROL 3
#define RT5636_PRIV_INDEX			(0x6a)
#define RT5636_PRIV_DATA			(0x6c)
#define RT5636_VEND_ID_1			(0x7c)	 //VENDOR ID 1
#define RT5636_VEND_ID_2			(0x7e)	 //VENDOR ID 2	

/*mic input volume mx10*/
#define EN_BSTL_DF			(0x1 << 15)

#define SEL_BSTL_MASK		(0x7 << 8)
#define SEL_BSTL_BYPASS		(0x0 << 8)
#define SEL_BSTL_20DB		(0x1 << 8)
#define SEL_BSTL_30DB		(0x2 << 8)
#define SEL_BSTL_35DB		(0x3 << 8)
#define SEL_BSTL_40DB		(0x4 << 8)
#define SEL_BSTL_44DB		(0x5 << 8)
#define SEL_BSTL_50DB		(0x6 << 8)
#define SEL_BSTL_52DB		(0x7 << 8)

/*ADC control1 mx12*/
#define ADC_MUTE			(0x1 << 15)

/*ADC control2 mx14*/
#define AD_GAIN_MASK		(0xff << 8)

/*Mic Control mx22*/
#define MICBIAS_VSET_MASK	(0x1 << 15)
#define MICBIAS_VSET_0_9	(0x0 << 15)
#define MICBIAS_VSET_0_7_5	(0x1 << 15)

#define SEL_MIC_OVCD_TH_MASK	(0x3)
#define SEL_MIC_OVCD_600		(0x0)
#define SEL_MIC_OVCD_1500		(0x1)
#define SEL_MIC_OVCD_2000		(0x2)

/*DIGITAL Mic Ctrl mx24*/
#define DMIC_MUTE_L				(0x01 << 13)
#define DMIC_MUTE_R				(0x01 << 12)
#define MU_ADC_TO_ADCMIX		(0x1 << 11)
#define DMICL_RI_FA_SEL_MASK	(0x1 << 9)
#define DMICL_RI_FA_SEL_FALL	(0x1 << 9)
#define DMICL_RI_FA_SEL_RISE	(0x0 << 9)

#define DMICR_RI_FA_SEL_MASK	(0x1 << 8)
#define DMICR_RI_FA_SEL_FALL	(0x1 << 8)
#define DMICR_RI_FA_SEL_RISE	(0x0 << 8)
	
#define SEL_DMIC_CLK_MASK		(0x7 << 5)
#define SEL_DMIC_CLK_2			(0x0 << 5)
#define SEL_DMIC_CLK_4			(0x01 << 5)
#define SEL_DMIC_CLK_6			(0x02 << 5)
#define SEL_DMIC_CLK_12			(0x03 << 5)
#define SEL_DMIC_CLK_24			(0x04 << 5)

/*audio serial data port contrl mx34*/
#define SEL_MS_MASK				(0x1 << 15)
#define SEL_MS_SLAV				(0x1 << 15)
#define SEL_MS_MAST				(0x0 << 15)

#define INV_BCLK				(0x01 << 7)

#define SEL_I2S_LEN_MASK		(0x03 << 2)
#define SEL_I2S_LEN_16			(0x00 << 2)
#define SEL_I2S_LEN_20			(0x01 << 2)
#define SEL_I2S_LEN_24			(0x02 << 2)

#define SEL_I2S_FORMAT_MASK		(0x03)
#define SEL_I2S_FORMAT_I2S		(0x00)
#define SEL_I2S_FORMAT_LEFT		(0x01)
#define SEL_I2S_FORMAT_PCM_A	(0x02)
#define SEL_I2S_FORMAT_PCM_B	(0x03)

/*ADC Clk Control mx38*/
#define SEL_I2S_PRE_DIV_MASK	(0x7 << 13)
#define SEL_I2S_PRE_DIV_1		(0x7 << 0)
#define SEL_I2S_PRE_DIV_2		(0x7 << 1)
#define SEL_I2S_PRE_DIV_4		(0x7 << 2)
#define SEL_I2S_PRE_DIV_8		(0x7 << 3)
#define SEL_I2S_PRE_DIV_16		(0x7 << 4)
#define SEL_I2S_PRE_DIV_32		(0x7 << 5)

#define SEL_I2S_BCLK_MS_MASK	(0x1 << 12)
#define SEL_I2S_BCLK_MS_64FS	(0x0 << 12)
#define SEL_I2S_BCLK_MS_32FS	(0x1 << 12)

#define SEL_ADC_FILT_CLK_MASK	(0x01 << 2)
#define SEL_ADC_FILT_CLK_256	(0x00 << 2)
#define SEL_ADC_FILT_CLK_384	(0x01 << 2)

#define SEL_ADC_OSR_MASK		(0x3)
#define SEL_ADC_OSR_128			(0x0)
#define SEL_ADC_OSR_64			(0x1)
#define SEL_ADC_OSR_32			(0x2)

/*power manager add1 mx3a*/
#define PWR_MAIN_BIAS			(0x01 << 15)
#define PWR_VREF				(0x01 << 14)
#define PWR_PLL					(0x01 << 13)
#define PWR_I2S					(0x01 << 12)
#define PWR_ADC					(0x01 << 11)
#define PWR_MICBIAS				(0x01 << 9)
#define PWR_VREF_FASTB			(0x01 << 5)
#define PWR_MICBIAS_OVCD		(0x01 << 4)
#define PWR_LDO					(0x01 << 2)
#define PWR_MIC_PRE_BOOST		(0x01 << 1)

/*general purpose control mx40*/
#define AD_HPF_EN				(0x01 << 8)

/*global clock ctrl mx42*/
#define SEL_SYSCLK_MASK			(0x01 << 15)
#define SEL_SYSCLK_MCLK			(0x00 << 15)
#define SEL_SYSCLK_PLL			(0x01 << 15)

#define SEL_PLL_SOUR_MASK		(0x01 << 14)
#define SEL_PLL_SOUR_MCLK		(0x00 << 14)
#define SEL_PLL_SOUR_BCLK		(0x01 << 14)

#define PLL_PRE_DIV_MASK		(0x01)
#define PLL_PRE_DIV_1			(0x00)
#define PLL_PRE_DIV_2			(0x01)


/*gpio control1 mx4a*/
#define SEL_PIN_SHARE_MASK		(0x01 << 15)
#define SEL_PIN_SHARE_DMIC		(0x00 << 15)
#define SEL_PIN_SHARE_GPIO		(0x01 << 15)

#define SEL_GPIO0_IO_MASK		(0x01 << 3)
#define SEL_GPIO0_IO_INS		(0x01 << 3)
#define SEL_GPIO0_IO_OUT		(0x00 << 3)

#define SEL_GPIO0_O_LOGIC_MASK	(0x01 << 2)
#define SEL_GPIO0_O_LOGIC_HIGH	(0x01 << 2)
#define SEL_GPIO0_O_LOGIC_LOW	(0x00 << 2)

#define STATUS_GPIO0_MASK		(0x01)

/*gpio ctrl2 mx4b*/
#define SEL_GPIO1_IO_MASK		(0x01 << 3)
#define SEL_GPIO1_IO_INS		(0x01 << 3)
#define SEL_GPIO1_IO_OUT		(0x00 << 3)

#define SEL_GPIO1_O_LOGIC_MASK	(0x01 << 2)
#define SEL_GPIO1_O_LOGIC_HIGH	(0x01 << 2)
#define SEL_GPIO1_O_LOGIC_LOW	(0x00 << 2)
#define STATUS_GPIO1_MASK		(0x01)


/*zero cross func ctrl mx54*/
#define ZDET_FUNC_MASK			(0x03 << 2)
#define ZDET_TOUT_MASK			(0x03)

/*ALC Control1 mx64*/
#define ALC_ATK_RATE_MASK		(0x1f << 8)
#define ALC_RC_SLOW_RATE_MASK	(0x1f)

/*alc control2 mx65*/
#define NOISE_GT_BST_MASK		(0x0f)

/*alc ctrl3 mx66*/
#define ALC_EN					(0x01 << 15)
#define ALC_NS_GT_EN			(0x01 << 14)
#define ALC_NS_GAIN_HD			(0x01 << 13)
#define ALC_THMAX_MASK			(0x1f << 8)
#define ALC_PARAM_UPDATE_BIT	(0x01 << 7)

#define ALC_THRE_NS_MASK		(0x1f)

#define RT5636_PLL_FR_MCLK 	1
#define RT5636_PLL_FR_BCLK	2

#define RT5636_NO_JACK		BIT(0)
#define RT5636_HEADSET_DET	BIT(1)
#define RT5636_HEADPHO_DET	BIT(2)

int rt5636_headset_detect(struct snd_soc_codec *codec, int jack_insert);

#endif
