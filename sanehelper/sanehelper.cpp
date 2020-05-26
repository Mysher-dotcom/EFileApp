#include "sanehelper.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  uint8_t *data;
  int width;
  int height;
  int x;
  int y;
}
Image;

#define STRIP_HEIGHT	256
static SANE_Handle device = NULL;
static int verbose;
static int progress = 0;
static SANE_Byte *buffer;
static size_t buffer_size;

static void
auth_callback (SANE_String_Const resource,
           SANE_Char * username, SANE_Char * password)
{
}

static void write_pnm_header (SANE_Frame format, int width, int height, int depth, FILE *ofp)
{
    switch (format)
    {
        case SANE_FRAME_RED:
        case SANE_FRAME_GREEN:
        case SANE_FRAME_BLUE:
        case SANE_FRAME_RGB:
            fprintf (ofp, "P6\n# SANE data follows\n%d %d\n%d\n", width, height, (depth <= 8) ? 255 : 65535);
            break;
        default:
            if (depth == 1)
                fprintf (ofp, "P4\n# SANE data follows\n%d %d\n", width, height);
            else
                fprintf (ofp, "P5\n# SANE data follows\n%d %d\n%d\n", width, height,(depth <= 8) ? 255 : 65535);
            break;
    }
}

static SANE_Status scan_it (FILE *ofp)
{
    int i, len, first_frame = 1, offset = 0, must_buffer = 0, hundred_percent;
    SANE_Byte min = 0xff, max = 0;
    SANE_Parameters parm;
    SANE_Status status;
    Image image = { 0, 0, 0, 0, 0 };
    static const char *format_name[] = {"gray", "RGB", "red", "green", "blue"};
    SANE_Word total_bytes = 0, expected_bytes;
    SANE_Int hang_over = -1;

    do
    {
        if (!first_frame)
        {
            status = sane_start (device);
            if (status != SANE_STATUS_GOOD)
            {
                goto cleanup;
            }
        }

        status = sane_get_parameters (device, &parm);
        if (status != SANE_STATUS_GOOD)
        {
          goto cleanup;
        }

        if (first_frame)
        {
            switch (parm.format)
            {
                case SANE_FRAME_RED:
                case SANE_FRAME_GREEN:
                case SANE_FRAME_BLUE:
                  printf("par.format=blue\n");
                  assert (parm.depth == 8);
                  must_buffer = 1;
                  offset = parm.format - SANE_FRAME_RED;
                  break;
                case SANE_FRAME_RGB:
                  printf("par.format=rgb\n");
                  assert ((parm.depth == 8) || (parm.depth == 16));
                case SANE_FRAME_GRAY:
                    printf("par.format=gray\n");
                    assert ((parm.depth == 1) || (parm.depth == 8) || (parm.depth == 16));
                    if (parm.lines < 0)
                    {
                        must_buffer = 1;
                        offset = 0;
                    }
                    else
                    {
                        write_pnm_header (parm.format, parm.pixels_per_line,parm.lines, parm.depth, ofp);
                    }
                  break;
                default:
                  break;
            }

            if (must_buffer)
            {
                image.width = parm.bytes_per_line;
                if (parm.lines >= 0)
                    image.height = parm.lines - STRIP_HEIGHT + 1;
                else
                    image.height = 0;

                image.x = image.width - 1;
                image.y = -1;
               // std::advance()
                //if (!advance (&image))
               // {
                //    status = SANE_STATUS_NO_MEM;
               //     goto cleanup;
                //}
            }
        }
        else
        {
            assert (parm.format >= SANE_FRAME_RED && parm.format <= SANE_FRAME_BLUE);
            offset = parm.format - SANE_FRAME_RED;
            image.x = image.y = 0;
        }

        //Mike Wrong
        hundred_percent = parm.bytes_per_line * parm.lines * ((parm.format == SANE_FRAME_RGB || parm.format == SANE_FRAME_GRAY) ? 1:3);

        while (1)
        {
            double progr;
            status = sane_read (device, buffer, buffer_size, &len);
            total_bytes += (SANE_Word) len;
            progr = ((total_bytes * 100.) / (double) hundred_percent);
            if (progr > 100.)
                progr = 100.;

            if (status != SANE_STATUS_GOOD)
            {
                if (status != SANE_STATUS_EOF)
                {
                    return status;
                }
                break;
            }

            if (must_buffer)
            {
                switch (parm.format)
                {
                    case SANE_FRAME_RED:
                    case SANE_FRAME_GREEN:
                    case SANE_FRAME_BLUE:
                        for (i = 0; i < len; ++i)
                        {
                            image.data[offset + 3 * i] = buffer[i];
                            //if (!advance (&image,1))
                            //{
                            //    status = SANE_STATUS_NO_MEM;
                            //    goto cleanup;
                           // }
                        }
                        offset += 3 * len;
                        break;
                        //Mike
                    case SANE_FRAME_RGB:
                        for (i = 0; i < len; ++i)
                        {
                            image.data[offset + i] = buffer[i];
                            //if (!advance (&image))
                            //{
                           //     status = SANE_STATUS_NO_MEM;
                           //     goto cleanup;
                          //  }
                        }
                        offset += len;
                        break;
                    case SANE_FRAME_GRAY:
                        for (i = 0; i < len; ++i)
                        {
                            image.data[offset + i] = buffer[i];
                            //if (!advance (&image))
                           // {
                           //     status = SANE_STATUS_NO_MEM;
                          //      goto cleanup;
                          //  }
                        }
                        offset += len;
                        break;
                    default:
                        break;
                }
            }
            else			/* ! must_buffer */
            {
                if ((parm.depth != 16))
                    fwrite (buffer, 1, len, ofp);
                else
                {
#if !defined(WORDS_BIGENDIAN)
                    int i, start = 0;
                    /* check if we have saved one byte from the last sane_read */
                    if (hang_over > -1)
                    {
                        if (len > 0)
                        {
                            fwrite (buffer, 1, 1, ofp);
                            buffer[0] = (SANE_Byte) hang_over;
                            hang_over = -1;
                            start = 1;
                        }
                    }
                    /* now do the byte-swapping */
                    for (i = start; i < (len - 1); i += 2)
                    {
                        unsigned char LSB;
                        LSB = buffer[i];
                        buffer[i] = buffer[i + 1];
                        buffer[i + 1] = LSB;
                    }
                    /* check if we have an odd number of bytes */
                    if (((len - start) % 2) != 0)
                    {
                        hang_over = buffer[len - 1];
                        len--;
                    }
#endif
                    fwrite (buffer, 1, len, ofp);
                }
            }

            if (verbose && parm.depth == 8)
            {
              for (i = 0; i < len; ++i)
                if (buffer[i] >= max)
                    max = buffer[i];
                else if (buffer[i] < min)
                    min = buffer[i];
            }
        }
        first_frame = 0;
    }while (!parm.last_frame);

    if (must_buffer)
    {
        image.height = image.y;
        write_pnm_header (parm.format, parm.pixels_per_line,image.height, parm.depth, ofp);

#if !defined(WORDS_BIGENDIAN)
        if (parm.depth == 16)
        {
            int i;
            for (i = 0; i < image.height * image.width; i += 2)
            {
                unsigned char LSB;
                LSB = image.data[i];
                image.data[i] = image.data[i + 1];
                image.data[i + 1] = LSB;
            }
        }
#endif
        fwrite (image.data, 1, image.height * image.width, ofp);
    }

    fflush( ofp );

cleanup:
    if (image.data)
        free (image.data);

    return status;
}

