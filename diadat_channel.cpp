
#include "diadat_channel.h"


DiaDat_ChannelDataBase::DiaDat_ChannelDataBase()
{
    dataSize=0;
    dataPtr = NULL;
}

DiaDat_ChannelDataBase::~DiaDat_ChannelDataBase()
{
}

class DiaDat_ChannelDataU8 : public DiaDat_ChannelDataBase
{
public:
    DiaDat_ChannelDataU8()
    {
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
protected:
    uint8_t min;
    uint8_t max;
    uint8_t rawValue;
    double offset, factor;
};

DiaDat_Channel::DiaDat_Channel()
{
    name = "";
    dataHandler = NULL;
}

DiaDat_Channel::DiaDat_Channel(const char *name, t_DiaDat_ChannelType type)
{
    this->name = name;
    switch(type)
    {
    case e_DiaDat_ChannelType_u8:
    {
        dataHandler = new DiaDat_ChannelDataU8();
        break;
    }
    default:
        dataHandler = NULL;
        throw "DiaDat_Channel - not implemented channel type!";
        break;
    }
}
