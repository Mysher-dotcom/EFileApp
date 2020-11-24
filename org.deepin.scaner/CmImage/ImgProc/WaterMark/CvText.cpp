
#include <cwchar>
#include <clocale>
#include <cctype>
#include <utility>
#include "tools.h"
#include "CvText.h"

 
CvText::CvText(const char *fontName) {
    ASSERT(fontName != NULL,"字体名称为空");
 
    // 打开字库文件, 创建一个字体
    if (FT_Init_FreeType(&m_library)) throw;
    if (FT_New_Face(m_library, fontName, 0, &m_face)) throw;
    FT_Select_Charmap(m_face,FT_ENCODING_UNICODE);
 
    // 设置字体输出参数
    restoreFont();
 
    // 设置C语言的字符集环境
//    setlocale(LC_ALL, "zh_CN.utf8");
}
 
// 释放FreeType资源
CvText::~CvText() {
    FT_Done_Face(m_face);
    FT_Done_FreeType(m_library);
}
 
// 设置字体属性
void CvText::setFont(int type, cv::Scalar size, bool underline, float diaphaneity) {
    // 参数合法性检查
	m_fontType = type;
 
    m_fontSize.val[0] = fabs(size.val[0]);
    m_fontSize.val[1] = fabs(size.val[1]);
    m_fontSize.val[2] = fabs(size.val[2]);
    m_fontSize.val[3] = fabs(size.val[3]);


    m_fontUnderline = underline;

	m_fontDiaphaneity = diaphaneity;
}
 
// 恢复默认的字体设置
void CvText::restoreFont() {
    m_fontType = 0;             // 字体类型(不支持)
 
    m_fontSize.val[0] = 15;     // 字体大小
    m_fontSize.val[1] = 0.5;    // 空白字符大小比例
    m_fontSize.val[2] = 0.1;    // 间隔大小比例
    m_fontSize.val[3] = 0;      // 旋转角度(不支持)
 
    m_fontUnderline = false;    // 下画线(不支持)
 
    m_fontDiaphaneity = 1.0;    // 色彩比例(可产生透明效果)
 
    // 设置字符大小
    FT_Set_Pixel_Sizes(m_face, (FT_UInt) m_fontSize.val[0], 0);
}
 
 
int CvText::putText(cv::Mat &frame, std::string text, cv::Point &pos, cv::Scalar color)
{
	return putText(frame,text.c_str(),pos,color);// std::move(color));
     
}
 
int CvText::putText(cv::Mat &frame, const char *text, cv::Point &pos, cv::Scalar color) {
 
 
    if (frame.empty())
        return -1;
    if (text == NULL)
        return -1;

    wchar_t *w_str ;
	cout<<text<<endl;
    int count = char2Wchar(text, w_str);
    cout<<count<<endl;
    int i=0;
    for (; i<count; ++i) {
        wchar_t wc = w_str[i];
        //如果是ascii字符(范围0~127)，调整字体大小
        //因为ascii字符在同样的m_fontSize下更小，所以要放大一点
        if(wc<128)
            FT_Set_Pixel_Sizes(m_face, (FT_UInt)(m_fontSize.val[0]*1.15), 0);
        else
            FT_Set_Pixel_Sizes(m_face, (FT_UInt)m_fontSize.val[0], 0);
        // 输出当前的字符
        putWChar(frame, wc, pos, color);
    }
    delete(w_str);
    return i;
}
 
/**
 * 将char字符数组转换为wchar_t字符数组
 * @param src char字符数组
 * @param dst wchar_t字符数组
 * @param locale 语言环境，mbstowcs函数依赖此值来判断src的编码方式
 * @return 运行成功返回0,否则返回-1
 */

