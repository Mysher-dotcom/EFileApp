#ifndef SANEHELPER_H
#define SANEHELPER_H


#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iterator>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <iterator>
#include "sane/sanei.h"
#include "sane/saneopts.h"
using namespace std;


#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Initialize SANE
SANE_Status init();

// Get all devices
SANE_Status get_devices(const SANE_Device ***device_list);

// Open a device
SANE_Status open_device(const SANE_Device *device, SANE_Handle *sane_handle);

// Start scanning
SANE_Status start_scan(SANE_Handle sane_handle, SANE_String_Const fileName);

// Cancel scanning
void cancle_scan(SANE_Handle sane_handle);

// Close SANE device
void close_device(SANE_Handle sane_handle);

// Release SANE resources
void app_exit();

void test_SANE();

#ifdef __cplusplus
}
#endif



#endif // SANEHELPER_H
