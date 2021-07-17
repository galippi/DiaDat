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
    DiaDat_ChannelDataU16(void *var = NULL) : DiaDat_ChannelDataBase(NULL, var)
    {
        channelTypeBase = &DiaDat_ChannelType_u16;
        dataSize = 2;
        rawValue = 0;
        min = 0;
        max = 0;
        offset = 0;
        factor = 1.0;
        if (var == NULL)
            dataPtr = &rawValue;
        else
            dataPtr = var;
    }
    ~DiaDat_ChannelDataU16()
    {
    }
    t_DiaDat_ChannelType getType() const
    {
        return e_DiaDat_ChannelType_u16;
    }
    virtual void setVar(uint16_t *var)
    {
        if (var == NULL)
        {
            dataPtr = &rawValue;
        }else
        {
            dataPtr = var;
        }
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

    virtual int8_t read()
    {
        return write();
    }
    virtual int8_t read(const uint8_t *block)
    {
        if (dataPtr != NULL)
        {
            *(uint16_t*)dataPtr = block[0] + (((uint16_t)block[1]) * 256);
        }
        else
        {
            rawValue = block[0] + (((uint16_t)block[1]) * 256);
        }
        return 0;
    }

    virtual int8_t write()
    {
        if (dataPtr != NULL)
            *(uint16_t*)dataPtr = (*(uint16_t*)dataPtr) + 1;
        else
            rawValue++;
        return 0;
    }
    virtual int8_t write(uint8_t *block)
    {
        uint16_t val;
        if (dataPtr != NULL)
            val = *(uint16_t*)dataPtr;
        else
            val = rawValue;
        block[0] = val & 0xFF;
        block[1] = (val >> 8) & 0xFF;
        return 0;
    }

protected:
    uint16_t min;
    uint16_t max;
    uint16_t rawValue;
    double offset, factor;
};

#endif /* DIADAT_DIADAT_CHANNEL_U16_H_ */
