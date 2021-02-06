#ifndef _DIADAT_CHANNEL_H_
#define _DIADAT_CHANNEL_H_

#include <stdint.h>

#include <string>

typedef enum
{
  e_DiaDat_ChannelType_u8,
  e_DiaDat_ChannelType_s8,
  e_DiaDat_ChannelType_u16,
  e_DiaDat_ChannelType_s16,
}t_DiaDat_ChannelType;

class DiaDat_ChannelDataBase
{
  public:
    DiaDat_ChannelDataBase();
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
    uint8_t getDataSize() const
    {
        return dataSize;
    }
  private:
    uint8_t dataSize;
    void *dataPtr;
};

class DiaDat_Channel
{
  public:
    DiaDat_Channel();
    DiaDat_Channel(const char *name, t_DiaDat_ChannelType type);
    ~DiaDat_Channel(){};
    DiaDat_ChannelDataBase *getDataHandler() const
    {
        return dataHandler;
    }
    void setBlockOffset(uint32_t _offset)
    {
        offset = _offset;
    }
  protected:
    std::string name;
    std::string unit;
    DiaDat_ChannelDataBase *dataHandler;
    uint32_t offset;
};

#endif /* _DIADAT_CHANNEL_H_ */
