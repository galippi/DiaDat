#include <string>

#include "diadat_file.h"
#include "channel_data.h"

#include "diadat_channel.h"

#include "diadat_channel_s8.h"
#include "diadat_channel_u8.h"

#include "my_debug.h"

#if 1
namespace std {
    static int stoi( const std::string& str, std::size_t* pos = 0, int base = 10 )
    {
        (void)str;
        (void)pos;
        (void)base;
        return 0;
    }
}

#endif

uint8_t c_DiaDat_ChannelTypeBase::idSource = 0;
std::map<t_DiaDat_ChannelType, std::string*> c_DiaDat_ChannelTypeBase::type2datChannelType;
std::map<std::string, t_DiaDat_ChannelType> c_DiaDat_ChannelTypeBase::datChannelType2type;

c_DiaDat_ChannelTypeBase DiaDat_ChannelType_u8("DiaDat_ChannelType_u8", "u8", e_DiaDat_ChannelType_u8, "WORD8");
//c_DiaDat_ChannelTypeBase DiaDat_ChannelType_s8("DiaDat_ChannelType_s8", "s8");
c_DiaDat_ChannelTypeBase DiaDat_ChannelType_u16("DiaDat_ChannelType_u16", "u16", e_DiaDat_ChannelType_u16, "WORD16");
//c_DiaDat_ChannelTypeBase DiaDat_ChannelType_s16("DiaDat_ChannelType_s16", "s16");

DiaDat_ChannelDataBase::DiaDat_ChannelDataBase(DiaDat_DataFile *_parent, void *var)
{
    parent = _parent;
    dataSize=0;
    dataPtr = var;
    conversionIsRequired = false;
}

DiaDat_ChannelDataBase::~DiaDat_ChannelDataBase()
{
}

DiaDat_Channel::DiaDat_Channel(DiaDat_DataFile *_parent, void *var)
{
    parent = _parent;
    name = "";
    dataHandler = NULL;
    if (var != NULL)
        throw dbg_spintf("DiaDat_Channel - not valid parameter (var != NULL)!");
}

DiaDat_Channel::DiaDat_Channel(DiaDat_DataFile *_parent, const char *name, t_DiaDat_ChannelType type, void *var)
{
    this->name = name;
    createDataHandler(_parent, type, var);
}

DiaDat_Channel::DiaDat_Channel(DiaDat_File *_parent, ChannelData *chData)
{
    name = chData->chName;
    t_DiaDat_ChannelType chType = c_DiaDat_ChannelTypeBase::convert2type(chData->dataType.c_str());
    auto fileChannel = _parent->getDataFile(chType);
    parent = fileChannel;
    offset = std::stoi(chData->channelIndex, nullptr, 10);
    createDataHandler(parent, chType, NULL);
}

void DiaDat_Channel::createDataHandler(DiaDat_DataFile *_parent, t_DiaDat_ChannelType type, void *var)
{
    switch(type)
    {
        case e_DiaDat_ChannelType_u8:
        {
            dataHandler = new DiaDat_ChannelDataU8(_parent, var);
            break;
        }
        case e_DiaDat_ChannelType_s8:
        {
            dataHandler = new DiaDat_ChannelDataS8(_parent, var);
            break;
        }
        default:
            dataHandler = NULL;
            throw dbg_spintf("DiaDat_Channel - not implemented channel type %d!", type);
            break;
    }
}

int8_t DiaDat_Channel::update(uint8_t *block)
{
    dataHandler->update(block, offset);
    return 0;
}
