#ifndef CPNG_H
#define CPNG_H

#define PNG_BYTES_TO_CHECK	8
#define HAVE_ALPHA			1
#define NOT_HAVE_ALPHA		0

typedef struct _pic_data pic_data;
struct _pic_data {
    int width, height; 	//长宽
    int bit_depth; 	   	//位深度
    int alpha_flag;		//是否有透明通道
    unsigned char *rgba;//实际rgb数据
};

class CPNG
{
public:
    CPNG();
    int write_png_file(const char *filename , pic_data *out);
    int decode_png(const char *filename, pic_data *out);
};

#endif // CPNG_H
