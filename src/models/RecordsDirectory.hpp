/*
 * Copyright (C) 2019 Tagberry authors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#pragma once

#include "models/Record.hpp"
#include "models/RecordSet.hpp"

#include <QHash>
#include <QList>
#include <QObject>

#include <unordered_set>

namespace tagberry::models {

class RecordsDirectory : public QObject {
    Q_OBJECT

public:
    RecordSetPtr recordsByDate(const QDate&);

    RecordSetPtr recordsWithoutDate();

    RecordPtr createRecord();

    RecordPtr getOrCreateRecord(const QString& id);

    void removeRecord(RecordPtr);

    void clearRecords();

private slots:
    void recordIdChanged(QString id);
    void recordDateChanged(QDate oldDate, QDate newDate);

private:
    std::unordered_set<RecordPtr> m_records;
    QHash<QString, RecordPtr> m_recordByID;
    QHash<QDate, RecordSetPtr> m_recordsByDate;
    RecordSetPtr m_recordWithoutDate;
};

} // namespace tagberry::models
