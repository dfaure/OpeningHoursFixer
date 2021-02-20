// SPDX-FileCopyrightText: 2021 David Faure <faure@kde.org>
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <QScrollArea>

class EntryWidget;
class EditData;
class MainWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit MainWidget(EditData &editData, const QString &fileName, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *ev) override;

private:
    void createUi();
    void save() const;

    std::vector<EntryWidget *> m_widgets;
    EditData &m_editData;
};
