//-
// ==========================================================================
// Copyright (C) 1995 - 2006 Autodesk, Inc. and/or its licensors.  All 
// rights reserved.
//
// The coded instructions, statements, computer programs, and/or related 
// material (collectively the "Data") in these files contain unpublished 
// information proprietary to Autodesk, Inc. ("Autodesk") and/or its 
// licensors, which is protected by U.S. and Canadian federal copyright 
// law and by international treaties.
//
// The Data is provided for use exclusively by You. You have the right 
// to use, modify, and incorporate this Data into other products for 
// purposes authorized by the Autodesk software license agreement, 
// without fee.
//
// The copyright notices in the Software and this entire statement, 
// including the above license grant, this restriction and the 
// following disclaimer, must be included in all copies of the 
// Software, in whole or in part, and all derivative works of 
// the Software, unless such copies or derivative works are solely 
// in the form of machine-executable object code generated by a 
// source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND. 
// AUTODESK DOES NOT MAKE AND HEREBY DISCLAIMS ANY EXPRESS OR IMPLIED 
// WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE WARRANTIES OF 
// NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR 
// PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE, OR 
// TRADE PRACTICE. IN NO EVENT WILL AUTODESK AND/OR ITS LICENSORS 
// BE LIABLE FOR ANY LOST REVENUES, DATA, OR PROFITS, OR SPECIAL, 
// DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES, EVEN IF AUTODESK 
// AND/OR ITS LICENSORS HAS BEEN ADVISED OF THE POSSIBILITY 
// OR PROBABILITY OF SUCH DAMAGES.
//
// ==========================================================================
//+

#define NOMINMAX
#define MNoVersionString
#define MNoPluginEntry
#include <maya/MTypes.h>
#if MAYA_API_VERSION >= 20180000 && MAYA_API_VERSION <= 20190200
#include <maya/MCppCompat.h>
#endif
#include <maya/MGlobal.h>
#include <maya/MDGModifier.h>
#include <maya/MFloatVector.h>
#include <maya/MPlugArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnLightDataAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnPlugin.h>
#include "maya_xray_material.h"
#include "xr_gamemtls_lib.h"
#include "xr_shaders_xrlc_lib.h"
#include "xr_shaders_lib.h"
#include "xr_file_system.h"
#include <algorithm>

using namespace xray_re;

const MString maya_xray_material::k_type_name("XRayMtl");
const MTypeId maya_xray_material::k_type_id(0x105440);

MObject maya_xray_material::g_tc;
MObject maya_xray_material::g_drfl;
MObject maya_xray_material::g_it;
MObject maya_xray_material::g_itr;
MObject maya_xray_material::g_itg;
MObject maya_xray_material::g_itb;
MObject maya_xray_material::g_c;
MObject maya_xray_material::g_cr;
MObject maya_xray_material::g_cg;
MObject maya_xray_material::g_cb;
MObject maya_xray_material::g_ic;
MObject maya_xray_material::g_ir;
MObject maya_xray_material::g_ig;
MObject maya_xray_material::g_ib;
MObject maya_xray_material::g_oc;
MObject maya_xray_material::g_ocr;
MObject maya_xray_material::g_ocg;
MObject maya_xray_material::g_ocb;
MObject maya_xray_material::g_ot;
MObject maya_xray_material::g_otr;
MObject maya_xray_material::g_otg;
MObject maya_xray_material::g_otb;
MObject maya_xray_material::g_n;
MObject maya_xray_material::g_nx;
MObject maya_xray_material::g_ny;
MObject maya_xray_material::g_nz;
MObject maya_xray_material::g_ltd;
MObject maya_xray_material::g_ld;
MObject maya_xray_material::g_ldx;
MObject maya_xray_material::g_ldy;
MObject maya_xray_material::g_ldz;
MObject maya_xray_material::g_li; 
MObject maya_xray_material::g_lir;
MObject maya_xray_material::g_lig;
MObject maya_xray_material::g_lib;
MObject maya_xray_material::g_la;
MObject maya_xray_material::g_ldf;
MObject maya_xray_material::g_ls;
MObject maya_xray_material::g_lsf;
MObject maya_xray_material::g_psi;
MObject maya_xray_material::g_lbld;

