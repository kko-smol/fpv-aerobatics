#include "telemetryreader.h"
#include <iostream>

TelemetryReader::TelemetryReader()
{
    _lastTime    =0;
    _lastLon     =0;
    _lastLat     =0;
    _lastAlt     =0;
    _lastRoll    =0;
    _lastPitch   =0;
    _lastHeading =0;
}

void TelemetryReader::onBufRx(std::vector<uint8_t> buf)
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

void TelemetryReader::subscribe(TelemetryListenerPtr s)
{
    _clients.push_back(s);
}

void TelemetryReader::unsubscribe(TelemetryListenerPtr s)
{
    _clients.remove(s);
}

uint64_t TelemetryReader::lastTime() const
{
    return _lastTime;
}

double TelemetryReader::lastLon() const
{
    return _lastLon;
}

double TelemetryReader::lastLat() const
{
    return _lastLat;
}

double TelemetryReader::lastAlt() const
{
    return _lastAlt;
}

double TelemetryReader::lastRoll() const
{
    return _lastRoll;
}

double TelemetryReader::lastPitch() const
{
    return _lastPitch;
}

double TelemetryReader::lastHeading() const
{
    return _lastHeading;
}

void TelemetryReader::readMessage(mavlink_message_t* msg)
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

            break;
        }

        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
        {
            mavlink_global_position_int_t pos;
            mavlink_msg_global_position_int_decode(msg, &pos);

            _lastAlt = pos.alt;
            _lastLat = pos.lat;
            _lastLon = pos.lon;

            break;
        }

        default:
            break;
        }
    }
}

void TelemetryReader::sendMessage(mavlink_message_t* msg)
{
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    int len = mavlink_msg_to_send_buffer(buffer, msg);
    std::vector<uint8_t> data;
    data.resize(len);
    memcpy(data.data(),buffer,len);
    write(data);
}

void TelemetryReader::onGotData(const std::vector<uint8_t> &buf)
{
    onBufRx(buf);
}
