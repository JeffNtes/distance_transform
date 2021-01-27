#include <iostream>
#include <fstream>
#include <distance_transform/tools.hpp>

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize.h"


namespace dt {

	template<typename sScaler, typename dScaler>
	void initializeEDTfromImage(const DopeVector<sScaler, 2> &s, DopeVector<dScaler, 2> &d)
	{
		for (dope::SizeType i = static_cast<dope::SizeType>(0); i < s.sizeAt(1); ++i)
		{
			for (dope::SizeType j = static_cast<dope::SizeType>(0); j < s.sizeAt(0); ++j)
			{
				Index2 idx({ i, j });
				const sScaler &u = s[idx];
				dScaler &f = d[idx];

				if (u == 1)
					f = 0.0f;
			}
		}
	}

	void initializeEDTfromImage(const DopeVector<RGB, 2> &s, DopeVector<fRGB, 2> &d)
	{
		for (dope::SizeType i = static_cast<dope::SizeType>(0); i < s.sizeAt(1); ++i)
		{
			for (dope::SizeType j = static_cast<dope::SizeType>(0); j < s.sizeAt(0); ++j)
			{
				Index2 idx({ i, j });
				const RGB &u = s[idx];
				fRGB &f = d[idx];

				for (dope::SizeType k=static_cast<dope::SizeType>(0); k < static_cast<dope::SizeType>(3); ++k)
				{
					if (u[k] != 0)
						f[k] = 0.0f;
				}
			}
		}
	}

	void initializeEDTfromImage(const DopeVector<RGBA, 2> &s, DopeVector<fRGBA, 2> &d)
	{
		for (dope::SizeType i = static_cast<dope::SizeType>(0); i < s.sizeAt(1); ++i)
		{
			for (dope::SizeType j = static_cast<dope::SizeType>(0); j < s.sizeAt(0); ++j)
			{
				Index2 idx({ i, j });
				const RGBA &u = s[idx];
				fRGBA &f = d[idx];

				for (dope::SizeType k = static_cast<dope::SizeType>(0); k < static_cast<dope::SizeType>(4); ++k)
				{
					if (u[k] != 0)
						f[k] = 0.0f;
				}
			}
		}
	}

	template <class T>
	inline T bound(const T &x, const T &min, const T &max) {
		return (x < min ? min : (x > max ? max : x));
	}

	template<typename Scaler>
	void caculateEDTImageMinMax(const DopeVector<Scaler, 2> &d, Scaler &min, Scaler &max)
	{
		for (dope::SizeType i = static_cast<dope::SizeType>(0); i < d.sizeAt(1); ++i)
		{
			for (dope::SizeType j = static_cast<dope::SizeType>(0); j < d.sizeAt(0); ++j)
			{
				Index2 idx({ i, j });
				const Scaler &val = d[idx];

				if (min > val)
					min = val;

				if (max < val)
					max = val;
			}
		}
	}

	void caculateEDTImageMinMax(const DopeVector<fRGB, 2> &d, fRGB &min, fRGB &max)
	{
		for (dope::SizeType i = static_cast<dope::SizeType>(0); i < d.sizeAt(1); ++i)
		{
			for (dope::SizeType j = static_cast<dope::SizeType>(0); j < d.sizeAt(0); ++j)
			{
				Index2 idx({ i, j });
				const fRGB &pixel = d[idx];

				for (dope::SizeType k = static_cast<dope::SizeType>(0); k < static_cast<dope::SizeType>(3); ++k)
				{
					if (min[k] > pixel[k])
						min[k] = pixel[k];

					if (max[k] < pixel[k])
						max[k] = pixel[k];
				}
			}
		}
	}

	void caculateEDTImageMinMax(const DopeVector<fRGBA, 2> &d, fRGBA &min, fRGBA &max)
	{
		for (dope::SizeType i = static_cast<dope::SizeType>(0); i < d.sizeAt(1); ++i)
		{
			for (dope::SizeType j = static_cast<dope::SizeType>(0); j < d.sizeAt(0); ++j)
			{
				Index2 idx({ i, j });
				const fRGBA &pixel = d[idx];

				for (dope::SizeType k = static_cast<dope::SizeType>(0); k < static_cast<dope::SizeType>(4); ++k)
				{
					if (min[k] > pixel[k])
						min[k] = pixel[k];

					if (max[k] < pixel[k])
						max[k] = pixel[k];
				}
			}
		}
	}

