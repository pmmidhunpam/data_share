/*
 * Copyright 2008, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_NDEBUG 1
#define LOG_TAG "Sensors"

#include <hardware/sensors.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <poll.h>

#include <linux/input.h>

#include <cutils/log.h>
#include <cutils/atomic.h>

#include "akm8973.h"

/*****************************************************************************/

#define AKM_DEVICE_NAME     "/dev/akm8973_aot"
#define ST_DEVICE_NAME      "/dev/lis35de_aot"

// sensor IDs must be a power of two and
// must match values in SensorManager.java
#define EVENT_TYPE_ACCEL_X          ABS_X
#define EVENT_TYPE_ACCEL_Y          ABS_Y
#define EVENT_TYPE_ACCEL_Z          ABS_Z
#define EVENT_TYPE_ACCEL_STATUS     ABS_WHEEL

#define EVENT_TYPE_YAW              ABS_RX
#define EVENT_TYPE_PITCH            ABS_RY
#define EVENT_TYPE_ROLL             ABS_RZ
#define EVENT_TYPE_ORIENT_STATUS    ABS_RUDDER

/*** AKEMD ATTENTION! To adjust Android ***/
/*** cordination, SWAP X and Y axis.    ***/
#define EVENT_TYPE_MAGV_X ABS_HAT0X
#define EVENT_TYPE_MAGV_Y ABS_HAT0Y
#define EVENT_TYPE_MAGV_Z ABS_BRAKE


#define EVENT_TYPE_TEMPERATURE      ABS_THROTTLE
#define EVENT_TYPE_STEP_COUNT       ABS_GAS

// 720 LSG = 1G
#define LSG                         (720.0f)

// conversion of acceleration data to SI units (m/s^2)
#define CONVERT_A                   (GRAVITY_EARTH / LSG)
#define CONVERT_A_X                 (CONVERT_A)
#define CONVERT_A_Y                 (-CONVERT_A)
#define CONVERT_A_Z                 (-CONVERT_A)

// conversion of magnetic data to uT units
#define CONVERT_M                   (1.0f/16.0f)
#define CONVERT_M_X (-CONVERT_M)
#define CONVERT_M_Y (-CONVERT_M)
#define CONVERT_M_Z (CONVERT_M)


#define CONVERT_O                   (1.0f/64.0f)
#define CONVERT_O_Y                 (CONVERT_O)
#define CONVERT_O_P                 (CONVERT_O)
#define CONVERT_O_R                 (CONVERT_O)


#define ID_A  (0)
#define ID_M  (1)
#define ID_O  (2)
#define ID_T  (3)
#define ID_P  (4)
#define ID_L  (5)

#define MAX_NUM_SENSORS 6

static int id_to_sensor[MAX_NUM_SENSORS] = {
    [ID_A] = SENSOR_TYPE_ACCELEROMETER,
    [ID_M] = SENSOR_TYPE_MAGNETIC_FIELD,
    [ID_O] = SENSOR_TYPE_ORIENTATION,
    [ID_T] = SENSOR_TYPE_TEMPERATURE,
    [ID_P] = SENSOR_TYPE_PROXIMITY,
    [ID_L] = SENSOR_TYPE_LIGHT,
};

#define __MAX(a,b) ((a)>=(b)?(a):(b))

#define SENSORS_AKM_MAGNETIC_FIELD (1<<ID_M)
#define SENSORS_AKM_ORIENTATION    (1<<ID_O)
#define SENSORS_AKM_TEMPERATURE    (1<<ID_T)
#define SENSORS_AKM_GROUP          ((1<<ID_M)|(1<<ID_O)|(1<<ID_T))


#define SENSORS_ST_ACCELERATION    (1<<ID_A)
#define SENSORS_ST_GROUP           (1<<ID_A)

#define SENSORS_LTR_PROXIMITY       (1<<ID_P)
#define SENSORS_LTR_LIGHT           (1<<ID_L)
#define SENSORS_LTR_GROUP           ((1<<ID_P)|(1<<ID_L))


