//////////////////////////////////////////////////////////////////////////////
//
//    DIGIKAMTHUMBNAIL.CPP
//
//    Copyright (C) 2003-2004 Renchi Raju <renchi at pooh.tam.uiuc.edu>
//                            Gilles CAULIER <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

#define XMD_H

// Qt Includes.

#include <qcstring.h>
#include <qstring.h>
#include <qimage.h>
#include <qdatastream.h>

// KDE includes.

#include <kdebug.h>
#include <kurl.h>
#include <kinstance.h>
#include <kio/global.h>
#include <kimageio.h>

// C Ansi includes.

extern "C" 
{
#include <stdlib.h>
#include <unistd.h>
#include <jpeglib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
}

// Local includes

#include "digikamthumbnail.h"

// X11 includes.

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Imlib2.h>

using namespace KIO;


kio_digikamthumbnailProtocol::kio_digikamthumbnailProtocol(const QCString &pool_socket,
                                                           const QCString &app_socket)
    : SlaveBase("kio_digikamthumbnail", pool_socket, app_socket)
{
}


kio_digikamthumbnailProtocol::~kio_digikamthumbnailProtocol()
{
}


void kio_digikamthumbnailProtocol::get(const KURL& url )
{
    size_ = metaData("size").toInt();
    
    if (size_ <= 0) 
        {
        error(KIO::ERR_INTERNAL, "No or invalid size specified");
        kdWarning() << "No or invalid size specified" << endl;
        return;
        }

    QImage img;
    
    if ( !loadJPEG(img, url.path()) )      // Try JPEG loading...
       {
       if ( !loadImlib2(img, url.path()) ) // Try to load with imlib2 API...
          {
          img.load(url.path());            // Try to load with QT/KDELib API...
          }
       }
    
    if (img.isNull()) 
        {
        error(KIO::ERR_INTERNAL, "Cannot create thumbnail for " + url.path());
        kdWarning() << "Cannot create thumbnail for " << url.path() << endl;
        return;
        }

    if (QMAX(img.width(),img.height()) != size_)
        img = img.smoothScale(size_, size_, QImage::ScaleMin);

    if (img.depth() != 32)
        img = img.convertDepth(32);
    
    QByteArray imgData;
    QDataStream stream( imgData, IO_WriteOnly );

    QString shmid = metaData("shmid");
    
    if (shmid.isEmpty()) 
        {
        stream << img;
        }
    else
        {
        void *shmaddr = shmat(shmid.toInt(), 0, 0);
        
        if (shmaddr == (void *)-1)
            {
            error(KIO::ERR_INTERNAL, "Failed to attach to shared memory segment " + shmid);
            kdWarning() << "Failed to attach to shared memory segment " << shmid << endl;
            return;
            }
            
        if (img.width() * img.height() > size_ * size_)
            {
            error(KIO::ERR_INTERNAL, "Image is too big for the shared memory segment");
            kdWarning() << "Image is too big for the shared memory segment" << endl;
            shmdt((char*)shmaddr);
            return;
            }
            
        stream << img.width() << img.height() << img.depth();
        memcpy(shmaddr, img.bits(), img.numBytes());
        shmdt((char*)shmaddr);
        }
    
    data(imgData);
    finished();
}


/////////////////////////////////////////////////////////////////////////////////////////
// JPEG Extraction

struct myjpeg_error_mgr : public jpeg_error_mgr 
{
    jmp_buf setjmp_buffer;
};

extern "C"
{
  static void myjpeg_error_exit(j_common_ptr cinfo)
  {
    myjpeg_error_mgr* myerr = 
      (myjpeg_error_mgr*) cinfo->err;

    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);
    kdWarning() << buffer << endl;
    longjmp(myerr->setjmp_buffer, 1);
  }
}

