#include "stdafx.h"
#include "../../xrEngine/xrLevel.h"

#include "../xrForms/xrThread.h"

#include "global_calculation_data.h"
#include "LightThread.h"

#define NUM_THREADS        CPU::ID.n_threads - 1

void xrLight()
{
    u32    range = gl_data.slots_data.size_z();

    // Start threads, wait, continue --- perform all the work
    CThreadManager Threads;
    CTimer start_time;

    u32    stride = range / NUM_THREADS;
    u32    last = range - stride * (NUM_THREADS - 1);

    for (u32 thID = 0; thID < NUM_THREADS; thID++) {
        CThread* T = new LightThread(thID, thID * stride, thID * stride + ((thID == (NUM_THREADS - 1)) ? last : stride));
        T->thMessages = FALSE;
        T->thMonitor = FALSE;
        Threads.start(T);
    }

    Threads.wait();

    Msg("%d seconds elapsed.", (start_time.GetElapsed_ms()) / 1000);
}
