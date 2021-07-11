/*
 * diadat_channel_u32.h
 *
 *  Created on: 2021. febr. 6.
 *      Author: liptakok
 */

#ifndef DIADAT_DIADAT_CHANNEL_U32_H_
#define DIADAT_DIADAT_CHANNEL_U32_H_

#include "diadat_channel.h"

class DiaDat_ChannelDataU32 : public DiaDat_ChannelDataBase
{
public:
    DiaDat_ChannelDataU32(void *var = NULL) : DiaDat_ChannelDataBase(NULL, var)
    {
        dataSize = 4;
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
    const char *getFileExtension()
    {
        return "u32";
    }
    ~DiaDat_ChannelDataU32()
    {
    }
    t_DiaDat_ChannelType getType() const
    {
        return e_DiaDat_ChannelType_u32;
    }
    virtual void setVar(uint32_t *var)
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
        rawValue = (uint32_t)((data - offset + (factor/2)) / factor);
        return 0;
    }

    virtual int8_t read()
    {
        return write();
    }
    virtual int8_t read(const uint8_t *block)
    {
        rawValue = block[0] + (((uint32_t)block[1]) * 256) +
                (((uint32_t)block[2]) * 65536) +
                (((uint32_t)block[3]) * 16777216);
        if (dataPtr != NULL)
        {
            *(uint32_t*)dataPtr = rawValue;
        }
        return 0;
    }

    virtual int8_t write()
    {
        if (dataPtr != NULL)
            *(uint32_t*)dataPtr = (*(uint32_t*)dataPtr) + 1;
        else
            rawValue++;
        return 0;
    }
    virtual int8_t write(uint8_t *block)
    {
        uint32_t val;
        if (dataPtr != NULL)
            val = *(int32_t*)dataPtr;
        else
            val = rawValue;
        block[0] = val & 0xFF;
        block[1] = (val >>  8) & 0xFF;
        block[2] = (val >> 16) & 0xFF;
        block[3] = (val >> 24) & 0xFF;
        return 0;
    }

protected:
    uint32_t min;
    uint32_t max;
    uint32_t rawValue;
    double offset, factor;
};

#endif /* DIADAT_DIADAT_CHANNEL_U32_H_ */