#define SENSOR_STATE_MASK           (0x7FFF)
#define SUPPORTED_SENSORS  ((1<<MAX_NUM_SENSORS)-1)
			  
int akmd_fd = -1;
int st_fd = -1;
int input_fd1 = -1;
int input_fd2 = -1;
uint32_t active_sensors = 0;
sensors_event_t sensor_events[MAX_NUM_SENSORS];
uint32_t pendingSensors = 0;

/*****************************************************************************/
static int sensors_device_close(struct hw_device_t *dev);

static int sensors_activate(struct sensors_poll_device_t *dev, int handle, int enabled);

static int sensors_poll(struct sensors_poll_device_t *dev, sensors_event_t* data, int count);

static int sensors_get_sensors_list(struct sensors_module_t* module,
	struct sensor_t const**);

static int sensors_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device);

/*****************************************************************************/



/*****************************************************************************/

/*
 * We use a Least Mean Squares filter to smooth out the output of the yaw
 * sensor.
 *
 * The goal is to estimate the output of the sensor based on previous acquired
 * samples.
 *
 * We approximate the input by a line with the equation:
 *      Z(t) = a * t + b
 *
 * We use the Least Mean Squares method to calculate a and b so that the
 * distance between the line and the measured COUNT inputs Z(t) is minimal.
 *
 * In practice we only need to compute b, which is the value we're looking for
 * (it's the estimated Z at t=0). However, to improve the latency a little bit,
 * we're going to discard a certain number of samples that are too far from
 * the estimated line and compute b again with the new (trimmed down) samples.
 *
 * notes:
 * 'a' is the slope of the line, and physicaly represent how fast the input
 * is changing. In our case, how fast the yaw is changing, that is, how fast the
 * user is spinning the device (in degre / nanosecond). This value should be
 * zero when the device is not moving.
 *
 * The minimum distance between the line and the samples (which we are not
 * explicitely computing here), is an indication of how bad the samples are
 * and gives an idea of the "quality" of the estimation (well, really of the
 * sensor values).
 *
 */

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

/*****************************************************************************/

static int open_input(int mode, int *akm_fd, int *p_fd)
{
    /* scan all input drivers and look for "compass" */
    int fd = -1;
    const char *dirname = "/dev/input";
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;
    dir = opendir(dirname);
    if(dir == NULL)
        return -1;
    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';
    while((de = readdir(dir))) {
        if(de->d_name[0] == '.' &&
           (de->d_name[1] == '\0' ||
            (de->d_name[1] == '.' && de->d_name[2] == '\0')))
            continue;
        strcpy(filename, de->d_name);
        fd = open(devname, mode);
        if (fd>=0) {
            char name[80];
            if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
                name[0] = '\0';
            }
			if (!strcmp(name, "compass")) {
                LOGE("using %s (name=%s)", devname, name);
                *akm_fd = fd;
            }
            else if (!strcmp(name, "acc")) {
                LOGV("using %s (name=%s)", devname, name);
                *p_fd = fd;
            }
            else
                close(fd);
		}
           
        }
    closedir(dir);
	fd = 0;
    if (*akm_fd < 0) {
        LOGE("Couldn't find or open 'compass' driver (%s)", strerror(errno));
        fd = -1;
    }
    if (*p_fd < 0) {
        LOGE("Couldn't find or open 'Accelerometer' driver (%s)", strerror(errno));
        fd = -1;
    }
    return fd;
}

static int open_akm(struct sensors_poll_device_t* dev)
{
    LOGV(">>>>>>>>>> open_akm() start\n");
    if (akmd_fd < 0) {
        akmd_fd = open(AKM_DEVICE_NAME, O_RDONLY);
        LOGV("%s, fd=%d", __PRETTY_FUNCTION__, akmd_fd);
        LOGE_IF(akmd_fd<0, "Couldn't open %s (%s)",
                AKM_DEVICE_NAME, strerror(errno));
        if (akmd_fd >= 0) {
            active_sensors &= ~SENSORS_AKM_GROUP;
        }
    }
    return akmd_fd;
}

