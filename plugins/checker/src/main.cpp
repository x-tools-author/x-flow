/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include <QApplication>

#include "xflowpluginchecker.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    xFlowPluginChecker w;
    w.show();
    return a.exec();
}