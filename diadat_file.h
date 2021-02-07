#ifndef _DIADAT_FILE_H_
#define _DIADAT_FILE_H_

#include <vector>
#include <map>

#include <stdio.h>

#include "diadat_channel.h"

typedef enum
{
  e_DiaDatFileType_None,
  e_DiaDatFileType_Read,
  e_DiaDatFileType_Write,
}t_DiaDatFileType;

class DiaDat_DataFile
{
public:
    DiaDat_DataFile(const char *filenameBase, t_DiaDat_ChannelType type);
    void addChannel(DiaDat_Channel *channel);
protected:
    FILE *file;
    std::string filename;
    uint32_t blockSize;
    uint32_t channelCount;
    t_DiaDat_ChannelType type;
    std::vector<DiaDat_Channel*> channels;
};

class DiaDat_ChannelType
{
public:
    DiaDat_ChannelType(t_DiaDat_ChannelType type)
    {
        this->type = type;
    }
    bool operator< (const DiaDat_ChannelType& other) const
    {
        if(other.type < this->type)
            return true;
        return false;
    }
protected:
    t_DiaDat_ChannelType type;
};

class DiaDat_FileChannel : public DiaDat_Channel
{
public:
    DiaDat_FileChannel(const char *name, t_DiaDat_ChannelType type, DiaDat_DataFile *file);
protected:
    DiaDat_DataFile *file;
};

class DiaDat_File
{
  public:
    DiaDat_File();
    ~DiaDat_File();
    int8_t open(const char *filename);
    int8_t create(const char *filename);
    int32_t createChannel(const char *name, t_DiaDat_ChannelType type);
    DiaDat_FileChannel *getChannel(int32_t chIdx);
    DiaDat_DataFile *getDataFile(t_DiaDat_ChannelType type);

  protected:
    std::string name;
    t_DiaDatFileType type;
    std::vector<DiaDat_FileChannel*> channels;
    std::map<t_DiaDat_ChannelType, DiaDat_DataFile*> dataFiles;
    std::map<const char *, int32_t> channelNumber;
    bool datHeaderIsNotYetWritten;
    FILE *file;
};

#endif /* _DIADAT_FILE_H_ */
