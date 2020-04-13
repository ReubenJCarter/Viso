
#pragma once

#include "../BaseNode.h"

#include "../DataTypes.h"

namespace Visi
{
namespace GUI
{
namespace NodeEditor
{
namespace Nodes 
{

class ImageAllocate: public BaseNode
{
	private:
        std::weak_ptr<ImageData> imageIn;
		std::weak_ptr<IntData> width;
		std::weak_ptr<IntData> height;
        std::weak_ptr<ImageTypeData> type;

        std::shared_ptr<ImageData> imageOut;
	 
	public:
		ImageAllocate()
        {
            std::vector<BaseNode::InputPortInfo> inputPorts( { {"im", true, ImageData().type()}, 
                                                          {"W", true, IntData().type()}, 
                                                          {"H", true, IntData().type()}, 
                                                          {"T", true, ImageTypeData().type()}} );

            std::vector<BaseNode::OutputPortInfo> outputPorts({ {"im", true, ImageData().type()} }); 
            
            Init("ImageAllocate", inputPorts, outputPorts, true, "Image Allocate", false); 
            SetValidationState(QtNodes::NodeValidationState::Error, "input error"); 
        }

		virtual ~ImageAllocate(){}	
};

class ImageDeallocate: public BaseNode
{
	private:
        std::weak_ptr<ImageData> imageIn;

        std::shared_ptr<ImageData> imageOut;

	public:
		ImageDeallocate()
        {
            std::vector<BaseNode::InputPortInfo> inputPorts( { {"im", true, ImageData().type()} } );
            std::vector<BaseNode::OutputPortInfo> outputPorts({ {"im", true, ImageData().type()} }); 
            
            Init("ImageDeallocate", inputPorts, outputPorts, true, "Image Deallocate", false); 
            SetValidationState(QtNodes::NodeValidationState::Error, "input error"); 
        }

		virtual ~ImageDeallocate(){}
};

class ImageGetDims: public BaseNode
{
	private:
        std::weak_ptr<ImageData> imageIn;

		std::shared_ptr<IntData> width;
		std::shared_ptr<IntData> height;
        std::shared_ptr<ImageTypeData> type;

	public:
		ImageGetDims()
        {
            std::vector<BaseNode::InputPortInfo> inputPorts( { {"im", true, ImageData().type()} } );
            std::vector<BaseNode::OutputPortInfo> outputPorts({ {"W", true, IntData().type()}, 
                                                                {"H", true, IntData().type()}, 
                                                                {"T", true, ImageTypeData().type()} }); 
            
            Init("ImageGetDims", inputPorts, outputPorts, true, "Image Get Dims", false); 
            SetValidationState(QtNodes::NodeValidationState::Error, "input error"); 
        }

		virtual ~ImageGetDims(){}
};

}
}
}
}