#pragma once

#include <string>

#include "../Core/Image.h"
#include "../Core/ImageGPU.h"

namespace Visi
{
namespace IO
{

class VISI_EXPORT ImageFile
{	
    public:
        static bool Write(std::string fileName, Image* image);
        static bool Write(std::string fileName, ImageGPU* image);
        static bool Read(std::string fileName, Image* image); 
        static bool Read(std::string fileName, ImageGPU* image); 
};
	
}
}