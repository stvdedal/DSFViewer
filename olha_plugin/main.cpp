#include "include_gl.h"

#include <XPLMPlugin.h>
#include <XPLMDisplay.h>
#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>
#include <XPLMGraphics.h>
#include <XPLMProcessing.h>


#include "conf.h"
#include "log.h"
#include "map_tablet.h"

static olha::MapTablet* mapTablet;

static int MyDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void* inRefcon);

PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc)
{
    strcpy(outName, PLUGIN_NAME);
    strcpy(outSig, PLUGIN_SIG);
    strcpy(outDesc, PLUGIN_DESC);

    openLog();

    mapTablet = new olha::MapTablet;

    XPLMRegisterDrawCallback(MyDrawCallback, xplm_Phase_Gauges, 0, NULL);
    log("%s:%d\n", __func__, __LINE__);
    return 1;
}

PLUGIN_API void	XPluginStop(void)
{
    log("%s:%d\n", __func__, __LINE__);
    XPLMUnregisterDrawCallback(MyDrawCallback, xplm_Phase_Gauges, 0, NULL);
    delete mapTablet;
    closeLog();
}

PLUGIN_API int XPluginEnable(void)
{
    log("%s:%d\n", __func__, __LINE__);
    return 1;
}

PLUGIN_API void XPluginDisable(void)
{
    log("%s:%d\n", __func__, __LINE__);
    mapTablet->off();
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMessage, void* inParam)
{
    log("%s:%d\tinFrom=%d inMessage=%d\n", __func__, __LINE__, inFrom, inMessage);

    if (inFrom == 0 && inMessage == XPLM_MSG_AIRPORT_LOADED) {
        mapTablet->on();
    }
}

static int MyDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void* inRefcon)
{
    mapTablet->render();
    return 1;
}
