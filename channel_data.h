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
    t_DiaDatFileStoreType storeType;
    std::string filename;
    std::string dataType;
    std::string recordCount;
    std::string channelIndex;
    std::string offset;
    std::string resolution;
    std::string min;
    std::string max;
    void clear()
    {
        chName.clear();
        unit.clear();
        storeType = e_DiaDatFileStoreType_Undef;
        filename.clear();
        dataType.clear();
        recordCount.clear();
        channelIndex.clear();
        offset.clear();
        resolution.clear();
        min.clear();
        max.clear();
    }
    bool isOk()
    {
        if (isEmpty())
            return false;
        if (chName.empty())
            return false;
        //if (unit.empty())
        //    return false;
        if (storeType == e_DiaDatFileStoreType_Undef)
            return false;
        if (filename.empty())
        {
            if (storeType != e_DiaDatFileStoreType_Implicit)
                return false;
        }
        if (dataType.empty())
            return false;
        if (recordCount.empty())
            return false;
        if (channelIndex.empty())
        {
            if (storeType != e_DiaDatFileStoreType_Implicit)
                return false;
        }
        if (offset.empty())
        {
            if (storeType != e_DiaDatFileStoreType_Implicit)
                return false;
        }
        if (resolution.empty())
            return false;
        if (min.empty())
            return false;
        if (max.empty())
            return false;
        return true;
    }
    bool isEmpty()
    {
        if (!chName.empty())
            return false;
        if (!unit.empty())
            return false;
        if (storeType != e_DiaDatFileStoreType_Undef)
            return false;
        if (!filename.empty())
            return false;
        if (!dataType.empty())
            return false;
        if (!recordCount.empty())
            return false;
        if (!channelIndex.empty())
            return false;
        if (!offset.empty())
            return false;
        if (!resolution.empty())
            return false;
        if (!min.empty())
            return false;
        if (!max.empty())
            return false;
        return true;
    }
};

#endif /* DIADAT_CHANNEL_DATA_H_ */
