#include "editdata.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>

EditData::EditData()
{
}

bool EditData::load(const QString &fileName)
{
    const QString dir = QFileInfo(fileName).absolutePath();
    loadBlacklist(dir);

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    while (!file.atEnd()) {
        Entry entry;
        entry.key = QString::fromLatin1(file.readLine().chopped(1));
        entry.oldHours = QString::fromUtf8(file.readLine().mid(6).chopped(1));
        entry.newHours = QString::fromUtf8(file.readLine().mid(6).chopped(1));
        m_entries.push_back(std::move(entry));
    }
    m_fileName = fileName;
    return true;
}

bool EditData::save() const
{
    if (m_allDone) {
        save(doneFileName());
        const QString dir = QFileInfo(m_fileName).absolutePath();
        saveBlacklist(dir);
    }
    return save(m_fileName);
}

bool EditData::save(const QString &fileName) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't open" << file.fileName() << "for writing";
        return false;
    }
    for (const Entry &entry : m_entries) {
        if (!entry.unfixable) {
            file.write(entry.key.toLatin1() + '\n');
            file.write("  OLD " + entry.oldHours.toUtf8() + '\n');
            file.write("  NEW " + entry.newHours.toUtf8() + '\n');
        }
    }
    return true;
}

void EditData::setDone()
{
    m_allDone = true;
}

void EditData::loadBlacklist(const QString &dir)
{
    QFile file(dir + QLatin1String("/blacklist"));
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    while (!file.atEnd()) {
        const QString line = QString::fromLatin1(file.readLine().chopped(1));
        if (!line.isEmpty()) {
            m_blacklist.push_back(line);
        }
    }
}

void EditData::saveBlacklist(const QString &dir) const
{
    QFile file(dir + QLatin1String("/blacklist"));
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    for (const QString &line : m_blacklist) {
        file.write(line.toLatin1() + '\n');
    }
    for (const Entry &entry : m_entries) {
        if (entry.unfixable) {
            file.write(entry.key.toLatin1() + '\n');
        }
    }
}

QString EditData::doneFileName() const
{
    const QString path = QFileInfo(m_fileName).absolutePath();
    return path + QLatin1String("/done.txt");
}
