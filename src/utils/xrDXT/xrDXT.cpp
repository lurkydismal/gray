#include "StdAfx.h"
#include "xrDXT.h"

#include <RedImage.hpp>

#include "../../Layers/xrRender/ETextureParams.h"

extern int DXTCompressImageRI(LPCSTR out_name, u8* raw_data, u32 w, u32 h, u32 pitch, STextureParams* fmt, u32 depth);
extern int DXTCompressImageNVTT(LPCSTR out_name, u8* raw_data, u32 w, u32 h, u32 pitch, STextureParams* fmt, u32 depth);
extern int DXTCompressBump(LPCSTR out_name, u8* raw_data, u8* normal_map, u32 w, u32 h, u32 pitch, STextureParams* fmt, u32 depth);

void DXTUtils::Converter::MakeTGA(xr_path From, xr_path To)
{
    RedImageTool::RedImage Surface;
    if (Surface.LoadFromFile(From.xstring().data()))
    {
        Surface.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);
        Surface.SaveToTga(To.xstring().data());
    }
}

void DXTUtils::Converter::MakeTGA(u32 w, u32 h, U32Vec& From, xr_path To)
{
    RedImageTool::RedImage Surface;
    Surface.Create(w, h);

    for (u32 y = 0; y < h; y++)
    {
        for (u32 x = 0; x < w;x++)
        {
            Surface.SetPixel(From[y * w + x], x, y);
        }
    }

    Surface.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);
    Surface.SwapRB();
    Surface.SaveToTga(To.xstring().data());
}

void DXTUtils::Converter::MakePNG(xr_path From, xr_path To)
{
    RedImageTool::RedImage Surface;
    if (Surface.LoadFromFile(From.xstring().data()))
    {
        Surface.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);
        Surface.SaveToPng(To.xstring().data());
    }
}

int DXT_API DXTUtils::Compress(const char* out_name, u8* raw_data, u8* normal_map, u32 w, u32 h, u32 pitch, STextureParams* fmt, u32 depth)
{
    switch (fmt->type) 
    {
    case STextureParams::ttImage:
    case STextureParams::ttCubeMap:
    case STextureParams::ttNormalMap:
    case STextureParams::ttTerrain:
    {
        switch (fmt->mip_filter)
        {
            case STextureParams::kMIPFilterAdvanced:
            case STextureParams::kMIPFilterBox:
            case STextureParams::kMIPFilterTriangle:
            case STextureParams::kMIPFilterKaiser:
            {
                if (fmt->fmt == STextureParams::tfBC7)
                {
                    // FX: Dirty hack for nvtt
                    fmt->fmt = STextureParams::tfRGBA;

                    int RetCode = DXTCompressImageNVTT(out_name, raw_data, w, h, pitch, fmt, depth);

                    RedImageTool::RedImage Img;
                    Img.LoadFromFile(out_name);
                    Img.Convert(RedImageTool::RedTexturePixelFormat::BC7);
                    Img.SaveToDds(out_name);

                    return RetCode;
                }

                return DXTCompressImageNVTT(out_name, raw_data, w, h, pitch, fmt, depth);
            }

            case STextureParams::kMIPFilterPoint:
            case STextureParams::kMIPFilterCubic:
            case STextureParams::kMIPFilterCatrom:
            case STextureParams::kMIPFilterMitchell:
                return DXTCompressImageRI(out_name, raw_data, w, h, pitch, fmt, depth);
                
            case STextureParams::kMIPFilterGaussian:
            case STextureParams::kMIPFilterSinc    :
            case STextureParams::kMIPFilterBessel:
            case STextureParams::kMIPFilterHanning:
            case STextureParams::kMIPFilterHamming:
            case STextureParams::kMIPFilterBlackman:
            {
                Msg("Unsupported mip filter in %s!", out_name);
                fmt->mip_filter = STextureParams::kMIPFilterKaiser;
                return DXTCompressImageNVTT(out_name, raw_data, w, h, pitch, fmt, depth);
            }
        }
        
        break;
    }
    case STextureParams::ttBumpMap:
        return DXTCompressBump(out_name, raw_data, normal_map, w, h, pitch, fmt, depth);
        break;
    default: NODEFAULT;
    }
    return -1;
}

U8Vec DXT_API DXTUtils::GitPixels(const char* FileName, u32 NewW, u32 NewH)
{
    U8Vec Pixels;

    RedImageTool::RedImage Img;
    Img.LoadFromFile(FileName);
    Img.ClearMipLevels();
    Img.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);
    Img.Scale(NewW, NewH, RedImageTool::RedResizeFilter::Cubicbspline);
    Img.SwapRB();

    Pixels.resize(NewH * NewW * 4);
    memcpy(Pixels.data(), *Img, NewH * NewW * 4);

    return Pixels;
}

DXTUtils::ImageInfo DXT_API DXTUtils::GitPixels(const char* FileName)
{
    U8Vec Pixels;

    RedImageTool::RedImage Img;
    Img.LoadFromFile(FileName);
    Img.ClearMipLevels();
    Img.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);
    Img.SwapRB();

    size_t PixelCount = Img.GetWidth() * Img.GetHeight() * 4;
    Pixels.resize(PixelCount);
    memcpy(Pixels.data(), *Img, PixelCount);

    return { Img.GetWidth(), Img.GetHeight(), Pixels };
}

void DXTUtils::Filter::Resize(u32* dst, u32 dstW, u32 dstH, u32* out, u32 outW, u32 outH)
{
    RedImageTool::RedImage Surface;
    Surface.Create(dstW, dstH);

    for (u32 y = 0; y < dstH; y++)
    {
        for (u32 x = 0; x < dstW; x++)
        {
            Surface.SetPixel(dst[y * dstW + x], x, y);
        }
    }

    Surface.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);
    //Surface.SwapRB();
    Surface.Scale(outW, outH, RedImageTool::RedResizeFilter::Default);

    size_t PixelCount = Surface.GetWidth() * Surface.GetHeight() * 4;
    memcpy(out, *Surface, PixelCount);
}