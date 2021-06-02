/* kiko@idiospace.com 2021.01 */

/* Serial API
*  1. int send(const std::string& s);
*  @Brief: Send 'send_msg' to serial port
*  @return: bytes sent to serial port (int Serial::serial_port)
*
*
*  2. void recv(std::string& s);
*  @Brief: Retrieve a at least one byte and store in string s
*  @return: none
*  @NOTICE: This method will block the process if reach no data!
*
*
*  3. bool try_recv_for(std::string& s, int milli_secs = 3);
*  @Brief:  wait up to mill_secs milliseconds (default 3 ms)
*  @return: if retrieved at least one byte, then return true, otherwise return false.
*  @Notice: This method does not block, good feature for games like RM.
*/


// C library
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h>    // Contains file controls like open(), fcntl(), O_RDWR
#include <errno.h>    // Error integer and strerror() function
#include <termios.h>  // Contains POSIX terminal control definitions
#include <unistd.h>   // write(), read(), close()
#include <string>     // Store the Msg
#include <iostream>
#include <chrono>

namespace hnurm 
{

class Serial
{
protected:
    using Ms = std::chrono::milliseconds;   // ms
    using clk = std::chrono::high_resolution_clock;  // clk

public:
    Serial(){}
    ~Serial(); 

    bool rm_init();                    // Set baud rate, parity, RTS/CTS, etc

 // None block by default
    int send(const std::string& s);     // Send 'send_msg' to serial port
    void recv(std::string& s);          // Retrieve at least one byte store in string s
    // Try to retrive str from serial port and store in s for up to milli_secs
    // return true if at least one byte has been retrieved, otherwise return false
    bool try_recv_for(std::string& s, int milli_secs);
    bool open_port(std::string dev_name);

private:
    int serial_port;                    // A file descriptor for the opened serial port
    struct termios tty;                 // Create new termios struct, call it 'tty' for convention
   
    void close_port();

    

    int raw_write(const std::string& s);// Write to STM32
    std::string raw_read();      // Read from STM32
    
    void clear_input_buffer();          // Clear read buffer
    void clear_output_buffer();         // Clear write buffer
    void clear_input_output_buffer();   // Clear input & output buffer  
    
};

}




