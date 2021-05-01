
#include "mongoose.h"

#include "XPLMProcessing.h"
#include "XPLMPlugin.h"
#include "XPLMMenus.h"
#include "XPLMUtilities.h"

#include "kmlclient.h"

using namespace std;

PLUGIN_API float flightloopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void * inRefcon) {
    if (inRefcon)
        return ((KmlClient*)inRefcon)->flightloop(inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter, inRefcon);
    return 0;
}


static void ev_handler(struct mg_connection* conn, int ev, void* p) {
    if (ev == MG_EV_HTTP_REQUEST) {
        struct http_message* hm = (struct http_message*) p;
        KmlClient* kc = (KmlClient*)conn->user_data;

        float lon = XPLMGetDataf(kc->mCurrentLongitude);
        float lat = XPLMGetDataf(kc->mCurrentLatitude);
        float alt = XPLMGetDataf(kc->mCurrentAltitude) + kc->height;
        float hdg = XPLMGetDataf(kc->mCurrentHeading);
        float roll = -XPLMGetDataf(kc->mCurrentRoll);
        float pitch = XPLMGetDataf(kc->mCurrentPitch) + kc->tilt;
        if (kc->is_looking_around()) {
            hdg += XPLMGetDataf(kc->mPilotHeadPsi);
            if (hdg < 0.0f) hdg += 360.0f;
            if (hdg > 360.0f) hdg -= 360.0f;
            roll -= XPLMGetDataf(kc->mPilotHeadPhi);
            pitch += XPLMGetDataf(kc->mPilotHeadTheta);
        }

        // We have received an HTTP request. Parsed request is contained in `hm`.
        char content[2048];
        _snprintf_s(content, sizeof(content), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<kml xmlns=\"http://earth.google.com/kml/2.2\"> \
            <Document> \
            <Camera> \
            <altitudeMode>absolute</altitudeMode> \
            <longitude>%f</longitude > \
            <latitude>%f</latitude > \
            <altitude>%f</altitude > \
            <heading>%f</heading > \
            <roll>%f</roll > \
            <tilt>%f</tilt > \
            </Camera> \
            </Document> \
            </kml>\r\n", lon, lat, alt, hdg, roll, pitch);

        mg_send_head(conn, 200, (int)strlen(content), "Content-type: application/vnd.google-earth.kml+xml");
        mg_printf(conn, "%.*s", (int)strlen(content), content);
    }
}

KmlClient::KmlClient() :
    mCurrentLatitude(XPLMFindDataRef("sim/flightmodel/position/latitude")),
    mCurrentLongitude(XPLMFindDataRef("sim/flightmodel/position/longitude")),
    mCurrentAltitude(XPLMFindDataRef("sim/flightmodel/position/elevation")),
    mCurrentHeading(XPLMFindDataRef("sim/flightmodel/position/psi")),
    mCurrentPitch(XPLMFindDataRef("sim/flightmodel/position/theta")),
    mCurrentRoll(XPLMFindDataRef("sim/flightmodel/position/phi")),
    mPilotHeadPhi(XPLMFindDataRef("sim/graphics/view/pilots_head_phi")),
    mPilotHeadPsi(XPLMFindDataRef("sim/graphics/view/pilots_head_psi")),
    mPilotHeadTheta(XPLMFindDataRef("sim/graphics/view/pilots_head_the")),
    mDelay(0.05f),
    mgr(),
    conn(),
    enabled(false),
    height(),
    tilt(),
    look_around(false)
{
    trace("xp2earth enable\n");

    mg_mgr_init(&mgr, NULL);
    conn = mg_bind(&mgr, s_http_port, ev_handler);
    if (conn != NULL) {
        conn->user_data = this;
        mg_set_protocol_http_websocket(conn);
    }
    XPLMRegisterFlightLoopCallback(flightloopCallback, mDelay, this);
}

KmlClient::~KmlClient() {
    trace("xp2earth disable\n");
    if (enabled) {
        stop_tracking();
    }
    XPLMUnregisterFlightLoopCallback(flightloopCallback, this);
    mg_mgr_free(&mgr);
    trace("xp2earth disabled\n");
}

void KmlClient::trace(const char* msg) {
    XPLMDebugString(msg);
}

void KmlClient::start_tracking() {
    trace("xp2earth start_tracking\n");
    enabled = true;
}

void KmlClient::stop_tracking() {
    trace("xp2earth stop_tracking\n");
    enabled = false;
}

void KmlClient::connect_earth() {
    trace("xp2earth connect_earth\n");
    XPLMPluginID id = XPLMGetMyID();
    char filepath[MAX_PATH]; 
    XPLMGetSystemPath(filepath);
    strcat(filepath, "Resources\\plugins\\xp2earth\\xp2earth.kml");
    trace(filepath);
    ShellExecuteA(NULL, "OPEN", filepath, NULL, NULL, SW_SHOWNORMAL);
}

bool KmlClient::isConnected() {
    return conn != NULL;
}

void KmlClient::set_view(float newHeight, float newTilt) {
    height = newHeight;
    tilt = newTilt;
}

void KmlClient::set_look_around_on()
{
    look_around = true;
}

void KmlClient::set_look_around_off()
{
    look_around = false;
}

bool KmlClient::is_looking_around()
{
    return look_around;
}


float KmlClient::flightloop(float, float, int, void *) {
    if (enabled) {
        mg_mgr_poll(&mgr, 0);
    }

    return mDelay;
}

