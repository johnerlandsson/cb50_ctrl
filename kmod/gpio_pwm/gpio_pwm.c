/* 
 * This file is part of the cb50_ctrl distribution (https://github.com/johnerlandsson/cb50_ctrl).
 * Copyright (c) 2017 John Erlandsson
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <asm/io.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <mach/platform.h>

MODULE_LICENSE("GPL");

/* Prototype declarations */
static ssize_t set_period_callback(struct device *, struct device_attribute *,
                                   const char *, size_t);
static ssize_t set_value_callback(struct device *, struct device_attribute *,
                                  const char *, size_t);
static ssize_t get_value_callback(struct device *, struct device_attribute *,
                                  char *);
static ssize_t get_period_callback(struct device *, struct device_attribute *,
                                   char *);

/* Type declarations */
struct GpioRegisters {
    uint32_t GPFSEL[6];
    uint32_t Reserved1;
    uint32_t GPSET[2];
    uint32_t Reserved2;
    uint32_t GPCLR[2];
};

/* Global definitions */
struct GpioRegisters *s_pGpioRegisters;
static struct timer_list s_PwmTimer;
static int s_PwmPeriod = 1000;
static int s_PwmValue = 0;
static const int PwmGpioPin = 9;
static struct class *s_pDeviceClass;
static struct device *s_pDeviceObject;
static struct device_attribute dev_attr_period = {
    .attr = {.name = "period", .mode = S_IWUSR | S_IWGRP | S_IWOTH},
    .show = get_period_callback,
    .store = set_period_callback};
static struct device_attribute dev_attr_value = {
    .attr = {.name = "value", .mode = S_IWUSR | S_IWGRP | S_IWOTH},
    .show = get_value_callback,
    .store = set_value_callback};

module_param(PwmGpioPin, int, 0);

/* set_period_callback
 * Callback function for sysfs write to period file
 */
static ssize_t set_period_callback(struct device *dev,
                                   struct device_attribute *attr,
                                   const char *buff, size_t count) {
    long period_value = 0;

    if (kstrtol(buff, 10, &period_value) < 0) return -EINVAL;
    if (period_value < 10) return -EINVAL;

    s_PwmPeriod = period_value;

    return count;
}

static ssize_t get_period_callback(struct device *dev,
                                   struct device_attribute *attr, char *buff) {
    return sprintf(buff, "%d", s_PwmPeriod);
}

/* set_value_callback
 * Callback function for sysfs write to value file
 */
static ssize_t set_value_callback(struct device *dev,
                                  struct device_attribute *attr,
                                  const char *buff, size_t count) {
    long value = 0;

    if (kstrtol(buff, 10, &value) < 0) return -EINVAL;
    if (value < 0)
        return -EINVAL;
    else if (value > s_PwmPeriod)
        return -EINVAL;

    s_PwmValue = value;

    return count;
}

static ssize_t get_value_callback(struct device *dev,
                                  struct device_attribute *attr, char *buff) {
    return sprintf(buff, "%d", s_PwmValue);
}
/* SetGPIOFunction
 * Set the direction of a specific GPIO pin
 */
static void SetGPIOFunction(int GPIO, int functionCode) {
    int registerIndex = GPIO / 10;
    int bit = (GPIO % 10) * 3;

    unsigned oldValue = s_pGpioRegisters->GPFSEL[registerIndex];
    unsigned mask = 0b111 << bit;

    printk(KERN_INFO "Changing function of GPIO%d from %x to %x\n", GPIO,
           (oldValue >> bit) & 0b111, functionCode);
    s_pGpioRegisters->GPFSEL[registerIndex] =
        (oldValue & ~mask) | ((functionCode << bit) & mask);
}

/* SetGPIOOutputValue
 * Sets or clears a specific GPIO output pin
 */
static void SetGPIOOutputValue(int GPIO, bool outputValue) {
    if (outputValue)
        s_pGpioRegisters->GPSET[GPIO / 32] = (1 << (GPIO % 32));
    else
        s_pGpioRegisters->GPCLR[GPIO / 32] = (1 << (GPIO % 32));
}

/* BlinkTimerHandler
 * Handler for timer timeout. Switch output pin
 * and fire timer again.
 */
static void BlinkTimerHandler(unsigned long unused) {
    static bool on = false;
    int sleep_time;

    if (!on && s_PwmValue > 0) {
        sleep_time = s_PwmValue;
        on = true;
    } else {
        sleep_time = s_PwmPeriod - s_PwmValue;
        on = false;
    }

    SetGPIOOutputValue(PwmGpioPin, on);
    mod_timer(&s_PwmTimer, jiffies + msecs_to_jiffies(sleep_time));
}

/* init_gpio_pwm
 * Init function for kernel module
 */
int init_gpio_pwm(void) {
    int result;
    char dev_name[10];

    printk(KERN_INFO "Loading module gpio_pwm\n");

    if (PwmGpioPin < 0) {
        printk(KERN_ERR "Invalid gpio pin\n");
        return -1;
    }

    s_pGpioRegisters = (struct GpioRegisters *)__io_address(GPIO_BASE);
    SetGPIOFunction(PwmGpioPin, 0b001);

    /* Setup timer */
    setup_timer(&s_PwmTimer, BlinkTimerHandler, 0);
    result = mod_timer(&s_PwmTimer, jiffies + msecs_to_jiffies(s_PwmPeriod));
    BUG_ON(result < 0);

    /* Create GpioPwm device */
    snprintf(dev_name, sizeof(dev_name), "GpioPwm%d", PwmGpioPin);
    s_pDeviceClass = class_create(THIS_MODULE, dev_name);
    BUG_ON(IS_ERR(s_pDeviceClass));
    s_pDeviceObject = device_create(s_pDeviceClass, NULL, 0, NULL, dev_name);
    BUG_ON(IS_ERR(s_pDeviceObject));

    /* Create period file */
    result = device_create_file(s_pDeviceObject, &dev_attr_period);
    BUG_ON(result < 0);
    /* Create value file */
    result = device_create_file(s_pDeviceObject, &dev_attr_value);
    BUG_ON(result < 0);

    return 0;
}

/* exit_gpio_pwm
 * Exit function for kernel module
 */
void exit_gpio_pwm(void) {
    printk(KERN_INFO "Unloading module gpio_pwm\n");

    device_remove_file(s_pDeviceObject, &dev_attr_period);
    device_remove_file(s_pDeviceObject, &dev_attr_value);
    device_destroy(s_pDeviceClass, 0);
    class_destroy(s_pDeviceClass);

    SetGPIOFunction(PwmGpioPin, 0);

    del_timer(&s_PwmTimer);
}

module_init(init_gpio_pwm);
module_exit(exit_gpio_pwm);
