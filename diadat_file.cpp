#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "DiaDat_DataFile.h"
#include "channel_data.h"

#include "my_debug.h"

#include "diadat_file.h"

DiaDat_File::DiaDat_File()
{
    init();
}

DiaDat_File::DiaDat_File(const char *filename, t_DiaDatFileType _type)
{
    init();
    switch(_type)
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
    if ((type == e_DiaDatFileType_None) || (file == NULL))
        throw dbg_spintf("DiaDat_File::dtor - file is not yet open/created (%s)!", name.c_str());
    if (recordCount == 0)
        throw dbg_spintf("DiaDat_File::dtor - file is empty (%s)!", name.c_str());
    writeHeader();
    for (auto it = dataFiles.begin() ; it != dataFiles.end(); ++it)
    {
        it->second->close();
        delete it->second;
    }
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

DiaDat_Channel *DiaDat_File::addChannel(ChannelData *chData)
{
    if (type != e_DiaDatFileType_Read)
        throw dbg_spintf("DiaDat_File::addChannel - channel cannot be added for this type of file (%d - %s)!", type, chData->chName.c_str());
    auto it = channels.find(chData->chName.c_str());
    if (it != channels.end())
        throw dbg_spintf("Duplicated channel name %s in file %s!", chData->chName.c_str(), name.c_str());
    DiaDat_FileChannel *ch ;
    if (chData->storeType == e_DiaDatFileStoreType_Implicit)
    {
        ch = new DiaDat_FileChannelImplicit(this, chData);
    }else
    {
        //t_DiaDat_ChannelType chType = c_DiaDat_ChannelTypeBase::convert2type(chData->storeType.c_str());
        //auto fileChannel = getDataFile(chType);
        ch = new DiaDat_FileChannelExplicit(this, chData);
    }
    DiaDat_DataFile *fileChannel = ch->getDataFile();
    dataFiles.insert(std::pair<std::string, DiaDat_DataFile*>(fileChannel->getName(), fileChannel));
    channels[chData->chName.c_str()] = ch;
    return ch;
}

DiaDat_Channel *DiaDat_File::createChannel(const char *name, t_DiaDat_ChannelType chType, void *var)
{
    (void)var;
#if 0
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
#endif
    if (type != e_DiaDatFileType_Write)
        throw dbg_spintf("DiaDat_File::createChannel - channel cannot be created for this type of file (%d - %s)!", type, name);
    auto it = channels.find(name);
    if (it != channels.end())
    {
        throw dbg_spintf("Duplicated channel name %s!", name);
        exit(1);
    }
    DiaDat_Channel * ch = DiaDat_Channel::createChannel(this, name, chType);
    channels[name] = ch;
    return ch;
}

void DiaDat_File::connectVar(const char *chName, void *var)
{
    DiaDat_Channel *ch = getChannel(chName);
    ch->getDataHandler()->set(var);
}

DiaDat_Channel *DiaDat_File::getChannel(const char *chName)
{
    auto it = channels.find(chName);
    if (it == channels.end())
        throw dbg_spintf("Error: unable to connect variable to channel name %s!", name);
     return it->second;
}

#if 0
DiaDat_Channel *DiaDat_File::getChannel(int32_t chIdx)
{
  return channels[chIdx];
}
#endif

#if 0
DiaDat_DataFile *DiaDat_File::getDataFile(t_DiaDat_ChannelType type, const char *filename)
{
    std::map<t_DiaDat_ChannelType, DiaDat_DataFile*>::iterator it = dataFiles.find(type);
    if (it == dataFiles.end())
    {
        DiaDat_DataFile *file = new DiaDat_DataFile(this, name.c_str(), type, filename);
        dataFiles[type] = file;
        return file;
    }else
        return it->second;
}
#endif

DiaDat_DataFile *DiaDat_File::registerChannel(DiaDat_Channel *ch, const std::string &dataFileName, t_DiaDatFileStoreType storeType)
{
    DiaDat_DataFile *file;
    std::map<std::string, DiaDat_DataFile*>::iterator it = dataFiles.find(dataFileName);
    if (it == dataFiles.end())
    {
        if (storeType == e_DiaDatFileStoreType_Explicit)
            file = new DiaDat_DataFileExplicit(this, ch, dataFileName);
        else
            file = new DiaDat_DataFileImplicit(this, ch);
        dataFiles[dataFileName] = file;
    }else
        file = it->second;
    file->addChannel(ch);
    channels[ch->getName()] = ch;
    //if (!file->isImplicit())
    //{
    //    DiaDat_DataFileExplicit *fe = dynamic_cast<DiaDat_DataFileExplicit *>(file);
    //    fe->addChannel(ch);
    //}
    return file;
}

DiaDat_DataFile *DiaDat_File::getDataFile(const char *filename)
{
    return dataFiles[filename];
}

int8_t DiaDat_File::close(void)
{
    if (type == e_DiaDatFileType_None)
        throw dbg_spintf("DiaDat_File::close - file is not yet open/created (%s)!", name.c_str());
    if (type == e_DiaDatFileType_Read)
    {
        if (file != NULL)
            fclose(file);
        for (auto it = dataFiles.begin() ; it != dataFiles.end(); ++it)
            it->second->close();
    }else
    { /* e_DiaDatFileType_Write */
        if (t < 1e-12)
            throw dbg_spintf("DiaDat_File::close - file is created, but no record is added (%s)!", name.c_str());
        writeHeader();
        if (file != NULL)
            fclose(file);
        for (auto it = dataFiles.begin() ; it != dataFiles.end(); ++it)
            it->second->close();
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
            if (dT < 1e-12)
                throw dbg_spintf("DiaDat_File::step - dT is not yet set!", name.c_str());
            writeRecord();
            break;
        default:
            throw dbg_spintf("DiaDat_File::step - file is not yet open/created (%s)!", name.c_str());
    }
    t = t + dT;
    recordCount++;
    return 0;
}

void DiaDat_File::chomp(char *line)
{
    int len = strlen(line);
    while(len > 0)
    {
        len--;
        if ((line[len] == '\r') || (line[len] == '\n') || (line[len] == '\t') || (line[len] == ' '))
            line[len] = 0; // cut the white spaces from the end of the line
        else
            break;
    }
}

typedef enum
{
    e_FileBegin,
    e_BeforeFileHeader,
    e_FileHeader,
    e_AfterFileHeader,
    e_Channel
}t_ReadHeaderState;

typedef struct
{
    int headerType;
    char *value;
}t_HeaderInfo;

static t_HeaderInfo processLine(const char *filename, char *line)
{
    static char val[1024];
    t_HeaderInfo headerInfo;
    if (sscanf(line, "%d,", &headerInfo.headerType) != 1)
        throw dbg_spintf("DiaDat_File::processLine - sscanf error (%s - %s)!", filename, line);
    char *pos = strchr(line, ',');
    if (pos == NULL)
        throw dbg_spintf("DiaDat_File::processLine - strchr error (%s - %s)!", filename, line);
    strcpy(val, pos + 1);
    headerInfo.value = val;
    return headerInfo;
}

int8_t DiaDat_File::readHeader()
{
    char line[1024];
    t_ReadHeaderState headerState = e_FileBegin;
    ChannelData channelData;
    int lineNum = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        lineNum++;
        chomp(line);
        if (line[0] == 0)
            continue;
        switch(headerState)
        {
            case e_FileBegin:
                if (strcmp(line, "DIAEXTENDED  {@:ENGLISH") != 0)
                    throw dbg_spintf("DiaDat_File::readHeader - invalid file header (%s - %s)!", name.c_str(), line);
                headerState = e_BeforeFileHeader;
                break;
            case e_BeforeFileHeader:
                if (strcmp(line, "#BEGINGLOBALHEADER") != 0)
                    throw dbg_spintf("DiaDat_File::readHeader - e_BeforeFileHeader - invalid file header (%s - %s)!", name.c_str(), line);
                headerState = e_FileHeader;
                break;
            case e_FileHeader:
                if (strcmp(line, "#ENDGLOBALHEADER") == 0)
                    headerState = e_AfterFileHeader;
                else
                {
                    t_HeaderInfo headerInfo = processLine(name.c_str(), line);
                    switch (headerInfo.headerType)
                    {
                        case 1:
                        case 2:
                        case 101:
                        case 103:
                        case 104:
                        case 105:
                        case 111:
                        case 112:
                            // not yet used value
                            break;
                        default:
                            throw dbg_spintf("DiaDat_File::readHeader - invalid file header type (%s - line=%s type=%d val=%s)!", name.c_str(), line, headerInfo.headerType, headerInfo.value);
                    }
                }
                break;
            case e_AfterFileHeader:
                if (strcmp(line, "#BEGINCHANNELHEADER") != 0)
                    throw dbg_spintf("DiaDat_File::readHeader - e_AfterFileHeader - invalid file header (%s - %s)!", name.c_str(), line);
                headerState = e_Channel;
                channelData.clear();
                break;
            case e_Channel:
                if (strcmp(line, "#ENDCHANNELHEADER") == 0)
                {
                    headerState = e_AfterFileHeader;
                    if (!channelData.isEmpty())
                    {
                        if (!channelData.isOk())
                            throw dbg_spintf("DiaDat_File::readHeader - channel is not ok (%s - line=%d)!", name.c_str(), lineNum);
                        addChannel(&channelData);
                        channelData.clear();
                    }
                }
                else
                {
                    t_HeaderInfo headerInfo = processLine(name.c_str(), line);
                    switch (headerInfo.headerType)
                    {
                        case 200:
                            channelData.chName = headerInfo.value;
                            break;
                        case 201: // channel description - not yet used value
                            break;
                        case 202:
                            channelData.unit = headerInfo.value;
                            break;
                        case 210:
                            if (strcmp(headerInfo.value, "IMPLICIT") == 0)
                                channelData.storeType = e_DiaDatFileStoreType_Implicit;
                            else if (strcmp(headerInfo.value, "EXPLICIT") == 0)
                                channelData.storeType = e_DiaDatFileStoreType_Explicit;
                            else
                                throw dbg_spintf("DiaDat_File::readHeader - invalid channel header store type (%s - line=%s type=%d val=%s)!", name.c_str(), line, headerInfo.headerType, headerInfo.value);
                            break;
                        case 211:
                            channelData.filename = headerInfo.value;
                            break;
                        case 213:
                            if (strcmp(headerInfo.value, "BLOCK") != 0)
                                throw dbg_spintf("DiaDat_File::readHeader - invalid channel header store format (%s - line=%s type=%d val=%s)!", name.c_str(), line, headerInfo.headerType, headerInfo.value);
                            break;
                        case 214:
                            channelData.dataType = headerInfo.value;
                            break;
                        case 220:
                            channelData.recordCount = headerInfo.value;
                            break;
                        case 221:
                            channelData.channelIndex = headerInfo.value;
                            break;
                        case 240:
                            channelData.offset = headerInfo.value;
                            break;
                        case 241:
                            channelData.resolution = headerInfo.value;
                            break;
                        case 250:
                            channelData.min = headerInfo.value;
                            break;
                        case 251:
                            channelData.max = headerInfo.value;
                            break;
                        case 252:
                            if ((channelData.storeType != e_DiaDatFileStoreType_Implicit) || (strcmp(headerInfo.value, "NO") != 0))
                                throw dbg_spintf("DiaDat_File::readHeader - invalid channel header store format (%s - line=%s type=%d val=%s storeType=%d)!", name.c_str(), line, headerInfo.headerType, headerInfo.value, channelData.storeType);
                            break;
                        case 253:
                            if ((channelData.storeType != e_DiaDatFileStoreType_Implicit) || (strcmp(headerInfo.value, "increasing") != 0))
                                throw dbg_spintf("DiaDat_File::readHeader - invalid channel header store format (%s - line=%s type=%d val=%s storeType=%d)!", name.c_str(), line, headerInfo.headerType, headerInfo.value, channelData.storeType);
                            break;
                        case 260:
                            if (! (((channelData.storeType == e_DiaDatFileStoreType_Implicit) && (strcmp(headerInfo.value, "Time") == 0)) ||
                                   ((channelData.storeType == e_DiaDatFileStoreType_Explicit) && (strcmp(headerInfo.value, "Numeric") == 0))))
                                throw dbg_spintf("DiaDat_File::readHeader - invalid channel header store format (%s - line=%s type=%d val=%s storeType=%d)!", name.c_str(), line, headerInfo.headerType, headerInfo.value, channelData.storeType);
                            break;
                        default:
                            throw dbg_spintf("DiaDat_File::readHeader - invalid channel header type (%s - line=%s type=%d val=%s)!", name.c_str(), line, headerInfo.headerType, headerInfo.value);
                    }
                }
                break;
            default:
                throw dbg_spintf("DiaDat_File::readHeader - invalid header state (%s - %d)!", name.c_str(), headerState);
                break;
        }
    }
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
        writeChannelHeader(it->second);
    return 0;
}

int8_t DiaDat_File::writeChannelHeader(DiaDat_Channel *ch)
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
    writeHeaderLine("221,%u", (ch->getFileOffset() / ch->getDataSize()) + 1);
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
    for (auto it = dataFiles.begin() ; it != dataFiles.end(); ++it)
        it->second->readRecord();
    return 0;
}

int8_t DiaDat_File::writeRecord()
{
    for (auto it = dataFiles.begin() ; it != dataFiles.end(); ++it)
        it->second->writeRecord();
    return 0;
}
