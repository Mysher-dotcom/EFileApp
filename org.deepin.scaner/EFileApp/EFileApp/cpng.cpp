#include "cpng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "png.h"

CPNG::CPNG()
{

}
int check_is_png(FILE **fp, const char *filename) //检查是否png文件
{
    char checkheader[PNG_BYTES_TO_CHECK]; //查询是否png头
    *fp = fopen(filename, "rb");
    if (*fp == NULL) {
        printf("open failed ...1\n");
        return 0;
    }
    if (fread(checkheader, 1, PNG_BYTES_TO_CHECK, *fp) != PNG_BYTES_TO_CHECK) //读取png文件长度错误直接退出
        return 0;
    return png_sig_cmp((png_bytep)checkheader, (png_size_t)0, PNG_BYTES_TO_CHECK); //0正确, 非0错误
}

void CPNG::read_png(char *file_name)  /* We need to open the file */
{
   png_structp png_ptr;
   png_infop info_ptr;
   int i;
   png_bytep* row_pointers;
   int sig_read = 0;
   char buf[PNG_BYTES_TO_CHECK];
   png_uint_32 width, height;
   int bit_depth,  interlace_type;
   FILE *fp;
   int w, h, x, y, temp, color_type;
   int buffer[500000] = {0};
   int count = 0;
   int png_transforms;
   if ((fp = fopen(file_name, "rb")) == NULL)
      return ;

   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
   /* 为这个结构体分配空间 存储这图像的信息*/
   info_ptr = png_create_info_struct(png_ptr);
   setjmp( png_jmpbuf(png_ptr) );
   /* 若读到的数据并没有PNG_BYTES_TO_CHECK个字节 */
   temp = fread( buf, 1, PNG_BYTES_TO_CHECK, fp );
   if( temp < PNG_BYTES_TO_CHECK ) {
                fclose(fp);
                png_destroy_read_struct( &png_ptr, &info_ptr, 0);
                printf("没有充足的字节\n");
                return /* 返回值 */;
     }
   temp = png_sig_cmp( (png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK );
   if( temp != 0 ) {
                fclose(fp);
                printf("不匹配字符串\n");
                png_destroy_read_struct( &png_ptr, &info_ptr, 0);
                return /* 返回值 */;
      }
   /* 复位文件指针 */
   rewind( fp );
   /*如果我们已经读取了这些信息  跳过相应的函数*/
   /* 开始读文件 */
   png_init_io(png_ptr, fp);
   /* 获取图像的色彩类型 */
   png_read_png(png_ptr, info_ptr, png_transforms, NULL);
   color_type = png_get_color_type( png_ptr, info_ptr );
   /* 获取图像的宽高 */
   //color_type = info_ptr->color_type;
   w = png_get_image_width( png_ptr, info_ptr );
   h = png_get_image_height( png_ptr, info_ptr );
   /* 获取图像的所有行像素数据，row_pointers里边就是rgba数据 */
   row_pointers = png_get_rows( png_ptr, info_ptr );
           /* 根据不同的色彩类型进行相应处理 */
    switch( color_type ) {
    case PNG_COLOR_TYPE_RGB_ALPHA:
            for( y=0; y<h; ++y ) {
                    for( x=0; x<w*4; ) {
                            /* 以下是RGBA数据，需要自己补充代码，保存RGBA数据 */
                            /* 目标内存 */buffer[count++] = row_pointers[y][x++]; // red
                            /* 目标内存 */buffer[count++] = row_pointers[y][x++]; // green
                            /* 目标内存 */buffer[count++] = row_pointers[y][x++]; // blue
                            /* 目标内存 */buffer[count++] = row_pointers[y][x++]; // alpha
                    }
            }
            break;

    case PNG_COLOR_TYPE_RGB:
            for( y=0; y<h; ++y ) {
                    for( x=0; x<w*3; ) {
                            /* 目标内存 */buffer[count++] = row_pointers[y][x++]; // red
                            /* 目标内存 */buffer[count++] = row_pointers[y][x++]; // green
                            /* 目标内存 */buffer[count++] = row_pointers[y][x++]; // blue
                    }
            }
            break;
    /* 其它色彩类型的图像就不读了 */
    default:
            fclose(fp);
            temp = PNG_COLOR_TYPE_RGB_ALPHA;
            printf("其他格式不读了%d \n", color_type);
            png_destroy_read_struct( &png_ptr, &info_ptr, 0);
            return /* 返回值 */;
    }
   /* One of the following I/O initialization methods is REQUIRED */
    for(i = 0; i < 200 ; i++)
    {
       printf("%d ",buffer[i]);
    }
    png_destroy_read_struct( &png_ptr, &info_ptr, 0);
    //return 0;

}

int CPNG::decode_png(const char *filename, pic_data *out) //取出png文件中的rgb数据
{
    printf("decode_png filename is %s\n",filename);
    png_structp png_ptr; //png文件句柄
    png_infop	info_ptr;//png图像信息句柄
    int ret;
    FILE *fp;
    fp = fopen(filename, "rb");
    if (check_is_png(&fp, filename) != 0) {
        printf("file is not png ...\n");
        return -1;
    }

    printf("launcher[%s] ...\n", PNG_LIBPNG_VER_STRING); //打印当前libpng版本号

    //1: 初始化libpng的数据结构 :png_ptr, info_ptr
    png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);
    //png_set_chunk_malloc_max(png_ptr,0);
    //2: 设置错误的返回点
    setjmp(png_jmpbuf(png_ptr));
    rewind(fp); //等价fseek(fp, 0, SEEK_SET);

    //3: 把png结构体和文件流io进行绑定
    png_init_io(png_ptr, fp);
    //4:读取png文件信息以及强转转换成RGBA:8888数据格式
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0); //读取文件信息
    int channels, color_type;
    channels 	= png_get_channels(png_ptr, info_ptr); //通道数量
    color_type 	= png_get_color_type(png_ptr, info_ptr);//颜色类型
    out->bit_depth = png_get_bit_depth(png_ptr, info_ptr);//位深度
    out->width 	 = png_get_image_width(png_ptr, info_ptr);//宽
    out->height  = png_get_image_height(png_ptr, info_ptr);//高
    out->color_type = color_type;
    //if(color_type == PNG_COLOR_TYPE_PALETTE)
    //	png_set_palette_to_rgb(png_ptr);//要求转换索引颜色到RGB
    //if(color_type == PNG_COLOR_TYPE_GRAY && out->bit_depth < 8)
    //	png_set_expand_gray_1_2_4_to_8(png_ptr);//要求位深度强制8bit
    //if(out->bit_depth == 16)
    //	png_set_strip_16(png_ptr);//要求位深度强制8bit
    //if(png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS))
    //	png_set_tRNS_to_alpha(png_ptr);
    //if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    //	png_set_gray_to_rgb(png_ptr);//灰度必须转换成RG
    printf("channels = %d color_type = %d bit_depth = %d width = %d height = %d ...\n",
            channels, color_type, out->bit_depth, out->width, out->height);

    int i, j, k;
    int size, pos = 0;
    int temp;

    //5: 读取实际的rgb数据
    png_bytepp row_pointers; //实际存储rgb数据的buf
    printf("1111111111\n");
    row_pointers = png_get_rows(png_ptr, info_ptr); //也可以分别每一行获取png_get_rowbytes();
    printf("22222222222222\n");
    size = out->width * out->height; //申请内存先计算空间
    if (channels == 4 || color_type == PNG_COLOR_TYPE_RGB_ALPHA) { //判断是24位还是32位
        out->alpha_flag = HAVE_ALPHA; //记录是否有透明通道
        size *= (sizeof(unsigned char) * 4); //size = out->width * out->height * channel
        out->rgba = (png_bytep)malloc(size);
        if (NULL == out->rgba) {
            printf("malloc rgba faile ...\n");
            png_destroy_read_struct(&png_ptr, &info_ptr, 0);
            fclose(fp);
            return -1;
        }
        //从row_pointers里读出实际的rgb数据出来
        temp = channels - 1;
        for (i = 0; i < out->height; i++)
            for (j = 0; j < out->width * 4; j += 4)
                for (k = temp; k >= 0; k--)
                    out->rgba[pos++] = row_pointers[i][j + k];
    } else if (channels == 3 || color_type == PNG_COLOR_TYPE_RGB ||channels == 1 ||  color_type == PNG_COLOR_TYPE_GRAY) { //判断颜色深度是24位还是32位
        out->alpha_flag = NOT_HAVE_ALPHA;
        int widthStep =  (out->width * channels * 8  + 31)/32 * 4;
        size =  widthStep * out->height;
        out->rgba = (png_bytep)malloc(size);
        if (NULL == out->rgba) {
            printf("malloc rgba faile ...\n");
            png_destroy_read_struct(&png_ptr, &info_ptr, 0);
            fclose(fp);
            return -1;
        }
        //从row_pointers里读出实际的rgb数据

        for (i = 0; i < out->height; i ++) {
            memcpy(out->rgba + pos,row_pointers[i],out->width * channels);
            pos += (widthStep);
        }
    }

    else return -1;
    //6:销毁内存
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    fclose(fp);
    //此时， 我们的out->rgba里面已经存储有实际的rgb数据了
    //处理完成以后free(out->rgba)
    return 0;
}

