/*
Copyright (c) 2013, Intel Corporation

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.

David Navarro <david.navarro@intel.com>

*/

#include "core/liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "externals/er-coap-13/er-coap-13.h"


#define PRV_MANUFACTURER      "Open Mobile Alliance"
#define PRV_MODEL_NUMBER      "Lightweight M2M Client"
#define PRV_SERIAL_NUMBER     "345000123"
#define PRV_FIRMWARE_VERSION  "1.0"
#define PRV_POWER_STATUS      0
#define PRV_BATTERY_LEVEL     100
#define PRV_MEMORY_FREE       15
#define PRV_ERROR_CODE        0

#define PRV_OFFSET_MAXLEN 7 //+HH:MM\0 at max


typedef struct
{
    int64_t time;
    char time_offset[PRV_OFFSET_MAXLEN];
} device_data_t;

// basic check that the time offset value is at ISO 8601 format
// bug: +12:30 is considered a valid value by this function
static int prv_check_time_offset(char * buffer,
                                 int length)
{
    int min_index;

    if (length != 3 && length != 5 && length != 6) return 0;
    if (buffer[0] != '-' && buffer[0] != '+') return 0;
    switch (buffer[1])
    {
    case '0':
        if (buffer[2] < '0' || buffer[2] > '9') return 0;
        break;
    case '1':
        if (buffer[2] < '0' || buffer[2] > '2') return 0;
        break;
    default:
        return 0;
    }
    switch (length)
    {
    case 3:
        return 1;
    case 5:
        min_index = 3;
        break;
    case 6:
        if (buffer[3] != ':') return 0;
        min_index = 4;
        break;
    default:
        // never happen
        return 0;
    }
    if (buffer[min_index] < '0' || buffer[min_index] > '5') return 0;
    if (buffer[min_index+1] < '0' || buffer[min_index+1] > '9') return 0;

    return 1;
}

static uint8_t prv_device_read(lwm2m_uri_t * uriP,
                               char ** bufferP,
                               int * lengthP,
                               void * userData)
{
    *bufferP = NULL;
    *lengthP = 0;

    // this is a single instance object
    if (0 != uriP->objInstance && LWM2M_URI_NOT_DEFINED != uriP->objInstance)
    {
        return NOT_FOUND_4_04;
    }

    switch (uriP->resID)
    {
    case 0:
        *bufferP = strdup(PRV_MANUFACTURER);
        if (NULL != *bufferP)
        {
            *lengthP = strlen(*bufferP);
            return CONTENT_2_05;
        }
        else
        {
            return MEMORY_ALLOCATION_ERROR;
        }
    case 1:
        *bufferP = strdup(PRV_MODEL_NUMBER);
        if (NULL != *bufferP)
        {
            *lengthP = strlen(*bufferP);
            return CONTENT_2_05;
        }
        else
        {
            return MEMORY_ALLOCATION_ERROR;
        }
    case 2:
        *bufferP = strdup(PRV_SERIAL_NUMBER);
        if (NULL != *bufferP)
        {
            *lengthP = strlen(*bufferP);
            return CONTENT_2_05;
        }
        else
        {
            return MEMORY_ALLOCATION_ERROR;
        }
    case 3:
        *bufferP = strdup(PRV_FIRMWARE_VERSION);
        if (NULL != *bufferP)
        {
            *lengthP = strlen(*bufferP);
            return CONTENT_2_05;
        }
        else
        {
            return MEMORY_ALLOCATION_ERROR;
        }
    case 4:
        return METHOD_NOT_ALLOWED_4_05;
    case 5:
        return METHOD_NOT_ALLOWED_4_05;
    case 6:
        *lengthP = lwm2m_int8ToPlainText(PRV_POWER_STATUS, bufferP);
        if (0 != *lengthP)
        {
            return CONTENT_2_05;
        }
        else
        {
            return MEMORY_ALLOCATION_ERROR;
        }
    case 7:
        *lengthP = lwm2m_int8ToPlainText(PRV_BATTERY_LEVEL, bufferP);
        if (0 != *lengthP)
        {
            return CONTENT_2_05;
        }
        else
        {
            return MEMORY_ALLOCATION_ERROR;
        }
    case 8:
        *lengthP = lwm2m_int8ToPlainText(PRV_MEMORY_FREE, bufferP);
        if (0 != *lengthP)
        {
            return CONTENT_2_05;
        }
        else
        {
            return MEMORY_ALLOCATION_ERROR;
        }
    case 9:
        // TODO: this is a multi-instance ressource
        *lengthP = lwm2m_int8ToPlainText(PRV_ERROR_CODE, bufferP);
        if (0 != *lengthP)
        {
            return CONTENT_2_05;
        }
        else
        {
            return MEMORY_ALLOCATION_ERROR;
        }
    case 10:
        return METHOD_NOT_ALLOWED_4_05;
    case 11:
        *lengthP = lwm2m_int64ToPlainText(((device_data_t*)userData)->time, bufferP);
        if (0 != *lengthP)
        {
            return CONTENT_2_05;
        }
        else
        {
            return MEMORY_ALLOCATION_ERROR;
        }
    case 12:
        *bufferP = strdup(((device_data_t*)userData)->time_offset);
        if (NULL != *bufferP)
        {
            *lengthP = strlen(*bufferP);
            return CONTENT_2_05;
        }
        else
        {
            return MEMORY_ALLOCATION_ERROR;
        }
    case LWM2M_URI_NOT_DEFINED:
        // TODO: return whole object
        return NOT_IMPLEMENTED_5_01;
    default:
        return NOT_FOUND_4_04;
    }

}

