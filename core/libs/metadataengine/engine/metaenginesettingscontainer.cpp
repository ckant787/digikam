/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2010-08-20
 * Description : MetaEngine Settings Container.
 *
 * Copyright (C) 2010-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "metaenginesettingscontainer.h"

// Qt includes

#include <QStringList>

// KDE includes

#include <kconfiggroup.h>

// Local includes

#include "metaenginesettings.h"

namespace Digikam
{

MetaEngineSettingsContainer::MetaEngineSettingsContainer()
    : exifRotate(true),
      exifSetOrientation(true),
      saveComments(false),
      saveDateTime(false),
      savePickLabel(false),
      saveColorLabel(false),
      saveRating(false),
      saveTemplate(false),
      saveTags(false),
      saveFaceTags(false),
      writeRawFiles(false),
      updateFileTimeStamp(true),
      rescanImageIfModified(false),
      clearMetadataIfRescan(false),
      useXMPSidecar4Reading(false),
      useLazySync(false),
      metadataWritingMode(MetaEngine::WRITE_TO_FILE_ONLY),
      rotationBehavior(RotatingFlags | RotateByLosslessRotation),
      sidecarExtensions(QStringList())
{
}

MetaEngineSettingsContainer::~MetaEngineSettingsContainer()
{
}

void MetaEngineSettingsContainer::readFromConfig(KConfigGroup& group)
{
    exifRotate            = group.readEntry("EXIF Rotate",                 true);
    exifSetOrientation    = group.readEntry("EXIF Set Orientation",        true);

    saveTags              = group.readEntry("Save Tags",                   false);
    saveTemplate          = group.readEntry("Save Template",               false);
    saveFaceTags          = group.readEntry("Save FaceTags",               false);

    saveComments          = group.readEntry("Save EXIF Comments",          false);
    saveDateTime          = group.readEntry("Save Date Time",              false);
    savePickLabel         = group.readEntry("Save Pick Label",             false);
    saveColorLabel        = group.readEntry("Save Color Label",            false);
    saveRating            = group.readEntry("Save Rating",                 false);

    writeRawFiles         = group.readEntry("Write Metadata To RAW Files", false);
    useXMPSidecar4Reading = group.readEntry("Use XMP Sidecar For Reading", false);
    metadataWritingMode   = (MetaEngine::MetadataWritingMode)
                            group.readEntry("Metadata Writing Mode",       (int)MetaEngine::WRITE_TO_FILE_ONLY);
    updateFileTimeStamp   = group.readEntry("Update File Timestamp",       true);
    rescanImageIfModified = group.readEntry("Rescan File If Modified",     false);
    clearMetadataIfRescan = group.readEntry("Clear Metadata If Rescan",    false);
    useLazySync           = group.readEntry("Use Lazy Synchronization",    false);

    rotationBehavior      = NoRotation;

    sidecarExtensions     = group.readEntry("Custom Sidecar Extensions",   QStringList());

    if (group.readEntry("Rotate By Internal Flag", true))
    {
        rotationBehavior |= RotateByInternalFlag;
    }

    if (group.readEntry("Rotate By Metadata Flag", true))
    {
        rotationBehavior |= RotateByMetadataFlag;
    }

    if (group.readEntry("Rotate Contents Lossless", true))
    {
        rotationBehavior |= RotateByLosslessRotation;
    }

    if (group.readEntry("Rotate Contents Lossy", false))
    {
        rotationBehavior |= RotateByLossyRotation;
    }
}

void MetaEngineSettingsContainer::writeToConfig(KConfigGroup& group) const
{
    group.writeEntry("EXIF Rotate",                 exifRotate);
    group.writeEntry("EXIF Set Orientation",        exifSetOrientation);

    group.writeEntry("Save Tags",                   saveTags);
    group.writeEntry("Save Template",               saveTemplate);
    group.writeEntry("Save FaceTags",               saveFaceTags);

    group.writeEntry("Save EXIF Comments",          saveComments);
    group.writeEntry("Save Date Time",              saveDateTime);
    group.writeEntry("Save Pick Label",             savePickLabel);
    group.writeEntry("Save Color Label",            saveColorLabel);
    group.writeEntry("Save Rating",                 saveRating);

    group.writeEntry("Write RAW Files",             writeRawFiles);
    group.writeEntry("Use XMP Sidecar For Reading", useXMPSidecar4Reading);
    group.writeEntry("Metadata Writing Mode",       (int)metadataWritingMode);
    group.writeEntry("Update File Timestamp",       updateFileTimeStamp);
    group.writeEntry("Rescan File If Modified",     rescanImageIfModified);
    group.writeEntry("Clear Metadata If Rescan",    clearMetadataIfRescan);

    group.writeEntry("Rotate By Internal Flag",     bool(rotationBehavior & RotateByInternalFlag));
    group.writeEntry("Rotate By Metadata Flag",     bool(rotationBehavior & RotateByMetadataFlag));
    group.writeEntry("Rotate Contents Lossless",    bool(rotationBehavior & RotateByLosslessRotation));
    group.writeEntry("Rotate Contents Lossy",       bool(rotationBehavior & RotateByLossyRotation));
    group.writeEntry("Use Lazy Synchronization",    useLazySync);

    group.writeEntry("Custom Sidecar Extensions",   sidecarExtensions);
}

QDebug operator<<(QDebug dbg, const MetaEngineSettingsContainer& inf)
{
    dbg.nospace() << "[MetaEngineSettingsContainer] exifRotate("
                  << inf.exifRotate << "), ";
    dbg.nospace() << "exifSetOrientation("
                  << inf.exifSetOrientation << "), ";
    dbg.nospace() << "saveComments("
                  << inf.saveComments << "), ";
    dbg.nospace() << "saveDateTime("
                  << inf.saveDateTime << "), ";
    dbg.nospace() << "savePickLabel("
                  << inf.saveColorLabel << "), ";
    dbg.nospace() << "saveColorLabel("
                  << inf.savePickLabel << "), ";
    dbg.nospace() << "saveRating("
                  << inf.saveRating << "), ";
    dbg.nospace() << "saveTemplate("
                  << inf.saveTemplate << "), ";
    dbg.nospace() << "saveTags("
                  << inf.saveTags << "), ";
    dbg.nospace() << "saveFaceTags("
                  << inf.saveFaceTags << "), ";
    dbg.nospace() << "writeRawFiles("
                  << inf.writeRawFiles << "), ";
    dbg.nospace() << "updateFileTimeStamp("
                  << inf.updateFileTimeStamp << "), ";
    dbg.nospace() << "rescanImageIfModified("
                  << inf.rescanImageIfModified << "), ";
    dbg.nospace() << "clearMetadataIfRescan("
                  << inf.clearMetadataIfRescan << "), ";
    dbg.nospace() << "useXMPSidecar4Reading("
                  << inf.useXMPSidecar4Reading << "), ";
    dbg.nospace() << "useLazySync("
                  << inf.useLazySync << "), ";
    dbg.nospace() << "metadataWritingMode("
                  << inf.metadataWritingMode << "), ";
    dbg.nospace() << "rotationBehavior("
                  << inf.rotationBehavior << "), ";
    dbg.nospace() << "sidecarExtensions("
                  << inf.sidecarExtensions << ")";

    return dbg.space();
}

} // namespace Digikam
