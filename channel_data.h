/*
 * channel_data.h
 *
 *  Created on: 2021. márc. 20.
 *      Author: liptakok
 */

#ifndef DIADAT_CHANNEL_DATA_H_
#define DIADAT_CHANNEL_DATA_H_

#include <string>

class ChannelData
{
public:
    std::string chName;
    std::string unit;
    std::string storeType;
    std::string filename;
    std::string dataType;
    std::string recordCount;
    std::string channelIndex;
    std::string offset;
    std::string resolution;
    std::string min;
    std::string max;
    void init()
    {
        chName = "";
        unit = "";
        storeType = "";
        filename = "";
        dataType = "";
        recordCount = "";
        channelIndex = "";
        offset = "";
        resolution = "";
        min = "";
        max = "";
    }
    bool isOk()
    {
        return true;
    }
    bool isEmpty()
    {
        if (chName == "")
            return true;
        return false;
    }
};

#endif /* DIADAT_CHANNEL_DATA_H_ */
