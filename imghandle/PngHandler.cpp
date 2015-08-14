/*
 * PngHandler.cpp
 *
 *  Created on: 2015-05-12
 *      Author: Samson
 */

#include "PngHandler.h"
#include <common/KLog.h>

#include "PngReader.h"
#include "PngWriter.h"

//static bool setWriteEndInfoByReadEndInfo(png_structp read_ptr, png_infop end_info_ptr, png_structp write_ptr, png_infop write_end_info_ptr)
//{
//	bool result = false;
//
//#ifdef PNG_TEXT_SUPPORTED
//   {
//      png_textp text_ptr;
//      int num_text;
//
//      if (png_get_text(read_ptr, end_info_ptr, &text_ptr, &num_text) > 0)
//      {
////         pngtest_debug1("Handling %d iTXt/tEXt/zTXt chunks", num_text);
//
////         pngtest_check_text_support(read_ptr, text_ptr, num_text);
//
//         if (verbose != 0)
//         {
//            int i;
//
//            printf("\n");
//            for (i=0; i<num_text; i++)
//            {
//               printf("   Text compression[%d]=%d\n",
//                     i, text_ptr[i].compression);
//            }
//         }
//
//         png_set_text(write_ptr, write_end_info_ptr, text_ptr, num_text);
//      }
//   }
//#endif
//#ifdef PNG_tIME_SUPPORTED
//   {
//      png_timep mod_time;
//
//      if (png_get_tIME(read_ptr, end_info_ptr, &mod_time) != 0)
//      {
//         png_set_tIME(write_ptr, write_end_info_ptr, mod_time);
//#ifdef PNG_TIME_RFC1123_SUPPORTED
//         if (png_convert_to_rfc1123_buffer(tIME_string, mod_time) != 0)
//            tIME_string[(sizeof tIME_string) - 1] = '\0';
//
//         else
//         {
//            strncpy(tIME_string, "*** invalid time ***", sizeof tIME_string);
//            tIME_string[(sizeof tIME_string)-1] = '\0';
//         }
//
//#endif /* TIME_RFC1123 */
//      }
//   }
//#endif
//
//	return result;
//}
//
//
//// 由于不希望暴露pnglib相关函数或数据类型给调用者，因此子函数没有定义成成员
//// ConvertImage的子函数，用于处理png文件的配置
//static bool setWriteInfoByReadInfo(png_structp read_ptr, png_infop read_info_ptr, png_structp write_ptr, png_infop write_info_ptr, png_uint_32 imageSize)
//{
//	bool result = false;
//
//	// 读取png文件信息
//    png_uint_32 width;
//    png_uint_32 height;
//	int bit_depth;
//    int color_type;
//    int interlace_type;
//	int compression_type;
//	int filter_type;
//	if (png_get_IHDR(read_ptr, read_info_ptr, &width, &height, &bit_depth,
//          &color_type, &interlace_type, &compression_type, &filter_type) != 0)
//      {
//		png_set_IHDR(write_ptr, write_info_ptr, imageSize, imageSize, bit_depth,
//            color_type, interlace_type, compression_type, filter_type);
//
//		result = true;
//	}
//
//	#ifdef PNG_FIXED_POINT_SUPPORTED
//#ifdef PNG_cHRM_SUPPORTED
//   {
//      png_fixed_point white_x, white_y, red_x, red_y, green_x, green_y, blue_x,
//         blue_y;
//
//      if (png_get_cHRM_fixed(read_ptr, read_info_ptr, &white_x, &white_y,
//         &red_x, &red_y, &green_x, &green_y, &blue_x, &blue_y) != 0)
//      {
//         png_set_cHRM_fixed(write_ptr, write_info_ptr, white_x, white_y, red_x,
//            red_y, green_x, green_y, blue_x, blue_y);
//      }
//   }
//#endif
//#ifdef PNG_gAMA_SUPPORTED
//   {
//      png_fixed_point gamma;
//
//      if (png_get_gAMA_fixed(read_ptr, read_info_ptr, &gamma) != 0)
//         png_set_gAMA_fixed(write_ptr, write_info_ptr, gamma);
//   }
//#endif
//#else /* Use floating point versions */
//#ifdef PNG_FLOATING_POINT_SUPPORTED
//#ifdef PNG_cHRM_SUPPORTED
//   {
//      double white_x, white_y, red_x, red_y, green_x, green_y, blue_x,
//         blue_y;
//
//      if (png_get_cHRM(read_ptr, read_info_ptr, &white_x, &white_y, &red_x,
//         &red_y, &green_x, &green_y, &blue_x, &blue_y) != 0)
//      {
//         png_set_cHRM(write_ptr, write_info_ptr, white_x, white_y, red_x,
//            red_y, green_x, green_y, blue_x, blue_y);
//      }
//   }
//#endif
//#ifdef PNG_gAMA_SUPPORTED
//   {
//      double gamma;
//
//      if (png_get_gAMA(read_ptr, read_info_ptr, &gamma) != 0)
//         png_set_gAMA(write_ptr, write_info_ptr, gamma);
//   }
//#endif
//#endif /* Floating point */
//#endif /* Fixed point */
//#ifdef PNG_iCCP_SUPPORTED
//   {
//      png_charp name;
//      png_bytep profile;
//      png_uint_32 proflen;
//      int compression_type;
//
//      if (png_get_iCCP(read_ptr, read_info_ptr, &name, &compression_type,
//                      &profile, &proflen) != 0)
//      {
//         png_set_iCCP(write_ptr, write_info_ptr, name, compression_type,
//                      profile, proflen);
//      }
//   }
//#endif
//#ifdef PNG_sRGB_SUPPORTED
//   {
//      int intent;
//
//      if (png_get_sRGB(read_ptr, read_info_ptr, &intent) != 0)
//         png_set_sRGB(write_ptr, write_info_ptr, intent);
//   }
//#endif
//   {
//      png_colorp palette;
//      int num_palette;
//
//      if (png_get_PLTE(read_ptr, read_info_ptr, &palette, &num_palette) != 0)
//         png_set_PLTE(write_ptr, write_info_ptr, palette, num_palette);
//   }
//#ifdef PNG_bKGD_SUPPORTED
//   {
//      png_color_16p background;
//
//      if (png_get_bKGD(read_ptr, read_info_ptr, &background) != 0)
//      {
//         png_set_bKGD(write_ptr, write_info_ptr, background);
//      }
//   }
//#endif
//#ifdef PNG_hIST_SUPPORTED
//   {
//      png_uint_16p hist;
//
//      if (png_get_hIST(read_ptr, read_info_ptr, &hist) != 0)
//         png_set_hIST(write_ptr, write_info_ptr, hist);
//   }
//#endif
//#ifdef PNG_oFFs_SUPPORTED
//   {
//      png_int_32 offset_x, offset_y;
//      int unit_type;
//
//      if (png_get_oFFs(read_ptr, read_info_ptr, &offset_x, &offset_y,
//          &unit_type) != 0)
//      {
//         png_set_oFFs(write_ptr, write_info_ptr, offset_x, offset_y, unit_type);
//      }
//   }
//#endif
//#ifdef PNG_pCAL_SUPPORTED
//   {
//      png_charp purpose, units;
//      png_charpp params;
//      png_int_32 X0, X1;
//      int type, nparams;
//
//      if (png_get_pCAL(read_ptr, read_info_ptr, &purpose, &X0, &X1, &type,
//         &nparams, &units, &params) != 0)
//      {
//         png_set_pCAL(write_ptr, write_info_ptr, purpose, X0, X1, type,
//            nparams, units, params);
//      }
//   }
//#endif
//#ifdef PNG_pHYs_SUPPORTED
//   {
//      png_uint_32 res_x, res_y;
//      int unit_type;
//
//      if (png_get_pHYs(read_ptr, read_info_ptr, &res_x, &res_y,
//          &unit_type) != 0)
//         png_set_pHYs(write_ptr, write_info_ptr, res_x, res_y, unit_type);
//   }
//#endif
//#ifdef PNG_sBIT_SUPPORTED
//   {
//      png_color_8p sig_bit;
//
//      if (png_get_sBIT(read_ptr, read_info_ptr, &sig_bit) != 0)
//         png_set_sBIT(write_ptr, write_info_ptr, sig_bit);
//   }
//#endif
//#ifdef PNG_sCAL_SUPPORTED
//#if defined(PNG_FLOATING_POINT_SUPPORTED) && \
//   defined(PNG_FLOATING_ARITHMETIC_SUPPORTED)
//   {
//      int unit;
//      double scal_width, scal_height;
//
//      if (png_get_sCAL(read_ptr, read_info_ptr, &unit, &scal_width,
//         &scal_height) != 0)
//      {
//         png_set_sCAL(write_ptr, write_info_ptr, unit, scal_width, scal_height);
//      }
//   }
//#else
//#ifdef PNG_FIXED_POINT_SUPPORTED
//   {
//      int unit;
//      png_charp scal_width, scal_height;
//
//      if (png_get_sCAL_s(read_ptr, read_info_ptr, &unit, &scal_width,
//          &scal_height) != 0)
//      {
//         png_set_sCAL_s(write_ptr, write_info_ptr, unit, scal_width,
//             scal_height);
//      }
//   }
//#endif
//#endif
//#endif
//#ifdef PNG_TEXT_SUPPORTED
//   {
//      png_textp text_ptr;
//      int num_text;
//
//      if (png_get_text(read_ptr, read_info_ptr, &text_ptr, &num_text) > 0)
//      {
//         png_set_text(write_ptr, write_info_ptr, text_ptr, num_text);
//      }
//   }
//#endif
//#ifdef PNG_tIME_SUPPORTED
//   {
//      png_timep mod_time;
//
//      if (png_get_tIME(read_ptr, read_info_ptr, &mod_time) != 0)
//      {
//         png_set_tIME(write_ptr, write_info_ptr, mod_time);
//      }
//   }
//#endif
//#ifdef PNG_tRNS_SUPPORTED
//   {
//      png_bytep trans_alpha;
//      int num_trans;
//      png_color_16p trans_color;
//
//      if (png_get_tRNS(read_ptr, read_info_ptr, &trans_alpha, &num_trans,
//         &trans_color) != 0)
//      {
//         int sample_max = (1 << bit_depth);
//         /* libpng doesn't reject a tRNS chunk with out-of-range samples */
//         if (!((color_type == PNG_COLOR_TYPE_GRAY &&
//             (int)trans_color->gray > sample_max) ||
//             (color_type == PNG_COLOR_TYPE_RGB &&
//             ((int)trans_color->red > sample_max ||
//             (int)trans_color->green > sample_max ||
//             (int)trans_color->blue > sample_max))))
//            png_set_tRNS(write_ptr, write_info_ptr, trans_alpha, num_trans,
//               trans_color);
//      }
//   }
//#endif
//#ifdef PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED
//   {
//      png_unknown_chunkp unknowns;
//      int num_unknowns = png_get_unknown_chunks(read_ptr, read_info_ptr,
//         &unknowns);
//
//      if (num_unknowns != 0)
//      {
//         png_set_unknown_chunks(write_ptr, write_info_ptr, unknowns,
//           num_unknowns);
//#if PNG_LIBPNG_VER < 10600
//         /* Copy the locations from the read_info_ptr.  The automatically
//          * generated locations in write_end_info_ptr are wrong prior to 1.6.0
//          * because they are reset from the write pointer (removed in 1.6.0).
//          */
//         {
//            int i;
//            for (i = 0; i < num_unknowns; i++)
//              png_set_unknown_chunk_location(write_ptr, write_info_ptr, i,
//                unknowns[i].location);
//         }
//#endif
//      }
//   }
//#endif
//
//#ifdef PNG_WRITE_SUPPORTED
//   /* Write the info in two steps so that if we write the 'unknown' chunks here
//    * they go to the correct place.
//    */
//   png_write_info_before_PLTE(write_ptr, write_info_ptr);
//   png_write_info(write_ptr, write_info_ptr);
//#endif
//
//	return result;
//}
//
//#define SUBFILENAME "_%d"
//// ConvertImage的子函数，用于生成小图片
//static bool CutImage(
//		const string& path
//		, png_structp readPtr
//		, png_infop readInfoPtr
//		, png_infop readEndInfoPtr
//		, png_uint_32 width
//		, png_uint_32 height
//		, int numPass
//		, png_bytep* srcBuf
//		)
//{
//	bool result = false;
//
//	FileLog("imghandle", "CutImage() path:%s", path.c_str());
//
//	string desFilePath("");
//	string desFileExt("");
//	size_t pos = path.rfind(".");
//	if (pos != string::npos)
//	{
//		desFileExt = path.substr(pos);
//		desFilePath = path.substr(0, pos);
//	}
//	// 是否纵向（否则表示横向）
//	bool isVertical = height > width;
//
//	// 判断是否可以裁剪成完整的图片文件
//	if ((isVertical ? height%width : width%height) == 0)
//	{
//		// 裁剪生成的文件数量及图片大小
//		int fileCount = isVertical ? height/width : width/height;
//		png_uint_32 imageSize = isVertical ? width : height;
//
//		if (fileCount > 1)
//		{
//			FileLog("imghandle", "CutImage() build sub file now");
//
//			result = true;
//
//			string subFilePath("");
//			char subFileName[64] = {0};
//			for (int i = 0; i < fileCount; i++)
//			{
//				// 生成裁剪的文件路径
//				sprintf(subFileName, SUBFILENAME, i);
//				subFilePath = desFilePath;
//				subFilePath += subFileName;
//				subFilePath += desFileExt;
//
//				FileLog("imghandle", "CutImage() subFilePath:%s", subFilePath.c_str());
//
//				// 打开文件
//				FILE* file = fopen(subFilePath.c_str(), "wb");
//				if (NULL != file)
//				{
//					FileLog("imghandle", "CutImage() open file success");
//
//					png_structp writePtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//					png_infop writeInfoPtr = png_create_info_struct(writePtr);
//					png_infop writeEndInfoPtr = png_create_info_struct(writePtr);
//					setjmp(png_jmpbuf(writePtr));
//					png_init_io(writePtr, file);
//					setWriteInfoByReadInfo(readPtr, readInfoPtr, writePtr, writeInfoPtr, imageSize);
//
//					FileLog("imghandle", "CutImage() setWriteInfoByReadInfo() success");
//
//					for (int pass = 0; pass < numPass; pass++)
//					{
//						for (png_uint_32 k = 0; k < imageSize; k++)
//						{
//							if (isVertical)
//							{
//								// 纵向
//								int row = imageSize * i;
//								png_write_row(writePtr, srcBuf[row + k]);
//							}
//							else
//							{
//								// 横向
//								int offset = imageSize * i;
//								png_write_row(writePtr, srcBuf[k] + offset);
//							}
//						}
//					}
//
//					FileLog("imghandle", "CutImage() copy data success");
//
//					setWriteEndInfoByReadEndInfo(readPtr, readEndInfoPtr, writePtr, writeEndInfoPtr);
//
//					FileLog("imghandle", "CutImage() setWriteEndInfoByReadEndInfo() success");
//
//					png_write_end(writePtr, writeEndInfoPtr);
//					png_destroy_info_struct(writePtr, &writeEndInfoPtr);
//					png_destroy_write_struct(&writePtr, &writeInfoPtr);
//
//					// 关闭文件
//					fclose(file);
//					file = NULL;
//				}
//			}
//		}
//	}
//
//	FileLog("imghandle", "CutImage() result:%d", result);
//
//	return result;
//}
//
//// 把一张很长的图片裁剪成一张张小图片
//bool PngHandler::ConvertImage(const string& path)
//{
//	FileLog("imghandle", "ConvertImage() path:%s", path.c_str());
//
//	bool result = false;
//	FILE *file = NULL;
//	png_structp readPtr = NULL;
//	png_infop readInfoPtr = NULL;
//	png_infop readEndInfoPtr = NULL;
//	int num_pass = 0;
//	png_bytep* srcBuf = NULL;
//	int srcBufSize = 0;
//	do {
//		png_uint_32 i = 0;
//
//		// 打开源文件
//		file = fopen(path.c_str(), "rb");
//		if (file == NULL) {
//			FileLog("imghandle", "ConvertImage() open file fail! path:%s", path.c_str());
//			break;
//		}
//		FileLog("imghandle", "ConvertImage() fopen success");
//
//		// 初始化libpng变量
//		readPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//		if (readPtr == NULL)
//		{
//			FileLog("imghandle", "ConvertImage() png_create_read_struct() read fail!");
//			break;
//		}
//		FileLog("imghandle", "ConvertImage() readPtr:%p", readPtr);
//
//		readInfoPtr = png_create_info_struct(readPtr);
//		if (readInfoPtr == NULL)
//		{
//			FileLog("imghandle", "ConvertImage() png_create_info_struct() read info fail!");
//			break;
//		}
//		FileLog("imghandle", "ConvertImage() readInfoPtr:%p", readInfoPtr);
//
//		readEndInfoPtr = png_create_info_struct(readPtr);
//		if (readEndInfoPtr == NULL)
//		{
//			FileLog("imghandle", "ConvertImage() png_create_info_struct() read end info fail!");
//			break;
//		}
//		FileLog("imghandle", "ConvertImage() readEndInfoPtr:%p", readEndInfoPtr);
//
//		if (setjmp(png_jmpbuf(readPtr)))
//		{
//			break;
//		}
//
//		png_init_io(readPtr, file);
//		png_read_info(readPtr, readInfoPtr);
//
//		FileLog("imghandle", "ConvertImage() get png file info");
//
//		// 读取png文件信息
//        png_uint_32 width;
//        png_uint_32 height;
//		int bitDepth;
//        int colorType;
//        int interlaceType;
//		int compressionType;
//		int filterType;
//		if ( png_get_IHDR(readPtr, readInfoPtr, &width, &height
//			  , &bitDepth, &colorType
//			  , &interlaceType, &compressionType
//			  , &filterType) == 0 )
//		{
//			FileLog("imghandle", "ConvertImage() png_get_IHDR() fail!");
//			break;
//		}
//		if (height <= 0)
//		{
//			FileLog("imghandle", "ConvertImage() height:%d", height);
//			break;
//		}
//		switch (interlaceType)
//		{
//		case PNG_INTERLACE_NONE:
//			num_pass = 1;
//			break;
//		case PNG_INTERLACE_ADAM7:
//			num_pass = 7;
//			break;
//		default:
//			/*NOT REACHED*/
//			break;
//         }
//
//		// 获取png每行所需的字节数
//		int readRowBuf = png_get_rowbytes(readPtr, readInfoPtr);
//		if (readRowBuf <= 0)
//		{
//			FileLog("imghandle", "ConvertImage() readRowBuf:%d", readRowBuf);
//			break;
//		}
//		FileLog("imghandle", "ConvertImage() readRowBuf:%d", readRowBuf);
//
//		// 新建png数据所需的内存空间
//		srcBuf = new png_bytep[height];
//		if (srcBuf == NULL)
//		{
//			FileLog("imghandle", "ConvertImage() srcBuf = NULL");
//			break;
//		}
//		srcBufSize = height;
//		memset(srcBuf, 0, sizeof(png_bytep) * height);
//		for (i = 0; i < height; i++)
//		{
//			srcBuf[i] = new png_byte[readRowBuf];
//			memset(srcBuf[i], 0, sizeof(png_byte) * readRowBuf);
//		}
//		FileLog("imghandle", "ConvertImage() new srcBuf:%p", srcBuf);
//
//		// 读取png文件数据
//		int pass = 0;
//		for (pass = 0; pass < num_pass; pass++)
//		{
//			for (i = 0; i < height; i++)
//			{
//				png_read_row(readPtr, srcBuf[i], NULL);
//			}
//		}
//		FileLog("imghandle", "ConvertImage() read row");
//
//		png_colorp palette;
//		int num_palette;
//		png_get_PLTE(readPtr, readInfoPtr, &palette, &num_palette);
//
//		png_read_end(readPtr, readEndInfoPtr);
//		result = CutImage(path, readPtr, readInfoPtr, readEndInfoPtr, width, height, num_pass, srcBuf);
//	} while (0);
//
//	// 释放srcBuf(源png数据空间)
//	if (NULL != srcBuf)
//	{
//		for (int i = 0; i < srcBufSize; i++)
//		{
//			if (srcBuf[i] != NULL)
//			{
//				delete srcBuf[i];
//			}
//		}
//		delete srcBuf;
//		srcBuf = NULL;
//	}
//
//	// 释放libpng变量
//	if (NULL != readEndInfoPtr) {
//		png_destroy_read_struct(&readPtr, &readInfoPtr, &readEndInfoPtr);
//	}
//
//	// 关闭源文件
//	if (file != NULL) {
//		fclose(file);
//		file = NULL;
//	}
//
//	return result;
//}

