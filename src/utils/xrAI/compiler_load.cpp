#include "StdAfx.h"
#include "compiler.h"
//.#include "communicate.h"
#include "levelgamedef.h"
#include "level_graph.h"
#include "AIMapExport.h"

IC    const Fvector vertex_position(const CLevelGraph::CPosition &Psrc, const Fbox &bb, const SAIParams &params)
{
    Fvector                Pdest;
    int    x,z, row_length;
    row_length            = iFloor((bb.max.z - bb.min.z)/params.fPatchSize + EPS_L + 1.5f);
    x                    = Psrc.xz() / row_length;
    z                    = Psrc.xz() % row_length;
    Pdest.x =            float(x)*params.fPatchSize + bb.min.x;
    Pdest.y =            (float(Psrc.y())/65535)*(bb.max.y-bb.min.y) + bb.min.y;
    Pdest.z =            float(z)*params.fPatchSize + bb.min.z;
    return                (Pdest);
}

struct CNodePositionConverter {
    IC        CNodePositionConverter(const SNodePositionOld &Psrc, hdrNODES &m_header, NodePosition &np);
};

IC CNodePositionConverter::CNodePositionConverter(const SNodePositionOld &Psrc, hdrNODES &m_header, NodePosition &np)
{
    Fvector        Pdest;
    Pdest.x        = float(Psrc.x)*m_header.size;
    Pdest.y        = (float(Psrc.y)/65535)*m_header.size_y + m_header.aabb.min.y;
    Pdest.z        = float(Psrc.z)*m_header.size;
    CNodePositionCompressor(np,Pdest,m_header);
    np.y        (Psrc.y);
}

//-----------------------------------------------------------------
template <class T>
void transfer(const char *name, xr_vector<T> &dest, IReader& F, u32 chunk)
{
    IReader*    O    = F.open_chunk(chunk);
    u32        count    = O?(O->length()/sizeof(T)):0;
    clMsg            ("* %16s: %d",name,count);
    if (count)  
    {
        dest.reserve(count);
        dest.insert    (dest.begin(), (T*)O->pointer(), (T*)O->pointer() + count);
    }
    if (O)        O->close    ();
}

extern u32*        Surface_Load    (char* name, u32& w, u32& h);
extern void        Surface_Init    ();