static void close_akm(struct sensors_poll_device_t* dev)
{
    if (akmd_fd >= 0) {
        LOGV("%s, fd=%d", __PRETTY_FUNCTION__, akmd_fd);
        close(akmd_fd);
        akmd_fd = -1;
    }
}
static int open_st(struct sensors_poll_device_t* dev)
{
    LOGV(">>>>>>>>>>> open_st()\n");
    if (st_fd < 0) {
        st_fd = open(ST_DEVICE_NAME, O_RDONLY);
        LOGV("%s, fd=%d", __PRETTY_FUNCTION__, st_fd);
        LOGE_IF(st_fd<0, "Couldn't open %s (%s)",
                ST_DEVICE_NAME, strerror(errno));
        if (st_fd >= 0) {
            active_sensors &= ~SENSORS_ST_GROUP;
        }
    }
    return st_fd;
}

static void close_st(struct sensors_poll_device_t* dev)
{
    if (st_fd >= 0) {
        LOGV("%s, fd=%d", __PRETTY_FUNCTION__, st_fd);
        close(st_fd);
        st_fd = -1;
    }
}

static uint32_t read_st_sensors_state(int fd)
{
    short flags;
    uint32_t sensors = 0;
    // read the actual value of all sensors

    if (!ioctl(fd, ECS_IOCTL_APP_GET_LIS35DEFLAG, &flags)) {
        if (flags)  sensors |= SENSORS_ST_ACCELERATION;
        else        sensors &= ~SENSORS_ST_ACCELERATION;
    }

    return sensors;
}
static uint32_t read_akm_sensors_state(int fd)
{
    short flags;
    uint32_t sensors = 0;
    // read the actual value of all sensors
    if (!ioctl(fd, ECS_IOCTL_APP_GET_MFLAG, &flags)) {
        if (flags)  sensors |= SENSORS_AKM_ORIENTATION;
        else        sensors &= ~SENSORS_AKM_ORIENTATION;
    }
    if (!ioctl(fd, ECS_IOCTL_APP_GET_TFLAG, &flags)) {
        if (flags)  sensors |= SENSORS_AKM_TEMPERATURE;
        else        sensors &= ~SENSORS_AKM_TEMPERATURE;
    }
    if (!ioctl(fd, ECS_IOCTL_APP_GET_MVFLAG, &flags)) {
        if (flags)  sensors |= SENSORS_AKM_MAGNETIC_FIELD;
        else        sensors &= ~SENSORS_AKM_MAGNETIC_FIELD;
    }
    return sensors;
}

