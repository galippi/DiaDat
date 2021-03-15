#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "diadat_file.h"

#include "my_debug.h"

DiaDat_DataFile::DiaDat_DataFile(DiaDat_File *_parent, const char *filenameBase, t_DiaDat_ChannelType type)
{
    parent = _parent;
    blockSize = 0;
    block = NULL;
    channelCount = 0;
    this->type = type;
    filename = filenameBase;
    switch(type)
    {
        case e_DiaDat_ChannelType_u8:
        {
            filename = filename + ".u8";
            datFileType = "WORD8";
            break;
        }
        case e_DiaDat_ChannelType_s8:
        {
            filename = filename + ".s8";
            datFileType = "WORD8";
            break;
        }
        default:
            throw dbg_spintf("DiaDat_DataFile - Not implemented channel type %d!", type);
            break;
    }
    file = NULL;
}

DiaDat_DataFile::~DiaDat_DataFile()
{
    if (file != NULL)
        fclose(file);
    if (block != NULL)
        free(block);
}

t_DiaDatFileType DiaDat_DataFile::getDirection() const
{
    return parent->getDirection();
}

void DiaDat_DataFile::addChannel(DiaDat_Channel *channel)
{
    if (block != NULL)
        throw dbg_spintf("DiaDat_DataFile::addChannel - not allowed state (%s)!", channel->getName().c_str());
    channel->setBlockOffset(blockSize);
    blockSize += channel->getDataHandler()->getDataSize();
    channelCount++;
    channels.push_back(channel);
}

int8_t DiaDat_DataFile::writeRecord()
{
    if (file == NULL)
    {
        file = fopen(filename.c_str(), "wb");
        if (file == NULL)
            throw dbg_spintf("DiaDat_DataFile::writeRecord - fopen failure - unable to create file (%s)!", filename.c_str());
    }
    if (block == NULL)
    {
        block = (uint8_t*)malloc(blockSize);
        if (block == NULL)
            throw dbg_spintf("DiaDat_DataFile::writeRecord - malloc failure (%u)!", blockSize);
    }
    for (auto it = channels.begin() ; it != channels.end(); ++it)
    {
        (*it)->update(block);
    }
    if (fwrite(block, 1, blockSize, file) != blockSize)
    {
        throw dbg_spintf("DiaDat_DataFile::writeRecord - fwrite failure. Is disk full? (%u)", blockSize);
        return 1;
    }
    return 0;
}

DiaDat_FileChannel::DiaDat_FileChannel(const char *name, t_DiaDat_ChannelType type, DiaDat_DataFile *file, void *var)
    : DiaDat_Channel(file, name, type, var)
{
    this->file = file;
    file->addChannel(this);
}

DiaDat_File::DiaDat_File()
{
    init();
}

DiaDat_File::DiaDat_File(const char *filename, t_DiaDatFileType _type)
{
    init();
    switch(type)
    {
        case e_DiaDatFileType_Read:
            open(filename);
            break;
        case e_DiaDatFileType_Write:
            create(filename);
            break;
        default:
            throw dbg_spintf("DiaDat_File - invalid file type in ctor (%s - %d)!", filename, _type);
    }
}

void DiaDat_File::init()
{
    type = e_DiaDatFileType_None;
    file = NULL;
    dT = -1;
    t = 0;
    recordCount = 0;
}

DiaDat_File::~DiaDat_File()
{
    if (type == e_DiaDatFileType_None)
        throw dbg_spintf("DiaDat_File::dtor - file is not yet open/created (%s)!", name.c_str());
    if (file != NULL)
        fclose(file);
}

int8_t DiaDat_File::open(const char *filename)
{
    if (type != e_DiaDatFileType_None)
        throw dbg_spintf("DiaDat_File::open - file is already open/created (%s - %s)!", name.c_str(), filename);
    name = filename;
    type = e_DiaDatFileType_Read;
    file = fopen(name.c_str(), "rt");
    if (file == NULL)
        throw dbg_spintf("DiaDat_File::open - file cannot be open (%s)!", name.c_str());
    readHeader();
    return 0;
}

int8_t DiaDat_File::create(const char *filename)
{
    if (type != e_DiaDatFileType_None)
        throw dbg_spintf("DiaDat_File::create - file is already open/created (%s - %s)!", name.c_str(), filename);
    name = filename;
    file = fopen(filename, "wt");
    if (file == NULL)
        throw dbg_spintf("DiaDat_File::create - unable to create file (%s)!", name.c_str());
    type = e_DiaDatFileType_Write;
    return 0;
}

int32_t DiaDat_File::createChannel(const char *name, t_DiaDat_ChannelType chType, void *var)
{
    if (type != e_DiaDatFileType_Write)
        throw dbg_spintf("DiaDat_File::createChannel - channel cannot be created for this type of file (%d - %s)!", type, name);
    auto it = channelNumber.find(name);
    if (it != channelNumber.end())
        throw dbg_spintf("Duplicated channel name %s!", name);
    auto fileChannel = getDataFile(chType);
    DiaDat_FileChannel *ch = new DiaDat_FileChannel(name, chType, fileChannel, var);
    int32_t chIdx = channels.size();
    channels.push_back(ch);
    channelNumber[name] = chIdx;
    return chIdx;
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
        DiaDat_DataFile *file = new DiaDat_DataFile(this, name.c_str(), type);
        dataFiles[type] = file;
        return file;
    }else
        return it->second;
}

