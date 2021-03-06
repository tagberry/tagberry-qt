/*
 * Copyright (C) 2019 Tagberry authors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#pragma once

#include "widgets/Calendar.hpp"
#include "widgets/TagLabel.hpp"

#include <QHBoxLayout>
#include <QHash>
#include <QList>
#include <QWidget>

namespace tagberry::widgets {

class TagCalendar : public QWidget {
    Q_OBJECT

public:
    explicit TagCalendar(QWidget* parent = nullptr);

    QPair<QDate, QDate> getVisibleRange() const;

    QList<TagLabel*> getTags(const QDate& date) const;
    void addTag(const QDate& date, TagLabel* tag);
    void clearTags(const QDate& date);
    void clearTags();

    void setToday();

    int headerHeight();

signals:
    void pageChanged();

    void currentDateChanged(QDate);

    void tagFocusCleared();
    void tagFocusChanged(TagLabel*);

    void focusTaken();

public slots:
    void setColors(QHash<QString, QColor>);

private slots:
    void changePage();
    void changeCell(CalendarCell*);

private:
    void removeCellTags(CalendarCell*);

    QHBoxLayout* m_layout;
    Calendar* m_calendar;

    QHash<QDate, QList<TagLabel*>> m_tags;
};

} // namespace tagberry::widgets
