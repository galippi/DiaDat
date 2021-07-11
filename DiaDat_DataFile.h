/*
 * DiaDat_DataFile.h
 *
 *  Created on: 2021. máj. 19.
 *      Author: liptakok
 */

#ifndef DIADAT_DIADAT_DATAFILE_H_
#define DIADAT_DIADAT_DATAFILE_H_

#include <vector>

#include "diadat_channel.h"

#include "DiaDat_FileType.h"

class ChannelData;

class DiaDat_File;

class DiaDat_FileChannel : public DiaDat_Channel
{
public:
    DiaDat_FileChannel(const char *name, t_DiaDat_ChannelType type, DiaDat_DataFile *file, void *var);
    DiaDat_FileChannel(DiaDat_File *_parent, ChannelData *chData);
    const std::string getFileName() const;
    const std::string getDiaDatFileType() const;
    uint32_t getFileOffset() const
    {
        return offset;
    }
    DiaDat_DataFile *getDataFile();
protected:
    DiaDat_DataFile *file;
};

class DiaDat_FileChannelImplicit : public DiaDat_FileChannel
{
public:
    DiaDat_FileChannelImplicit(DiaDat_File *_parent, ChannelData *chData);
    const char *getFileName()
    {
        return "IMPLICIT:";
    }
};

class DiaDat_FileChannelExplicit : public DiaDat_FileChannel
{
public:
    DiaDat_FileChannelExplicit(DiaDat_File *_parent, ChannelData *chData);
    const char *getFileName() const;
};

#endif /* DIADAT_DIADAT_DATAFILE_H_ */

class DiaDat_File;

class DiaDat_DataFile
{
public:
    DiaDat_DataFile(DiaDat_File *_parent, const char *filenameBase, t_DiaDat_ChannelType type, const char *dataFileName);
    DiaDat_DataFile(DiaDat_File *_parent, DiaDat_Channel *ch, const std::string &dataFileName);
    virtual ~DiaDat_DataFile();
    virtual void addChannel(DiaDat_Channel *channel);
    const std::string getName() const
    {
        return filename;
    }
    const std::string getDiaDatFileType() const
    {
        return datFileType;
    }
    virtual bool isImplicit() const = 0;
    virtual uint8_t *getBuffer() = 0;
    t_DiaDatFileType getDirection() const;
    virtual int8_t writeRecord() = 0;
    virtual int8_t readRecord() = 0;
    virtual void close() = 0;
protected:
    std::string filename;
    uint32_t channelCount;
    t_DiaDat_ChannelType type;
    std::vector<DiaDat_Channel*> channels;
    std::string datFileType;
    DiaDat_File *parent;
};

class DiaDat_DataFileImplicit : public DiaDat_DataFile
{
public:
    DiaDat_DataFileImplicit(DiaDat_File *_parent, DiaDat_Channel *ch);
    virtual bool isImplicit() const
    {
        return true;
    }
    virtual uint8_t *getBuffer();
    virtual int8_t writeRecord();
    virtual int8_t readRecord();
    void close()
    { // implicit file - do nothing
    }

};

class DiaDat_DataFileExplicit : public DiaDat_DataFile
{
public:
    DiaDat_DataFileExplicit(DiaDat_File *_parent, DiaDat_Channel *ch, const std::string &dataFileName);
    ~DiaDat_DataFileExplicit();
    virtual bool isImplicit() const
    {
        return false;
    }
    virtual uint8_t *getBuffer();
    void addChannel(DiaDat_Channel *channel);
    virtual int8_t writeRecord();
    virtual int8_t readRecord();
    void close()
    {
        fclose(file);
        file = NULL;
    }
protected:
    FILE *file;
    uint32_t blockSize;
    uint8_t *block;
};