static uint32_t enable_disable_akm(struct sensors_poll_device_t *dev,
                                   uint32_t active, uint32_t sensors,
                                   uint32_t mask)
{
    uint32_t now_active_akm_sensors;
	int fd = open_akm(dev);
    if (fd < 0)
        return 0;

    short flags;
    if (mask & SENSORS_AKM_ORIENTATION) {
	flags = (sensors & SENSORS_AKM_ORIENTATION) ? 1 : 0;
	LOGI(">>>>>>>>>> enable_disable_akm SENSORS_AKM_ORIENTATION %d\n", flags);
        if (ioctl(fd, ECS_IOCTL_APP_SET_MFLAG, &flags) < 0) {
            LOGE("ECS_IOCTL_APP_SET_MFLAG error (%s)", strerror(errno));
        }
    }
    if (mask & SENSORS_AKM_TEMPERATURE) {
	LOGI(">>>>>>>>>> enable_disable_akm %d\n", flags);
        flags = (sensors & SENSORS_AKM_TEMPERATURE) ? 1 : 0;
        if (ioctl(fd, ECS_IOCTL_APP_SET_TFLAG, &flags) < 0) {
            LOGE("ECS_IOCTL_APP_SET_TFLAG error (%s)", strerror(errno));
        }
    }
    if (mask & SENSORS_AKM_MAGNETIC_FIELD) {
	LOGI(">>>>>>>>>> enable_disable_akm SENSORS_AKM_MAGNETIC_FIELD%d\n", flags);
        flags = (sensors & SENSORS_AKM_MAGNETIC_FIELD) ? 1 : 0;
        if (ioctl(fd, ECS_IOCTL_APP_SET_MVFLAG, &flags) < 0) {
            LOGE("ECS_IOCTL_APP_SET_MVFLAG error (%s)", strerror(errno));
        }
    }

    now_active_akm_sensors = read_akm_sensors_state(fd);

    LOGV("(after) akm sensors = %08x, real = %08x",
         sensors, now_active_akm_sensors);

    return now_active_akm_sensors;
}
static int enable_disable_st(struct sensors_poll_device_t *dev,
                             uint32_t active, uint32_t sensors, uint32_t mask)
{
    uint32_t now_active_st_sensors;
    int fd = open_st(dev);

    if (fd < 0) {
        LOGE("Couldn't open %s (%s)", ST_DEVICE_NAME, strerror(errno));
        return 0;
    }

    LOGV("(before) st sensors = %08x, real = %08x",
         sensors, read_st_sensors_state(fd));
	short flags;
    if (mask & SENSORS_ST_ACCELERATION) {
        flags = (sensors & SENSORS_ST_ACCELERATION) ? 1 : 0;
	LOGI(">>>>>>>>>>>> enable_disable_st SENSORS_ST_ACCELERATION %d\n", flags);
        if ((ioctl(fd,ECS_IOCTL_APP_SET_LIS35DEFLAG, &flags))<0)
			LOGE("ECS_IOCTL_APP_SET_LIS35DEFLAG error (%s)", strerror(errno));

    }

    now_active_st_sensors = read_st_sensors_state(fd);

    LOGV("(after) st sensors = %08x, real = %08x",
         sensors, now_active_st_sensors);

    return now_active_st_sensors;
}


/*****************************************************************************/

static int sensors_activate(struct sensors_poll_device_t *dev, int handle, int enabled)
{
    LOGI(">>>>>>>>>>>>> sensors_activate() handle=%d, enable=%d\n", handle, enabled);
    if ((handle < SENSORS_HANDLE_BASE) ||
            (handle >= SENSORS_HANDLE_BASE+MAX_NUM_SENSORS))
        return -1;

    uint32_t mask = (1 << handle);
    uint32_t sensors = enabled ? mask : 0;

    uint32_t active = active_sensors;
    uint32_t new_sensors = (active & ~mask) | (sensors & mask);
    uint32_t changed = active ^ new_sensors;

    LOGE("%s active=%08x sensor=%08x new_sensors=%x changed=%x", __FUNCTION__,
        active, sensors, new_sensors, changed);

    if (changed) {
        if (!active && new_sensors)
            // force all sensors to be updated
            changed = SUPPORTED_SENSORS;

        active_sensors =
            enable_disable_akm(dev,
                               active & SENSORS_AKM_GROUP,
                               new_sensors & SENSORS_AKM_GROUP,
                               changed & SENSORS_AKM_GROUP) |
            enable_disable_st(dev,
                              active & SENSORS_ST_GROUP,
                              new_sensors & SENSORS_ST_GROUP,
                              changed & SENSORS_ST_GROUP);
    }
    return 0;
}

static int sensors_set_delay(struct sensors_poll_device_t *dev, int handle, int64_t ns)
{
 if (handle == ID_A) {
    //ACCELEROMETER
    if (st_fd <= 0) {
        return -1;
    }
    short delay = ns / 1000000;
    LOGI(">>>>>>>>>>>>> sensors_set_delay  ECS_IOCTL_APP_SET_ST_DELAY delay = %d\n", delay);
    if (!ioctl(st_fd, ECS_IOCTL_APP_SET_ST_DELAY, &delay)) {
	LOGE("oh god\n");
        return -errno;
    }
    return 0;
 } else if(handle >= ID_B && handle <= ID_L) { 
    //compass 
    if (akmd_fd <= 0) {
        return -1;
    }
    //ns to ms
    unsigned long delay = ns / 1000000;
    LOGI(">>>>>>>>>>>>> sensors_set_delay ECS_IOCTL_APP_SET_DELAY delay = %d\n", delay);
    if (!ioctl(akmd_fd, ECS_IOCTL_APP_SET_DELAY, &delay)) {
	LOGE("oh god\n");
        return -errno;
    }
    return 0;
 }
 return -1;
}


