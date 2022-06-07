#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "ztimer.h"
#include "xtimer.h"
#include "shell.h"

#include "mutex.h"

/* Add lps331ap related include here */
#include "lpsxxx.h"
#include "lpsxxx_params.h"

/* Add lsm303dlhc related include here */
#include "lsm303dlhc.h"
#include "lsm303dlhc_params.h"

/* Add l3g4200d related include here */
#include "l3g4200d.h"
#include "l3g4200d_params.h"

/* Add isl29020 related include here */
#include "isl29020.h"
#include "isl29020_params.h"

#define SLEEP_USEC  (250 * 1000U)

/* delay value */
static uint32_t msn = 500;

/* Declare the lps331ap device variable here */
static lpsxxx_t lpsxxx;

/* Declare the lps331ap device variable here */
static isl29020_t isl29020;

/* Declare the lsm303dlhc device variable here */
static lsm303dlhc_t lsm303dlhc;

/* Declare the l3g4200d device variable here */
static l3g4200d_t l3g4200d;
/* Declare and initialize the lsm303dlhc thread lock here */
static mutex_t lsm_lock = MUTEX_INIT_LOCKED;
/* Declare and initialize the l3g4200d thread lock here */
static mutex_t l3g_lock = MUTEX_INIT_LOCKED;

/* stack memory allocated for the lsm303dlhc thread */
static char lsm303dlhc_stack[THREAD_STACKSIZE_MAIN];
/* stack memory allocated for the lsm303dlhc thread */
static char l3g4200d_stack[THREAD_STACKSIZE_MAIN];

static void *lsm303dlhc_thread(void *arg)
{
    (void)arg;

    while (1) {
        /* Acquire the mutex here */
        mutex_lock(&lsm_lock);

        /* Read the accelerometer/magnetometer values here */
        lsm303dlhc_3d_data_t mag_value;
        lsm303dlhc_3d_data_t acc_value;
        lsm303dlhc_read_acc(&lsm303dlhc, &acc_value);
        printf("Accelerometer x: %i y: %i z: %i\n",
               acc_value.x_axis, acc_value.y_axis, acc_value.z_axis);
        lsm303dlhc_read_mag(&lsm303dlhc, &mag_value);
        printf("Magnetometer x: %i y: %i z: %i\n",
               mag_value.x_axis, mag_value.y_axis, mag_value.z_axis);

        /* Release the mutex here */

        mutex_unlock(&lsm_lock);
        ztimer_sleep(ZTIMER_MSEC, msn);
    }

    return 0;
}

static void _lsm303dlhc_usage(char *cmd)
{
    printf("usage: %s <start|stop> [delay ms] \n", cmd);
}

static int lsm303dlhc_handler(int argc, char *argv[])
{
    if (argc < 2) {
        _lsm303dlhc_usage(argv[0]);
        return -1;
    }

    /* Implement the lsm303dlhc start/stop subcommands here */
    if (!strcmp(argv[1], "start")) {
        if (argc < 3) {
            msn = 500;
            mutex_unlock(&lsm_lock);
        }else if (argc == 3) {
            msn = atoi(argv[2]);
            mutex_unlock(&lsm_lock);
        }
    }
    else if (!strcmp(argv[1], "stop")) {
        mutex_trylock(&lsm_lock);
    }
    else {
        _lsm303dlhc_usage(argv[0]);
        return -1;
    }

    return 0;
}
static void *l3g4200d_thread(void *arg)
{
    (void)arg;

    while (1) {
        /* Acquire the mutex here */
        mutex_lock(&l3g_lock);

        /* Read the accelerometer/magnetometer values here */
        l3g4200d_data_t gyro_value;
        l3g4200d_read(&l3g4200d, &gyro_value);
        printf("Gyroscope x: %i y: %i z: %i\n",
               gyro_value.acc_x, gyro_value.acc_y, gyro_value.acc_z);

        /* Release the mutex here */

        mutex_unlock(&l3g_lock);
        ztimer_sleep(ZTIMER_MSEC, msn);
    }

    return 0;
}

