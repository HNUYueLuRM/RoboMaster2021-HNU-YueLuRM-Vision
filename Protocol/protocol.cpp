/*
	@SEASKY---2020/09/05
*/

/*
    kiko@idiospace.com --- 2021.01
*/

#include "protocol.h"
#include "./CRC/bsp_crc8.h"
#include "./CRC/bsp_crc16.h"
#include <cstring>

namespace hnurm 
{

/*
@param: Vision_send_data  
    int cam_id;     // formatted in lower 8 bits of flag_register, higher 8 bits for futher exploration
    float pitch;    // formatted in float data 
    float yaw;      // formatted in float data
    float distance; // formatted in float data
    
@return: formatted std::string
        which will then be send to STM32 through Serial::send(const std::string& s)
*/
std::string Protocol::encode(const Vision_send_data& data)
{
    static char ch_arr[32];
    static float f_arr[8]; 
    static uint16_t tx_buf_len;  
    static std::string s;        //storation of modes

    f_arr[0] = data.pitch, f_arr[1] = data.yaw, f_arr[2] = data.distance;

    get_protocol_send_data_vision(0x0001,                 // vision data identifier
                                  data.cam_id & 0xff,     // camera id
                                  f_arr,                  // start pos of float array
                                  3,                      // pitch, yaw, distance
                                  (unsigned char*)ch_arr, // start pos of formatted send buffer
                                  &tx_buf_len);           // get length of valid characters in the send buffer
    s.resize(tx_buf_len);
    for(unsigned i = 0; i < s.size(); ++i) s[i] = ch_arr[i];
    return s;
}




/*
@param: std::string

@return: Vision_recv_data
    1-4 bits: self_color Protocol::Self_color        // formatted in flag_register, low  1-4 bits 
    5-8 bits: actuator_id int                        // formatted in flag_register, high 5-8 bits
    9-12 bits: mode Protocol::Work_mode              // formatted in flag_register, low  9-12 bits 
    13-16 bits: tracker_mode Protocal::Tracker_mode  // formatted in flag_register, low  13-16 bits
*/
bool Protocol::decode(const std::string& s,Vision_recv_data & decoded_data)
{
    static char ch_arr[32];
    static uint16_t msg;
    static float f_arr[8];  // for futher exploration, now receieve no float data from STM32
    s.copy(ch_arr, s.size());

    get_protocol_info_vision((uint8_t*)(&ch_arr[0]),  // start pos of recieved data string
                             &msg,                    // msg = (self_color: 4bits, actuator_id: 4bits) | (work_mode: 4bits | trace_mode: 4bits)
                             f_arr                    // store the float data sent from STM32, currently no data
                            );
    
//    while (msg & 0xf != 0B0101 && msg &0xf != 0B1010)
//    {
//        std::cerr << "[ERROR] decode self_color failure !" << std::endl;
//    }
    decoded_data=  Vision_recv_data(static_cast<Self_color>(msg & 0xf),           // self_color
                             (msg >> 4) & 0xf,                            // actuator_id
                            static_cast<Work_mode>((msg >> 8) & 0xf),     // mode: 0 manual, 1 auto_shoot, 2 auto_windmill, 3 auto_save, 4 auto_grab
                            static_cast<Tracker_mode>((msg >> 12) & 0xf));// trace: 0 off, 1 on
    return true;
}

void Protocol::get_protocol_send_data_vision
(uint16_t send_id,	 	       
 uint16_t flags_register,      
 float    *tx_data,		 
 uint8_t  float_length,        
 uint8_t  *tx_buf,		
 uint16_t *tx_buf_len)	      
{
    uint16_t crc16;
    uint16_t data_len;

    data_len = float_length * 4 + 2;

  
    tx_buf[0] = PROTOCOL_CMD_ID;            
    tx_buf[1] = data_len & 0xff;		      
    tx_buf[2] = (data_len >> 8) & 0xff;       
    tx_buf[3] = Get_CRC8_Check(&tx_buf[0],3);                                              
    tx_buf[4] = send_id & 0xff;      
    tx_buf[5] = (send_id >> 8) & 0xff;   
    tx_buf[6] = flags_register & 0xff;
    tx_buf[7] = (flags_register >> 8) & 0xff;

    for(int i=0; i<4*float_length; i++)
    {
        //first cast float to char,then read the content in float
        tx_buf[i+8]  = ((uint8_t*)(&tx_data[i/4]))[i%4];
        //tx_buf[i + 8] = ((uint8_t*)(&tx_data[i >> 2]))[i % 4];
    }
    // memcpy(tx_buf+8, tx_data, sizeof(tx_data));
    crc16 = Get_CRC16_Check(&tx_buf[0], data_len+6);
    tx_buf[data_len + 6] = crc16 & 0xff;
    tx_buf[data_len + 7] = (crc16 >> 8) & 0xff;

    *tx_buf_len = data_len + 8;
}


uint16_t Protocol::get_protocol_info_vision 
(uint8_t  *rx_buf,			
 uint16_t *flags_register,	 
 float    *rx_data)		
{
	static protocol pro;
	static uint16_t date_length;
    if(protocol_head_check_vision(&pro, rx_buf))                     
    {
        date_length = OFFSET_BYTE + pro.header.data_length;          
        while(CRC16_Check_Sum(&rx_buf[0], date_length))           
        {
            *flags_register = (rx_buf[7] << 8) | rx_buf[6];           
            for(int i=0; i < ((pro.header.data_length - 2) >> 2); i++)
            {
                rx_data[i] = float_protocol(&rx_buf[8+4*i]);            
            }
            for(int i=0; i < date_length; i++)
            {
                rx_buf[i] = 0;                                     
            }
            return pro.cmd_id;                               
        }
    }
    return false;
}

uint8_t Protocol::Get_CRC8_Check(uint8_t *pchMessage, uint16_t dwLength)
{
    return crc_8(pchMessage,dwLength);
}

uint8_t Protocol::CRC8_Check_Sum(uint8_t *pchMessage, uint16_t dwLength)
{
    uint8_t ucExpected = 0;
    if ((pchMessage == 0) || (dwLength <= 2)) return 0;
    ucExpected = Get_CRC8_Check(pchMessage, dwLength-1);
    return ( ucExpected == pchMessage[dwLength-1] );
}

uint16_t Protocol::Get_CRC16_Check(uint8_t *pchMessage, uint32_t dwLength)
{
    return crc_16(pchMessage,dwLength);
}

uint16_t Protocol::CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
    uint16_t  wExpected = 0;
    if ((pchMessage == 0) || (dwLength <= 2))
        {
            return 0;
        }
    wExpected = Get_CRC16_Check ( pchMessage, dwLength - 2);
    return (((wExpected & 0xff) == pchMessage[dwLength - 2] )&& (((wExpected >> 8) & 0xff) == pchMessage[dwLength - 1]));
}

uint8_t Protocol::protocol_head_check_vision(
    protocol *pro,
    uint8_t  *rx_buf)
{
    if(rx_buf[0] == PROTOCOL_CMD_ID)
    {
        pro->header.sof = rx_buf[0];
        if(CRC8_Check_Sum(&rx_buf[0], 4))
        {
            pro->header.data_length = (rx_buf[2] << 8) | rx_buf[1];
            pro->header.crc_check = rx_buf[3];
            pro->cmd_id = (rx_buf[5] << 8) | rx_buf[4];
            return  1;
        }
    }
    return 0;
}


float Protocol::float_protocol(uint8_t *dat_t)
{
    uint8_t f_data[4];
    f_data[0] = *(dat_t+0);
    f_data[1] = *(dat_t+1);
    f_data[2] = *(dat_t+2);
    f_data[3] = *(dat_t+3);
    return *(float*)f_data;
}

}

