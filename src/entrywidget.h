// SPDX-FileCopyrightText: 2021 David Faure <faure@kde.org>
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <QWidget>

class EditData;
class QLineEdit;
class QLabel;

class EntryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EntryWidget(EditData &editData, int idx, QWidget *parent = nullptr);

    void save();

private:
    void removeDots();
    void showContextMenu(const QPoint &pos);
    void setUnfixable(bool unfixable);
    void revalidate();
    void normalize();

    QLineEdit *m_newHours;
    QLabel *m_errorLabel;
    EditData &m_editData;
    int m_index;
};