bool kio_digikamthumbnailProtocol::loadJPEG(QImage& image, const QString& path)
{
    QString format=QImageIO::imageFormat(path);
    if (format !="JPEG") return false;
    
    FILE* inputFile=fopen(path.latin1(), "rb");
    if(!inputFile)
        return false;

    struct jpeg_decompress_struct    cinfo;
    struct myjpeg_error_mgr jerr;

    // JPEG error handling - thanks to Marcus Meissner
    cinfo.err             = jpeg_std_error(&jerr);
    cinfo.err->error_exit = myjpeg_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
      jpeg_destroy_decompress(&cinfo);
      fclose(inputFile);
      return false;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, inputFile);
    jpeg_read_header(&cinfo, TRUE);

    int imgSize = QMAX(cinfo.image_width, cinfo.image_height);

    int scale=1;
    while(size_*scale*2<=imgSize) {
        scale*=2;
    }
    if(scale>8) scale=8;

    cinfo.scale_num=1;
    cinfo.scale_denom=scale;

    // Create QImage
    jpeg_start_decompress(&cinfo);

    QImage img;
    
    switch(cinfo.output_components) {
    case 3:
    case 4:
        img.create( cinfo.output_width, cinfo.output_height, 32 );
        break;
    case 1: // B&W image
        img.create( cinfo.output_width, cinfo.output_height,
                      8, 256 );
        for (int i=0; i<256; i++)
            img.setColor(i, qRgb(i,i,i));
        break;
    default:
        return false;
    }

    uchar** lines = img.jumpTable();
    while (cinfo.output_scanline < cinfo.output_height)
        jpeg_read_scanlines(&cinfo, lines + cinfo.output_scanline,
                            cinfo.output_height);
    jpeg_finish_decompress(&cinfo);

    // Expand 24->32 bpp
    if ( cinfo.output_components == 3 ) {
        for (uint j=0; j<cinfo.output_height; j++) {
            uchar *in = img.scanLine(j) + cinfo.output_width*3;
            QRgb *out = (QRgb*)( img.scanLine(j) );

            for (uint i=cinfo.output_width; i--; ) {
                in-=3;
                out[i] = qRgb(in[0], in[1], in[2]);
            }
        }
    }

    int newMax = QMAX(cinfo.output_width, cinfo.output_height);
    int newx = size_*cinfo.output_width / newMax;
    int newy = size_*cinfo.output_height / newMax;

    jpeg_destroy_decompress(&cinfo);
    fclose(inputFile);

    image = img.smoothScale(newx,newy);
    
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Load using Imlib2 API

bool kio_digikamthumbnailProtocol::loadImlib2(QImage& image, const QString& path)
{
    Imlib_Image imlib2_im = imlib_load_image_without_cache(path.latin1());
        
    if (imlib2_im == NULL) return false;
    
    imlib_context_set_image(imlib2_im);     
    
    org_width_  = imlib_image_get_width();
    org_height_ = imlib_image_get_height();
    
    if ( QMAX(org_width_, org_height_) != size_ )
        {
        imlib2_im = imlib_create_cropped_scaled_image(0, 0, 
                                                      org_width_, org_height_, 
                                                      size_, size_); 
        }

    new_width_  = imlib_image_get_width();
    new_height_ = imlib_image_get_height(); 
       
    image.create( new_width_, new_height_, 32 );
    image.setAlphaBuffer(true) ;
    
    DATA32 *data;
    
    for (int y = 0 ; y < new_height_ ; ++y)
       {
       data = imlib_image_get_data() + (DATA32)( y * new_width_ );
       memcpy(image.scanLine(y), data, new_width_ * 4 );
       }
    
    imlib_free_image();        
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// KIO slave registration

extern "C"
{
    int kdemain(int argc, char **argv)
    {
        KInstance instance( "kio_digikamthumbnail" );
        
        kdDebug() << "*** Starting kio_digikamthumbnail " << endl;
        
        if (argc != 4) {
            kdDebug() << "Usage: kio_digikamthumbnail  protocol domain-socket1 domain-socket2" << endl;
            exit(-1);
        }

        KImageIO::registerFormats();
        
        kio_digikamthumbnailProtocol slave(argv[2], argv[3]);
        slave.dispatchLoop();
        
        kdDebug() << "*** kio_digikamthumbnail Done" << endl;
        return 0;
    }
}
