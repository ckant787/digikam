/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-08-08
 * Description : a camera name parser class
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "cameranameparser.h"
#include "cameranameparser.moc"

// KDE includes

#include <kiconloader.h>
#include <klocale.h>

namespace Digikam
{

CameraNameParser::CameraNameParser()
                : SubParser(i18n("Camera Name"), SmallIcon("camera-photo"))
{
    addToken("[cam]", i18n("Camera Name"), i18n("camera name"));
}

void CameraNameParser::parseOperation(QString& parseString, const ParseInformation& info)
{
    QString cameraName = info.cameraName;

    QRegExp regExp("\\[cam\\]");
    regExp.setCaseSensitivity(Qt::CaseInsensitive);

    PARSE_LOOP_START(parseString, regExp)

    QString parsed = stringIsValid(cameraName) ? cameraName : QString();

    PARSE_LOOP_END(parseString, regExp, parsed)
}

} // namespace Digikam