void xrLoad(LPCSTR name, bool draft_mode)
{
    FS.get_path                    ("$level$")->_set    ((LPSTR)name);
    string256                    N;
    if (!draft_mode)    {
        // shaders
        string_path                N__;
        FS.update_path            (N__,"$game_data$","shaders_xrlc.xr");
        g_shaders_xrlc            = new Shader_xrLC_LIB ();
        g_shaders_xrlc->Load    (N__);

        // Load CFORM
        {
            xr_strconcat(N__,name,"build.cform");
            IReader*            fs = FS.r_open(N__);
            R_ASSERT            (fs->find_chunk(0));

            hdrCFORM            H;
            fs->r                (&H,sizeof(hdrCFORM));
            R_ASSERT            (CFORM_CURRENT_VERSION==H.version);

            Fvector*    verts    = (Fvector*)fs->pointer();
            CDB::TRI*    tris    = (CDB::TRI*)(verts+H.vertcount);
            Level.build            ( verts, H.vertcount, tris, H.facecount );
            Level.syncronize    ();
            Msg("* Level CFORM: %dK",Level.memory()/1024);

            g_rc_faces.resize    (H.facecount);
            R_ASSERT(fs->find_chunk(1));
            fs->r                (&*g_rc_faces.begin(),g_rc_faces.size()*sizeof(b_rc_face));

            LevelBB.set            (H.aabb);
            FS.r_close            (fs);
        }

        // Load level data
        {
            xr_strconcat(N__,name,"build.prj");
            IReader*    fs        = FS.r_open (N__);
            IReader*    F;

            // Version
            u32 version;
            fs->r_chunk            (EB_Version,&version);
            R_ASSERT            (XRCL_CURRENT_VERSION >= 17);
            R_ASSERT            (XRCL_CURRENT_VERSION <= 18);

            // Header
            b_params            Params;
            fs->r_chunk            (EB_Parameters,&Params);

            // Load level data
            transfer("materials",    g_materials,            *fs,        EB_Materials);
            transfer("shaders_xrlc",g_shader_compile,        *fs,        EB_Shaders_Compile);

            // process textures
            bool is_thm_missing = false;
            bool is_tga_missing = false;

            Status            ("Processing textures...");
            {
                Surface_Init        ();
                F = fs->open_chunk    (EB_Textures);
#ifdef _M_X64
                u32 tex_count = F->length() / sizeof(b_texture64);
#else
                u32 tex_count = F->length() / sizeof(b_texture);
#endif
                for (u32 t=0; t<tex_count; t++)
                {
                    Progress        (float(t)/float(tex_count));

#ifdef _M_X64
                    b_texture64    TEX;
                    F->r(&TEX, sizeof(TEX));
                    b_BuildTexture    BT;

                    // ptr should be copied separately
                    CopyMemory(&BT, &TEX, sizeof(TEX) - 4);
                    BT.pSurface = (u32*)TEX.pSurface;
#else
                    b_texture TEX;
                    F->r(&TEX, sizeof(TEX));

                    b_BuildTexture BT;
                    CopyMemory(&BT, &TEX, sizeof(TEX));
#endif

                    // load thumbnail
                    string128        &N_ = BT.name;
                    LPSTR            extension = strext(N_);
                    if (extension)
                        *extension    = 0;

                    xr_strlwr        (N_);

                    if (0==xr_strcmp(N_,"level_lods")) {
                        // HACK for merged lod textures
                        BT.dwWidth    = 1024;
                        BT.dwHeight    = 1024;
                        BT.bHasAlpha= TRUE;
                        BT.pSurface    = 0;
                    } else {
                        xr_strcat        (N_,".thm");
                        IReader* THM    = FS.r_open("$game_textures$",N_);

                        if (!THM) {
                            clMsg("cannot find thm: %s", N);
                            is_thm_missing = true;
                            continue;
                        }

                        // version
                        u32 version_                = 0;
                        R_ASSERT                (THM->r_chunk(THM_CHUNK_VERSION,&version_));
                        // if( version!=THM_CURRENT_VERSION )    FATAL    ("Unsupported version of THM file.");

                        // analyze thumbnail information
                        R_ASSERT(THM->find_chunk(THM_CHUNK_TEXTUREPARAM));
                        THM->r                  (&BT.THM.fmt,sizeof(STextureParams::ETFormat));
                        BT.THM.flags.assign        (THM->r_u32());
                        BT.THM.border_color        = THM->r_u32();
                        BT.THM.fade_color        = THM->r_u32();
                        BT.THM.fade_amount        = THM->r_u32();
                        BT.THM.mip_filter        = THM->r_u32();
                        BT.THM.width            = THM->r_u32();
                        BT.THM.height           = THM->r_u32();
                        BOOL            bLOD=FALSE;
                        if (N_[0]=='l' && N_[1]=='o' && N_[2]=='d' && N_[3]=='\\') bLOD = TRUE;

                        // load surface if it has an alpha channel or has "implicit lighting" flag
                        BT.dwWidth                = BT.THM.width;
                        BT.dwHeight                = BT.THM.height;
                        BT.bHasAlpha            = BT.THM.HasAlphaChannel();
                        BT.pSurface                = 0;
                        if (!bLOD) 
                        {
                            if (BT.bHasAlpha || BT.THM.flags.test(STextureParams::flImplicitLighted))
                            {
                                clMsg        ("- loading: %s",N_);
                                u32            w=0, h=0;
                                BT.pSurface = Surface_Load(N_,w,h); 

                                if (!BT.pSurface) {
                                    clMsg("cannot find tga texture: %s", N);
                                    is_tga_missing = true;
                                    continue;
                                }

                                if ((w != BT.dwWidth) || (h != BT.dwHeight)) {
                                    Msg("! THM doesn't correspond to the texture: %dx%d -> %dx%d", BT.dwWidth, BT.dwHeight, w, h);

                                    BT.dwWidth = BT.THM.width = w;
                                    BT.dwHeight = BT.THM.height = h;
                                }
                                BT.Vflip    ();
                            } else {
                                // Free surface memory
                            }
                        }
                    }

                    // save all the stuff we've created
                    g_textures.push_back    (BT);
                }

                R_ASSERT2(!is_thm_missing, "Some of required thm's are missing. See log for details.");
                R_ASSERT2(!is_tga_missing, "Some of required tga_textures are missing. See log for details.");
            }
        }
    }
    
    // Load lights
    {
        xr_strconcat(N,name,"build.prj");

        IReader*    F            = FS.r_open(N);
        R_ASSERT2                (F,"There is no file 'build.prj'!");
        IReader                    &fs= *F;

        // Version
        u32 version;
        fs.r_chunk                (EB_Version,&version);
        R_ASSERT                (XRCL_CURRENT_VERSION >= 17);
        R_ASSERT                (XRCL_CURRENT_VERSION <= 18);

        // Header
        b_params                Params;
        fs.r_chunk                (EB_Parameters,&Params);

        // Lights (Static)
        {
            F = fs.open_chunk(EB_Light_static);
            b_light_static    temp;
            u32 cnt        = F->length()/sizeof(temp);
            for                (u32 i=0; i<cnt; i++)
            {
                R_Light_Fast RL;
                F->r        (&temp,sizeof(temp));
                Flight&        L = temp.data;
                if (_abs(L.range) > 10000.f) {
                    Msg        ("! BAD light range : %f",L.range);
                    L.range    = L.range > 0.f ? 10000.f : -10000.f;
                }

                // type
                if            (L.type == D3DLIGHT_DIRECTIONAL)    RL.type    = LT_DIRECT;
                else                                            RL.type = LT_POINT;

                // generic properties
                RL.position.set                (L.position);
                RL.direction.normalize_safe    (L.direction);
                RL.range                =    L.range*1.1f;
                RL.range2                =    RL.range*RL.range;
                RL.attenuation0            =    L.attenuation0;
                RL.attenuation1            =    L.attenuation1;
                RL.attenuation2            =    L.attenuation2;

                RL.amount                =    L.diffuse.magnitude_rgb    ();
                RL.tri[0].set            (0,0,0);
                RL.tri[1].set            (0,0,0);
                RL.tri[2].set            (0,0,0);

                // place into layer
                if (0==temp.controller_ID)    g_lights.push_back        (RL);
            }
            F->close        ();
        }
    }

    // Load initial map from the Level Editor
    {
        string_path            file_name;
        xr_strconcat(file_name,name,"build.aimap");
        IReader                *F = FS.r_open(file_name);
        R_ASSERT2            (F, file_name);

        R_ASSERT            (F->open_chunk(E_AIMAP_CHUNK_VERSION));
        R_ASSERT            (F->r_u16() == E_AIMAP_VERSION);

        R_ASSERT            (F->open_chunk(E_AIMAP_CHUNK_BOX));
        F->r                (&LevelBB,sizeof(LevelBB));

        R_ASSERT            (F->open_chunk(E_AIMAP_CHUNK_PARAMS));
        F->r                (&g_params,sizeof(g_params));

        R_ASSERT            (F->open_chunk(E_AIMAP_CHUNK_NODES));
        u32                    N_ = F->r_u32();
        R_ASSERT2            (N_ < ((u32(1) << u32(MAX_NODE_BIT_COUNT)) - 2),"Too many nodes!");
        g_nodes.resize        (N_);

        hdrNODES            H;
        H.version            = XRAI_CURRENT_VERSION;
        H.count                = N_+1;
        H.size                = g_params.fPatchSize;
        H.size_y            = 1.f;
        H.aabb                = LevelBB;
        
        typedef u32 NodeLink;
        for (u32 i=0; i<N_; i++) {
            NodeLink            id;
            u16                 pl;
            SNodePositionOld     _np;
            NodePosition         np;
            
            for (int j=0; j<4; ++j) {
                F->r(&id, 3);
                id = id & 0x00ffffff;
                if (id == InvalidNode)
                    id = InvalidNode;
                g_nodes[i].n[j] = id;
            }

            pl                = F->r_u16();
            pvDecompress    (g_nodes[i].Plane.n,pl);
            F->r            (&_np,sizeof(_np));
            CNodePositionConverter(_np,H,np);
            g_nodes[i].Pos    = vertex_position(np,LevelBB,g_params);

            g_nodes[i].Plane.build(g_nodes[i].Pos,g_nodes[i].Plane.n);
        }

        F->close            ();

        if (strstr(Core.Params,"-clear_temp_files"))
            DeleteFileA        (file_name);
    }
}
