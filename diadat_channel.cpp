#include "diadat_channel.h"

#include "diadat_channel_s8.h"
#include "diadat_channel_u8.h"

#include "my_debug.h"

DiaDat_ChannelDataBase::DiaDat_ChannelDataBase()
{
    dataSize=0;
    dataPtr = NULL;
    conversionIsRequired = false;
}

DiaDat_ChannelDataBase::~DiaDat_ChannelDataBase()
{
}

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
    case e_DiaDat_ChannelType_s8:
    {
        dataHandler = new DiaDat_ChannelDataS8();
        break;
    }
    default:
        dataHandler = NULL;
        throw dbg_spintf("DiaDat_Channel - not implemented channel type %d!", type);
        break;
    }
}
