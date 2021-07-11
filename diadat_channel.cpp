#include <string>

#include "diadat_file.h"
#include "diadat_DataFile.h"
#include "channel_data.h"

#include "diadat_channel_s8.h"
#include "diadat_channel_u8.h"
#include "diadat_channel_s16.h"
#include "diadat_channel_u16.h"
#include "diadat_channel_r64.h"

#include "my_debug.h"

#include "diadat_channel.h"

#ifdef STD_STOI_IMPL
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
std::map<t_DiaDat_ChannelType, std::string> c_DiaDat_ChannelTypeBase::type2datChannelType;
std::map<std::string, t_DiaDat_ChannelType> c_DiaDat_ChannelTypeBase::datChannelType2type;

c_DiaDat_ChannelTypeBase DiaDat_ChannelType_u8("DiaDat_ChannelType_u8", "u8", e_DiaDat_ChannelType_u8, "WORD8");
c_DiaDat_ChannelTypeBase DiaDat_ChannelType_s8("DiaDat_ChannelType_s8", "s8", e_DiaDat_ChannelType_s8, "INT8");
c_DiaDat_ChannelTypeBase DiaDat_ChannelType_u16("DiaDat_ChannelType_u16", "u16", e_DiaDat_ChannelType_u16, "WORD16");
c_DiaDat_ChannelTypeBase DiaDat_ChannelType_s16("DiaDat_ChannelType_s16", "s16", e_DiaDat_ChannelType_s16, "INT16");
c_DiaDat_ChannelTypeBase DiaDat_ChannelType_d64("DiaDat_ChannelType_d64", "r64", e_DiaDat_ChannelType_d64, "REAL64");

t_DiaDat_ChannelType c_DiaDat_ChannelTypeBase::convert2type(const char *typeName)
{
    auto it = datChannelType2type.find(typeName);
    if (it == datChannelType2type.end())
        throw dbg_spintf("DiaDat_Channel - not implemented channel type %s!", typeName);
    return it->second;
}

const std::string &c_DiaDat_ChannelTypeBase::getChannelType(t_DiaDat_ChannelType type)
{
    auto it = type2datChannelType.find(type);
    if (it == type2datChannelType.end())
        throw dbg_spintf("DiaDat_Channel - not implemented channel type %d!", type);
    return it->second;
}

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

static DiaDat_ChannelDataBase *createChannelBase(t_DiaDat_ChannelType type, void *var)
{
    DiaDat_ChannelDataBase *data;
    switch (type)
    {
        case e_DiaDat_ChannelType_u8:
            data = new DiaDat_ChannelDataU8(var);
            break;
        case e_DiaDat_ChannelType_s8:
            data = new DiaDat_ChannelDataS8(var);
            break;
        case e_DiaDat_ChannelType_s16:
        {
            data = new DiaDat_ChannelDataS16(var);
            break;
        }
        case e_DiaDat_ChannelType_u16:
        {
            data = new DiaDat_ChannelDataU16(var);
            break;
        }
        case e_DiaDat_ChannelType_d64:
        {
            data = new DiaDat_ChannelDataR64(var);
            break;
        }

        default:
            throw dbg_spintf("DiaDat_Channel - not implemented channel type %d!", type);
    }
    return data;
}

static DiaDat_ChannelDataBase *createChannelBase(ChannelData *chData)
{
    t_DiaDat_ChannelType chType = c_DiaDat_ChannelTypeBase::convert2type(chData->dataType.c_str());
    DiaDat_ChannelDataBase *dataHandler = createChannelBase(chType, NULL);
    return dataHandler;
}

DiaDat_Channel *DiaDat_Channel::createChannel(DiaDat_File *_parent, const char *name, t_DiaDat_ChannelType type, void *var)
{
    DiaDat_ChannelDataBase *data = createChannelBase(type, var);
    return new DiaDat_Channel(_parent, name, data);
}

#if 0
DiaDat_Channel::DiaDat_Channel(DiaDat_File *_parent, ChannelData *chData)
{
    void *var = NULL;
    parent = _parent->getDataFile(chData->filename.c_str());
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
#endif

DiaDat_Channel::DiaDat_Channel(DiaDat_File *_parent, const char *name, DiaDat_ChannelDataBase *chData)
{
    this->name = name;
    std::string dataFileName = _parent->getName() + "." + chData->getFileExtension();
    dataHandler = chData;
    parent = _parent->registerChannel(this, dataFileName);
}

DiaDat_Channel::DiaDat_Channel(DiaDat_File *_parent, ChannelData *chData)
{
    name = chData->chName;
    dataHandler = createChannelBase(chData);
    std::string dataFileName;
    if (chData->storeType == e_DiaDatFileStoreType_Explicit)
        dataFileName = _parent->getName() + "." + dataHandler->getFileExtension();
    else
        dataFileName = "IMPLICIT:";
    if (chData->storeType == e_DiaDatFileStoreType_Explicit)
    {
        offset = (std::stoi(chData->channelIndex, nullptr, 10) - 1) * dataHandler->getDataSize();
    }else
        offset = -1;
    parent = _parent->registerChannel(this, dataFileName, chData->storeType);
}

int8_t DiaDat_Channel::read()
{
    if (parent->isImplicit())
        dataHandler->read();
    else
        dataHandler->read(parent->getBuffer() + offset);
    return 0;
}

int8_t DiaDat_Channel::read(const uint8_t *buffer)
{
    dataHandler->read(buffer + offset);
    return 0;
}


int8_t DiaDat_Channel::write()
{
    if (parent->isImplicit())
        dataHandler->write();
    else
        dataHandler->write(parent->getBuffer() + offset);
    return 0;
}

int8_t DiaDat_Channel::write(uint8_t *buffer)
{
    dataHandler->write(buffer + offset);
    return 0;
}

const std::string DiaDat_Channel::getFileName()
{
    return parent->getName();
}

const std::string DiaDat_Channel::getDiaDatFileType()
{
    return c_DiaDat_ChannelTypeBase::getChannelType(dataHandler->getType());
}

unsigned DiaDat_Channel::getFileOffset()
{
    return offset;
}

#if 0
DiaDat_FileChannel::DiaDat_FileChannel(const char *name, t_DiaDat_ChannelType type, DiaDat_DataFile *file, void *var)
    : DiaDat_Channel(file, name, type, var)
{
    this->file = file;
    file->addChannel(this);
}
#endif

DiaDat_FileChannel::DiaDat_FileChannel(DiaDat_File *_parent, ChannelData *chData)
    : DiaDat_Channel(_parent, chData)
{
    //t_DiaDat_ChannelType chType = c_DiaDat_ChannelTypeBase::convert2type(chData->storeType.c_str());
    //auto fileChannel = _parent->getDataFile(chType);
    //file =_parent->registerChannel(this, chData->filename, chData->storeType);
    file = parent;
    //parent->addChannel(this);
}