// 将char类型转化为wchar
// src:  源
// dest: 目标
// locale: 环境变量的值，mbstowcs依赖此值来判断src的编码方式
// 运行成功返回0 否则返回-1
//
int CvText::char2Wchar(const char* &src, wchar_t* &dest, const char *locale)
{
  if (src == NULL) {
    dest = NULL;
    return 0;
  }
 
  // 根据环境变量设置locale
  setlocale(LC_CTYPE, locale);
 
  // 得到转化为需要的宽字符大小
  int w_size = mbstowcs(NULL, src, 0) + 1;
 
  // w_size = 0 说明mbstowcs返回值为-1。即在运行过程中遇到了非法字符(很有可能使locale
  // 没有设置正确)
  if (w_size == 0) {
    dest = NULL;
    return -1;
  }
 
  wcout << "w_size" << w_size << endl;
  dest = new wchar_t[w_size];
  if (!dest) {
      return -1;
  }
 
  int ret = mbstowcs(dest, src, strlen(src)+1);
  if (ret <= 0) {
    return -1;
  }
  return ret;
}

/*
int CvText::char2Wchar(const char *&src, wchar_t *&dst, const char *locale)
{

    if (src == NULL) {
        dst = NULL;
        return 0;
    }

#if 0
	string temp = src;
	int ret = 0;
	for (int ii = 0; ii < temp.size(); ii++)
	{
		char aa = temp.at(ii);
		string aa1;
		if (~(aa >> 8) == 0)
		{
			aa1 = temp.substr(ii, 2);
			ii += 1;
		}
		else
			aa1 = temp.substr(ii, 1);

		ret += 1;

	}

	using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
 
    std::wstring tt = converterX.from_bytes(temp);
	dst = new wchar_t[200];
	memset(dst, 0, 200);
	memccpy(dst, tt.data(), sizeof(wchar_t), tt.size());
	return ret;
#else 
    // 设置C语言的字符集环境
	setlocale(LC_CTYPE, locale);
	
    // 得到转化为需要的宽字符大小
	string temp = src;
	int w_size = 0;
	for (int ii = 0; ii < temp.size(); ii++)
	{
		char aa = temp.at(ii);
		string aa1;
		if (~(aa >> 8) == 0)
		{
			aa1 = temp.substr(ii, 2);
			ii += 1;
		}
		else
			aa1 = temp.substr(ii, 1);

		w_size += 1;

	}
	w_size += 1;
    //int w_size = (int)mbstowcs(NULL, src, 0) + 1;
 
    // w_size = 0 说明mbstowcs返回值为-1。即在运行过程中遇到了非法字符(很有可能是locale没有设置正确)
    if (w_size == 0) 
	{
        dst = NULL;
        return -1;
    }

    dst = new wchar_t[w_size];
    if (dst == NULL) 
	{
        return -1;
    }

    int ret = (int)mbstowcs(dst, src, strlen(src)+1);
	cout<<"ret=:"<<ret<<endl;
    if (ret <= 0) 
	{
        return -1;
    }
    return ret;
#endif
}
 */
 
// 输出当前字符, 更新m_pos位置
void CvText::putWChar(cv::Mat &frame, wchar_t wc, cv::Point &pos, cv::Scalar color) {
    // 根据unicode生成字体的二值位图  
    IplImage img = IplImage(frame);
 
    FT_UInt glyph_index = FT_Get_Char_Index(m_face, (FT_ULong)wc);
    FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT);
    FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_MONO);
 
    FT_GlyphSlot slot = m_face->glyph;
 
    // 行列数
    int rows = slot->bitmap.rows;
    int cols = slot->bitmap.width;
 
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int off = ((img.origin == 0) ? i : (rows - 1 - i)) * slot->bitmap.pitch + j / 8;
 
            if (slot->bitmap.buffer[off] & (0xC0 >> (j % 8))) {
                int r = (img.origin == 0) ? pos.y - (rows - 1 - i) : pos.y + i;;
                int c = pos.x + j;
 
                if (r >= 0 && r < img.height
                    && c >= 0 && c < img.width) {
                    CvScalar scalar = cvGet2D(&img, r, c);
 
                    // 进行色彩融合
                    float p = m_fontDiaphaneity;
                    for (int k = 0; k < 4; ++k) {
                        scalar.val[k] = scalar.val[k] * (1 - p) + color.val[k] * p;
                    }
                    cvSet2D(&img, r, c, scalar);
                }
            }
        } // end for  
    } // end for  
 
    // 修改下一个字的输出位置
    double space = m_fontSize.val[0] * m_fontSize.val[1];
    double sep = m_fontSize.val[0] * m_fontSize.val[2];

    pos.x += (int) ((cols ? cols : space) + sep);
}

