// SPDX-FileCopyrightText: 2021 David Faure <faure@kde.org>
// SPDX-License-Identifier: BSD-3-Clause

#include "mainwidget.h"
#include "editdata.h"
#include "entrywidget.h"

#include <QCloseEvent>
#include <QPushButton>
#include <QScreen>
#include <QVBoxLayout>

MainWidget::MainWidget(EditData &editData, QWidget *parent)
    : QScrollArea(parent), m_editData(editData)
{
    setWidget(new QWidget);
    setWidgetResizable(true);
    createUi();

    setGeometry(screen()->availableGeometry().adjusted(50, 50, -50, -50));
}

void MainWidget::closeEvent(QCloseEvent *ev)
{
    // ask for confirmation?
    ev->accept();
    save();
}

void MainWidget::createUi()
{
    auto *lay = new QVBoxLayout(widget());
    const int count = m_editData.count();
    m_widgets.reserve(count);
    for (int i = 0; i < count; ++i) {
        EntryWidget *w = new EntryWidget{m_editData, i, this};
        lay->addWidget(w);
        m_widgets.push_back(w);
    }
    auto *doneButton = new QPushButton(QStringLiteral("All done"), this);
    lay->addWidget(doneButton);
    connect(doneButton, &QPushButton::clicked,
            this, [this]() { m_editData.setDone(); });
}

void MainWidget::save()
{
    for (EntryWidget *w : m_widgets) {
        w->save();
    }
    m_editData.save();
}
