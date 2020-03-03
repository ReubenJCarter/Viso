//Thanks to http://mccormickml.com/2014/01/10/stereo-vision-tutorial-part-i/

#include "StereoMatchSAD.h"

#include "../Core/ComputeShader.h"
#include "../Core/ProcessHelper.h"
#include "../Core/ParallelFor.h"

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <math.h>

namespace Visi
{
namespace Process
{

class StereoMatchSAD::Internal
{
    private:
        static std::map<ImageType, ComputeShader> computeShaders; 
        static std::string shaderSrc; 
        static bool shaderCompiled; 

        int blockW;
        int blockH;
        int maxK; 

    public:
        Internal(); 
        void Run(ImageGPU* inputL, ImageGPU* inputR, ImageGPU* output); 
        void Run(Image* inputL, Image* inputR, Image* output); 
        void SetBlockSize(int W, int H); 
        void SetMaxK(int mk); 
};


std::map<ImageType, ComputeShader> StereoMatchSAD::Internal::computeShaders;

std::string StereoMatchSAD::Internal::shaderSrc = R"(

layout(binding=0) writeonly uniform image2D outputImage;
layout(FORMAT_QUALIFIER, binding=1) uniform image2D inputImageL;
layout(FORMAT_QUALIFIER, binding=2) uniform image2D inputImageR;

uniform int blockW;
uniform int blockH;
uniform int blockA; 
uniform int halfBlockW;
uniform int halfBlockH;
uniform int maxK; 

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);

    float cachel[256];
	float lowestsad = 1000.0f;
	int lowestsadK = 0;
	float sad = 0;

    //Build Cache for left image 
    for(int i = -halfBlockW; i <= halfBlockW; i++)
    {
        for(int j = -halfBlockH; j <= halfBlockH; j++)
        {
            vec4 pixl =  imageLoad(inputImageL, id + ivec2(i, j));
            cachel[(i + halfBlockW) * blockW + j + halfBlockH] = pixl.r;
        }
    }

    //Run SAD operation on right image at each horizontal position
    for(int k = -maxK; k < maxK; k++)
    {
        sad = 0;
        for(int i = -halfBlockW; i <= halfBlockW; i++)
        {
            for(int j = -halfBlockH; j <= halfBlockH; j++)
            {
                vec4 pixr = imageLoad(inputImageR, id + ivec2(-k + i, j));
                float avpixl = cachel[(i + halfBlockW) * blockW + j + halfBlockH];
                avpixl =  imageLoad(inputImageL, id + ivec2(i, j)).r;
                float pixAbsDiff = length (avpixl - pixr.r);
                sad += pixAbsDiff;
            }
        }
        if(sad < lowestsad)
        {
            lowestsad = sad;
            lowestsadK = abs(k);
        }
    }
    float finalPix = float(lowestsadK) / float(maxK);
	vec4 outVec = vec4(finalPix, finalPix, finalPix, 1.0f);
    imageStore(outputImage, id, outVec); 
}

)";

bool StereoMatchSAD::Internal::shaderCompiled = false; 

StereoMatchSAD::Internal::Internal()
{
    blockW = 16;
    blockH = 16;
    maxK = 80;
}

void StereoMatchSAD::Internal::Run(ImageGPU* inputL, ImageGPU* inputR, ImageGPU* output)
{
    if(!shaderCompiled)
    {
        CompileImageComputeShaders(computeShaders, shaderSrc); 
        shaderCompiled = true; 
    }

    if(!inputL->IsSameDimensions(inputR))
    {
        std::cout << "Visi:StereoMatchSAD:inputL is not the same dimentions as inputR\n";
        return;
    }

    if(output->GetWidth() != inputL->GetWidth() || 
       output->GetHeight() != inputL->GetHeight())
    {
        output->Allocate(inputL->GetWidth(), inputL->GetHeight(), ImageType::GRAYSCALE16); 
    }
    
    ImageType inputType = inputL->GetType();

    glm::ivec2 groupCount = ComputeWorkGroupCount(glm::ivec2(output->GetWidth(), output->GetHeight()), glm::i32vec2(16, 16)); 

    ComputeShader& computeShader = computeShaders[inputType];

    computeShader.SetInt("blockW", blockW);
    computeShader.SetInt("blockH", blockH);
    computeShader.SetInt("blockA", blockW * blockH);
    computeShader.SetInt("halfBlockW", blockW * 0.5);
    computeShader.SetInt("halfBlockH", blockH * 0.5);
    computeShader.SetInt("maxK", maxK);

    computeShader.SetImage("inputImageL", inputL);
    computeShader.SetImage("inputImageR", inputR);
    computeShader.SetImage("outputImage", output, ComputeShader::WRITE_ONLY);

    computeShader.Dispatch(groupCount.x, groupCount.y, 1); 
    computeShader.Block();
}

void StereoMatchSAD::Internal::Run(Image* inputL, Image* inputR, Image* output)
{
    if(!inputL->IsSameDimensions(inputR))
    {
        std::cout << "Visi:StereoMatchSAD:inputL is not the same dimentions as inputR\n";
        return;
    }

    if(output->GetWidth() != inputL->GetWidth() || 
       output->GetHeight() != inputL->GetHeight() )
    {
        output->Allocate(inputL->GetWidth(), inputL->GetHeight(), ImageType::GRAYSCALE16); 
    }
    
    ParallelFor& pf = ParallelFor::GetInstance(); 

    auto kernel = [this, inputL, inputR, output](int x, int y)
    {
        glm::vec4 outVec = GetPixel(inputL, x, y);
       
        SetPixel(output, x, y, outVec); 
    };

    pf.Run(inputL->GetWidth(), inputL->GetHeight(), kernel);
}






StereoMatchSAD::StereoMatchSAD()
{
    internal = new Internal(); 
}

StereoMatchSAD::~StereoMatchSAD()
{
    delete internal; 
}

void StereoMatchSAD::Run(ImageGPU* inputL, ImageGPU* inputR, ImageGPU* output)
{
    internal->Run(inputL, inputR, output); 
}

void StereoMatchSAD::Run(Image* inputL, Image* inputR, Image* output)
{
    internal->Run(inputL, inputR, output); 
}

}
}