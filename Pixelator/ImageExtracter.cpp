#include "ImageExtracter.h"

namespace
{
	template <typename Key, typename Elem>
	Elem GetVal(const std::map<Key, Elem>& maps, Key key)
	{
		auto it = maps.lower_bound(key);
		auto pr = std::prev(it);

		if (pr != maps.end())
		{
			if (std::abs(static_cast<double>(pr->first - key)) < std::abs(static_cast<double>(it->first - key)))
				return pr->second;
			else
				return it->second;
		}

		return it->second;
	}

	constexpr long long split(unsigned int a)
	{
		a = (a | a << 12) & 00014000377;
		a = (a | a << 8) & 00014170017;
		a = (a | a << 4) & 00303030303;
		a = (a | a << 2) & 01111111111;
		return a;
	}

	constexpr long long zValue(unsigned int r, unsigned g, unsigned b)
	{
		return split(r) + (split(g) << 1) + (split(b) << 2);
	}
}

double ImageExtracter::GetImageHash(const Image& image)
{
	unsigned long long R{ 0llu }, G{ 0llu }, B{ 0llu }, A{ 0llu };
	size_t pixels = image.m_width * image.m_height;
	for (auto row{ 0u }; row < image.m_height; ++row)
	{
		for (auto column{ 0u }; column < image.m_width; ++column)
		{
			R += image.GetR(row, column);
			G += image.GetG(row, column);
			B += image.GetB(row, column);
		}
	}

	if (pixels)
	{
		R /= pixels;
		G /= pixels;
		B /= pixels;
	}

	//return std::sqrt(R * R + G * G + B * B);
	return zValue(R, G, B);
}

void ImageExtracter::AddImage(const Image& image)
{
	std::lock_guard<std::mutex> lock(mtx);
	const auto hash = GetImageHash(image);
	if (hash >= m_biggest.first)
	{
		m_biggest.first = hash;
		m_biggest.second = image;
	}
	m_imagesByPixels[hash] = image;
}

Image ImageExtracter::CreatePixelatedPicture(const Image& original)
{
	Image newImage;
	newImage.m_width = original.m_width * size;
	newImage.m_height = original.m_height * size;
	newImage.m_channels = 4;
	newImage.m_size = newImage.m_width * newImage.m_height * newImage.m_channels;
	newImage.m_imageData.resize(newImage.m_size);

	for (auto row{ 0u }; row < original.m_height; ++row)
	{
		for (auto column{ 0u }; column < original.m_width; ++column)
		{
			const auto imageCopy = FindByPixels(original.GetR(row, column), original.GetG(row, column), original.GetB(row, column));
			for (size_t rowI = 0; rowI < size; ++rowI)
			{
				for (size_t columnI = 0; columnI < size; ++columnI)
				{
					const auto i = row * size + rowI;
					const auto j = column * size + columnI;
					newImage.SetR(i, j, imageCopy.GetR(rowI, columnI));
					newImage.SetG(i, j, imageCopy.GetG(rowI, columnI));
					newImage.SetB(i, j, imageCopy.GetB(rowI, columnI));
					newImage.SetA(i, j, 255);
				}
			}
		}
	}

	return newImage;
}

Image ImageExtracter::FindByPixels(unsigned char R, unsigned char G, unsigned char B)
{
	//auto hash = std::sqrt(R * R + G * G + B * B);
	double hash = zValue(R, G, B);
	return GetVal(m_imagesByPixels, hash);
}
