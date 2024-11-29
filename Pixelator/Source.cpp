#include <iostream>
#include "RawImageHandler.h"
#include "ResolutionChanger.h"
#include "ImageExtracter.h"
#include "MultiProcess.hpp"
#include "filesystem"
#include <string>
#include <format>
#include <chrono>
#include <map>

auto main(int args, char* arguments[]) -> int
{
	std::string rootPath{};
	std::string imagePath{};

	std::cout << "Type root path: \n";
	std::cin >> rootPath;

	unsigned int imageSize{};
	std::cout << "Type in image width and height" << std::endl;
	std::cin >> imageSize;

	auto start = std::chrono::steady_clock::now();

	ImageExtracter extracter(imageSize);

	std::vector<std::string> pathsPng;

	for (auto paths : std::filesystem::recursive_directory_iterator(rootPath))
	{
		const auto pathString = paths.path().string();
		if (pathString.find(".png") != std::string::npos)
		{
			pathsPng.push_back(pathString);
		}
	}

	const auto Threads = std::min(std::thread::hardware_concurrency(), 12u);
	const auto elemsPerThread = pathsPng.size() / Threads;
	std::vector<std::jthread> threads;
	threads.resize(Threads);

	auto func = [imageSize, &extracter](const std::string& pathString) 
		{
			try
			{
				auto image = RawImageHandler::LoadImageFile(pathString);

				if (image.m_width >= imageSize && image.m_height >= imageSize/* && image.m_channels == 4*/)
				{
					auto extracted = ResolutionChanger::ChangeResolution(image, imageSize, imageSize);
					extracter.AddImage(extracted);
				}
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}

		};

	for (size_t i = 0; i < Threads; ++i)
	{
		const auto left = i * elemsPerThread;
		const auto right = (i == Threads - 1) ? pathsPng.size() : (i + 1) * elemsPerThread;
		threads[i] = std::jthread([left, right, func, &pathsPng]()
			{
				for (size_t j = left; j < right; ++j)
				{
					func(pathsPng[j]);
				}
			});
	}

	for (auto& th : threads)
		th.join();

	auto end = std::chrono::steady_clock::now();

	std::cout << "Time with mutex: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
	std::cout << "Elements: " << extracter.GetHashedImages() << std::endl;
	std::cout << "Number of threads " << Threads << std::endl;

	do
	{
		std::cout << "Type image path \n";
		std::cin >> imagePath;

		auto newImage = extracter.CreatePixelatedPicture(RawImageHandler::LoadImageFile(imagePath));
		RawImageHandler::WriteImage(newImage, imagePath);
	} while (imagePath != "end");
}