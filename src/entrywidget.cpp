// SPDX-FileCopyrightText: 2021 David Faure <faure@kde.org>
// SPDX-License-Identifier: BSD-3-Clause

#include "editdata.h"
#include "entrywidget.h"

#include <KOpeningHours/OpeningHours>

#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScreen>
#include <QVBoxLayout>

using namespace KOpeningHours;

EntryWidget::EntryWidget(EditData &editData, int idx, QWidget *parent)
    : QWidget(parent)
    , m_editData(editData)
    , m_index(idx)
{
    const EditData::Entry &entry = editData.at(idx);

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(1);
    auto *keyLabel = new QLabel(entry.key, this);
    lay->addWidget(keyLabel);
    auto *oldLabel = new QLabel(entry.oldHours, this);
    oldLabel->setMaximumWidth(screen()->availableSize().width() - 200);
    lay->addWidget(oldLabel);
    QHBoxLayout *hLayout = new QHBoxLayout;
    m_newHours = new QLineEdit(entry.newHours, this);
    m_newHours->setAutoFillBackground(true);
    hLayout->addWidget(m_newHours);
    m_errorLabel = new QLabel(this);
    hLayout->addWidget(m_errorLabel);
    auto *unfixableButton = new QPushButton(QStringLiteral("Unfixable"), this);
    unfixableButton->setCheckable(true);
    hLayout->addWidget(unfixableButton);
    connect(unfixableButton, &QPushButton::toggled,
            this, &EntryWidget::setUnfixable);
    lay->addLayout(hLayout);

    if (entry.newHours.isEmpty()) {
        m_newHours->setText(entry.oldHours);
    }
    revalidate();

    connect(m_newHours, &QLineEdit::textChanged,
            this, &EntryWidget::revalidate);
    connect(m_newHours, &QLineEdit::returnPressed,
            this, &EntryWidget::normalize);
}

void EntryWidget::save()
{
    m_editData.set(m_index, m_newHours->text());
}

void EntryWidget::setUnfixable(bool unfixable)
{
    m_editData.setUnfixable(m_index, unfixable);
    revalidate();
}

void EntryWidget::revalidate()
{
    const QString curText = m_newHours->text();
    OpeningHours parser(curText.toUtf8());
    QColor col = Qt::black;
    if (parser.error() == OpeningHours::SyntaxError) {
        m_errorLabel->setText(QStringLiteral("SYNTAX ERROR"));
        col = m_editData.isUnfixable(m_index) ? Qt::gray : Qt::red;
    } else {
        const QString normalized = QString::fromUtf8(parser.normalizedExpression());
        if (curText != normalized) {
            m_errorLabel->setText(QStringLiteral("Press Enter to normalize"));
            col = Qt::yellow;
        } else {
            m_errorLabel->setText(QStringLiteral("OK"));
            col = Qt::green;
        }
    }
    QPalette pal = m_newHours->palette();
    pal.setColor(QPalette::Base, QColor(col).lighter(180));
    m_newHours->setPalette(pal);
}

void EntryWidget::normalize()
{
    const QString curText = m_newHours->text();
    OpeningHours parser(curText.toUtf8());
    if (parser.error() != OpeningHours::SyntaxError) {
        const QString normalized = QString::fromUtf8(parser.normalizedExpression());
        if (curText != normalized) {
            m_newHours->setText(normalized);
        }
    }
}