MObject maya_xray_material::g_xrd;
MObject maya_xray_material::g_xre;
MObject maya_xray_material::g_xrc;
MObject maya_xray_material::g_xrm;

void* maya_xray_material::creator() { return new maya_xray_material; }

class maya_field_maker {
public:
            maya_field_maker();
    short        get(const std::string& name, MString& maya_name);

private:
    std::string    m_temp;
    uint16_t    m_crc16_table[256];
};

maya_field_maker::maya_field_maker()
{
    static const uint16_t crc16_init[8] = {
        0xc0c1, 0xc181, 0xc301, 0xc601, 0xcc01, 0xd801, 0xf001, 0xa001,
    };
    for (unsigned i = xr_dim(m_crc16_table); i != 0;) {
        --i;
        uint_fast16_t temp = 0;
        for (unsigned j = xr_dim(crc16_init); j != 0;) {
            if (i & (1 << (--j)))
                temp ^= crc16_init[j];
        }
        m_crc16_table[i] = uint16_t(temp & UINT16_MAX);
    }
}

short maya_field_maker::get(const std::string& name, MString& maya_name)
{
    uint_fast16_t crc = 0;
    m_temp.resize(std::max(name.length(), m_temp.capacity()));
    std::string::iterator dest_it = m_temp.begin();
    for (std::string::const_iterator it = name.begin(), end = name.end(); it != end;) {
        char c = *it++;
        if (c == '\\')
            c = '/';
        *dest_it++ = c;
#if 1
        crc = ((crc >> 8) & 0xff) ^ m_crc16_table[(crc ^ uint_fast16_t(c)) & 0xff];
#else
        uint_fast16_t temp = m_crc16_table[uint_fast16_t(c) ^ (crc & 0xff)];
        crc >>= 8;
        crc ^= temp;
#endif
    }
    maya_name.set(m_temp.data(), int(name.length() & INT_MAX));
    return short(crc);
}

template <class T>
bool less_func(T elem1, T elem2)
{
    if(elem1->name == "default" && elem1->name != elem2->name)
        return true;
    else if(elem2->name == "default")
        return false;
    return elem1->name < elem2->name;
}

template <> bool less_func<std::string&>(std::string &elem1, std::string &elem2)
{
    if(elem1 == "default" && elem1 != elem2)
        return true;
    else if(elem2 == "default")
        return false;
    return elem1 < elem2;
}

