#include <iostream>
#include <distance_transform/tools.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize.h"


namespace dt {

	bool processImage(char const *filename)
	{
		int x, y, n;
		unsigned char *imgData = stbi_load(filename, &x, &y, &n, 0);

		if (imgData == nullptr) return false;

		Index2 size({ static_cast<dope::SizeType>(x), static_cast<dope::SizeType>(y)});
		std::chrono::steady_clock::time_point start;
		if ( n == 3)
		{
			DopeVector<RGB, 2> image((RGB*)imgData, 0, size);

			start = std::chrono::steady_clock::now();
			DistanceTransform::distanceTransformL2(image, image, false, 1);
			std::cout << std::endl << "image: width("<< x << ") height("<< y << ") channel("<< n << "->" << filename << std::endl << " distance function (concurrently) computed in: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms. (with " << std::thread::hardware_concurrency() << " threads)." << std::endl;
		}
		else if (n == 4)
		{
			DopeVector<RGBA, 2> image((RGBA*)imgData, 0, size);

			start = std::chrono::steady_clock::now();
			DistanceTransform::distanceTransformL2(image, image, false, 1);
			std::cout << std::endl << "image: width(" << x << ") height(" << y << ") channel(" << n << "->" << filename << std::endl << " distance function (concurrently) computed in: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms. (with " << std::thread::hardware_concurrency() << " threads)." << std::endl;
		}
		else
		{
			throw std::runtime_error("unsupported image type!");
		}

		return stbi_write_tga("test.tga", x, y, n, imgData);
	}
}
