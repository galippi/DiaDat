#ifndef _DIADAT_CHANNEL_H_
#define _DIADAT_CHANNEL_H_

#include <stdint.h>

#include <string>
#include <map>

class DiaDat_DataFile;

typedef enum
{
    e_DiaDat_Implicit,
    e_DiaDat_Explicit,
}t_DiaDat_ChannelStoreMode;

typedef enum
{
  e_DiaDat_ChannelType_u8,
  e_DiaDat_ChannelType_s8,
  e_DiaDat_ChannelType_u16,
  e_DiaDat_ChannelType_s16,
  e_DiaDat_ChannelType_d32,
  e_DiaDat_ChannelType_d64,
}t_DiaDat_ChannelType;

class c_DiaDat_ChannelTypeBase
{
public:
    c_DiaDat_ChannelTypeBase(const char *name, const char *datFileSuffix, t_DiaDat_ChannelType _type, const char *_datChannelType)
    {
        id = idSource;
        idSource++;
        this->name = name;
        this->datFileSuffix = datFileSuffix;
        datChannelType = _datChannelType;
        type = _type;
        type2datChannelType[type] = datChannelType;
        datChannelType2type[datChannelType] = type;
    }
    uint8_t getId() const
    {
        return id;
    }
    const char *getName() const
    {
        return name;
    }
    const char *getDatFileSuffix() const
    {
        return datFileSuffix;
    }
    static t_DiaDat_ChannelType convert2type(const char *typeName);
    static const std::string &getChannelType(t_DiaDat_ChannelType type);
protected:
    static uint8_t idSource;
    uint8_t id;
    const char *name;
    const char *datFileSuffix;
    t_DiaDat_ChannelType type;
    std::string datChannelType;
    static std::map<t_DiaDat_ChannelType, std::string> type2datChannelType;
    static std::map<std::string, t_DiaDat_ChannelType> datChannelType2type;
};

extern c_DiaDat_ChannelTypeBase DiaDat_ChannelType_u8;
extern c_DiaDat_ChannelTypeBase DiaDat_ChannelType_s8;
extern c_DiaDat_ChannelTypeBase DiaDat_ChannelType_u16;
extern c_DiaDat_ChannelTypeBase DiaDat_ChannelType_s16;

class DiaDat_ChannelDataBase
{
  public:
    DiaDat_ChannelDataBase(DiaDat_DataFile *_parent, void *var = NULL);
    virtual ~DiaDat_ChannelDataBase();
    virtual t_DiaDat_ChannelType getType() const = 0;
    virtual double getMin() const = 0;
    virtual double getMax() const = 0;
    virtual double getFactor() const = 0;
    virtual const void *getRaw() const
    {
        return dataPtr;
    }
    virtual int8_t set(double data) = 0;
    virtual void set(double *data)
    {
        conversionIsRequired = true;
        dataPtr = data;
    }
    virtual void set(void *data)
    {
        conversionIsRequired = false;
        dataPtr = data;
    }
    uint8_t getDataSize() const
    {
        return dataSize;
    }
    virtual int8_t read() = 0;
    virtual int8_t read(const uint8_t *buffer) = 0;
    virtual int8_t write() = 0;
    virtual int8_t write(uint8_t *buffer) = 0;
    virtual const char *getFileExtension() = 0;
  protected:
    uint8_t dataSize;
    void *dataPtr;
    bool conversionIsRequired;
    DiaDat_DataFile *parent;
};

class ChannelData;
class DiaDat_File;

class DiaDat_Channel
{
  public:
    //DiaDat_Channel(DiaDat_DataFile *_parent, void *var = NULL);
    //DiaDat_Channel(DiaDat_DataFile *_parent, const char *name, t_DiaDat_ChannelType type, void *var = NULL);
    //static DiaDat_Channel *createChannel(DiaDat_File *_parent, ChannelData *chData);
    static DiaDat_Channel *createChannel(DiaDat_File *_parent, const char *name, t_DiaDat_ChannelType type, void *var = NULL);
    DiaDat_Channel(DiaDat_File *_parent, ChannelData *chData);
    DiaDat_Channel(DiaDat_File *_parent, const char *name, DiaDat_ChannelDataBase *chData);
    ~DiaDat_Channel(){};
    DiaDat_ChannelDataBase *getDataHandler() const
    {
        return dataHandler;
    }
    void setBlockOffset(uint32_t _offset)
    {
        offset = _offset;
    }
    const std::string getName() const
    {
        return name;
    }
    const std::string getUnit() const
    {
        return unit;
    }
    int8_t read();
    int8_t read(const uint8_t *buffer);
    int8_t write();
    int8_t write(uint8_t *buffer);
    uint32_t getDataSize() const
    {
        return dataHandler->getDataSize();
    }
    const std::string getFileName();
    const std::string getDiaDatFileType();
    unsigned getFileOffset();
  protected:
    std::string name;
    std::string unit;
    DiaDat_ChannelDataBase *dataHandler;
    uint32_t offset;
    DiaDat_DataFile *parent;
  private:
    void createDataHandler(DiaDat_DataFile *_parent, t_DiaDat_ChannelType type, void *var);
};

#endif /* _DIADAT_CHANNEL_H_ */