int CPNG::write_png_file(const char *filename , pic_data *out) //生成一个新的png图像
{

    png_structp png_ptr;
    png_infop 	info_ptr;
    png_byte color_type;
    png_bytep * row_pointers;
    FILE *fp = fopen(filename, "wb");
    if (NULL == fp) {
        printf("open failed ...2\n");
        return -1;
    }
    //1: 初始化libpng结构体
    png_ptr	= png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png_ptr) {
        printf("png_create_write_struct failed ...\n");
        return -1;
    }
    //2: 初始化png_infop结构体 ，
    //此结构体包含了图像的各种信息如尺寸，像素位深, 颜色类型等等
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        printf("png_create_info_struct failed ...\n");
        return -1;
    }
    //3: 设置错误返回点
    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("error during init_io ...\n");
        return -1;
    }
    //4:绑定文件IO到Png结构体
    png_init_io(png_ptr, fp);
    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("error during init_io ...\n");
        return -1;
    }
    color_type = out->color_type;
    //if (out->alpha_flag == HAVE_ALPHA) color_type = PNG_COLOR_TYPE_RGB_ALPHA;
    //else color_type = PNG_COLOR_TYPE_RGB;
    //5：设置以及写入头部信息到Png文件
    png_set_IHDR(png_ptr, info_ptr, out->width, out->height, out->bit_depth,
    color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);
    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("error during init_io ...\n");
        return -1;
    }
    int channels, temp;
    int i, j, pos = 0;
    if (out->alpha_flag == HAVE_ALPHA&&out->color_type==2) {
        channels = 4;
        temp = (4 * out->width);
        printf("have alpha ...\n");
    }
    else if(out->alpha_flag==NOT_HAVE_ALPHA&&out->color_type==2){
        channels = 3;
        temp = (3 * out->width);
        printf("not have alpha ...\n");
    }
    else {
        channels = 1;
        temp = (out->width);
        printf("not have alpha Gary...\n");
    }
    // 顺时针旋转90度 ， 旋转完了一定要把width 和height调换 不然得到的图像是花的  旋转三次就是逆时针旋转一次
    //RotationRight90(out->rgba, out->width, out->height, channels);
    row_pointers = (png_bytep*)malloc(out->height * sizeof(png_bytep));
    for (i = 0; i < out->height; i++) {
        row_pointers[i] = (png_bytep)malloc(temp* sizeof(unsigned char));
        for (j = 0; j < temp; j += channels) {
            if (channels == 4) {
                row_pointers[i][j+3] = out->rgba[pos++];
                row_pointers[i][j+2] = out->rgba[pos++];
                row_pointers[i][j+1] = out->rgba[pos++];
                row_pointers[i][j+0] = out->rgba[pos++];
            } else  if (channels == 3){
                row_pointers[i][j+0] = out->rgba[pos++];
                row_pointers[i][j+1] = out->rgba[pos++];
                row_pointers[i][j+2] = out->rgba[pos++];
            }
            else {
                row_pointers[i][j] = out->rgba[pos++];
            }
        }
    }

    //6: 写入rgb数据到Png文件
    png_write_image(png_ptr, row_pointers);
    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("error during init_io ...\n");
        return -1;
    }
    //7: 写入尾部信息
    png_write_end(png_ptr, NULL);
    //8:释放内存 ,销毁png结构体
    for (i = 0; i < out->height; i ++)
        free(row_pointers[i]);
    free(row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
}
