#include "include_gl.h"
#include "dsfnavmap_thread.h"

#include <XPLMDisplay.h>
#include <XPLMDataAccess.h>
#include <XPLMGraphics.h>
#include <XPLMUtilities.h>
#include <XPLMProcessing.h>
#include <XPLMNavigation.h>

#include <cstdio>
#include <cstring>
#include <cmath>

static const GLint gTexBottom = 10;
static const GLint gTexLext = 818;
static const GLint gTexWidth = 2043 - gTexLext;
static const GLint gTexHeight = 1236 - gTexBottom;

static XPLMDataRef posLongitude;
static XPLMDataRef posLatitude;
static XPLMDataRef posMagPsi;

static const char* strDistanceToNearestAiroport = "stvdedal/yak-42/olha/distance_nearest_airoport";
static const char* strLEDNearestAiroport = "stvdedal/yak-42/olha/led_nearest_airoport";

static XPLMDataRef drefDistanceToNearestAiroport;
static XPLMDataRef drefLEDNearestAiroport;

static FILE* logFile = nullptr;
static DsfNavMapThread* navMap = nullptr;

static bool LED_NearestAiroport = false;
static bool LED_State_NearestAiroport = false;

static int MyDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void* inRefcon);
static float TimerCallback(
    float                inElapsedSinceLastCall,
    float                inElapsedTimeSinceLastFlightLoop,
    int                  inCounter,
    void *               inRefcon);

static float TimerCallbackBlink(
    float                inElapsedSinceLastCall,
    float                inElapsedTimeSinceLastFlightLoop,
    int                  inCounter,
    void *               inRefcon);

static float MyGetDistanceToNearestAiroport(void* inRefcon);
static int MyGetLEDNearestAiroport(void* inRefcon);

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

    drefDistanceToNearestAiroport = XPLMRegisterDataAccessor(
        strDistanceToNearestAiroport,
        xplmType_Float,
        0,
        NULL,                           // inReadInt
        NULL,                           // inWriteInt
        MyGetDistanceToNearestAiroport, // inReadFloat
        NULL,                           // inWriteFloat
        NULL,                           // inReadDouble
        NULL,                           // inWriteDouble
        NULL,                           // inReadIntArray
        NULL,                           // inWriteIntArray
        NULL,                           // inReadFloatArray
        NULL,                           // inWriteFloatArray
        NULL,                           // inReadData
        NULL,                           // inWriteData
        NULL,                           // inReadRefcon
        NULL                            // inWriteRefcon
    );

    drefLEDNearestAiroport = XPLMRegisterDataAccessor(
        strLEDNearestAiroport,
        xplmType_Int,
        0,
        MyGetLEDNearestAiroport,        // inReadInt
        NULL,                           // inWriteInt
        NULL,                           // inReadFloat
        NULL,                           // inWriteFloat
        NULL,                           // inReadDouble
        NULL,                           // inWriteDouble
        NULL,                           // inReadIntArray
        NULL,                           // inWriteIntArray
        NULL,                           // inReadFloatArray
        NULL,                           // inWriteFloatArray
        NULL,                           // inReadData
        NULL,                           // inWriteData
        NULL,                           // inReadRefcon
        NULL                            // inWriteRefcon
    );

    posLongitude = XPLMFindDataRef("sim/flightmodel/position/longitude");
    posLatitude = XPLMFindDataRef("sim/flightmodel/position/latitude");
    posMagPsi = XPLMFindDataRef("sim/flightmodel/position/magpsi");

    navMap = new DsfNavMapThread;

    XPLMRegisterDrawCallback(MyDrawCallback, xplm_Phase_Gauges, 0, NULL);

    XPLMRegisterFlightLoopCallback(TimerCallback, 1.0f, NULL);
    XPLMRegisterFlightLoopCallback(TimerCallbackBlink, 0.5f, NULL);
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
    navMap->setScale(2.0, 1.0);

    navMap->start(gTexWidth, gTexHeight);
    navMap->setRect(GLfloat(gTexLext), GLfloat(gTexBottom), GLfloat(gTexWidth), GLfloat(gTexHeight));
}

PLUGIN_API void XPluginDisable(void)
{
    navMap->stop();
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMessage, void* inParam)
{
}

static int MyDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void* inRefcon)
{
    navMap->render();
    return 1;
}

static double getDistanceToNearestAiroport()
{
    float planeLon = XPLMGetDataf(posLongitude);
    float planeLat = XPLMGetDataf(posLatitude);

    float airportLon = planeLon;
    float airportLat = planeLat;

    XPLMNavRef ref = XPLMFindNavAid(NULL, NULL, &airportLat, &airportLon, NULL, xplm_Nav_Airport);

    if (ref == XPLM_NAV_NOT_FOUND)
    {
        return 10000000;
    }

    XPLMGetNavAidInfo(ref, NULL, &airportLat, &airportLon, NULL, NULL, NULL, NULL, NULL, NULL);

    float diffLon = abs(airportLon - planeLon);
    float diffLat = abs(airportLat - planeLat);

    float dist = sqrt(diffLon*diffLon + diffLat*diffLat);

    // 1 degree is 111.32 km

    return dist * 111.32;
}

static float TimerCallback(
    float                inElapsedSinceLastCall,
    float                inElapsedTimeSinceLastFlightLoop,
    int                  inCounter,
    void *               inRefcon)
{
    // update plane position on the map
    navMap->setPlane(XPLMGetDataf(posLongitude), XPLMGetDataf(posLatitude), XPLMGetDataf(posMagPsi));

    LED_NearestAiroport = getDistanceToNearestAiroport() < 100.0;

    return 0.2f;
}

static float TimerCallbackBlink(
    float                inElapsedSinceLastCall,
    float                inElapsedTimeSinceLastFlightLoop,
    int                  inCounter,
    void *               inRefcon)
{
    //fprintf(logFile, "getDistanceToNearestAiroport = %f\n", getDistanceToNearestAiroport());
    //fflush(logFile);

    if (LED_NearestAiroport)
        LED_State_NearestAiroport = !LED_State_NearestAiroport;
    else
        LED_State_NearestAiroport = false;

    return 0.5f;
}

static float MyGetDistanceToNearestAiroport(void* inRefcon)
{
    return (float)getDistanceToNearestAiroport();
}

int MyGetLEDNearestAiroport(void* inRefcon)
{
    return LED_State_NearestAiroport;
}
