#ifndef _DIADAT_FILE_H_
#define _DIADAT_FILE_H_

#include <vector>
#include <map>

#include <stdio.h>

#include "diadat_channel.h"

#include "DiaDat_FileType.h"

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

class DiaDat_FileChannel;

class DiaDat_File
{
  public:
    DiaDat_File();
    DiaDat_File(const char *filename, t_DiaDatFileType _type = e_DiaDatFileType_Read);
    ~DiaDat_File();
    int8_t open(const char *filename);
    int8_t create(const char *filename);
    DiaDat_Channel *createChannel(const char *name, t_DiaDat_ChannelType type, void *var = NULL);
    DiaDat_Channel *addChannel(ChannelData *chData);
    void connectVar(const char *chName, void *var);
    DiaDat_Channel *getChannel(const char *chName);
    //DiaDat_Channel *getChannel(int32_t chIdx);
    //DiaDat_DataFile *getDataFile(t_DiaDat_ChannelType type, const char *filename = NULL);
    DiaDat_DataFile *registerChannel(DiaDat_Channel *ch, const std::string &dataFileName, t_DiaDatFileStoreType storeType = e_DiaDatFileStoreType_Explicit);
    DiaDat_DataFile *getDataFile(const char *filename);
    int8_t close(void);
    int8_t step();
    void set_dT(double _dT){dT = _dT;}
    t_DiaDatFileType getDirection() const
    {
        return type;
    }
    std::string getName()
    {
        return name;
    }

  protected:
    int8_t readHeader();
    int8_t readRecord();
    int8_t writeHeader();
    int8_t writeRecord();
    int8_t writeChannelHeader(DiaDat_Channel *ch);
    int8_t writeHeaderLine(const char *fmt, ...);
    std::string name;
    t_DiaDatFileType type;
    std::map<std::string, DiaDat_DataFile*> dataFiles;
    std::map<std::string, DiaDat_Channel*> channels;
    FILE *file;
    double dT;
    double t;
    uint32_t recordCount;

  private:
    void init();
    void chomp(char *line);
};

#endif /* _DIADAT_FILE_H_ */
