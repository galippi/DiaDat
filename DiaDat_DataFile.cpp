/*
 * DiaDat_DataFile.cpp
 *
 *  Created on: 2021. máj. 19.
 *      Author: liptakok
 */

#include <stdlib.h>

#include <string>

#include "my_debug.h"

#include "DiaDat_File.h"

#include "DiaDat_DataFile.h"

const std::string DiaDat_FileChannel::getFileName() const
{
    return file->getName();
}

DiaDat_DataFile *DiaDat_FileChannel::getDataFile()
{
    return parent;
}

const std::string DiaDat_FileChannel::getDiaDatFileType() const
{
    return file->getDiaDatFileType();
}

DiaDat_FileChannelImplicit::DiaDat_FileChannelImplicit(DiaDat_File *_parent, ChannelData *chData) :
        DiaDat_FileChannel(_parent, chData)
{
}

DiaDat_FileChannelExplicit::DiaDat_FileChannelExplicit(DiaDat_File *_parent, ChannelData *chData) :
        DiaDat_FileChannel(_parent, chData)
{
}

const char *DiaDat_FileChannelExplicit::getFileName() const
{
    if (file != NULL)
        return file->getName().c_str();
    else
        throw dbg_spintf("DiaDat_FileChannelExplicit.getFileName - not yet implemented!");
}

DiaDat_DataFile::DiaDat_DataFile(DiaDat_File *_parent, DiaDat_Channel *ch, const std::string &dataFileName)
{
    filename = dataFileName;
    parent = _parent;
    channelCount = 0;
    datFileType = ch->getDiaDatFileType();
}

#if 0
DiaDat_DataFile::DiaDat_DataFile(DiaDat_File *_parent, const char *filenameBase, t_DiaDat_ChannelType type, const char *dataFileName)
{
    parent = _parent;
    blockSize = 0;
    block = NULL;
    channelCount = 0;
    this->type = type;
    const char *dataFileExt;
    switch(type)
    {
        case e_DiaDat_ChannelType_u8:
        {
            dataFileExt = ".u8";
            datFileType = "WORD8";
            break;
        }
        case e_DiaDat_ChannelType_s8:
        {
            dataFileExt = ".s8";
            datFileType = "WORD8";
            break;
        }
        case e_DiaDat_ChannelType_u16:
        {
            dataFileExt = ".u16";
            datFileType = "WORD16";
            break;
        }
        default:
            throw dbg_spintf("DiaDat_DataFile - Not implemented channel type %d!", type);
            break;
    }
    if (dataFileName != NULL)
        filename = dataFileName;
    else
        filename = std::string(filenameBase) + dataFileExt;
    file = NULL;
}
#endif

DiaDat_DataFile::~DiaDat_DataFile()
{
}

t_DiaDatFileType DiaDat_DataFile::getDirection() const
{
    return parent->getDirection();
}

void DiaDat_DataFile::addChannel(DiaDat_Channel *channel)
{
    channelCount++;
    channels.push_back(channel);
}

int8_t DiaDat_DataFile::writeRecord()
{
    return 0;
}

DiaDat_DataFileImplicit::DiaDat_DataFileImplicit(DiaDat_File *_parent, DiaDat_Channel *ch)
    : DiaDat_DataFile(_parent, ch, "IMPLICIT:")
{
}

uint8_t *DiaDat_DataFileImplicit::getBuffer()
{
    throw dbg_spintf("DiaDat_DataFileImplicit::getBuffe - not yet implemented!");
}

int8_t DiaDat_DataFileImplicit::writeRecord()
{
    for (auto it = channels.begin(); it < channels.end(); it++)
    {
        DiaDat_Channel *ch = (*it);
        ch->write();
    }
    return 0;
}

int8_t DiaDat_DataFileImplicit::readRecord()
{
    for (auto it = channels.begin(); it < channels.end(); it++)
    {
        DiaDat_Channel *ch = (*it);
        ch->read();
    }
    return 0;
}

DiaDat_DataFileExplicit::DiaDat_DataFileExplicit(DiaDat_File *_parent, DiaDat_Channel *ch, const std::string &dataFileName)
    : DiaDat_DataFile(_parent, ch, dataFileName)
{
    if (_parent->getDirection() == e_DiaDatFileType_Write)
    {
        file = fopen(dataFileName.c_str(), "wb");
        if (file == NULL)
            throw dbg_spintf("DiaDat_DataFileExplicit::ctor - unable to create data file %s!", dataFileName.c_str());
    }else
    {
        file = fopen(dataFileName.c_str(), "rb");
        if (file == NULL)
            throw dbg_spintf("DiaDat_DataFileExplicit::ctor - unable to open data file %s!", dataFileName.c_str());
    }
    blockSize = 0;
    block = NULL;
}

DiaDat_DataFileExplicit::~DiaDat_DataFileExplicit()
{
    if (file != NULL)
        fclose(file);
    if (block != NULL)
        free(block);
}

uint8_t *DiaDat_DataFileExplicit::getBuffer()
{
    throw dbg_spintf("DiaDat_DataFileExplicit::getBuffe - not yet implemented!");
}

void DiaDat_DataFileExplicit::addChannel(DiaDat_Channel *channel)
{
    DiaDat_DataFile::addChannel(channel);
    if (block != NULL)
        throw dbg_spintf("DiaDat_DataFile::addChannel - not allowed state (%s)!", channel->getName().c_str());
    if (getDirection() == e_DiaDatFileType_Write)
    {
        channel->setBlockOffset(blockSize);
        blockSize += channel->getDataHandler()->getDataSize();
    }else
    {
        uint32_t varBlockSize = channel->getFileOffset() + channel->getDataSize();
        if (blockSize < varBlockSize)
            blockSize = varBlockSize;
    }
}

int8_t DiaDat_DataFileExplicit::writeRecord()
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
        DiaDat_Channel *ch = (*it);
        ch->write(block);
    }
    if (fwrite(block, 1, blockSize, file) != blockSize)
    {
        throw dbg_spintf("DiaDat_DataFile::writeRecord - fwrite failure. Is disk full? (%u)", blockSize);
        return 1;
    }
    return 0;
}

int8_t DiaDat_DataFileExplicit::readRecord()
{
    if (block == NULL)
    {
        if (blockSize < 1)
            throw dbg_spintf("DiaDat_DataFileExplicit::readRecord - invalid block size (%u)!", blockSize);
        block = (uint8_t*)malloc(blockSize);
    }
    if (fread(block, 1, blockSize, file) != blockSize)
        throw dbg_spintf("DiaDat_DataFileExplicit::readRecord - error loading block!");
    for (auto it = channels.begin(); it < channels.end(); it++)
    {
        DiaDat_Channel *ch = (*it);
        ch->read(block);
    }
    return 0;
}