SANE_Status do_scan(const char *fileName)
{
    SANE_Status status;
    FILE *ofp = NULL;
    char path[PATH_MAX];
    char part_path[PATH_MAX];
    buffer_size = (32 * 1024);
    //vector 20200119 修改
    //buffer = malloc (buffer_size);
    buffer = (SANE_Byte *) malloc(buffer_size);
    do
    {
        int dwProcessID = getpid();
        sprintf (path, "/tmp/%s-%d.jpg\n", fileName, dwProcessID);
        strcpy (part_path, path);
        strcat (part_path, ".part");

        printf("folder:%s\n",part_path);

        status = sane_start (device);
        if (status != SANE_STATUS_GOOD)
        {
            break;
        }

        if (NULL == (ofp = fopen (part_path, "w+")))
        {
            printf("open fail errno = %d reason = %s \n", errno, strerror(errno));
            status = SANE_STATUS_ACCESS_DENIED;
            break;
        }

        status = scan_it (ofp);

        switch (status)
        {
            case SANE_STATUS_GOOD:
            case SANE_STATUS_EOF:
                 {
                      status = SANE_STATUS_GOOD;
                      if (!ofp || 0 != fclose(ofp))
                      {
                          status = SANE_STATUS_ACCESS_DENIED;
                          break;
                      }
                      else
                      {
                          ofp = NULL;
                          if (rename (part_path, path))
                          {
                              status = SANE_STATUS_ACCESS_DENIED;
                              break;
                          }
                      }
                  }
                  break;
            default:
                  break;
        }
    }while (0);

    if (SANE_STATUS_GOOD != status)
    {
        sane_cancel (device);
    }
    if (ofp)
    {
        fclose (ofp);
        ofp = NULL;
    }
    if (buffer)
    {
        free (buffer);
        buffer = NULL;
    }

    return status;
}

// Initialize SANE
SANE_Status init()
{
    SANE_Int version_code = 0;
    SANE_Status status = sane_init (&version_code, auth_callback);
    printf("SANE version code: %d\n", version_code);
    return status;
}

// Get all devices
SANE_Status get_devices(const SANE_Device ***device_list)
{
    printf("Get all devices...\n");
    //vector 20200118 修改
    //SANE_Status sane_status = 0;
    SANE_Status sane_status =  sane_get_devices (device_list, SANE_TRUE);
    printf("sane_get_devices status: %s\n", sane_strstatus(sane_status));
    return sane_status;
}

