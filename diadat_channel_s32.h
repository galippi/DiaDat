/*
 * diadat_channel_s32.h
 *
 *  Created on: 2021. febr. 6.
 *      Author: liptakok
 */

#ifndef DIADAT_DIADAT_CHANNEL_S32_H_
#define DIADAT_DIADAT_CHANNEL_S32_H_

#include "diadat_channel.h"

class DiaDat_ChannelDataS32 : public DiaDat_ChannelDataBase
{
public:
    DiaDat_ChannelDataS32(void *var = NULL) : DiaDat_ChannelDataBase(NULL, var)
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
        return "s32";
    }
    ~DiaDat_ChannelDataS32()
    {
    }
    t_DiaDat_ChannelType getType() const
    {
        return e_DiaDat_ChannelType_s32;
    }
    virtual void setVar(int32_t *var)
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
        rawValue = (int32_t)((data - offset + (factor/2)) / factor);
        return 0;
    }

    virtual int8_t read()
    {
        return write();
    }
    virtual int8_t read(const uint8_t *block)
    {
        rawValue = block[0] + (((int32_t)block[1]) * 256) +
                (((int32_t)block[2]) * 65536) +
                (((int32_t)block[3]) * 16777216);
        if (dataPtr != NULL)
        {
            *(int32_t*)dataPtr = rawValue;
        }
        return 0;
    }

    virtual int8_t write()
    {
        if (dataPtr != NULL)
            *(int32_t*)dataPtr = (*(int32_t*)dataPtr) + 1;
        else
            rawValue++;
        return 0;
    }
    virtual int8_t write(uint8_t *block)
    {
        int32_t val;
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
    int32_t min;
    int32_t max;
    int32_t rawValue;
    double offset, factor;
};

#endif /* DIADAT_DIADAT_CHANNEL_S132_H_ */
