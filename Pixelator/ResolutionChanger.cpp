#include "ResolutionChanger.h"

Image ResolutionChanger::ChangeResolution(const Image& original, unsigned int newWidth, unsigned int newHeight)
{
	Image image{};
	image.m_channels = original.m_channels;
	image.m_width = newWidth;
	image.m_height = newHeight;
	image.m_size = image.m_channels * newWidth * newHeight;
	image.path = original.path;
	image.m_imageData.resize(image.m_size);
	
	const auto pixelsWidthRatio = static_cast<float>(original.m_width) / newWidth;
	const auto pixelsHeightRatio = static_cast<float>(original.m_height) / newHeight;

	for (size_t row{ 0u }; row < newHeight; ++row)
	{
		const auto startingRow = static_cast<size_t>(row * pixelsHeightRatio);
		for (size_t column{ 0u }; column < newWidth; ++column)
		{
			const auto startingColumn = static_cast<size_t>(column * pixelsWidthRatio);
			std::array<unsigned long, 4> pixelValues{ 0u, 0u, 0u, 0u };
			size_t pixelObserved = 0;
			auto lastRow = std::clamp(static_cast<unsigned int>(startingRow + pixelsHeightRatio), 0u, original.m_height);
			for (size_t rowI = startingRow; rowI < lastRow; ++rowI)
			{
				auto lastColumn = std::clamp(static_cast<unsigned int>(startingColumn + pixelsWidthRatio), 0u, original.m_width);
				for (size_t columnI = startingColumn; columnI < lastColumn; ++columnI)
				{
					pixelValues[0] += original.GetR(rowI, columnI);
					if (image.m_channels >= 2)
					  pixelValues[1] += original.GetG(rowI, columnI);
					if (image.m_channels >= 3)
					  pixelValues[2] += original.GetB(rowI, columnI);
					if (image.m_channels >= 4)
					  pixelValues[3] += original.GetA(rowI, columnI);
					pixelObserved++;
				}
			}

			if (pixelObserved > 0)
			{
				std::ranges::for_each(pixelValues, [pixelObserved](auto& v) {v /= pixelObserved; });
			}

			image.SetR(row, column, static_cast<unsigned char>(pixelValues[0]));
			if (image.m_channels >= 2)
			  image.SetG(row, column, static_cast<unsigned char>(pixelValues[1]));
			if (image.m_channels >= 3)
			  image.SetB(row, column, static_cast<unsigned char>(pixelValues[2]));
			if (image.m_channels == 4)
			  image.SetA(row, column, static_cast<unsigned char>(pixelValues[3]));
		}
	}

	return image;
}
