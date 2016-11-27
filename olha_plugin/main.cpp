#include "include_gl.h"
#include "dsfnavmap_thread.h"

#include <XPLMDisplay.h>
#include <XPLMDataAccess.h>
#include <XPLMGraphics.h>
#include <XPLMUtilities.h>
#include <XPLMProcessing.h>

#include <cstdio>
#include <cstring>

static const GLint gTexBottom = 10;
static const GLint gTexLext = 818;
static const GLint gTexWidth = 2043 - gTexLext;
static const GLint gTexHeight = 1236 - gTexBottom;

static XPLMDataRef posLongitude;
static XPLMDataRef posLatitude;
static XPLMDataRef posMagPsi;


static FILE* logFile = nullptr;
static DsfNavMapThread* navMap = nullptr;

static int MyDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void* inRefcon);
static float MyFlightLoopCallback(
    float                inElapsedSinceLastCall,
    float                inElapsedTimeSinceLastFlightLoop,
    int                  inCounter,
    void *               inRefcon);

PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc)
{
    strcpy(outName, "Olha");
    strcpy(outSig, "stvdedal.yak-42.olha");
    strcpy(outDesc, "Navigation device olha-1");

    logFile = fopen("olha.log", "w");
    if (!logFile) {
        XPLMDebugString("Olha: log file open error; aborting");
        return 0;
    }

    posLongitude = XPLMFindDataRef("sim/flightmodel/position/longitude");
    posLatitude = XPLMFindDataRef("sim/flightmodel/position/latitude");
    posMagPsi = XPLMFindDataRef("sim/flightmodel/position/magpsi");

    navMap = new DsfNavMapThread;

    XPLMRegisterDrawCallback(MyDrawCallback, xplm_Phase_Gauges, 0, NULL);
    XPLMRegisterFlightLoopCallback(MyFlightLoopCallback, 1.0f, NULL);

    return 1;
}

PLUGIN_API void	XPluginStop(void)
{
    XPLMUnregisterDrawCallback(MyDrawCallback, xplm_Phase_Gauges, 0, NULL);
    delete navMap;
    fclose(logFile);
}

PLUGIN_API void XPluginEnable(void)
{
    navMap->setPlane(XPLMGetDataf(posLongitude), XPLMGetDataf(posLatitude), XPLMGetDataf(posMagPsi));
    navMap->setPlaneScale(0.1, 0.1);
    navMap->setScale(1.0, 1.0);

    navMap->start(gTexWidth, gTexHeight);
    navMap->setRect(GLfloat(gTexLext), GLfloat(gTexBottom), GLfloat(gTexWidth), GLfloat(gTexHeight));
}

PLUGIN_API void XPluginDisable(void)
{
    navMap->stop();
}

static int MyDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void* inRefcon)
{
    navMap->render();

    return 1;
}

static float MyFlightLoopCallback(
    float                inElapsedSinceLastCall,
    float                inElapsedTimeSinceLastFlightLoop,
    int                  inCounter,
    void *               inRefcon)
{
    navMap->setPlane(XPLMGetDataf(posLongitude), XPLMGetDataf(posLatitude), XPLMGetDataf(posMagPsi));
    return 0.2f;
}