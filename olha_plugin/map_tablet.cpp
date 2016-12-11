#include <XPLMGraphics.h>
#include <XPLMNavigation.h>

#include "map_tablet.h"
#include "conf.h"
#include "log.h"

#include <dsf_map.h>
#include <simple_marker.h>

#include <cmath>

namespace olha
{

    IMarkerRender* MapTablet::DsfNavMapThread::createMarkerRender()
    {
        return new SimpleMarker;
    }

    IMapRender* MapTablet::DsfNavMapThread::createMapRender()
    {
        DsfMap* dsfMap = new DsfMap;
        dsfMap->setDsfDirectory(DSF_ROOT_DIR);
        dsfMap->setTmpDirectory(TMP_DIR);
        return dsfMap;
    }

    MapTablet::DsfNavMapThread::DsfNavMapThread()
    {
        XPLMGenerateTextureNumbers(&_textId, 1);
        XPLMBindTexture2d(_textId, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    MapTablet::DsfNavMapThread::~DsfNavMapThread()
    {
        GLuint id = _textId;
        glDeleteTextures(1, &id);
    }

    void MapTablet::DsfNavMapThread::render()
    {
        XPLMBindTexture2d(_textId, 0);

        _texture.guard.lock();
        if (!_texture.dirty)
            glTexImage2D(GL_TEXTURE_2D, 0, _texture.format, _texture.width, _texture.height, 0, _texture.format, GL_UNSIGNED_BYTE, _texture.buf.data());
        _texture.guard.unlock();

        XPLMSetGraphicsState(
            0,        // No fog, equivalent to glDisable(GL_FOG);
            1,        // One texture, equivalent to glEnable(GL_TEXTURE_2D);
            0,        // No lighting, equivalent to glDisable(GL_LIGHT0);
            1,        // No alpha testing, e.g glDisable(GL_ALPHA_TEST);
            1,        // Use alpha blending, e.g. glEnable(GL_BLEND);
            0,        // No depth read, e.g. glDisable(GL_DEPTH_TEST);
            0);       // No depth write, e.g. glDepthMask(GL_FALSE);

        glColor3f(1, 1, 1);        // Set color to white.
        GLfloat x1 = MAP_TEXTURE_LEFT;
        GLfloat y1 = MAP_TEXTURE_BUTTOM;
        GLfloat x2 = x1 + MAP_TEXTURE_WIDTH;
        GLfloat y2 = y1 + MAP_TEXTURE_HEIGHT;
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0, 0);        glVertex2f(x1, y1);
        glTexCoord2f(0, 1);        glVertex2f(x1, y2);
        glTexCoord2f(1, 1);        glVertex2f(x2, y2);
        glTexCoord2f(1, 0);        glVertex2f(x2, y1);
        glEnd();
    }

    float MapTablet::flightDataUpdater(
        float   inElapsedSinceLastCall,
        float   inElapsedTimeSinceLastFlightLoop,
        int     inCounter,
        void*   inRefcon)
    {
        MapTablet* self = reinterpret_cast<MapTablet*>(inRefcon);

        if (!self->_status_on)
            return 0.0f;

        self->updateNavMap();
        self->_ledStatusAiroportApproachEnable = self->getDistanceToNearestAiroport() < 100.0;
        return 0.2f;
    }

    void MapTablet::updateNavMap()
    {
        _plane_lon = XPLMGetDataf(_drefPosLongitude);
        _plane_lat = XPLMGetDataf(_drefPosLatitude);
        _plane_hdg = XPLMGetDataf(_drefPosMagPsi);

        if (_map_mode == MAPMODE_AUTO)
        {
            if (_navMap.isMarkerOutOfBorder())
            {
                _map_lon = _plane_lon;
                _map_lat = _plane_lat;
            }
        }

        _navMap.setMapScale(_map_scale_x, _map_scale_y);
        _navMap.setMap(_map_lon, _map_lat);
        _navMap.setMarker(_plane_lon, _plane_lat, _plane_hdg);
    }

    float MapTablet::ledStatusUpdater(
        float   inElapsedSinceLastCall,
        float   inElapsedTimeSinceLastFlightLoop,
        int     inCounter,
        void*   inRefcon)
    {
        MapTablet* self = reinterpret_cast<MapTablet*>(inRefcon);

        double distanceToNearestAiroport = self->getDistanceToNearestAiroport();
        if (distanceToNearestAiroport < 100.0)
        {
            if (distanceToNearestAiroport < 45.0)
                self->_ledStatusAiroportApproach = !self->_ledStatusAiroportApproach; // blinking
            else
                self->_ledStatusAiroportApproach = true;

            if (self->_mode == MODE_AIROPORT)
                self->_ledStatusAiroportApproach = true;
        }
        else
            self->_ledStatusAiroportApproach = false;

        //log("ledStatusUpdater: distanceToNearestAiroport=%f led=%d\n", distanceToNearestAiroport, self->_ledStatusAiroportApproach);
        return 0.5;
    }

    int MapTablet::getLedStatusNearestAiroport(void* inRefcon)
    {
        MapTablet* self = reinterpret_cast<MapTablet*>(inRefcon);
        return self->_ledStatusAiroportApproach;
    }

    int MapTablet::getDataRefFlightMode(void* inRefcon)
    {
        MapTablet* self = reinterpret_cast<MapTablet*>(inRefcon);
        return self->_mode;
    }

    void MapTablet::setDataRefFlightMode(void* inRefcon, int inValue)
    {
        MapTablet* self = reinterpret_cast<MapTablet*>(inRefcon);
        self->_mode = static_cast<Mode>(inValue);
        self->calcMapScale();
    }

    double MapTablet::getDistanceToNearestAiroport()
    {
        float airportLon = float(_plane_lon);
        float airportLat = float(_plane_lat);
        XPLMNavRef ref = XPLMFindNavAid(NULL, NULL, &airportLat, &airportLon, NULL, xplm_Nav_Airport);
        if (ref == XPLM_NAV_NOT_FOUND)
            return 10000000;
        XPLMGetNavAidInfo(ref, NULL, &airportLat, &airportLon, NULL, NULL, NULL, NULL, NULL, NULL);
        double diffLon = abs(airportLon - _plane_lon);
        double diffLat = abs(airportLat - _plane_lat);
        double dist = sqrt(diffLon*diffLon + diffLat*diffLat);
        // 1 degree is 111.32 km
        return dist * 111.32;
    }

    void MapTablet::calcMapScale()
    {
        double sc = 1.0;
        switch (_mode)
        {
        case MODE_OVERVIEW:
            sc = 3000000.0;
            break;
        case MODE_ROUTE:
            sc = 2000000.0;
            break;
        case MODE_AIROPORT:
            sc = 500000.0;
            break;
        }

        _map_scale_x = ((sc * MAP_WIDTH_CM) / 100000.0) / 111.32;
        _map_scale_y = ((sc * MAP_HEIGHT_CM) / 100000.0) / 111.32;
    }

    MapTablet::MapTablet()
    {
        _status_on = false;
        _mode = MODE_AIROPORT;
        _map_mode = MAPMODE_AUTO;

        _ledStatusAiroportApproachEnable = false;
        _ledStatusAiroportApproach = false;

        _drefPosLongitude = XPLMFindDataRef("sim/flightmodel/position/longitude");
        _drefPosLatitude = XPLMFindDataRef("sim/flightmodel/position/latitude");
        _drefPosMagPsi = XPLMFindDataRef("sim/flightmodel/position/magpsi");

        _drefLEDNearestAiroportApproach = XPLMRegisterDataAccessor(
            DREF_LED_NEAREST_AIROPORT_APPROACH,
            xplmType_Int,
            0,                              // inIsWritable
            getLedStatusNearestAiroport,    // inReadInt
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
            this,                           // inReadRefcon
            NULL                            // inWriteRefcon
        );

        _drefFlightMode = XPLMRegisterDataAccessor(
            DREF_MAP_TABLET_MODE,
            xplmType_Int,
            true,                           // inIsWritable
            getDataRefFlightMode,           // inReadInt
            setDataRefFlightMode,           // inWriteInt
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
            this,                           // inReadRefcon
            this                            // inWriteRefcon
        );

        _plane_lon = 0.0;
        _plane_lat = 0.0;
        _plane_hdg = 0.0;

        _map_lon = 0.0;
        _map_lat = 0.0;

        _map_scale_x = 0.2;
        _map_scale_y = 0.2;

        calcMapScale();

        XPLMCreateFlightLoop_t info;
        info.structSize = sizeof(info);
        info.phase = xplm_FlightLoop_Phase_AfterFlightModel;
        info.callbackFunc = flightDataUpdater;
        info.refcon = this;
        _flightDataUpdaterId = XPLMCreateFlightLoop(&info);

        info.callbackFunc = ledStatusUpdater;
        info.refcon = this;
        _ledStatusUpdaterId = XPLMCreateFlightLoop(&info);
    }

    MapTablet::~MapTablet()
    {
        _status_on = false;
        XPLMDestroyFlightLoop(_ledStatusUpdaterId);
        XPLMDestroyFlightLoop(_flightDataUpdaterId);
    }

    void MapTablet::on()
    {
        if (_status_on)
            return;

        _status_on = true;

        _plane_lon = XPLMGetDataf(_drefPosLongitude);
        _plane_lat = XPLMGetDataf(_drefPosLatitude);
        _plane_hdg = XPLMGetDataf(_drefPosMagPsi);

        _map_lon = _plane_lon;
        _map_lat = _plane_lat;

        _navMap.setMap(_map_lon, _plane_lat);
        _navMap.setMapScale(_map_scale_x, _map_scale_y);
        _navMap.setMarker(_plane_lon, _plane_lat, _plane_hdg);
        _navMap.setMarkerScale(0.1, 0.1);

        _navMap.start(MAP_TEXTURE_WIDTH, MAP_TEXTURE_HEIGHT);

        XPLMScheduleFlightLoop(_flightDataUpdaterId, 1, true);
        XPLMScheduleFlightLoop(_ledStatusUpdaterId, 1, true);
    }

    void MapTablet::off()
    {
        _status_on = false;
        XPLMScheduleFlightLoop(_flightDataUpdaterId, 0, true);
        XPLMScheduleFlightLoop(_ledStatusUpdaterId, 0, true);
        _navMap.stop();
    }

    void MapTablet::lighting(double value)
    {

    }

    void MapTablet::render()
    {
        _navMap.render();
    }

}
