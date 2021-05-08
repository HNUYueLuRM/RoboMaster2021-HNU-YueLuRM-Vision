/* kiko@idiospace.com 2020.01.20 */


#ifndef DataBuf_HPP
#define DataBuf_HPP

#include <mutex>
#include <chrono>
#include <vector>
#include <memory>
#include <exception>
#include<iostream>
namespace hnurm {


// Add time_stamp to the raw data type
//包裹类用于给每个数据类型增加时间戳，以标记这个数据的产出时间便于调试
//使用时可以直接包裹，而不必在每个数据类型中间增加时间戳这个成员变量，符合开发的复用性
template <typename RawDataType>
class Wrapped
{
public:
    RawDataType raw_data;
    short time_stamp;

public:
    Wrapped<RawDataType>& operator= (const Wrapped<RawDataType>& wrapped_data)
    {
        raw_data = wrapped_data.raw_data;
        time_stamp = wrapped_data.time_stamp;
        return *this;
    }
    Wrapped():
       time_stamp(clock())
    {}
    Wrapped(const RawDataType& data_, short time_stamp_=clock()):
        time_stamp(time_stamp_),
        raw_data(data_)
    {

    }
    ~Wrapped() = default;
    void wrap(RawDataType& raw_data_)
    {
        raw_data = raw_data_;
    }
private:
    static short clock_time;
    static short clock()
    {
        return ++clock_time;
    }
};

template <typename RawDataType>
short Wrapped<RawDataType>::clock_time = 0;


template <typename DataType>
class DataBuf
{
protected:
    using Ms = std::chrono::milliseconds;          //用于标记时间
    using validator = bool (*) (const DataType&);  //一个校验函数，用于检测数据的完整性和有效性

public:
    DataBuf(int size=16);
    ~DataBuf() = default;
    bool get(DataType& data, validator v=nullptr); //取出一个数据
    bool update(const DataType& data, validator v=nullptr); //更新buffer中的内容，往buffer中填入一个数据

private:
    std::vector<DataType> data_buf;       
    std::vector<std::timed_mutex> mtx_buf;  //向量中的每一个元素都对应着一把分时锁，从而使得读写分离，并且允许多个线程同时读写
    int head_idx, tail_idx;                //标记循环队列的头尾
    short latest_time_stamp;               //记录最后入队的数据的时间戳
};

template <class DataType>
DataBuf<DataType>::DataBuf(int size):
    head_idx(0),
    tail_idx(0),
    latest_time_stamp(0),
    data_buf(size),
    mtx_buf(size)
{

}

template <class DataType>
bool DataBuf<DataType>::get(DataType& data,validator v)
{

    try{
        const int cur_head_idx = head_idx;

        if (mtx_buf[cur_head_idx].try_lock_for(Ms(2)))  // lock succeed
        {
            if(data_buf.empty())
            {
                std::cout << "buffer empty----------------------------------------" << std::endl;
                return false;}
            // std::cout << "[get] lock succeed" << std::endl;
            if ((v == nullptr ? true: v(data_buf[cur_head_idx]))) // check data integrity if validator is given
            {
                // std::cout << "[get] data valid" << std::endl;
                if (data_buf[cur_head_idx].time_stamp != latest_time_stamp)  // data has not been used before
                {
                    latest_time_stamp = data_buf[cur_head_idx].time_stamp;
                    data = data_buf[cur_head_idx];
                    mtx_buf[cur_head_idx].unlock();
                    return true;
                }
            }
            mtx_buf[cur_head_idx].unlock();
        }
        else
        {
            // std::cout << "[get] failed to lock" << std::endl;
        }

        return false;
    } catch (std::exception& e)
    {
#ifdef DEBUG
        std::cerr << e.what() << std::endl;
#endif
    }
}



template <class DataType>
bool DataBuf<DataType>::update(const DataType& data, validator v)
{
    try{
        if (v!=nullptr && !v(data)) return false;

        const int new_head_idx = (head_idx + 1) % data_buf.size();

        if (mtx_buf[new_head_idx].try_lock_for(Ms(2)))  // lock succeed
        {
            data_buf[new_head_idx] = data;
            if (new_head_idx == tail_idx)
            {
                tail_idx = (tail_idx + 1) % data_buf.size();
            }
            head_idx = new_head_idx;              //？此处似乎会发生读写冲突？
            mtx_buf[new_head_idx].unlock();
            return true;
         }
        return false;
    } catch (std::exception& e) {
#ifdef DEBUG
        std::cerr << e.what() << std::endl;
#endif
    }
}


}
#endif // DataBuf_H

