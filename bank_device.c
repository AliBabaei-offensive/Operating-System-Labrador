#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define INIT_BALANCE 2000000
#define ACCOUNT_COUNT 100

#define DEVICE_NAME "bank_device"
MODULE_LICENSE("GPL");
static int bank_open(struct inode *, struct file *);
static int bank_release(struct inode *, struct file *);
static ssize_t bank_read(struct file *, char *, size_t, loff_t *);
static ssize_t bank_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
	.open = bank_open,
	.read = bank_read,
	.release = bank_release,
	.write = bank_write,
};

static int major; // device major number. driver reacts to this major number.

struct bank
{
	int acc[ACCOUNT_COUNT];
};
static struct bank bal;

int atoi(char *data)
{
	printk(KERN_ALERT "atoi working...\n");
	printk(KERN_ALERT "atoi input: %s\n", data);
	int sum, i;
	sum = 0;
	for (i = 0; data[i] >= '0' && data[i] <= '9'; i++)
	{
		sum *= 10;
		sum += (data[i] - '0');
	}
	printk(KERN_ALERT "atoi result: %d\n", sum);
	return sum;
}
int int_len(int data)
{
	int len = 0;
	if (!data)
		len++; // In case of data=0
	while (data)
	{
		len++;
		data /= 10;
	}
	return len;
}
void decode_input(char *data, int *out)
{ // out: from, to, amount (-1 if '-')
	int temp;

	// from
	if (data[2] == '-')
		temp = -1;
	else
		temp = atoi(&(data[2]));
	out[0] = temp;
	printk(KERN_ALERT "out[0] set to %d and its len is %d.\n", out[0], int_len(out[0]));
	// to
	if (data[2 + int_len(out[0]) + 1] == '-')
		temp = -1;
	else
		temp = atoi(&(data[2 + int_len(out[0]) + 1]));
	out[1] = temp;
	printk(KERN_ALERT "out[1] set to %d and its len is %d.\n", out[1], int_len(out[1]));
	// amount
	temp = atoi(&(data[2 + int_len(out[0]) + 1 + int_len(out[1]) + 1]));
	out[2] = temp;
	printk(KERN_ALERT "out[2] set to %d and its len is %d.\n", out[2], int_len(out[2]));
	printk(KERN_ALERT "decoded input: %d,%d,%d\n", out[0], out[1], out[2]);
}

static int __init bank_init(void)
{
	major = register_chrdev(0, DEVICE_NAME, &fops); // 0: auto major ||| name is displayed in /proc/devices ||| fops.
	if (major < 0)
	{
		printk(KERN_ALERT "Device001 load failed!\n");
		return major;
	}
	printk(KERN_INFO "bank device module has been loaded: %d\n", major);
	int i;
	for (i = 0; i < 100; i++)
	{
		bal.acc[i] = 100;
	}

	return 0;
}

static void __exit bank_exit(void)
{
	unregister_chrdev(major, DEVICE_NAME);
	printk(KERN_INFO " bank device module has been unloaded.\n");
}

static int bank_open(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO " bank device opened.\n");
	return 0;
}

static int bank_release(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "bank device closed.\n");
	return 0;
}

static ssize_t bank_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int errors = 0;
	// int account[100] = {0};
	char str[3024] = "";
	char buff[50];
	int i = 0;
	for (i = 0; i < 100; i++)
	{
		sprintf(buff, "%d,", bal.acc[i]);
		strcat(str, buff);
	}
	errors = copy_to_user(buffer, str, strlen(str));
	return errors == 0 ? strlen(str) : -EFAULT;
}

static ssize_t bank_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	int errors = 0;
	char writebuffer[256] = "";
	int outbuffer[3] = {0};
	printk(KERN_INFO "Entered to the write\n");
	errors = copy_from_user(writebuffer, buffer, strlen(buffer));
	// printk(KERN_INFO "%s\n",writebuffer);
	decode_input(writebuffer, outbuffer);
	printk(KERN_INFO "%s\n", writebuffer);
	int sender = outbuffer[0];
	int reciver = outbuffer[1];
	int amount = outbuffer[2];

	if (sender != -1 && reciver != -1)
	{
		printk(KERN_INFO "id range is ok");

		if (writebuffer[0] == 't')
		{
			if (bal.acc[sender] < amount)
			{
				printk(KERN_INFO "No mony in %d account",reciver);
				return 1;
			}
			
			bal.acc[sender] -= amount;
			bal.acc[reciver] += amount;
		}
		else if (writebuffer[0] == 'd')
		{
			bal.acc[reciver] += amount;
		}

		else if (writebuffer[0] == 'w')
		{
			bal.acc[sender] -= amount;
		}
		else
		{
			printk(KERN_INFO "Command not find\n");
		}

		int i;
		for (i = 0; i < 100; i++)
		{
			printk(KERN_INFO "%d, ", bal.acc[i]);
		}
	}

	else
	{
		printk(KERN_INFO "Wrong format\n");
	}

	printk(KERN_INFO "Exited from the write");
	return errors == 0 ? strlen(buffer) : -EFAULT;
}

module_init(bank_init);
module_exit(bank_exit);
