#ifndef _DIADAT_CHANNEL_H_
#define _DIADAT_CHANNEL_H_

#include <stdint.h>

#include <string>

class DiaDat_DataFile;

typedef enum
{
  e_DiaDat_ChannelType_u8,
  e_DiaDat_ChannelType_s8,
  e_DiaDat_ChannelType_u16,
  e_DiaDat_ChannelType_s16,
}t_DiaDat_ChannelType;

class c_DiaDat_ChannelTypeBase
{
public:
    c_DiaDat_ChannelTypeBase(const char *name, const char *datFileSuffix)
    {
        id = idSource;
        idSource++;
        this->name = name;
        this->datFileSuffix = datFileSuffix;
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
protected:
    static uint8_t idSource;
    uint8_t id;
    const char *name;
    const char *datFileSuffix;
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
    uint8_t getDataSize() const
    {
        return dataSize;
    }
    virtual int8_t update(uint8_t *block, uint32_t offset) = 0;
  protected:
    uint8_t dataSize;
    void *dataPtr;
    bool conversionIsRequired;
    DiaDat_DataFile *parent;
};

class DiaDat_Channel
{
  public:
    DiaDat_Channel(DiaDat_DataFile *_parent, void *var = NULL);
    DiaDat_Channel(DiaDat_DataFile *_parent, const char *name, t_DiaDat_ChannelType type, void *var = NULL);
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
    int8_t update(uint8_t *block);
  protected:
    std::string name;
    std::string unit;
    DiaDat_ChannelDataBase *dataHandler;
    uint32_t offset;
    DiaDat_DataFile *parent;
};

#endif /* _DIADAT_CHANNEL_H_ */
