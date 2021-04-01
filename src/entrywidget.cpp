// SPDX-FileCopyrightText: 2021 David Faure <faure@kde.org>
// SPDX-License-Identifier: BSD-3-Clause

#include "editdata.h"
#include "entrywidget.h"

#include <KOpeningHours/OpeningHours>

#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
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
    auto *keyLabel = new QLabel(QStringLiteral("<a href=\"https://www.openstreetmap.org/%1\">%1</a>").arg(entry.key), this);
    keyLabel->setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::LinksAccessibleByMouse);
    keyLabel->setOpenExternalLinks(true);
    lay->addWidget(keyLabel);
    auto *oldLabel = new QLabel(entry.oldHours, this);
    oldLabel->setMaximumWidth(screen()->availableSize().width() - 200);
    oldLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    oldLabel->setIndent(5); // try to align with the text in the QLineEdit. Could use QStyle...
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

    m_newHours->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_newHours, &QLineEdit::customContextMenuRequested,
            this, &EntryWidget::showContextMenu);
}

void EntryWidget::save()
{
    m_editData.set(m_index, m_newHours->text());
}

void EntryWidget::removeDots()
{
    QString text = m_newHours->text();
    text.remove(QLatin1Char('.'));
    m_newHours->setText(text);
}

void EntryWidget::showContextMenu(const QPoint &pos)
{
    QMenu menu(this);
    menu.addAction(QStringLiteral("Remove '.'"), this, &EntryWidget::removeDots);
    menu.addAction(QStringLiteral("'+' -> ','"), this, [this]() {
        QString text = m_newHours->text();
        m_newHours->setText(text.replace(QLatin1Char('+'), QLatin1Char(',')));
    });
    menu.addAction(QStringLiteral("', ' -> '; '"), this, [this]() {
        QString text = m_newHours->text();
        m_newHours->setText(text.replace(QLatin1String(", "), QLatin1String("; ")));
    });
    menu.addAction(QStringLiteral("',' -> ':'"), this, [this]() {
        QString text = m_newHours->text();
        m_newHours->setText(text.replace(QLatin1Char(','), QLatin1Char(':')));
    });
    menu.addAction(QStringLiteral("';' -> ','"), this, [this]() {
        QString text = m_newHours->text();
        m_newHours->setText(text.replace(QLatin1Char(';'), QLatin1Char(',')));
    });
    menu.addAction(QStringLiteral("'.' -> ':'"), this, [this]() {
        QString text = m_newHours->text();
        m_newHours->setText(text.replace(QLatin1Char('.'), QLatin1Char(':')));
    });
    menu.addAction(QStringLiteral("'-' -> ';'"), this, [this]() {
        QString text = m_newHours->text();
        m_newHours->setText(text.replace(QLatin1Char('-'), QLatin1Char(';')));
    });
    menu.addAction(QStringLiteral("'/' -> ','"), this, [this]() {
        QString text = m_newHours->text();
        m_newHours->setText(text.replace(QLatin1Char('/'), QLatin1Char(',')));
    });
    menu.exec(m_newHours->mapToGlobal(pos));
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
    if (parser.error() == OpeningHours::SyntaxError
            || parser.error() == OpeningHours::IncompatibleMode) {
        m_errorLabel->setText(QStringLiteral("SYNTAX ERROR"));
        col = m_editData.isUnfixable(m_index) ? Qt::gray : Qt::red;
    } else {
        const QString normalized = QString::fromUtf8(parser.simplifiedExpression());
        if (curText != normalized) {
            m_errorLabel->setText(QStringLiteral("Press Enter to normalize"));
            col = Qt::yellow;
        } else {
            m_errorLabel->setText(QStringLiteral("OK"));
            if (curText.contains(QLatin1String("+,")) || curText.contains(QLatin1String("+;")))
                col = QColor(180, 100, 20); // orange
            else
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
    if (parser.error() != OpeningHours::SyntaxError &&
            parser.error() != OpeningHours::IncompatibleMode) {
        const QString normalized = QString::fromUtf8(parser.simplifiedExpression());
        if (curText != normalized) {
            m_newHours->setText(normalized);
        }
    }
}
