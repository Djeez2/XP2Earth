#ifndef KMLCLIENT_H
#define KMLCLIENT_H

#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <iomanip>

#include "mongoose.h"

const float DEFAULT_HEIGHT = 20.0f;
const float DEFAULT_TILT = 80.0f;

class KmlClient
{
public:
    explicit KmlClient();
    virtual ~KmlClient();

    float flightloop(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void * inRefcon);
    void start_tracking();
    void stop_tracking();
    void connect_earth();
    bool isConnected();
    void set_pilot_view_on();
    void set_pilot_view_off();
    void set_look_around_on();
    void set_look_around_off();
    bool is_looking_around();

    float height;  // height above ground in meter
    float tilt;   // tilt angle when at zero pitch

    XPLMDataRef mCurrentLatitude, mCurrentLongitude, mCurrentAltitude, mCurrentHeading, mCurrentPitch, mCurrentRoll, mPilotHeadPsi, mPilotHeadPhi, mPilotHeadTheta;

private:
    const char* s_http_port = "8000";
    

    float mDelay;
    std::ofstream handle;
    bool enabled;
    bool look_around;

    struct mg_mgr mgr;
    struct mg_connection* conn;

    void trace(const char* msg);
};

#endif // KMLCLIENT_H
