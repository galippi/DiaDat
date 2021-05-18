/*
 * diadat_channel_u16.h
 *
 *  Created on: 2021. febr. 6.
 *      Author: liptakok
 */

#ifndef DIADAT_DIADAT_CHANNEL_U16_H_
#define DIADAT_DIADAT_CHANNEL_U16_H_

#include "diadat_channel.h"

class DiaDat_ChannelDataU16 : public DiaDat_ChannelDataBase
{
public:
    DiaDat_ChannelDataU16(DiaDat_DataFile *_parent, void *var = NULL) : DiaDat_ChannelDataBase(_parent, var)
    {
        dataSize = 2;
        rawValue = 0;
        min = 0;
        max = 0;
        offset = 0;
        factor = 1.0;
    }
    ~DiaDat_ChannelDataU16()
    {
    }
    t_DiaDat_ChannelType getType() const
    {
        return e_DiaDat_ChannelType_u16;
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
        rawValue = (uint16_t)((data - offset + (factor/2)) / factor);
        return 0;
    }
    virtual int8_t update(uint8_t *block, uint32_t offset)
    {
    	uint16_t val;
        if (dataPtr != NULL)
            val = *(uint16_t*)dataPtr;
        else
            val = rawValue;
        block[offset] = val & 0xFF;
        block[offset + 1] = (val >> 8) & 0xFF;
        return 0;
    }

protected:
    uint16_t min;
    uint16_t max;
    uint16_t rawValue;
    double offset, factor;
};

#endif /* DIADAT_DIADAT_CHANNEL_U16_H_ */
