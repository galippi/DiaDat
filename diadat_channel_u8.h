/*
 * diadat_channel_u8.h
 *
 *  Created on: 2021. febr. 6.
 *      Author: liptakok
 */

#ifndef DIADAT_DIADAT_CHANNEL_U8_H_
#define DIADAT_DIADAT_CHANNEL_U8_H_

#include "diadat_channel.h"

class DiaDat_ChannelDataU8 : public DiaDat_ChannelDataBase
{
public:
    DiaDat_ChannelDataU8(DiaDat_DataFile *_parent, void *var = NULL) : DiaDat_ChannelDataBase(_parent, var)
    {
        dataSize = 1;
        rawValue = 0;
        min = 0;
        max = 0;
        offset = 0;
        factor = 1.0;
    }
    ~DiaDat_ChannelDataU8()
    {
    }
    t_DiaDat_ChannelType getType() const
    {
        return e_DiaDat_ChannelType_u8;
    }
    double getMin() const
    {
        return min * factor + offset;
    }
    virtual double getMax() const
    {
        return max * factor + offset;
    }
    virtual double getFactor() const
    {
        return factor;
    }
    virtual const void *getRaw() const
    {
        return &rawValue;
    }
    int8_t set(double data)
    {
        rawValue = (uint8_t)((data - offset + (factor/2)) / factor);
        return 0;
    }
    virtual int8_t update(uint8_t *block, uint32_t offset)
    {
        if (dataPtr != NULL)
            block[offset] = *(uint8_t*)dataPtr;
        else
            block[offset] = rawValue;
        return 0;
    }

protected:
    uint8_t min;
    uint8_t max;
    uint8_t rawValue;
    double offset, factor;
};

#endif /* DIADAT_DIADAT_CHANNEL_U8_H_ */
