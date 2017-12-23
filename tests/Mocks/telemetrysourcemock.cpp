#include "telemetrysourcemock.h"

#include <gis/CorrdsConverter.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

TelemetryReaderMock::TelemetryReaderMock(TrackPtr track)
{
    _lastTime    =0;
    _lastLon     =0;
    _lastLat     =0;
    _lastAlt     =0;
    _lastRoll    =0;
    _lastPitch   =0;
    _lastHeading =0;

    auto s = track->trackBase();

    auto ll = CoordsConverter::toLongLat(s);
    _lastLon = ll.x;
    _lastLat = ll.y;
    _lastAlt = ll.z;
}

void TelemetryReaderMock::onBufRx(std::vector<uint8_t> buf)
{
    mavlink_message_t message;
    mavlink_status_t status;

      for (int position = 0; position < buf.size(); position++) {
          unsigned int decodeState = mavlink_parse_char(MAVLINK_COMM_0, (uint8_t)(buf[position]), &message, &status);

          if (decodeState == 1)
          {
              readMessage(&message);
          }
      }
}

void TelemetryReaderMock::subscribe(TelemetryListenerPtr s)
{
    _clients.push_back(s);
}

void TelemetryReaderMock::unsubscribe(TelemetryListenerPtr s)
{
    _clients.remove(s);
}

uint64_t TelemetryReaderMock::lastTime() const
{
    return _lastTime;
}

double TelemetryReaderMock::lastLon() const
{
    return _lastLon;
}

double TelemetryReaderMock::lastLat() const
{
    return _lastLat;
}

double TelemetryReaderMock::lastAlt() const
{
    return _lastAlt;
}

double TelemetryReaderMock::lastRoll() const
{
    return _lastRoll;
}

double TelemetryReaderMock::lastPitch() const
{
    return _lastPitch;
}

double TelemetryReaderMock::lastHeading() const
{
    return _lastHeading;
}

void TelemetryReaderMock::readMessage(mavlink_message_t* msg)
{
    {
        _sysId = msg->sysid;
        _compId = msg->compid;

        switch (msg->msgid)
        {
        case MAVLINK_MSG_ID_HEARTBEAT:
        {
            mavlink_heartbeat_t state;

            mavlink_msg_heartbeat_decode(msg, &state);

            mavlink_heartbeat_t st;
            st.autopilot = MAV_AUTOPILOT_INVALID;
            st.base_mode = 0;
            st.custom_mode = 0;
            st.mavlink_version = state.mavlink_version;
            st.system_status = MAV_STATE::MAV_STATE_ACTIVE;
            st.type = MAV_TYPE_GCS;

            mavlink_message_t hb_msg;
            mavlink_msg_heartbeat_encode(0,0,&hb_msg,&st);
            sendMessage(&hb_msg);

            mavlink_request_data_stream_t rq;
            rq.target_system = msg->sysid;
            rq.target_component = msg->compid;
            rq.req_message_rate = 25;
            rq.req_stream_id = MAV_DATA_STREAM_EXTRA1;
            rq.start_stop = 1;
            mavlink_msg_request_data_stream_encode(0,0,&hb_msg,&rq);
            sendMessage(&hb_msg);
            break;
        }

        case MAVLINK_MSG_ID_ATTITUDE:
        {
            mavlink_attitude_t attitude;
            mavlink_msg_attitude_decode(msg, &attitude);

            _lastRoll = 180.0*attitude.roll/M_PI;
            _lastPitch = 180.0*attitude.pitch/M_PI;
            _lastHeading = 180.0*attitude.yaw/M_PI;

            //std::cout << "GonAngles:" << _lastHeading << " " << _lastPitch << std::endl;

            break;
        }

        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
        {
            mavlink_global_position_int_t pos;
            mavlink_msg_global_position_int_decode(msg, &pos);

            auto now = std::chrono::high_resolution_clock::now();

            float dt = std::chrono::duration_cast<std::chrono::milliseconds>(now-_lt).count();
            if (dt>1000){
                dt = 0.0;
            }
            _lt = now;

            float speed = 10.0f*dt/1000.0f;

            glm::mat4 tr = glm::rotate(glm::mat4(1.0f),glm::radians(-(float)_lastRoll),glm::vec3   (0.0f,1.0f,0.0f));
            glm::mat4 tp = glm::rotate(glm::mat4(1.0f),glm::radians(-(float)_lastPitch),glm::vec3  (1.0f,0.0f,0.0f));
            glm::mat4 th = glm::rotate(glm::mat4(1.0f),glm::radians(-(float)_lastHeading),glm::vec3(0.0f,0.0f,1.0f));
            glm::mat4 tm = th*tp*tr;
            glm::vec4 vv(0.0,speed,0.0,0.0);

            glm::vec3 from = CoordsConverter::toMercator(glm::vec3(_lastLon,_lastLat,_lastAlt));
            glm::vec3 to = from-glm::vec3(tm*vv);

            glm::vec3 np = CoordsConverter::toLongLat(to);

            //std::cout << "Angles:" << _lastRoll << " " << _lastPitch << " " << _lastHeading <<  std::endl;
            //std::cout << "IV" << np.x << "," << np.y << "," << np.z << std::endl;
            //std::cout << "OV" << rv.x << "," << rv.y << "," << rv.z << std::endl << std::endl;

            _lastAlt = np.z;
            _lastLat = np.y;
            _lastLon = np.x;

            break;
        }

        default:
            break;
        }
    }
}

void TelemetryReaderMock::sendMessage(mavlink_message_t* msg)
{
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    int len = mavlink_msg_to_send_buffer(buffer, msg);
    std::vector<uint8_t> data;
    data.resize(len);
    memcpy(data.data(),buffer,len);
    write(data);
}

void TelemetryReaderMock::onGotData(const std::vector<uint8_t> &buf)
{
    onBufRx(buf);
}
