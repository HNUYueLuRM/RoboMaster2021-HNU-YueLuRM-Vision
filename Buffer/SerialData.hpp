/*kiko@idiospace.com 2020.01*/

#ifndef SERIALDATA_HPP
#define SERIALDATA_HPP

namespace hnurm {

class SerialData
{
private:

typedef unsigned char uint8_t;

public:
    uint8_t camera_id;
    float pitch;
    float yaw;
    float distance;

public:
    SerialData():
        camera_id(0), pitch(0), yaw(0), distance(0)
    {
        
    }
    SerialData(uint8_t cam_id, float pitch_, float yaw_, float distance_):
        camera_id(cam_id), pitch(pitch_), yaw(yaw_), distance(distance_)
    {
    }
    void set(uint8_t cam_id, float pitch_, float yaw_, float distance_)
    {
        camera_id = cam_id;
        pitch = pitch_;
        yaw = yaw_;
        distance = distance_;
    }

};

}

#endif