	template<typename sScaler, typename dScaler>
	void convertEDTFloatToUcharImage(const DopeVector<sScaler, 2> &s, DopeVector<dScaler, 2> &d, const sScaler &min, const sScaler &max)
	{
		sScaler scale = std::numeric_limits<dScaler>::max() / (max - min);

		for (dope::SizeType i = static_cast<dope::SizeType>(0); i < d.sizeAt(1); ++i)
		{
			for (dope::SizeType j = static_cast<dope::SizeType>(0); j < d.sizeAt(0); ++j)
			{
				const sScaler &f = s[i][j];
				dScaler &u = d[i][j];

				dScaler val = (dScaler)((f - min) * scale);
				u = bound(val, static_cast<dScaler>(0), std::numeric_limits<dScaler>::max());
			}
		}
	}

	template<typename Scaler>
	void convertEDTFloatToUcharImage(const DopeVector<Scaler, 2> &s, DopeVector<RGB, 2> &d, const Scaler &min, const Scaler &max)
	{
		Scaler scale = 0xff / (max - min);

		for (dope::SizeType i = static_cast<dope::SizeType>(0); i < d.sizeAt(1); ++i)
		{
			for (dope::SizeType j = static_cast<dope::SizeType>(0); j < d.sizeAt(0); ++j)
			{
				Index2 idx({ i, j });
				const Scaler &f = s[idx];
				RGB &u = d[idx];
				
				unsigned char val = (unsigned char)((f - min) * scale);
				val = bound(val, static_cast<unsigned char>(0), std::numeric_limits<unsigned char>::max());
				u.extend(val);
			}
		}
	}

	void convertEDTFloatToUcharImage(const DopeVector<fRGB, 2> &s, DopeVector<RGB, 2> &d, const fRGB &min, const fRGB &max)
	{
		fRGB scale(1.0);
		for (dope::SizeType k = static_cast<dope::SizeType>(0); k < static_cast<dope::SizeType>(3); ++k)
		{
			scale[k] = 0xff / (max[k] - min[k]);
		}

		for (dope::SizeType i = static_cast<dope::SizeType>(0); i < d.sizeAt(1); ++i)
		{
			for (dope::SizeType j = static_cast<dope::SizeType>(0); j < d.sizeAt(0); ++j)
			{
				Index2 idx({ i, j });
				const fRGB &f = s[i][j];
				RGB &u = d[idx];

				for (dope::SizeType k = static_cast<dope::SizeType>(0); k < static_cast<dope::SizeType>(3); ++k)
				{
					unsigned char val = (unsigned char)((f[k] - min[k]) * scale[k]);
					u[k] = bound(val, static_cast<unsigned char>(0), std::numeric_limits<unsigned char>::max());
				}
			}
		}
	}

	void convertEDTFloatToUcharImage(const DopeVector<fRGBA, 2> &s, DopeVector<RGBA, 2> &d, const fRGBA &min, const fRGBA &max)
	{
		fRGBA scale(1.0);
		for (dope::SizeType k = static_cast<dope::SizeType>(0); k < static_cast<dope::SizeType>(4); ++k)
		{
			scale[k] = 0xff / (max[k] - min[k]);
		}

		for (dope::SizeType i = static_cast<dope::SizeType>(0); i < d.sizeAt(1); ++i)
		{
			for (dope::SizeType j = static_cast<dope::SizeType>(0); j < d.sizeAt(0); ++j)
			{
				Index2 idx({ i, j });
				const fRGBA &f = s[idx];
				RGBA &u = d[idx];

				for (dope::SizeType k = static_cast<dope::SizeType>(0); k < static_cast<dope::SizeType>(4); ++k)
				{
					unsigned char val = (unsigned char)((f[k] - min[k]) * scale[k]);
					u[k] = bound(val, static_cast<unsigned char>(0), std::numeric_limits<unsigned char>::max());
				}
			}
		}
	}

	void savePGM(unsigned char *im, const char *name, const int &w, const int &h) {

		std::ofstream file(name, std::ios::out | std::ios::binary);

		file << "P5\n" << w << " " << w << "\n" << 0xff << "\n";
		file.write((char*)im, w * h * sizeof(unsigned char));
	}

