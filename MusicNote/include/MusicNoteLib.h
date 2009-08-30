#ifndef __MUSICNOTE_LIB_H__EBEE094C_FF6E_43a1_A6CE_D619564F9C6A__
#define __MUSICNOTE_LIB_H__EBEE094C_FF6E_43a1_A6CE_D619564F9C6A__

/// <Summary>
/// This header file can be included directly in your project or through
/// MusicNoteLib.h of the MusicNoteDll project. If included directly, this
/// will be built directly as a satic library. If included through MusicNoteDll
/// this will use dllImports through MUSICNOTELIB_API
/// </Summary>
#ifndef MUSICNOTELIB_API
#define MUSICNOTELIB_API
#endif // MUSICNOTELIB_API

#include "Player.h"

namespace MusicNoteLib
{

extern "C"
{
    MUSICNOTELIB_API void* GetCarnaticMusicNoteReader();

    MUSICNOTELIB_API void* GetCarnaticMusicNoteReader();

} // extern "C"


} // namespace MusicNoteLib

/*! \mainpage MusicNoteLib, C++ Music Programming Library

\section overview Overview

This is the Overview section. The Library allows one to play music notes directly from C/C++ programs

\section usage Usage

\section history History

\section theory Theory of Operation

\section limitations Limitations

\section future The Future

*/

#endif // __MUSICNOTE_LIB_H__EBEE094C_FF6E_43a1_A6CE_D619564F9C6A__