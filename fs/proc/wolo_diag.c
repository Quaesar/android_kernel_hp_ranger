
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <linux/mfd/palmas.h>
#include <linux/gpio.h>

#define MODULE_VERS "1.0"
#define MODULE_NAME "wolo_diag"

#define WOLO_IO_LEN 20
#define TEGRA_GPIO_PK4		84
#define TEGRA_GPIO_PN4             108
#define TEGRA_GPIO_PX5             189
#define TEGRA_GPIO_PG1             49

struct wolo_diag_data_t {
	char name[WOLO_IO_LEN + 1];
	char value[WOLO_IO_LEN + 1];
};


static struct proc_dir_entry *wolo_diag_dir, *wolo_diag_file;


struct wolo_diag_data_t wolo_diag_data;

#define writel(b, addr) (void)((*(volatile unsigned int *)(addr)) = (b))
#define readl(addr) 	({ unsigned int __v = (*(volatile unsigned int *)(addr)); __v; })

static int proc_read_wolo_diag(char *page, char **start,
			    off_t off, int count, 
			    int *eof, void *data)
{
	int len;
//	int earphone_insert=0, dock_det=0;
//	unsigned int reg;

//	earphone_insert = 0;	// not insert

//	len = sprintf(page, "earphone=%d\ndock_det=%d\n", earphone_insert, dock_det);
	len = sprintf(page, "no data\n");

	return len;
}

#define LED_OFF	0
#define LED_BLINK	1

void LED1_test(int mode)
{
}

void LED2_test(int mode)
{
}

static int proc_write_wolo_diag(struct file *file,
			     const char *buffer,
			     unsigned long count, 
			     void *data)
{
	int len;
	unsigned int value; //, val2;

	if(count > WOLO_IO_LEN)
		len = WOLO_IO_LEN;
	else
		len = count;

	if(sscanf(buffer, "LED1=%d", &value) == 1)
		{
		 if(value == 0)
		 	{
		 	LED1_test(LED_OFF);
		 	}
		 else if(value == 1)
		 	LED1_test(LED_BLINK);
		}		
	else if(sscanf(buffer, "LED2=%d", &value) == 1)
		{
		 if(value == 0)
		 	{
		 	LED2_test(LED_OFF);
		 	}
		 else if(value == 1)
		 	LED2_test(LED_BLINK);
		}		
	else if(sscanf(buffer, "GPS=%d", &value) == 1)
		{
		if(value == 1)
			{
			gpio_request(TEGRA_GPIO_PX5, "gps_pwr_en");
			gpio_set_value(TEGRA_GPIO_PX5, 1);
			gpio_direction_output(TEGRA_GPIO_PX5, 1);
			gpio_request(TEGRA_GPIO_PG1, "gps_pwron");
			gpio_set_value(TEGRA_GPIO_PG1, 1);
			gpio_direction_output(TEGRA_GPIO_PG1, 1);
			}
		}		
	else if(sscanf(buffer, "USBID=%d", &value) == 1)
		{
		if(value == 0)
			{	// for host
			gpio_request(TEGRA_GPIO_PK4, "usb1_id");
			gpio_set_value(TEGRA_GPIO_PK4, 0);
			gpio_direction_output(TEGRA_GPIO_PK4, 0);
			set_usb1_id_status_to_tristate(0);
			}
		else	// for device
			set_usb1_id_status_to_tristate(1);
		}
	else if(sscanf(buffer, "VBUS=%d", &value) == 1)
		{
		if(value == 0)
			{
			gpio_request(TEGRA_GPIO_PN4, "usb1_vbus_en");
			gpio_set_value(TEGRA_GPIO_PN4, 0);
			gpio_direction_output(TEGRA_GPIO_PN4, 0);
			}
		else
			{
			gpio_request(TEGRA_GPIO_PN4, "usb1_vbus_en");
			gpio_set_value(TEGRA_GPIO_PN4, 1);
			gpio_direction_output(TEGRA_GPIO_PN4, 1);
			}
		}
	return len;
}


static int __init init_proc_wolo_diag(void)
{
	int rv = 0;

	/* create directory */
	wolo_diag_dir = proc_mkdir(MODULE_NAME, NULL);
	if(wolo_diag_dir == NULL) {
		rv = -ENOMEM;
		goto out;
	}
	
	
	wolo_diag_file = create_proc_entry("wolo_diag", 0644, wolo_diag_dir);
	if(wolo_diag_file == NULL) {
		rv = -ENOMEM;
		goto no_wolo_io;
	}

	strcpy(wolo_diag_data.name, "wolo_diag");
	strcpy(wolo_diag_data.value, "wolo_diag");
	wolo_diag_file->data = &wolo_diag_data;
	wolo_diag_file->read_proc = proc_read_wolo_diag;
	wolo_diag_file->write_proc = proc_write_wolo_diag;
		
	/* everything OK */
	printk(KERN_INFO "/proc/%s %s initialised \n",
	       MODULE_NAME, MODULE_VERS);

	return 0;

no_wolo_io:
	remove_proc_entry("wolo_diag", wolo_diag_dir);
out:
	return rv;
}


static void __exit cleanup_proc_wolo_diag(void)
{
	remove_proc_entry("wolo_diag", wolo_diag_dir);
	remove_proc_entry(MODULE_NAME, NULL);

	printk(KERN_INFO "/proc/%s %s removed\n",
	       MODULE_NAME, MODULE_VERS);
}


module_init(init_proc_wolo_diag);
module_exit(cleanup_proc_wolo_diag);

MODULE_AUTHOR("James");
MODULE_DESCRIPTION("wolo_diag");
MODULE_LICENSE("GPL");