/*****************************************************************************/

struct hw_module_methods_t sensors_module_methods = {
    open: sensors_device_open
};

const struct sensors_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: SENSORS_HARDWARE_MODULE_ID,
        name : "AK8973A & AK8973 & 502als Sensors Module",
        author : "The Android Open Source Project",
        methods: &sensors_module_methods,
    },
    get_sensors_list: sensors_get_sensors_list,
};

static const struct sensor_t sSensorList[] = {
        { "ST LIS35DE Accelerometer",
                "The Android Open Source Project",
                1, SENSORS_HANDLE_BASE+ID_A,
                SENSOR_TYPE_ACCELEROMETER, 2.8f, 1.0f/4032.0f, 3.0f, 0, { } },
		{ "AK8973 Magnetic field sensor",
                "The Android Open Source Project",
                1, SENSORS_HANDLE_BASE+ID_M,
                SENSOR_TYPE_MAGNETIC_FIELD, 2000.0f, 1.0f, 6.7f, 0, { } },
        { "AK8973 Orientation sensor",
                "The Android Open Source Project",
                1, SENSORS_HANDLE_BASE+ID_O,
                SENSOR_TYPE_ORIENTATION, 360.0f, 1.0f, 9.7f, 0, { } },
		{ "AK8973 Temperature sensor",
                "The Android Open Source Project",
                1, SENSORS_HANDLE_BASE+ID_T,
                SENSOR_TYPE_TEMPERATURE, 80.0f, 1.0f, 0.0f, 0, { } },
};


/*****************************************************************************/


static int sensors_get_sensors_list(struct sensors_module_t* module,
		struct sensor_t const** plist){
    *plist = sSensorList;
    return ARRAY_SIZE(sSensorList);
}

static int sensors_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device)
{
    LOGE(">>>>>>>>>>>>>> sensors_device_open() name = %s\n", name); 
    struct sensors_poll_device_t *dev;
    dev = (struct sensors_poll_device_t*)malloc(sizeof(*dev));

    /* initialize our state here */
    memset(dev, 0, sizeof(*dev));
    akmd_fd = -1;
    st_fd = -1;
    /* initialize the procs */
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t *)module;
    dev->common.close = sensors_device_close;
    dev->activate = sensors_activate;
    dev->setDelay = sensors_set_delay;
    dev->poll = sensors_poll;

    //open device node
    open_akm(dev);
    open_st(dev);
    //open input device node
    open_input(O_RDONLY, &input_fd1, &input_fd2);

    *device = &dev->common;
    return 0;
}

static int sensors_device_close(struct hw_device_t *dev)
{
    LOGI(">>>>>>>>>>>>>> sensors_device_close()"); 
    struct sensors_poll_device_t*  p = (struct sensors_poll_device_t*)dev;
	if (p)
		free(p);
    return 0;
}


/*****************************************************************************/

static int pick_sensor(struct sensors_poll_device_t *dev,
        sensors_event_t* values)
{
    uint32_t mask = SUPPORTED_SENSORS;
    while (mask) {
        uint32_t i = 31 - __builtin_clz(mask);
        mask &= ~(1<<i);
        if (pendingSensors & (1<<i)) {
            pendingSensors &= ~(1<<i);
            *values = sensor_events[i];
            //values->sensor = id_to_sensor[i];
	    //values->sensor is sensor handle id not sensor type
            values->sensor = i;
            LOGI_IF(1, "%d [%f, %f, %f]",
                    values->sensor,
                    values->acceleration.x,
                    values->acceleration.y,
                    values->acceleration.z);
            return 1;
        }
    }

    LOGE("no sensor to return: pendingSensors = %08x", pendingSensors);
    return -1;
}


