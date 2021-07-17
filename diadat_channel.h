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
  e_DiaDat_ChannelType_u32,
  e_DiaDat_ChannelType_s32,
  e_DiaDat_ChannelType_r32,
  e_DiaDat_ChannelType_r64,
}t_DiaDat_ChannelType;

class c_DiaDat_ChannelTypeBase
{
public:
    c_DiaDat_ChannelTypeBase(const char *datFileSuffix, t_DiaDat_ChannelType _type, const char *_datChannelType)
    {
        id = idSource;
        idSource++;
        this->datFileSuffix = datFileSuffix;
        datChannelType = _datChannelType;
        type = _type;
        type2datChannelType[type] = datChannelType;
        if (type != e_DiaDat_ChannelType_s8) // S8 channel is not a native channel - it would not be added
            datChannelType2type[datChannelType] = type;
    }
    uint8_t getId() const
    {
        return id;
    }
    const char *getDatFileSuffix() const
    {
        return datFileSuffix;
    }
    t_DiaDat_ChannelType getType() const
    {
        return type;
    }
    static t_DiaDat_ChannelType convert2type(const char *typeName);
    static const std::string &getChannelType(t_DiaDat_ChannelType type);
protected:
    static uint8_t idSource;
    uint8_t id;
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
extern c_DiaDat_ChannelTypeBase DiaDat_ChannelType_u32;
extern c_DiaDat_ChannelTypeBase DiaDat_ChannelType_s32;
extern c_DiaDat_ChannelTypeBase DiaDat_ChannelType_r32;
extern c_DiaDat_ChannelTypeBase DiaDat_ChannelType_r64;

class DiaDat_ChannelDataBase
{
  public:
    DiaDat_ChannelDataBase(DiaDat_DataFile *_parent, void *var = NULL);
    virtual ~DiaDat_ChannelDataBase();
    t_DiaDat_ChannelType getType() const
    {
        return channelTypeBase->getType();
    }
    virtual double getMin() const = 0;
    virtual double getMax() const = 0;
    double factor;
    void setFactor(double _factor)
    {
        factor = _factor;
    }
    virtual double getFactor() const = 0;
    double offset;
    void setOffset(double _offset)
    {
        offset = _offset;
    }
    virtual double getOffset() const
    {
        return 0.000000;
    }
    virtual const void *getRaw() const
    {
        return dataPtr;
    }
    virtual int8_t set(double data) = 0;
    virtual void setVar(double *data)
    {
        conversionIsRequired = true;
        dataPtr = data;
    }
    //virtual void set(void *data)
    //{
    //    conversionIsRequired = false;
    //    dataPtr = data;
    //}
    virtual void setVar(uint8_t *var);
    virtual void setVar(int8_t *var);
    virtual void setVar(uint16_t *var);
    virtual void setVar(int16_t *var);
    virtual void setVar(uint32_t *var);
    virtual void setVar(int32_t *var);
    virtual void setVar(float *var);
    uint8_t getDataSize() const
    {
        return dataSize;
    }
    virtual int8_t read() = 0;
    virtual int8_t read(const uint8_t *buffer) = 0;
    virtual int8_t write() = 0;
    virtual int8_t write(uint8_t *buffer) = 0;
    const char *getFileExtension()
    {
        return channelTypeBase->getDatFileSuffix();
    }
  protected:
    uint8_t dataSize;
    void *dataPtr;
    bool conversionIsRequired;
    DiaDat_DataFile *parent;
    c_DiaDat_ChannelTypeBase *channelTypeBase;
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
    void connectVar(uint8_t *var)
    {
        getDataHandler()->setVar(var);
    }
    void connectVar(int8_t *var)
    {
        getDataHandler()->setVar(var);
    }
    void connectVar(uint16_t *var)
    {
        getDataHandler()->setVar(var);
    }
    void connectVar(int16_t *var)
    {
        getDataHandler()->setVar(var);
    }
    void connectVar(uint32_t *var)
    {
        getDataHandler()->setVar(var);
    }
    void connectVar(int32_t *var)
    {
        getDataHandler()->setVar(var);
    }
    void connectVar(float *var)
    {
        getDataHandler()->setVar(var);
    }
    void connectVar(double *var)
    {
        getDataHandler()->setVar(var);
    }
    //void connectVar(void *var)
    //{
    //    getDataHandler()->set(var);
    //}
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
