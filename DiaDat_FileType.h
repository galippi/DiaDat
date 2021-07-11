/*
 * DiaDat_FileType.h
 *
 *  Created on: 2021. máj. 23.
 *      Author: liptakok
 */

#ifndef DIADAT_DIADAT_FILETYPE_H_
#define DIADAT_DIADAT_FILETYPE_H_

typedef enum
{
  e_DiaDatFileType_None,
  e_DiaDatFileType_Read,
  e_DiaDatFileType_Write,
}t_DiaDatFileType;

typedef enum
{
    e_DiaDatFileStoreType_Undef,
    e_DiaDatFileStoreType_Explicit,
    e_DiaDatFileStoreType_Implicit
}t_DiaDatFileStoreType;

#endif /* DIADAT_DIADAT_FILETYPE_H_ */