static void _l3g4200d_usage(char *cmd)
{
    printf("usage: %s <start|stop> [delay ms] \n", cmd);
}

static int l3g4200d_handler(int argc, char *argv[])
{
    if (argc < 2) {
        _l3g4200d_usage(argv[0]);
        return -1;
    }
    

    /* Implement the lsm303dlhc start/stop subcommands here */
    if (!strcmp(argv[1], "start")) {
        if (argc < 3) {
            msn = 500;
            mutex_unlock(&l3g_lock);
        }else if (argc == 3) {
            msn = atoi(argv[2]);
            mutex_unlock(&l3g_lock);
        }
        
    }
    else if (!strcmp(argv[1], "stop")) {
        mutex_trylock(&l3g_lock);
    }
    else {
        _l3g4200d_usage(argv[0]);
        return -1;
    }

    return 0;
}
static void _lpsxxx_usage(char *cmd)
{
    printf("usage: %s <temperature|pressure>\n", cmd);
}

static int lpsxxx_handler(int argc, char *argv[])
{
    if (argc < 2) {
        _lpsxxx_usage(argv[0]);
        return -1;
    }

    /* Implement the lps331ap temperature/pressure subcommands here */
    if (!strcmp(argv[1], "temperature")) {
        int16_t temp = 0;
        lpsxxx_read_temp(&lpsxxx, &temp);
        printf("Temperature: %i.%u°C\n", (temp / 100), (temp % 100));
    }
    else if (!strcmp(argv[1], "pressure")) {
        uint16_t pres = 0;
        lpsxxx_read_pres(&lpsxxx, &pres);
        printf("Pressure: %uhPa\n", pres);
    }
    else {
        _lpsxxx_usage(argv[0]);
        return -1;
    }

    return 0;
}
static void _isl29020_usage(char *cmd)
{
    printf("usage: %s <light>\n", cmd);
}

static int isl29020_handler(int argc, char *argv[])
{
    if (argc < 1) {
        _isl29020_usage(argv[0]);
        return -1;
    }
    
    isl29020_t dev;
    if (isl29020_init(&dev, &isl29020_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    printf("Light: %5i lx\n", isl29020_read(&dev));
    
    return 0;
}

static const shell_command_t commands[] = {
    /* lsm303dlhc shell command handler */
    { "lsm", "start/stop reading accelerometer values", lsm303dlhc_handler },

    /* l3g4200d shell command handler */
    { "l3g", "start/stop reading gyroscope values", l3g4200d_handler },

    /* Add the lps331ap command description here */
    { "lps", "read the lps331ap values", lpsxxx_handler },

    /* isl29020 shell command handler */
    { "isl", "read the isl29020 valuess", isl29020_handler },
    { NULL, NULL, NULL}
};

int main(void)
{
    /* Initialize the lps331ap sensor here */
    lpsxxx_init(&lpsxxx, &lpsxxx_params[0]);
    
    /* Initialize the isl29020 sensor here */
    isl29020_init(&isl29020, &isl29020_params[0]);

    /* Initialize the lsm303dlhc sensor here */
    lsm303dlhc_init(&lsm303dlhc, &lsm303dlhc_params[0]);

    thread_create(lsm303dlhc_stack, sizeof(lsm303dlhc_stack), THREAD_PRIORITY_MAIN - 1,
                  0, lsm303dlhc_thread, NULL, "lsm303dlhc");
    
    /* Initialize the l3g4200d sensor here */
    l3g4200d_init(&l3g4200d, &l3g4200d_params[0]);

    thread_create(l3g4200d_stack, sizeof(l3g4200d_stack), THREAD_PRIORITY_MAIN - 1,
                  0, l3g4200d_thread, NULL, "l3g4200d");

    /* Everything is ready, let's start the shell now */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
