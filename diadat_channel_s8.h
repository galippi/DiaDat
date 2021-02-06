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
    DiaDat_ChannelDataS8()
    {
        rawValue = 0;
        min = 0;
        max = 0;
        offset = 0;
        factor = 1.0;
    }
    ~DiaDat_ChannelDataS8()
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
        rawValue = (int8_t)((data - offset + (factor/2)) / factor);
        return 0;
    }
protected:
    int8_t min;
    int8_t max;
    int8_t rawValue;
    double offset, factor;
};

#endif /* DIADAT_DIADAT_CHANNEL_S8_H_ */