// 把一张很长的图片裁剪成一张张小图片
bool PngHandler::ConvertImage(const string& path)
{
	bool result = false;
	const char* SUBFILENAME = "_%d";
	FileLog("ImageHandler", "PngHandler::ConvertImage() begin, path:%s", path.c_str());

	do {
		// 读取图片信息
		png_uint_32 width = 0;
		png_uint_32 height = 0;
		int numPass = 0;
		PngReader readPng;
		if (!readPng.SetFilePath(path)) {
			break;
		}
		if (!readPng.Init()) {
			break;
		}
		if (!readPng.GetPngInfo(width, height, numPass)) {
			break;
		}
		FileLog("ImageHandler", "PngHandler::ConvertImage() init readPng success, path:%s", path.c_str());

		// 生成目录图片路径
		string desFilePath("");
		string desFileExt("");
		// 找文件名
		size_t beginPos = 0;
		size_t filePos = path.rfind("/");
		if (filePos != string::npos) {
			beginPos = filePos;
		}
		else {
			filePos = path.rfind("\\");
			beginPos = (filePos != string::npos ? filePos : beginPos);
		}
		// 找后缀
		size_t pos = path.find(".", beginPos);
		if (pos != string::npos) {
			desFileExt = path.substr(pos);
			desFilePath = path.substr(0, pos);
		}
		else {
			desFilePath = path;
		}
		// 是否纵向（否则表示横向）
		bool isVertical = height > width;

		// 判断是否可以裁剪成完整的图片文件
		if ((isVertical ? height%width : width%height) == 0)
		{
			// 裁剪生成的文件数量及图片大小
			int fileCount = isVertical ? height/width : width/height;
			png_uint_32 imageSize = isVertical ? width : height;
			// 生成row buffer
			size_t rowBytes = readPng.GetRowBytes();
			png_bytep rowBuffer = NULL;
			if (rowBytes > 0) {
				rowBuffer = new png_byte[rowBytes];
				memset(rowBuffer, 0, rowBytes);
			}

			if (fileCount > 1 && NULL != rowBuffer)
			{
				result = true;

				string subFilePath("");
				char subFileName[64] = {0};
				for (int i = 0; i < fileCount; i++)
				{
					// 生成子文件路径
					sprintf(subFileName, SUBFILENAME, i);
					subFilePath = desFilePath;
					subFilePath += subFileName;
					subFilePath += desFileExt;

					FileLog("ImageHandler", "PngHandler::ConvertImage() build sub file begin, isVertical:%d, width:%d, height:%d, imageSize:%d, subFilePath:%s, desFilePath:%s"
							, isVertical, width, height, imageSize, subFilePath.c_str(), desFilePath.c_str());

					if (!isVertical) {
						// 处理横向文件
						PngReader srcPng;
						PngWriter desPng;
						if (srcPng.SetFilePath(path)
							&& srcPng.Init()
							&& desPng.SetFilePath(subFilePath)
							&& desPng.Init()
							&& desPng.setWriteInfoByReadInfo(srcPng, imageSize))
						{
							for (int pass = 0; pass < numPass; pass++)
							{
								for (png_uint_32 k = 0; k < imageSize; k++)
								{
									int offset = imageSize * i;
									srcPng.ReadRowData(rowBuffer);
									desPng.WriteRowData(rowBuffer + offset);
								}
							}

							srcPng.SetEnd();
							desPng.setWriteEndInfoByReadEndInfo(srcPng);

							FileLog("ImageHandler", "PngHandler::ConvertImage() build sub file success, subFilePath:%s", subFilePath.c_str());
						}
					}
					else {
						// 处理纵向文件
					}

					FileLog("ImageHandler", "PngHandler::ConvertImage() build sub file end, subFilePath:%s", subFilePath.c_str());
				}
			}

			delete[] rowBuffer;
			rowBuffer = NULL;
		}
	} while (0);

	FileLog("ImageHandler", "PngHandler::ConvertImage() end, path:%s", path.c_str());

	return result;
}


