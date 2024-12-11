#define STB_IMAGE_IMPLEMENTATION
#include "../STB/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../STB/stb_image_write.h"
#include "RawImageHandler.h"

[[nodiscard]] Image RawImageHandler::LoadImageFile(const std::string& path)
{
    // Image properties
    int width, height, channels;

    ImageType type = ImageType::PNG;
    std::string copy = path;
    for (unsigned ch = 0; ch < copy.size(); ch++)
    {
        copy[ch] = std::tolower(copy[ch]);
    }

    if ((path.find(".png")) != std::string::npos)
    {
        type = ImageType::PNG;
    }
    else if ((path.find(".jpg")) != std::string::npos)
    {
        type = ImageType::JPG;
    }
    else if ((path.find(".JPG")) != std::string::npos)
    {
        type = ImageType::JPG;
    }
    else if ((path.find(".JPEG")) != std::string::npos)
    {
        type = ImageType::JPG;
    }
    else
    {
        type = ImageType::BMP;
    }

    // Load the image using stb_image
    unsigned char* image = stbi_load(path.data(), &width, &height, &channels, 0);
    if (!image) 
    {
        std::cout << "Failed to load image, reason: " << stbi_failure_reason() << " \n";
        return Image{};
    }
    Image im;
    im.m_channels = channels;
    im.m_height = height;
    im.m_width = width;
    im.m_size = height * width * channels;
    im.m_imageData = std::vector<unsigned char>(image, image + im.m_size);

    im.type = type;
    im.path = path;

    stbi_image_free(image);
    return im;
}

bool RawImageHandler::WriteImage(const Image& image, std::optional<std::string> path)
{
    static unsigned serial = 0;
    std::string withExt = path.value_or(image.path);
    return stbi_write_png(withExt.data(), image.m_width, image.m_height, image.m_channels, image.m_imageData.data(), image.m_width * image.m_channels);
}

std::pair<std::vector<unsigned char>, std::vector<unsigned char>> RawImageHandler::SeparateAlpha(Image& image)
{
    std::vector<unsigned char> data, alpha;
    data.resize(image.m_imageData.size() * 3 / 4);
    alpha.resize(image.m_imageData.size() / 4);

    size_t countData = 0, countAlpha = 0;

    for (auto pix = 0; pix < image.m_imageData.size(); pix++)
    {
        if ((pix + 1) % 4 == 0)
        {
            alpha[countAlpha++] = image.m_imageData[pix];
        }
        else
        {
            data[countData++] = image.m_imageData[pix];
        }
    }
    
    return std::make_pair(data, alpha);

}

MATRIX RawImageHandler::CreateMatrixFromImage(const Image& image)
{
    MATRIX matrix(image.m_height, std::vector<RGB>(image.m_width, RGB()));
    unsigned i = 0, j = 0;
    for (unsigned long pix = 0; pix < image.m_imageData.size(); pix += image.m_channels)
    {
        RGB t;
        t.R = image.m_imageData[pix];
        if (image.m_channels >= 2)
          t.G = image.m_imageData[pix + 1];
        if (image.m_channels >= 3)
          t.B = image.m_imageData[pix + 2];
        if (image.m_channels >= 4)
          t.A = image.m_imageData[pix + 3];
        matrix[i][j++] = t;
        if (j == image.m_width)
        {
            i++;
            j = 0;
        }
    }

    return matrix;
}



IMAGEDATA RawImageHandler::CreateImageFromMatrix(const MATRIX& matrix)
{
    IMAGEDATA data(matrix.size() * matrix[0].size() * (sizeof(RGB)));
    unsigned int elem = 0;
    for (unsigned long row = 0; row < matrix.size(); row++)
    {
        for (unsigned long column = 0; column < matrix[0].size(); column++)
        {
            data[elem++] = matrix[row][column].R;
            data[elem++] = matrix[row][column].G;
            data[elem++] = matrix[row][column].B;
            data[elem++] = matrix[row][column].A;
        }
    }
    return data;
}

 unsigned char Image::GetR(size_t i, size_t j) const
{
     return m_imageData[(i * m_width + j) * m_channels];
}

 unsigned char Image::GetG(size_t i, size_t j) const
{
     return m_imageData[(i * m_width + j) * m_channels + 1];
}

 unsigned char Image::GetB(size_t i, size_t j) const
{
     return m_imageData[(i * m_width + j) * m_channels + 2];
}

 unsigned char Image::GetA(size_t i, size_t j) const
{
     return m_imageData[(i * m_width + j) * m_channels + 3];
}

 void Image::SetR(size_t i, size_t j, unsigned char data)
 {
     m_imageData[(i * m_width + j) * m_channels] = data;
 }

 void Image::SetG(size_t i, size_t j, unsigned char data)
 {
     m_imageData[(i * m_width + j) * m_channels + 1] = data;
 }

 void Image::SetB(size_t i, size_t j, unsigned char data)
 {
     m_imageData[(i * m_width + j) * m_channels + 2] = data;
 }

 void Image::SetA(size_t i, size_t j, unsigned char data)
 {
     m_imageData[(i * m_width + j) * m_channels + 3] = data;
 }
