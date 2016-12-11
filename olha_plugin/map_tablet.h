#ifndef MAP_TABLET_H_
#define MAP_TABLET_H_

#include "include_gl.h"
#include <navmap_thread.h>

#include <XPLMDataAccess.h>
#include <XPLMProcessing.h>

namespace olha
{
    class MapTablet
    {
    public:
        enum Mode
        {
            MODE_OVERVIEW,
            MODE_ROUTE,
            MODE_AIROPORT
        };

        enum MapMode
        {
            MAPMODE_AUTO,
            MAPMODE_MANUAL
        };

    private:

        // on/off status
        bool _status_on;

        Mode _mode;
        MapMode _map_mode;

        XPLMDataRef _drefPosLongitude;
        XPLMDataRef _drefPosLatitude;
        XPLMDataRef _drefPosMagPsi;

        XPLMDataRef _drefFlightMode;
        XPLMDataRef _drefLEDNearestAiroportApproach;

        double _plane_lon;
        double _plane_lat;
        double _plane_hdg;

        double _map_lon;
        double _map_lat;

        double _map_scale_x;
        double _map_scale_y;

        void calcMapScale();

        class DsfNavMapThread : public NavMapThread
        {
            IMarkerRender* createMarkerRender();
            IMapRender*    createMapRender();
            int _textId;
        public:
            DsfNavMapThread();
            ~DsfNavMapThread();
            void render();
        } _navMap;

        XPLMFlightLoopID _flightDataUpdaterId;
        XPLMFlightLoopID _ledStatusUpdaterId;

        static float flightDataUpdater(
            float   inElapsedSinceLastCall,
            float   inElapsedTimeSinceLastFlightLoop,
            int     inCounter,
            void*   inRefcon);

        static float ledStatusUpdater(
            float   inElapsedSinceLastCall,
            float   inElapsedTimeSinceLastFlightLoop,
            int     inCounter,
            void*   inRefcon);

        void updateNavMap();

        bool _ledStatusAiroportApproachEnable;
        bool _ledStatusAiroportApproach;

        static int getLedStatusNearestAiroport(void* inRefcon);
        static int getDataRefFlightMode(void* inRefcon);
        static void setDataRefFlightMode(void* inRefcon, int inValue);

        double getDistanceToNearestAiroport();

    public:
        MapTablet();
        ~MapTablet();

        // device power on
        void on();

        // device power off
        void off();

        void lighting(double value);

        void render();
    };

}

#endif
