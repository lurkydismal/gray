///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef __CORE_H__
#define __CORE_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Core task/callback manager
#include "gsCommon.h"
#include "../darray.h"

#if defined(__cplusplus)
extern "C"
{
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define GSICORE_DYNAMIC_TASK_LIST
#define GSICORE_MAXTASKS       40


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef enum
{
    GSCore_IN_USE,
    GSCore_SHUTDOWN_PENDING,
    GSCore_SHUTDOWN_COMPLETE
} GSCoreValue;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef enum
{
    GSTaskResult_None,
    GSTaskResult_InProgress,
    GSTaskResult_Canceled,
    GSTaskResult_TimedOut,
    GSTaskResult_Finished
} GSTaskResult;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// delegates (optional, may be NULL)
typedef void(*GSTaskExecuteFunc) (void* theTaskData);
typedef void(*GSTaskCallbackFunc)(void* theTaskData, GSTaskResult theResult);
typedef void(*GSTaskCancelFunc)  (void* theTaskData);
typedef gsi_bool(*GSTaskCleanupFunc) (void* theTaskData); // post run cleanup
typedef GSTaskResult(*GSTaskThinkFunc)(void* theTaskData);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// "Private" struct for dispatching tasks.  Once tasks have been put in the queue
// they should only be modified from the think thread.
//      - When creating a task, you should set only the task data and delegates
typedef struct 
{
    int mId;
    gsi_time mTimeout;
    gsi_time mStartTime;
    gsi_bool mAutoThink;

    // These are not exclusive states (use bit flags?)
    gsi_i32  mIsStarted;   
    gsi_i32  mIsRunning;
    gsi_i32  mIsCanceled;
    gsi_i32  mIsCallbackPending; // does the task require a callback?

    // delegates
    void* mTaskData;
    GSTaskExecuteFunc  mExecuteFunc;
    GSTaskCallbackFunc mCallbackFunc;
    GSTaskCancelFunc   mCancelFunc;
    GSTaskCleanupFunc  mCleanupFunc;
    GSTaskThinkFunc    mThinkFunc;
} GSTask;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef struct 
{
    gsi_u32  mRefCount;

    gsi_bool volatile mIsStaticInitComplete;  // once per application init
    gsi_bool volatile mIsInitialized;  // gsi_true when ready to use
    gsi_bool volatile mIsShuttingDown; // gsi_true when shutting down

    GSICriticalSection mQueueCrit;
    #ifdef GSICORE_DYNAMIC_TASK_LIST
        DArray mTaskArray;
    #else
        GSTask* mTaskArray[GSICORE_MAXTASKS];
    #endif
        
} GSCoreMgr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void gsCoreInitialize       (void);
void gsCoreThink            (gsi_time theMS);
void gsCoreShutdown         (void);
GSCoreValue gsCoreIsShutdown(void);

GSTaskResult gsCoreTaskThink(GSTask* theTask);
void gsiCoreExecuteTask     (GSTask* theTask, gsi_time theTimeoutMs);
void gsiCoreCancelTask      (GSTask* theTask);

GSTask* gsiCoreCreateTask(void);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus)
}
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // __CORE_H__
