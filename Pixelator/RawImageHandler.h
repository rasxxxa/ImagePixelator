#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <optional>

enum class ImageType
{
	PNG,
	JPG,
	BMP,
};

struct RGB
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

using MATRIX = std::vector<std::vector<RGB>>;
using IMAGEDATA = std::vector<unsigned char>;

struct Image
{
	IMAGEDATA m_imageData;
	unsigned int m_width;
	unsigned int m_height;
	unsigned long long m_size;
	unsigned short m_channels;
	ImageType type = ImageType::PNG;
	std::string path;
	unsigned char GetR(size_t i, size_t j) const;
	unsigned char GetG(size_t i, size_t j) const;
	unsigned char GetB(size_t i, size_t j) const;
	unsigned char GetA(size_t i, size_t j) const;
	void SetR(size_t i, size_t j, unsigned char data);
	void SetG(size_t i, size_t j, unsigned char data);
	void SetB(size_t i, size_t j, unsigned char data);
	void SetA(size_t i, size_t j, unsigned char data);
};

class RawImageHandler
{
public:
	RawImageHandler() = default;
	static [[nodiscard]]Image LoadImageFile(const std::string& path);
	static bool WriteImage(const Image& image, std::optional<std::string> path = std::nullopt);
	std::pair<std::vector<unsigned char>, std::vector<unsigned char>> SeparateAlpha(Image& image);

	static MATRIX CreateMatrixFromImage(const Image& image);
	static IMAGEDATA CreateImageFromMatrix(const MATRIX& matrix);

};

