#include <linux/device.h>

struct wolo_reg_dev {
    int dev_num;
    void (*wolo_reg_uart_func)(char* buf,int count);
};
enum wolouart_regdrv
{
    WOLOUART_KP_REG=0,
    WOLOUART_EAR_REG,
    WOLOUART_VOL_REG,
    WOLOUART_BN_REG,
    WOLOUART_FW_REG,
    WOLOUART_MODE_REG,
    WOLOUART_TS_REG,
    WOLOUART_ROTATION_REG,
    WOLOUART_MAX_REG		    		
};
enum k2u_cmd
{
    K2U_SLEEP_CMD=0,
    K2U_POWER_KEY_CMD,
    K2U_JACK_TYPE,
    K2U_MAX_REG		    		
};

extern void RegUartDevFunc(struct wolo_reg_dev func);
extern void K2U_CMD(int cmd_index,int en);
extern char g_into_mode; //0 :A_mode 1:R_mode
extern char g_crypt_control_bl;
