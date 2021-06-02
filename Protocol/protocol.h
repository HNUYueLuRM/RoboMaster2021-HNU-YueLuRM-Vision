/* kiko@idiospace.com */

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

//#define __packed __attribute__((packed))

#include <string>
#include "stdint.h"  

namespace hnurm 
{

class Protocol 
{

protected:
    static short constexpr PROTOCOL_CMD_ID = 0XA5;
    static short constexpr OFFSET_BYTE = 8;

public: 
    //compulsory class enumerate
    enum class Work_mode: int { manual=0, auto_shoot=1, auto_tracker=2, auto_save=3, auto_grab=4 };
    enum class Self_color: int { red=0B0101, blue=0B1010 ,none=0};
    enum class Tracker_mode: int { on=1, off=0 };

    typedef struct
    {
        struct
        {
            uint8_t  sof;
            uint16_t data_length;
            uint8_t  crc_check;     
        } header;					
        uint16_t cmd_id;	 		
        uint16_t frame_tail; 		
    } protocol;

    struct Vision_send_data
    {
        //strut initiate function
        Vision_send_data(int cam_id_, float pitch_, float yaw_, float distance_):
            cam_id(cam_id_), pitch(pitch_), yaw(yaw_), distance(distance_)
        {
        }
        int cam_id;     // formatted in flag_register, low 8 bits  ||camera id(for engine car,there are two cameras)
        float pitch;
        float yaw;      // formatted in float data
        float distance; // formatted in float data
    };

    /* memory figure
    *
    * self_color | act_id
    *       work_mode
    *
    */

    struct Vision_recv_data
    {
        Vision_recv_data(Self_color self_color_, int actuator_id_, Work_mode mode_, Tracker_mode trace_flag_):
            self_color(self_color_), actuator_id(actuator_id_), mode(mode_), trace_flag(trace_flag_)
        {
        }
        Self_color self_color; // formatted in flag_register, lower 4 bits of low 8 bits
        int actuator_id;       // formatted in flag_register, higher 4 bits of low 8 bits
        Work_mode mode;        // formatted in flag_register, lower 4 bits of high 8 bits
        Tracker_mode trace_flag; // formatted in flag_register, higher 4 bits of high 8 bits
    };

    static std::string encode(const Vision_send_data&);   // Encode data from Vision_send_data to std::string 
    static bool decode(const std::string& s,Vision_recv_data& decoded_data); // Decode data from std::string to Vision_recv_data

private:

    static uint16_t get_protocol_info_vision (uint8_t  *rx_buf, uint16_t *flags_register, float *rx_data);

    static void get_protocol_send_data_vision (uint16_t send_id, uint16_t flags_register, float *tx_data,
                                        uint8_t float_length, uint8_t  *tx_buf,	uint16_t *tx_buf_len);	   

    static uint8_t Get_CRC8_Check(uint8_t *pchMessage, uint16_t dwLength);
    static uint8_t CRC8_Check_Sum(uint8_t *pchMessage, uint16_t dwLength);
    static uint16_t Get_CRC16_Check(uint8_t *pchMessage, uint32_t dwLength);
    static uint16_t CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
    static uint8_t protocol_head_check_vision(protocol *pro, uint8_t  *rx_buf);
    static float float_protocol(uint8_t *dat_t);

};

}
#endif