	bool processImage(const char *inFile, const char *outFile, const std::string &ext, int var)
	{
		int x, y, n;
		unsigned char *imgData = stbi_load(inFile, &x, &y, &n, 0);

		if (imgData == nullptr)
		{
			std::cout << std::endl << "stbi_load " << inFile << stbi_failure_reason() << std::endl;
			return false;
		}

		Index2 size({ static_cast<dope::SizeType>(x), static_cast<dope::SizeType>(y)});
		std::chrono::steady_clock::time_point start;
		if ( n == 3 )
		{
			DopeVector<RGB, 2> uImage((RGB*)imgData, 0, size);
			Grid<fRGB, 2> fImage(size, fRGB(std::numeric_limits<float>::max()));

			initializeEDTfromImage(uImage, fImage);

			start = std::chrono::steady_clock::now();
			unsigned int threads = std::thread::hardware_concurrency();
			DistanceTransform::distanceTransformL2(fImage, fImage, false, threads);
			std::cout << std::endl << "image: width("<< x << ") height("<< y << ") channel("<< n << ")->" << inFile << std::endl << "distance function (concurrently) computed in: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms. (with " << threads << " threads)." << std::endl;

			fRGB min(std::numeric_limits<float>::max()), max(-std::numeric_limits<float>::max());
			caculateEDTImageMinMax(fImage, min, max);

			convertEDTFloatToUcharImage(fImage, uImage, min, max);

		}
		else if (n == 4 )
		{
			DopeVector<RGBA, 2> uImage((RGBA*)imgData, 0, size);
			Grid<fRGBA, 2> fImage(size, fRGBA(std::numeric_limits<float>::max()));

			initializeEDTfromImage(uImage, fImage);

			start = std::chrono::steady_clock::now();
			unsigned int threads = std::thread::hardware_concurrency();
			DistanceTransform::distanceTransformL2(fImage, fImage, false, threads);
			std::cout << std::endl << "image: width(" << x << ") height(" << y << ") channel(" << n << ")->" << inFile << std::endl << "distance function (concurrently) computed in: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms. (with " << threads << " threads)." << std::endl;

			fRGBA min(std::numeric_limits<float>::max()), max(-std::numeric_limits<float>::max());
			caculateEDTImageMinMax(fImage, min, max);

			convertEDTFloatToUcharImage(fImage, uImage, min, max);
		}
		else if (n == 1)
		{
			DopeVector<unsigned char, 2> uImage(imgData, 0, size);
			Grid<float, 2> fImage(size, std::numeric_limits<float>::max());

			initializeEDTfromImage(uImage, fImage);

			start = std::chrono::steady_clock::now();
			unsigned int threads = std::thread::hardware_concurrency();
			DistanceTransform::distanceTransformL2(fImage, fImage, false, threads);
			std::cout << std::endl << "image: width(" << x << ") height(" << y << ") channel(" << n << ")->" << inFile << std::endl << "distance function (concurrently) computed in: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms. (with " << threads << " threads)." << std::endl;

			float min= std::numeric_limits<float>::max(), max= -std::numeric_limits<float>::max();
			caculateEDTImageMinMax(fImage, min, max);

			unsigned char *tempData = (unsigned char*)stbi__malloc(sizeof(RGB)*x*y);
			stbi_image_free(imgData);
			imgData = tempData;
			DopeVector<RGB, 2> tImage((RGB*)imgData, 0, size);

			convertEDTFloatToUcharImage(fImage, tImage, min, max);
		}
		else
		{
			std::cout << std::endl << "distance function process unsupported image type! " << inFile << std::endl;
			return false;
		}

		int ret = 0;
		if (ext == "tga")
			ret = stbi_write_tga(outFile, x, y, n, imgData);
		else if (ext == "bmp")
			ret = stbi_write_bmp(outFile, x, y, n, imgData);
		else if (ext == "png")
			ret = stbi_write_png(outFile, x, y, n, imgData, var);
		else if (ext == "jpg")
			ret = stbi_write_jpg(outFile, x, y, n, imgData, var);
		
		stbi_image_free(imgData);

		return ret;
	}
}