static uint32_t data__poll_process_akm_abs(struct sensors_poll_device_t *dev,
                                           int fd __attribute__((unused)),
                                           struct input_event *event)
{
    uint32_t new_sensors = 0;
    if (event->type == EV_ABS) {
        LOGV("compass type: %d code: %d value: %-5d time: %ds",
             event->type, event->code, event->value,
             (int)event->time.tv_sec);
        switch (event->code) {
        case EVENT_TYPE_MAGV_X:
            new_sensors |= SENSORS_AKM_MAGNETIC_FIELD;
            sensor_events[ID_M].magnetic.x = event->value * CONVERT_M_X;
            break;
        case EVENT_TYPE_MAGV_Y:
            new_sensors |= SENSORS_AKM_MAGNETIC_FIELD;
            sensor_events[ID_M].magnetic.y = event->value * CONVERT_M_Y;
            break;
        case EVENT_TYPE_MAGV_Z:
            new_sensors |= SENSORS_AKM_MAGNETIC_FIELD;
            sensor_events[ID_M].magnetic.z = event->value * CONVERT_M_Z;
            break;
        case EVENT_TYPE_YAW:
            new_sensors |= SENSORS_AKM_ORIENTATION;
            sensor_events[ID_O].orientation.azimuth =  event->value*CONVERT_O_Y;
            break;
        case EVENT_TYPE_PITCH:
            new_sensors |= SENSORS_AKM_ORIENTATION;
            sensor_events[ID_O].orientation.pitch = event->value*CONVERT_O_P;
            break;
        case EVENT_TYPE_ROLL:
            new_sensors |= SENSORS_AKM_ORIENTATION;
            sensor_events[ID_O].orientation.roll = event->value*CONVERT_O_R;
            break;
        case EVENT_TYPE_TEMPERATURE:
            new_sensors |= SENSORS_AKM_TEMPERATURE;
            sensor_events[ID_T].temperature = event->value & 0x7fffffff;
            break;
        case EVENT_TYPE_STEP_COUNT:
            // step count (only reported in MODE_FFD)
            // we do nothing with it for now.
            break;
        case EVENT_TYPE_ACCEL_STATUS:
            // accuracy of the calibration (never returned!)
            //LOGV("G-Sensor status %d", event->value);
            break;
        case EVENT_TYPE_ORIENT_STATUS: {
            // accuracy of the calibration
            uint32_t v = (uint32_t)(event->value & SENSOR_STATE_MASK);
            LOGV_IF(sensor_events[ID_O].orientation.status != (uint8_t)v,
                    "M-Sensor status %d", v);
            sensor_events[ID_O].orientation.status = (uint8_t)v;
        }
            break;
        }
    }

    return new_sensors;
}

static uint32_t data__poll_process_st_abs(struct sensors_poll_device_t *dev,
                                           int fd __attribute__((unused)),
                                          struct input_event *event)
{
    uint32_t new_sensors = 0;
    if (event->type == EV_ABS) {
        LOGV("proximity type: %d code: %d value: %-5d time: %ds",
             event->type, event->code, event->value,
             (int)event->time.tv_sec);
        
        switch (event->code) {
        case EVENT_TYPE_ACCEL_X:
            new_sensors |= SENSORS_ST_ACCELERATION;
            sensor_events[ID_A].acceleration.x = event->value * 9.8/1000;
            break;
        case EVENT_TYPE_ACCEL_Y:
            new_sensors |= SENSORS_ST_ACCELERATION;
            sensor_events[ID_A].acceleration.y = event->value * 9.8/1000;
            break;
        case EVENT_TYPE_ACCEL_Z:
            new_sensors |= SENSORS_ST_ACCELERATION;
            sensor_events[ID_A].acceleration.z = event->value * 9.8/1000;
            break;
        default:
            break;
        }
        
    }
    return new_sensors;
}

