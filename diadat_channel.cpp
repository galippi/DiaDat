#include "diadat_channel.h"

#include "diadat_channel_s8.h"
#include "diadat_channel_u8.h"

#include "my_debug.h"

uint8_t c_DiaDat_ChannelTypeBase::idSource = 0;
c_DiaDat_ChannelTypeBase DiaDat_ChannelType_u8("DiaDat_ChannelType_u8", "u8");
c_DiaDat_ChannelTypeBase DiaDat_ChannelType_s8("DiaDat_ChannelType_s8", "s8");
c_DiaDat_ChannelTypeBase DiaDat_ChannelType_u16("DiaDat_ChannelType_u16", "u16");
c_DiaDat_ChannelTypeBase DiaDat_ChannelType_s16("DiaDat_ChannelType_s16", "s16");

DiaDat_ChannelDataBase::DiaDat_ChannelDataBase(void *var)
{
    dataSize=0;
    dataPtr = var;
    conversionIsRequired = false;
}

DiaDat_ChannelDataBase::~DiaDat_ChannelDataBase()
{
}

DiaDat_Channel::DiaDat_Channel(void *var)
{
    name = "";
    dataHandler = NULL;
    if (var != NULL)
        throw dbg_spintf("DiaDat_Channel - not valid parameter (var != NULL)!");
}

DiaDat_Channel::DiaDat_Channel(const char *name, t_DiaDat_ChannelType type, void *var)
{
    this->name = name;
    switch(type)
    {
        case e_DiaDat_ChannelType_u8:
        {
            dataHandler = new DiaDat_ChannelDataU8(var);
            break;
        }
        case e_DiaDat_ChannelType_s8:
        {
            dataHandler = new DiaDat_ChannelDataS8(var);
            break;
        }
        default:
            dataHandler = NULL;
            throw dbg_spintf("DiaDat_Channel - not implemented channel type %d!", type);
            break;
    }
}
