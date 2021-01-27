// Copyright (c) 2016 Giorgio Marcias
//
// This file is part of distance_transform, a C++11 implementation of the
// algorithm in "Distance Transforms of Sampled Functions"
// Pedro F. Felzenszwalb, Daniel P. Huttenlocher
// Theory of Computing, Vol. 8, No. 19, September 2012
//
// This source code is subject to Apache 2.0 License.
//
// Author: Giorgio Marcias
// email: marcias.giorgio@gmail.com

#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <cctype>

#include <distance_transform/tools.hpp>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cerr << "usage: " << argv[0] << " input file name [output file name(tga|bmp|png|jpg)]\n";
		return 1;
	}

	char *input_name = argv[1];
	std::string output_name;
	std::string ext("tga");
	int var = 0;

	if (argc == 3)
	{
		output_name = argv[2];
		ext = output_name.substr(output_name.find(".") + 1);

		std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) -> unsigned char { return std::tolower(c); });

		if (ext != "tga" && ext != "bmp" && ext != "png" && ext != "jpg")
		{
			std::cerr << "output file name must be tga|bmp|png|jpg one of them!\n";
			return 1;
		}
	}
	else
	{
		output_name = input_name;
		ext = output_name.substr(output_name.find("."));

		output_name.replace(output_name.find("."), ext.size(), "_sdf.tga");
		ext = "tga";
	}

	if ((ext == "png" || ext == "jpg") && argc == 4)
	{
		var = std::stoi(argv[3]);
	}

	dt::processImage(input_name, output_name.c_str(), ext, var);

    return 0;
}
