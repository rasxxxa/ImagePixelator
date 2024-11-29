#pragma once
#include "RawImageHandler.h"
#include "map"
#include <mutex>

class ImageExtracter
{
private:
	std::map<double, Image> m_imagesByPixels;
	std::pair<double, Image> m_biggest;
	std::mutex mtx;
	unsigned int size;
	double GetImageHash(const Image& image);
	Image FindByPixels(unsigned char R, unsigned char G, unsigned char B);
public:
	ImageExtracter(unsigned int size) : size(size) {};
	void AddImage(const Image& image);
	Image CreatePixelatedPicture(const Image& original);
	size_t GetHashedImages() const { return m_imagesByPixels.size(); }
};