static void data__poll_process_syn(struct sensors_poll_device_t *dev,
                                   struct input_event *event,
                                   uint32_t new_sensors)
{
    if (new_sensors) {
        pendingSensors |= new_sensors;
        int64_t t = event->time.tv_sec*1000000000LL + event->time.tv_usec*1000;
        while (new_sensors) {
            uint32_t i = 31 - __builtin_clz(new_sensors);
            new_sensors &= ~(1<<i);
            sensor_events[i].timestamp = t;
        }
    }
}

static int sensors_poll(struct sensors_poll_device_t *dev, sensors_event_t* data, int count)
{
    LOGV(">>>>>>>>> sensors_poll() start\n");
    int akm_fd = input_fd1;
    int cm_fd = input_fd2;

    if (akm_fd < 0) {
        LOGE("invalid compass file descriptor, fd=%d", akm_fd);
        return -1;
    }

    if (cm_fd < 0) {
        LOGE("invalid acc-sensor file descriptor, fd=%d", cm_fd);
        return -1;
    }

    // there are pending sensors, returns them now...
    if (pendingSensors) {
        LOGV("pending sensors 0x%08x", pendingSensors);
        return pick_sensor(dev, data);
    }

    // wait until we get a complete event for an enabled sensor
    uint32_t new_sensors = 0;
    while (1) {
        /* read the next event; first, read the compass event, then the
           proximity event */
        struct input_event event;
        int got_syn = 0;
        int exit = 0;
        int nread;
        fd_set rfds;
        int n;

        FD_ZERO(&rfds);
        FD_SET(akm_fd, &rfds);
        FD_SET(cm_fd, &rfds);
      //  FD_SET(ls_fd, &rfds);
        n = select(__MAX(akm_fd, cm_fd) + 1, &rfds,
                   NULL, NULL, NULL);
        LOGV("return from select: %d\n", n);
        if (n < 0) {
            LOGE("%s: error from select(%d, %d): %s\n",
                 __FUNCTION__,
                 akm_fd, cm_fd, strerror(errno));
            return -1;
        }

        if (FD_ISSET(akm_fd, &rfds)) {
            nread = read(akm_fd, &event, sizeof(event));
               if(event.code==5||event.code==4||event.code==3){
                                }
            if (nread == sizeof(event)) {
                new_sensors |= data__poll_process_akm_abs(dev, akm_fd, &event);
                LOGV("akm abs %08x\n", new_sensors);
                got_syn = event.type == EV_SYN;
                exit = got_syn && event.code == SYN_CONFIG;
                if (got_syn) {
                    LOGV("akm syn %08x\n", new_sensors);
                    data__poll_process_syn(dev, &event, new_sensors);
                    new_sensors = 0;
                }
            }
            else LOGE("akm read too small %d\n", nread);
        }
        else LOGV("akm fd is not set\n");

        if (FD_ISSET(cm_fd, &rfds)) {
            nread = read(cm_fd, &event, sizeof(event));
            if (nread == sizeof(event)) {
                new_sensors |= data__poll_process_st_abs(dev, cm_fd, &event);
                LOGV("cm abs %08x\n", new_sensors);
                got_syn |= event.type == EV_SYN;
                exit |= got_syn && event.code == SYN_CONFIG;
                if (got_syn) {
                    data__poll_process_syn(dev, &event, new_sensors);
                    new_sensors = 0;
                }
            }
            else LOGE("cm read too small %d\n", nread);
        }
        else LOGV("cm fd is not set\n");

        if (exit) {
            // we use SYN_CONFIG to signal that we need to exit the
            // main loop.
            //LOGV("got empty message: value=%d", event->value);
            LOGV("exit\n");

	 return 0x7FFFFFFF;
        }

        if (got_syn && pendingSensors) {
            LOGV("got syn, picking sensor");
            return pick_sensor(dev, data);
        }
    }
}

