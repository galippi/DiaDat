/*
 * diadat_channel_R64.h
 *
 *  Created on: 2021. febr. 6.
 *      Author: liptakok
 */

#ifndef DIADAT_DIADAT_CHANNEL_R64_H_
#define DIADAT_DIADAT_CHANNEL_R64_H_

#include <memory.h>

#include "diadat_channel.h"

class DiaDat_ChannelDataR64 : public DiaDat_ChannelDataBase
{
public:
    DiaDat_ChannelDataR64(void *var = NULL) : DiaDat_ChannelDataBase(NULL, var)
    {
        channelTypeBase = &DiaDat_ChannelType_r64;
        dataSize = 8;
        rawValue = 0.0;
        min = 0;
        max = 0;
        offset = 0;
        factor = 1.0;
        if (var == NULL)
            dataPtr = &rawValue;
        else
            dataPtr = var;
    }
    ~DiaDat_ChannelDataR64()
    {
    }
    t_DiaDat_ChannelType getType() const
    {
        return e_DiaDat_ChannelType_d64;
    }
    virtual void setVar(double *var)
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
        rawValue = data;
        return 0;
    }

    virtual int8_t read()
    {
        return write();
    }
    virtual int8_t read(const uint8_t *block)
    {
        memcpy(dataPtr, block, sizeof(double));
        return 0;
    }

    virtual int8_t write()
    {
        if (dataPtr != NULL)
            *(double*)dataPtr = (*(double*)dataPtr) + 1;
        else
            rawValue += factor;
        return 0;
    }
    virtual int8_t write(uint8_t *block)
    {
        double val;
        if (dataPtr != NULL)
            val = *(double*)dataPtr;
        else
            val = rawValue;
        memcpy(block, &val, sizeof(val));
        return 0;
    }

protected:
    double min;
    double max;
    double rawValue;
    double offset, factor;
};

#endif /* DIADAT_DIADAT_CHANNEL_R64_H_ */