MStatus maya_xray_material::init()
{
    MStatus status;

    // input attributes
    MFnNumericAttribute num_attr_fn;
    g_tc = num_attr_fn.create("translucenceCoeff", "tc", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);
    num_attr_fn.setDefault(0.f);

    g_drfl = num_attr_fn.create("diffuseReflectivity", "drfl", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);
    num_attr_fn.setDefault(0.8f);

    g_cr = num_attr_fn.create("colorR", "cr",MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);
    num_attr_fn.setDefault(0.f);

    g_cg = num_attr_fn.create("colorG", "cg", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);
    num_attr_fn.setDefault(0.58824f);

    g_cb = num_attr_fn.create("colorB", "cb",MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);
    num_attr_fn.setDefault(0.644f);

    g_c = num_attr_fn.create("color", "c", g_cr, g_cg, g_cb, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);
    num_attr_fn.setDefault(0.f, 0.58824f, 0.644f);
    num_attr_fn.setUsedAsColor(true);

    g_ir = num_attr_fn.create("incandescenceR", "ir", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);
    num_attr_fn.setDefault(0.f);

    g_ig = num_attr_fn.create("incandescenceG", "ig", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);
    num_attr_fn.setDefault(0.f);

    g_ib = num_attr_fn.create("incandescenceB", "ib", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);
    num_attr_fn.setDefault(0.f);

    g_ic = num_attr_fn.create("incandescence", "ic", g_ir, g_ig, g_ib, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);
    num_attr_fn.setDefault(0.f, 0.f, 0.f);
    num_attr_fn.setUsedAsColor(true);

    g_itr = num_attr_fn.create("transparencyR", "itr", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);

    g_itg = num_attr_fn.create("transparencyG", "itg", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);

    g_itb = num_attr_fn.create("transparencyB", "itb", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);

    g_it = num_attr_fn.create("transparency", "it", g_itr, g_itg, g_itb, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);
    num_attr_fn.setDefault(0.f, 0.f, 0.f);
    num_attr_fn.setUsedAsColor(true);

    // xray attributes
    g_xrd = num_attr_fn.create("xrayDoubleSide", "xrd", MFnNumericData::kBoolean, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setKeyable(true);
    num_attr_fn.setStorable(true);
    num_attr_fn.setDefault(false);

    maya_field_maker maya_field;

    MFnEnumAttribute enum_attr_fn;
    g_xre = enum_attr_fn.create("xrayEngineShader", "xre", 0, &status);
    CHECK_MSTATUS(status);
    enum_attr_fn.setKeyable(true);
    enum_attr_fn.setStorable(true);
    xr_shaders_lib shaders_lib;
    if (shaders_lib.load(PA_GAME_DATA, "shaders.xr")) {
        MString field;
        std::vector<std::string> &names = const_cast<std::vector<std::string>&>(shaders_lib.names());
        std::sort(names.begin(), names.end(), less_func<std::string&>);
        int i = 0;
        for (std::vector<std::string>::iterator it = names.begin(),
                end = names.end(); it != end; ++it) {
            maya_field.get(*it, field);
            CHECK_MSTATUS(enum_attr_fn.addField(field, i++));
        }
        enum_attr_fn.setDefault("default");
    } else {
        msg("xray_re: can't open %s", "shaders.xr");
        MGlobal::displayError("xray_re: can't open shaders.xr");
    }
    if (shaders_lib.names().empty())
        enum_attr_fn.addField("default", 0);

    g_xrc = enum_attr_fn.create("xrayCompilerShader", "xrc", 0, &status);
    CHECK_MSTATUS(status);
    enum_attr_fn.setKeyable(true);
    enum_attr_fn.setStorable(true);
    xr_shaders_xrlc_lib shaders_xrlc_lib;
    if (shaders_xrlc_lib.load(PA_GAME_DATA, "shaders_xrlc.xr")) {
        MString field;
        int i = 0;
        xr_shader_xrlc_vec &shaders = const_cast<xr_shader_xrlc_vec&>(shaders_xrlc_lib.shaders());
        std::sort(shaders.begin(), shaders.end(), less_func<xr_shader_xrlc*>);
        for (xr_shader_xrlc_vec_cit it = shaders.begin(), end = shaders.end(); it != end; ++it) {
            short index = maya_field.get((*it)->name, field);
            CHECK_MSTATUS(enum_attr_fn.addField(field, i++));
        }
        enum_attr_fn.setDefault("default");
    } else {
        msg("xray_re: can't open %s", "shaders_xrlc.xr");
        MGlobal::displayError("xray_re: can't open shaders_xrlc.xr");
    }
    if (shaders_xrlc_lib.shaders().empty())
        enum_attr_fn.addField("default", 0);

    g_xrm = enum_attr_fn.create("xrayGameMaterial", "x", 0, &status);
    CHECK_MSTATUS(status);
    enum_attr_fn.setKeyable(true);
    enum_attr_fn.setStorable(true);
    xr_gamemtls_lib gamemtls_lib;
    if (gamemtls_lib.load(PA_GAME_DATA, "gamemtl.xr")) {
        MString field;
        int i = 0;
        xr_gamemtl_vec &gamemtl = const_cast<xr_gamemtl_vec&>(gamemtls_lib.materials());
        std::sort(gamemtl.begin(), gamemtl.end(), less_func<xr_gamemtl*>);
        for (xr_gamemtl_vec_cit it = gamemtl.begin(), end = gamemtl.end(); it != end; ++it) {
            short index = maya_field.get((*it)->name, field);
            CHECK_MSTATUS(enum_attr_fn.addField(field, i++));
        }
    } else {
        msg("xray_re: can't open %s", "gamemtl.xr");
        MGlobal::displayError("xray_re: can't open gamemtl.xr");
    }
    if (gamemtls_lib.materials().empty())
        enum_attr_fn.addField("default", 0);

    // color output
    g_ocr = num_attr_fn.create("outColorR", "ocr", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);

    g_ocg = num_attr_fn.create("outColorG", "ocg", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);

    g_ocb = num_attr_fn.create("outColorB", "ocb", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);

    g_oc = num_attr_fn.create("outColor", "oc", g_ocr, g_ocg, g_ocb, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setHidden(false);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);

    // transparency output
    g_otr = num_attr_fn.create("outTransparencyR", "otr", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);

    g_otg = num_attr_fn.create("outTransparencyG", "otg", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);

    g_otb = num_attr_fn.create("outTransparencyB", "otb", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);

    g_ot = num_attr_fn.create("outTransparency", "ot", g_otr, g_otg, g_otb, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setHidden(false);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);

    // camera normal
    g_nx = num_attr_fn.create( "normalCameraX", "nx", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setDefault(1.f);

    g_ny = num_attr_fn.create("normalCameraY", "ny", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setDefault(1.f);

    g_nz = num_attr_fn.create("normalCameraZ", "nz", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setDefault(1.f);

    g_n = num_attr_fn.create("normalCamera","n", g_nx, g_ny, g_nz, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setDefault(1.f, 1.f, 1.f);
    num_attr_fn.setHidden(true);

    // light direction
    g_ldx = num_attr_fn.create("lightDirectionX", "ldx", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(1.f);

    g_ldy = num_attr_fn.create("lightDirectionY", "ldy", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(1.f);

    g_ldz = num_attr_fn.create("lightDirectionZ", "ldz", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(1.f);

    g_ld = num_attr_fn.create("lightDirection", "ld", g_ldx, g_ldy, g_ldz, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(1.f, 1.f, 1.f);

    // light intensity
    g_lir = num_attr_fn.create( "lightIntensityR", "lir", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(1.f);

    g_lig = num_attr_fn.create("lightIntensityG", "lig", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(1.f);

    g_lib = num_attr_fn.create("lightIntensityB", "lib", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(1.f);

    g_li = num_attr_fn.create("lightIntensity", "li", g_lir, g_lig, g_lib, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(1.f, 1.f, 1.f);

    // light components
    g_la = num_attr_fn.create("lightAmbient", "la", MFnNumericData::kBoolean, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(true);

    g_ldf = num_attr_fn.create("lightDiffuse", "ldf", MFnNumericData::kBoolean, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(true);

    g_ls = num_attr_fn.create("lightSpecular", "ls", MFnNumericData::kBoolean, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(false);

    g_lsf = num_attr_fn.create("lightShadowFraction", "lsf", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(1.f);

    g_psi = num_attr_fn.create("preShadowIntensity", "psi", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);
    num_attr_fn.setDefault(1.f);

    g_lbld = num_attr_fn.createAddr("lightBlindData", "lbld", &status);
    CHECK_MSTATUS(status);
    num_attr_fn.setStorable(false);
    num_attr_fn.setHidden(true);
    num_attr_fn.setReadable(true);
    num_attr_fn.setWritable(false);

    MFnLightDataAttribute light_attr_fn;
    g_ltd = light_attr_fn.create("lightDataArray", "ltd",
            g_ld, g_li, g_la, g_ldf, g_ls, g_lsf, g_psi, g_lbld, &status);
    CHECK_MSTATUS(status);
    light_attr_fn.setArray(true);
    light_attr_fn.setStorable(false);
    light_attr_fn.setHidden(true);
    light_attr_fn.setDefault(0.f, 0.f, 0.f, 0.f, 0.f, 0.f, true, true, false, 1.f, 1.f, 0);

    // add attributes to the node
    addAttribute(g_tc);
    addAttribute(g_drfl);
    addAttribute(g_c);
    addAttribute(g_ic);
    addAttribute(g_it);
    addAttribute(g_oc);
    addAttribute(g_ot);
    addAttribute(g_n);
    addAttribute(g_ltd);

    addAttribute(g_xre);
    addAttribute(g_xrc);
    addAttribute(g_xrm);
    addAttribute(g_xrd);

    // define attribute dependencies
    attributeAffects(g_tc, g_oc);
    attributeAffects(g_drfl, g_oc);
    attributeAffects(g_cr, g_oc);
    attributeAffects(g_cg, g_oc);
    attributeAffects(g_cb, g_oc);
    attributeAffects(g_c, g_oc);
    attributeAffects(g_itr, g_ot);
    attributeAffects(g_itg, g_ot);
    attributeAffects(g_itb, g_ot);
    attributeAffects(g_it, g_ot);
    attributeAffects(g_it, g_oc);
    attributeAffects(g_ir, g_oc);
    attributeAffects(g_ig, g_oc);
    attributeAffects(g_ib, g_oc);
    attributeAffects(g_ic, g_oc);
    attributeAffects(g_lir, g_oc);
    attributeAffects(g_lig, g_oc);
    attributeAffects(g_lib, g_oc);
    attributeAffects(g_li, g_oc);
    attributeAffects(g_nx, g_oc);
    attributeAffects(g_ny, g_oc);
    attributeAffects(g_nz, g_oc);
    attributeAffects(g_n, g_oc);
    attributeAffects(g_ldx, g_oc);
    attributeAffects(g_ldy, g_oc);
    attributeAffects(g_ldz, g_oc);
    attributeAffects(g_ld, g_oc);
    attributeAffects(g_la, g_oc);
    attributeAffects(g_ldf, g_oc);
    attributeAffects(g_ls, g_oc);
    attributeAffects(g_lsf, g_oc);
    attributeAffects(g_psi, g_oc);
    attributeAffects(g_lbld, g_oc);
    attributeAffects(g_ltd, g_oc);

    return MS::kSuccess;
}

maya_xray_material::~maya_xray_material() {}

void maya_xray_material::postConstructor()
{
    return setMPSafe(true);
}

MStatus maya_xray_material::compute(const MPlug& plug, MDataBlock& block)
{
    if (plug == g_oc || plug == g_ocr || plug == g_ocg || plug == g_ocb ||
            plug == g_ot || plug == g_otr || plug == g_otg || plug == g_otb) {
        MStatus status;
        MFloatVector oc(0.f, 0.f, 0.f);

        // get surface shading parameters from input block
        MFloatVector& n = block.inputValue(g_n, &status).asFloatVector();
        if (!status)
            return status;

        MFloatVector& c = block.inputValue(g_c, &status).asFloatVector();
        if (!status)
            return status;

        MFloatVector& ic = block.inputValue(g_ic, &status).asFloatVector();
        if (!status)
            return status;

        float drfl = block.inputValue(g_drfl, &status).asFloat();
        if (!status)
            return status;

//         float tc = block.inputValue(g_tc, &status).asFloat();
//        if (!status)
//            return status;

        // get light list
        MArrayDataHandle ltdh = block.inputArrayValue(g_ltd, &status);
        if (!status)
            return status;

//        unsigned num_lights = ltdh.elementCount(&status);
//        CHECK_MSTATUS(status);

        // calculate the effect of the lights in the scene on the color

        // iterate through light list and get ambient/diffuse values
//        for (unsigned i = 1; i <= num_lights; ++i) {
        for (unsigned i = ltdh.elementCount(); i != 0; --i, ltdh.next()) {
            // Get the current light out of the array
            MDataHandle light = ltdh.inputValue(&status);
            if (!status)
                return status;

            // get the intensity of that light
            MFloatVector& li = light.child(g_li).asFloatVector();

            // find ambient component
            if (light.child(g_la).asBool())
                oc += li;

            // find diffuse component
            if (light.child(g_ldf).asBool()) {
                MFloatVector& ld = light.child(g_ld).asFloatVector();
                float cos_ln = ld * n;
                   if (cos_ln > 0.f)
                    oc += li * (cos_ln*drfl);
            }
//            if (i < num_lights)
//                status = ltdh.next();
        }

        // factor incident light with surface color and add incandescence
        oc.x = oc.x * c.x + ic.x;
        oc.y = oc.y * c.y + ic.y;
        oc.z = oc.z * c.z + ic.z;

        // set output color attribute
        if (plug == g_oc || plug == g_ocr || plug == g_ocg || plug == g_ocb) {
            // get the handle to the attribute
            MDataHandle och = block.outputValue(g_oc, &status);
            if (status) {
                och.asFloatVector() = oc;
                och.setClean();
            }
        }

        // set ouput transparency
        if (plug == g_ot || plug == g_otr || plug == g_otg || plug == g_otb) {
            MFloatVector& it = block.inputValue(g_it, &status).asFloatVector();
            CHECK_MSTATUS(status);

            // get the handle to the attribute
            MDataHandle oth = block.outputValue(g_ot, &status);
            if (status) {
                oth.asFloatVector() = it;
                oth.setClean();
            }
        }
        return MS::kSuccess;
    }
    msg("xray_re: unexpected plug %s", plug.name().asChar());
    MGlobal::displayError(MString("xray_re: unexpected plug ") + plug.name().asChar());
    return MS::kUnknownParameter;
}

#if 0
static inline const char* to_bool(bool value)
{
    return value ? "true" : "false";
}

MStatus maya_xray_material::legalConnection(const MPlug& plug, const MPlug& other_plug, bool as_src, bool& is_legal) const
{
    msg("legalConnection(%s, %s, %s)", plug.partialName(true).asChar(), other_plug.partialName(true).asChar(), to_bool(as_src));
#if 0
    if (plug.partialName() == "msg" && other_plug.partialName() == "t") {
        MStatus status;
        MObject other_obj = other_plug.node(&status);
        CHECK_MSTATUS(status);
        if (status && other_obj.apiType() == MFn::kMaterialInfo) {
#if 0
            MPlug color_plug(thisMObject(), g_c);
            MPlugArray connected_plugs;
            if (color_plug.connectedTo(connected_plugs, true, false, &status)) {
                CHECK_MSTATUS(status);
                msg("connections: %u", connected_plugs.length());
                for (unsigned i = connected_plugs.length(); i != 0;) {
                    if (connected_plugs[--i].partialName() != "oc")
                        continue;
                    MObject obj = connected_plugs[i].node(&status);
                    CHECK_MSTATUS(status);
                    if (status && obj.hasFn(MFn::kTexture2d)) {
                        MFnDependencyNode texture_fn(obj, &status);
                        CHECK_MSTATUS(status);
                        MPlug message_plug = texture_fn.findPlug("msg", &status);
                        CHECK_MSTATUS(status);
                        msg("  connecting %s to %s", message_plug.name().asChar(), other_plug.name().asChar());
#if 1
    MString command("connectAttr ");
    command += message_plug.name();
    command += " ";
    command += other_plug.name();
    status = MGlobal::executeCommand(command, true);
    CHECK_MSTATUS(status);
#else
                        MDGModifier dg_modifier;
                        status = dg_modifier.connect(message_plug, other_plug);
                        CHECK_MSTATUS(status);
                        status = dg_modifier.doIt();
                        CHECK_MSTATUS(status);
#endif
                        break;
                    }
                }
            }
#endif
            msg("  illegal, denied");
            is_legal = false;
            return MS::kSuccess;
        }
    }
#endif
    return MS::kUnknownParameter;
}

MStatus maya_xray_material::connectionMade(const MPlug& plug, const MPlug& other_plug, bool as_src)
{
    msg("connectionMade(%s, %s, %s)", plug.partialName(true).asChar(), other_plug.partialName(true).asChar(), to_bool(as_src));
    return MS::kSuccess;
}

MStatus maya_xray_material::connectionBroken(const MPlug& plug, const MPlug& other_plug, bool as_src)
{
    msg("connectionBroken(%s, %s, %s)", plug.partialName(true).asChar(), other_plug.partialName(true).asChar(), to_bool(as_src));
    return MS::kSuccess;
}
#endif

MStatus maya_xray_material::initialize(MFnPlugin& plugin_fn)
{
    const MString user_classify("shader/surface");
    MStatus status = plugin_fn.registerNode(k_type_name, k_type_id, creator, init,
            MPxNode::kDependNode, &user_classify);
    if (status) {
        MGlobal::executeCommand(
                "if(`window -exists createRenderNodeWindow`)"
                "{refreshCreateRenderNodeWindow(\"shader/surface\");}\n");
    }
    return status;
}

MStatus maya_xray_material::uninitialize(MFnPlugin& plugin_fn)
{
    MStatus status = plugin_fn.deregisterNode(k_type_id);
    MGlobal::executeCommand(
            "if(`window -exists createRenderNodeWindow`)"
            "{refreshCreateRenderNodeWindow(\"shader/surface\");}\n");
    return status;
}
