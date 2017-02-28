#ifndef OPENGLRGBIMAGE_H
#define OPENGLRGBIMAGE_H

#include <stdio.h>
#include <Graphics/RgbImage.h>


class OpenGlRgbImage: public RgbImage
{
public:
    OpenGlRgbImage(int numRows, int numCols);
    bool LoadFromOpenglBuffer();
    bool DrawToOpenglBuffer();
};

#endif // OPENGLRGBIMAGE_H
