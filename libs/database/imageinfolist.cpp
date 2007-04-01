/* ============================================================
 * Authors: Marcel Wiesweg
 * Date   : 2007-03-20
 * Description : database interface.
 * 
 * Copyright 2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#include "imageinfo.h"
#include "imageinfolist.h"

namespace Digikam
{

ImageInfoList ImageInfoList::copy()
{
    ImageInfoList copy;
    for (iterator it = begin(); it != end(); ++it)
    {
        ImageInfo *info = new ImageInfo(*(*it));
        copy.append(info);
    }
    return copy;
}


}

