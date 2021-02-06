#include "diadat_file.h"

DiaDat_DataFile::DiaDat_DataFile(const char *filenameBase, t_DiaDat_ChannelType type)
{
    file = NULL;
    blockSize = 0;
    channelCount = 0;
    this->type = type;
    filename = filenameBase;
    switch(type)
    {
        case e_DiaDat_ChannelType_u8:
        {
            filename = filename + ".u8";
            break;
        }
        default:
            throw "DiaDat_DataFile - Not implemented channel type!";
            break;
    }
}

void DiaDat_DataFile::addChannel(DiaDat_Channel *channel)
{
    channel->setBlockOffset(blockSize);
    blockSize += channel->getDataHandler()->getDataSize();
    channelCount++;
    channels.push_back(channel);
}

DiaDat_FileChannel::DiaDat_FileChannel(const char *name, t_DiaDat_ChannelType type, DiaDat_DataFile *file)
    : DiaDat_Channel(name, type)
{
    this->file = file;
    file->addChannel(this);
}

DiaDat_File::DiaDat_File()
{
    type = e_DiaDatFileType_None;
}

DiaDat_File::~DiaDat_File()
{
}

int8_t DiaDat_File::open(const char *filename)
{
    (void)filename;
    type = e_DiaDatFileType_Read;
    return 0;
}

int8_t DiaDat_File::create(const char *filename)
{
    (void)filename;
    type = e_DiaDatFileType_Write;
    return 0;
}

int32_t DiaDat_File::createChannel(const char *name, t_DiaDat_ChannelType type)
{
    auto it = channelNumber.find(name);
    if (it != channelNumber.end())
        throw "Duplicated channel name!";
    auto fileChannel = getDataFile(type);
    DiaDat_FileChannel *ch = new DiaDat_FileChannel(name, type, fileChannel);
    channels.push_back(ch);
    channelNumber[name] = channels.size() - 1;
    return 0;
}

DiaDat_FileChannel *DiaDat_File::getChannel(int32_t chIdx)
{
  return channels[chIdx];
}

DiaDat_DataFile *DiaDat_File::getDataFile(t_DiaDat_ChannelType type)
{
    std::map<t_DiaDat_ChannelType, DiaDat_DataFile*>::iterator it = dataFiles.find(type);
    if (it == dataFiles.end())
    {
        DiaDat_DataFile *file = new DiaDat_DataFile(name.c_str(), type);
        dataFiles[type] = file;
        return file;
    }else
        return it->second;
}
