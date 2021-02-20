// SPDX-FileCopyrightText: 2021 David Faure <faure@kde.org>
// SPDX-License-Identifier: BSD-3-Clause

#include "mainwidget.h"

#include <QApplication>
#include <editdata.h>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    EditData data;
    const QStringList args = app.arguments();
    if (args.count() < 2) {
        qInfo("Expected a filename as argument, the txt file to edit");
        return 1;
    }
    const QString fileName = args.at(1);
    if (!data.load(fileName)) {
        return 2;
    }
    MainWidget w(data, fileName);
    w.show();
    return app.exec();
}
