/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2016-04-21
 * Description : Qt Multimedia based video thumbnailer 
 *
 * Copyright (C) 2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef VIDEOTHUMBNAILER_P_H
#define VIDEOTHUMBNAILER_P_H

#include "videothumbnailer.h"

// Qt includes

#include <QThread>
#include <QMediaPlayer>
#include <QVideoFrame>
#include <QVideoProbe>
#include <QString>

namespace Digikam
{

class VideoThumbnailer::Private : public QThread
{
    Q_OBJECT

public:

    Private(VideoThumbnailer* const p);

    QString fileName() const;
    QString filePath() const;

    QImage imageFromVideoFrame(const QVideoFrame&) const;

public Q_SLOTS:

    void slotMediaStatusChanged(QMediaPlayer::MediaStatus);
    void slotHandlePlayerError();
    void slotProcessframe(QVideoFrame);

public:

    bool              createStrip;
    int               thumbSize;
    QMediaPlayer*     player;
    QVideoProbe*      probe;
    QMediaContent     media;

private:

    void run();

private:

    qint64            position;
    QImage            strip;
    QVideoFrame       frame;
    VideoThumbnailer* dd;
};

}  // namespace Digikam

#endif /* VIDEOTHUMBNAILER_P_H */