static uint8_t prv_device_write(lwm2m_uri_t * uriP,
                                char * buffer,
                                int length,
                                void * userData)
{
    // this is a single instance object
    if (0 != uriP->objInstance)
    {
        return NOT_FOUND_4_04;
    }

    switch (uriP->resID)
    {
    case 11:
        if (1 == lwm2m_PlainTextToInt64(buffer, length, &((device_data_t*)userData)->time))
        {
            return CHANGED_2_04;
        }
        else
        {
            return BAD_REQUEST_4_00;
        }
    case 12:
        if (1 == prv_check_time_offset(buffer, length))
        {
            strncpy(((device_data_t*)userData)->time_offset, buffer, length);
            ((device_data_t*)userData)->time_offset[length] = 0;
            return CHANGED_2_04;
        }
        else
        {
            return BAD_REQUEST_4_00;
        }
    default:
        return METHOD_NOT_ALLOWED_4_05;
    }
}

static uint8_t prv_device_execute(lwm2m_uri_t * uriP,
                                  void * userData)
{
    // this is a single instance object
    if (0 != uriP->objInstance)
    {
        return NOT_FOUND_4_04;
    }

    switch (uriP->resID)
    {
    case 4:
        fprintf(stdout, "\n\t REBOOT\r\n\n");
        return CHANGED_2_04;
    case 5:
        fprintf(stdout, "\n\t FACTORY RESET\r\n\n");
        return CHANGED_2_04;
    case 10:
        fprintf(stdout, "\n\t RESET ERROR CODE\r\n\n");
        return CHANGED_2_04;
    default:
        return METHOD_NOT_ALLOWED_4_05;
    }
}

lwm2m_object_t * get_object_device()
{
    lwm2m_object_t * deviceObj;

    deviceObj = (lwm2m_object_t *)malloc(sizeof(lwm2m_object_t));

    if (NULL != deviceObj)
    {
        memset(deviceObj, 0, sizeof(lwm2m_object_t));

        deviceObj->objID = 3;
        deviceObj->readFunc = prv_device_read;
        deviceObj->writeFunc = prv_device_write;
        deviceObj->executeFunc = prv_device_execute;
        deviceObj->userData = malloc(sizeof(device_data_t));
        if (NULL != deviceObj->userData)
        {
            ((device_data_t*)deviceObj->userData)->time = 1367491215;
            strcpy(((device_data_t*)deviceObj->userData)->time_offset, "+02:00");
        }
        else
        {
            free(deviceObj);
            deviceObj = NULL;
        }
    }

    return deviceObj;
}
