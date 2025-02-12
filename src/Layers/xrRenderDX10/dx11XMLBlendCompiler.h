#pragma once
#include "../../xrCore/XmlParser/xrXMLParser.h"
#include "../xrRender/blenders/Blender.h"
#include "../xrRender/blenders/Blender_Recorder.h"

class CXMLBlend
{
    CBlender_Compile* pCompiler;
    string256            File;
    CXml                Parser;

public:
    CXMLBlend(const char* FileName);
    ~CXMLBlend();
    Shader* Compile(const char* Texture);
    ShaderElement* MakeShader(const char* Texture, XML_NODE* pElement);

    static bool            Check(const char* FileName);

private:
    u32                BlendValidate(shared_str type);
    u32                StencilValidate(shared_str type);
    u32                CMPFunValidate(shared_str type);
    D3DCULL            CullModeValidate(shared_str type);
};