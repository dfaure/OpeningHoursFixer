#ifndef EDITDATA_H
#define EDITDATA_H

#include <QString>

class EditData
{
public:
    EditData();

    bool load(const QString &fileName);
    bool save();

    void setDone();

    struct Entry
    {
        QString key; // e.g. node/12345
        QString oldHours;
        QString newHours;
        bool unfixable = false;
    };
    int count() const { return m_entries.size(); }
    Entry at(int index) const { return m_entries[index]; }
    void set(int index, const QString &hours) { m_entries[index].newHours = hours; }
    void setUnfixable(int index, bool unfixable) { m_entries[index].unfixable = unfixable; }
    bool isUnfixable(int index) const { return m_entries[index].unfixable; }

private:
    bool save(const QString &fileName) const;
    QString doneFileName() const;

    std::vector<Entry> m_entries;
    QString m_fileName;
    bool m_allDone = false;
};

#endif // EDITDATA_H
