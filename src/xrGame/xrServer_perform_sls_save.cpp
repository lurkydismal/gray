#include "StdAfx.h"
#include "xrServer.h"
#include "xrMessages.h"
#include "xrserver_objects.h"

void xrServer::SLS_Save    (IWriter& fs)
{
    // Generate spawn+update
    NET_Packet        P;
    u32                position;
    xrS_entities::iterator    I=entities.begin(),E=entities.end();
    for (u32 C=0; I!=E; ++I,++C)
    {
        CSE_Abstract*    E_    = I->second;

        fs.open_chunk        (C);

        // Spawn
        E_->Spawn_Write        (P,TRUE);
        fs.w_u16            (u16(P.B.count));
        fs.w                (P.B.data,P.B.count);

        // Update
        P.w_begin            (M_UPDATE);
        P.w_u16                (E_->ID);
        P.w_chunk_open8        (position);
        E_->UPDATE_Write        (P);
        P.w_chunk_close8    (position);

        fs.w_u16            (u16(P.B.count));
        fs.w                (P.B.data,P.B.count);

        fs.close_chunk        ();
    }
}