// Open a device
SANE_Status open_device(const SANE_Device *device, SANE_Handle *sane_handle)
{
    printf("Open Device's Name: %s, vendor: %s, model: %s, type: %s\n", device->name, device->model, device->vendor, device->type);
    SANE_Status sane_status = sane_open(device->name, sane_handle);
    return sane_status;
}

// Start scanning
SANE_Status start_scan(SANE_Handle sane_handle, SANE_String_Const fileName)
{
    device = sane_handle;
    return do_scan(fileName);
}

// Cancel scanning
void cancle_scan(SANE_Handle sane_handle)
{
    sane_cancel(sane_handle);
}

// Close SANE device
void close_device(SANE_Handle sane_handle)
{
    sane_close(sane_handle);
}

// Release SANE resources
void app_exit()
{
    sane_exit();
}


void test_SANE(){

   printf("init sane\n");
   init();
   printf("init sane end\n");

   do{
       printf("get device list\n");
        const SANE_Device ** device_list=NULL;
        SANE_Status sane_status = SANE_Status::SANE_STATUS_GOOD;
        if(sane_status = get_devices(&device_list)){
            printf("no device \n");
            break;
        }
        printf("device count:%d\n",sizeof(device_list));


        printf("open device\n");
        SANE_Handle sane_handle = new SANE_Handle();
        const SANE_Device *device = *device_list;
        /*
        if(!device)
        {
            printf("no device connection\n");
            break;
        }
        */
        if(sane_status = open_device(device,&sane_handle))
        {
            break;
        }



        printf("set parameter\n");

        int nOptionIndex = -1;
        while(true){
            nOptionIndex ++;
            const SANE_Option_Descriptor * saneOptionDescriptor = sane_get_option_descriptor (sane_handle, nOptionIndex);

            if(saneOptionDescriptor== NULL){

                 break;
            }
            printf("name:%s\n",saneOptionDescriptor->name);
            printf("title:%s\n",saneOptionDescriptor->title);
            printf("type:%d\n",saneOptionDescriptor->type);
            printf("size:%d\n",saneOptionDescriptor->size);
            printf("constraint_type:%d\n",saneOptionDescriptor->constraint_type);
            printf("unit:%d\n",saneOptionDescriptor->unit);
            printf("cap:%d\n",saneOptionDescriptor->cap);

            if(saneOptionDescriptor->constraint_type == 1)
                 printf("rang Min:%d,rang max:%d,rang quant:%d\n",saneOptionDescriptor->constraint.range->min,saneOptionDescriptor->constraint.range->max,saneOptionDescriptor->constraint.range->quant);
            if(saneOptionDescriptor->constraint_type == 3)
            {
                 SANE_String * pstringValue = (SANE_String *)saneOptionDescriptor->constraint.string_list;

                  printf("string %s\n",pstringValue[0]);
                  printf("string %s\n",pstringValue[1]);
                  printf("string %s\n",pstringValue[2]);
                   printf("string %s\n",pstringValue[3]);

            }

            if(saneOptionDescriptor->constraint_type == 2)
            {
                 SANE_Word * wordValue = (SANE_Word *)saneOptionDescriptor->constraint.word_list;
                 int nwordSize = *wordValue;
                 for(int constranitWordIndex = 0; constranitWordIndex< nwordSize;constranitWordIndex++)
                    printf("word %d\n",*( ++wordValue));
            }


           char * value = new char[saneOptionDescriptor->size];
           SANE_Int info = 0;
           sane_status = sane_control_option (sane_handle,nOptionIndex,SANE_Action::SANE_ACTION_GET_VALUE, value,&info);
           if(saneOptionDescriptor->type == SANE_TYPE_INT)
                    printf("get parameter result is: %d\n",*(int *)value);

            if(saneOptionDescriptor->type == SANE_TYPE_STRING)
                printf("get parameter result is: %s\n",value);
            delete value;
        }

/*

        int nDPI = 300;
        SANE_String sScanSource = "ADF_DUPLEX";
        SANE_String sScanMode = "Gray8";
        SANE_Int info = 0;

        sane_status = sane_control_option (sane_handle, 2,SANE_Action::SANE_ACTION_SET_VALUE, sScanSource,&info);
        sane_status = sane_control_option (sane_handle, 3,SANE_Action::SANE_ACTION_SET_VALUE, &nDPI,&info);
        sane_status = sane_control_option (sane_handle, 1,SANE_Action::SANE_ACTION_SET_VALUE, sScanMode,&info);


        printf("scanning...\n");
        start_scan(sane_handle,"hello-sane");

        printf("cancle scan\n");
        cancle_scan(sane_handle);

        printf("close device\n");
        close_device(sane_handle);
*/

   }while (0);



}

#ifdef __cplusplus
}
#endif