int8_t DiaDat_File::close(void)
{
    if (type == e_DiaDatFileType_None)
        throw dbg_spintf("DiaDat_File::close - file is not yet open/created (%s)!", name.c_str());
    if (type == e_DiaDatFileType_Read)
    {
        if (file != NULL)
            fclose(file);
    }else
    { /* e_DiaDatFileType_Write */
        if (t < 1e-12)
            throw dbg_spintf("DiaDat_File::close - file is created, but no record is added (%s)!", name.c_str());
        writeHeader();
        if (file != NULL)
            fclose(file);
    }
    type = e_DiaDatFileType_None;
    return 0;
}

int8_t DiaDat_File::step(void)
{
    switch(type)
    {
        case e_DiaDatFileType_Read:
            readRecord();
            break;
        case e_DiaDatFileType_Write:
            writeRecord();
            break;
        default:
            throw dbg_spintf("DiaDat_File::step - file is not yet open/created (%s)!", name.c_str());
    }
    t = t + dT;
    recordCount++;
    return 0;
}

int8_t DiaDat_File::readHeader()
{
    return 0;
}

int8_t DiaDat_File::writeHeader()
{
    writeHeaderLine("DIAEXTENDED  {@:ENGLISH");
    writeHeaderLine("");
    writeHeaderLine("#BEGINGLOBALHEADER");
    writeHeaderLine("  1,WINDOWS 32Bit");
    const char *versionStr ="version: not yet implemented";
    writeHeaderLine("  2,LippiWare diadat_file %s", versionStr);
    writeHeaderLine("101,");
    writeHeaderLine("103,LippiWare diadat_file ", versionStr);
    time_t current_time = time(NULL);
    struct tm *tm = gmtime(&current_time);
    char dateBuff[64];
    strftime(dateBuff, sizeof(dateBuff), "%Y-%m-%d", tm);
    if (strftime(dateBuff, sizeof(dateBuff), "%Y-%m-%d", tm) == 0)
        throw dbg_spintf("DiaDat_File::writeHeader - strftime error (%s)!", name.c_str());
    writeHeaderLine("104,%s", dateBuff);
    if (strftime(dateBuff, sizeof(dateBuff), "%H-%M-%S", tm) == 0)
        throw dbg_spintf("DiaDat_File::writeHeader - strftime 2 error (%s)!", name.c_str());
    writeHeaderLine("105,%s", dateBuff);
    writeHeaderLine("111,9.900000e+034");
    writeHeaderLine("112,High -> Low");
    writeHeaderLine("#ENDGLOBALHEADER");
    writeHeaderLine("");
    writeHeaderLine("#BEGINCHANNELHEADER");
    writeHeaderLine("200,t");
    writeHeaderLine("201,timeaxis");
    writeHeaderLine("202,s");
    writeHeaderLine("210,IMPLICIT");
    writeHeaderLine("213,BLOCK");
    writeHeaderLine("214,REAL64");
    writeHeaderLine("220,%u", recordCount); /* number of records */
    writeHeaderLine("240,0.00000000");
    writeHeaderLine("241,%lf", dT);
    writeHeaderLine("250,0.00000000");
    writeHeaderLine("251,%lf", recordCount * dT);
    writeHeaderLine("252,NO");
    writeHeaderLine("253,increasing");
    writeHeaderLine("260,Time");
    writeHeaderLine("#ENDCHANNELHEADER");
    writeHeaderLine("");
    for (auto it = channels.begin() ; it != channels.end(); ++it)
        writeChannelHeader(*it);
    return 0;
}

int8_t DiaDat_File::writeChannelHeader(DiaDat_FileChannel *ch)
{
    writeHeaderLine("#BEGINCHANNELHEADER");
    writeHeaderLine("200,%s", ch->getName().c_str());
    writeHeaderLine("201,comment");
    writeHeaderLine("202,%s", ch->getUnit().c_str());
    writeHeaderLine("210,EXPLICIT");
    writeHeaderLine("211,%s", ch->getFileName().c_str());
    writeHeaderLine("213,BLOCK");
    writeHeaderLine("214,%s", ch->getDiaDatFileType().c_str());
    writeHeaderLine("220,%u", recordCount); /* number of records */
    writeHeaderLine("221,%u", ch->getFileOffset());
    writeHeaderLine("240,0.00000000");
    writeHeaderLine("241,%lf", 0.00000000);
    writeHeaderLine("250,0.00000000");
    writeHeaderLine("251,0.00000000");
    writeHeaderLine("260,Numeric");
    writeHeaderLine("#ENDCHANNELHEADER");
    writeHeaderLine("");
    return 0;
}

int8_t DiaDat_File::writeHeaderLine(const char *pszFormat, ...)
{
    va_list argptr;
    va_start(argptr, pszFormat);
    vfprintf(file, pszFormat, argptr);
    va_end(argptr);
    fputs("\n", file);
    return 0;
}

int8_t DiaDat_File::readRecord()
{
    return 0;
}

int8_t DiaDat_File::writeRecord()
{
    for (auto it = dataFiles.begin() ; it != dataFiles.end(); ++it)
        it->second->writeRecord();
    return 0;
}
