/* kiko@idispace.com 2021.01 */

//#define SERIAL_CODEC_DEBUG

/* SerialCodec API
 * 1. SerialCodec(int usb_id);
 * @Brief: open serial_port "/dev/ttyUSB(usb_id)"
 *
 * 2. SerialCode(std::string device_name);
 * @Brief: open serial_port "device_name"
 *
 * 3. bool try_get_recv_data_for(Protocol::Vision_recv_data& recv_data, int milli_secs = 10);
 * @Brief: try to retrieve data from serial port within mill_secs milliseconds
 * @return: if data has been fetched and decoded properly, then return true, otherwise return false
 * @NOTICE: this method will block the process for at most mill_secs milliseconds
 *
 * 4. bool send_data(int cam_id, float pitch, float yaw, float distance);
 * @Brief: encode and send data to the serial port
 * @return: true if data sent succeed
*/

#define MIN_DATA_LENGTH 
#define MAX_DATA_LENGTH

#include "../Protocol/protocol.h"
#include "serial.h"
#include <chrono>
#include <exception>

namespace hnurm
{

// Read & Write from or to the serial port
// Ensure integrity of receieved data pack thourgh this
// wrappered class combined with Serial and Protocol
class SerialCodec: public Serial, public Protocol
{
public:
    SerialCodec(){}

public:
    // may get broke msg or don't get enough msg to decode, then return false
    // if both retrieve data and decode work fine, then return true
    // by defualt wait for up to 10 milli-secs (tests shows that 3ms is enough to work)
    bool try_get_recv_data_for(Protocol::Vision_recv_data& recv_data,float &angle_data, int milli_secs = 10);

    // encode and send data to the serial port
    bool send_data(int cam_id, float pitch, float yaw, float distance);

    bool set_color(void);   //set color when initialize
    Protocol::Self_color get_color(void);

private:
    Protocol::Self_color my_color;  //used for init color
};



} // namespace hnurm


