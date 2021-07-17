/*
 * diadat_channel_s8.h
 *
 *  Created on: 2021. febr. 6.
 *      Author: liptakok
 */

#ifndef DIADAT_DIADAT_CHANNEL_S8_H_
#define DIADAT_DIADAT_CHANNEL_S8_H_

#include "diadat_channel.h"

class DiaDat_ChannelDataS8 : public DiaDat_ChannelDataBase
{
public:
    DiaDat_ChannelDataS8(void *var = NULL) : DiaDat_ChannelDataBase(NULL, var)
    {
        dataSize = 1;
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
        return "u8";
    }
    ~DiaDat_ChannelDataS8()
    {
    }
    t_DiaDat_ChannelType getType() const
    {
        return e_DiaDat_ChannelType_s8;
    }
    virtual void setVar(int8_t *var)
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
        rawValue = (int8_t)((data - offset + (factor/2)) / factor);
        return 0;
    }

    virtual int8_t read()
    {
        return write();
    }
    virtual int8_t read(const uint8_t *block)
    {
        rawValue = (int8_t)(block[0] - 128);
        if (dataPtr != NULL)
            *(int8_t*)dataPtr = rawValue ;
        return 0;
    }

    virtual int8_t write()
    {
        if (dataPtr != NULL)
            *(int8_t*)dataPtr = (*(int8_t*)dataPtr) + 1;
        else
            rawValue++;
        return 0;
    }
    virtual int8_t write(uint8_t *block)
    {
        if (dataPtr != NULL)
            block[0] = ((uint8_t)(*(int8_t*)dataPtr) + 128);
        else
            block[0] = (uint8_t)(rawValue + 128);
        return 0;
    }

    virtual double getOffset() const
    {
        return -128.0000;
    }

protected:
    int8_t min;
    int8_t max;
    int8_t rawValue;
    double offset, factor;
};

#endif /* DIADAT_DIADAT_CHANNEL_S8_H_ */
