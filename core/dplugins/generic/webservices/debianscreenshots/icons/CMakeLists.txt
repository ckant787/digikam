#
# Copyright (c) 2010-2019 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

file(GLOB _icons_files *.png *.svgz)

ecm_install_icons(ICONS       ${_icons_files}
                  DESTINATION ${ICON_INSTALL_DIR}
)
