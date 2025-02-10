#include "StdAfx.h"
#include "Build.h"
#include "../xrLC_Light/xrFace.h"
#include "../xrLC_Light/calculate_normals.h"
#include "../xrLC_Light/xrLC_GlobalData.h"



//void 
// Performs simple cross-smooth

void CBuild::CalcNormals()
{

    calculate_normals<Vertex>::calc_normals( lc_global_data()->g_vertices(), lc_global_data()->g_faces() );
    // Models
    Status    ("Models...");
    MU_ModelsCalculateNormals();
}

