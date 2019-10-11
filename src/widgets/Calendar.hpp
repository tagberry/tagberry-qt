/*
 * Copyright (C) 2019 Tagberry authors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#pragma once

#include "widgets/CalendarCell.hpp"
#include "widgets/CalendarSwitch.hpp"

#include <QColor>
#include <QDate>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QList>
#include <QPair>
#include <QVBoxLayout>
#include <QWidget>

namespace tagberry::widgets {

class Calendar : public QWidget {
    Q_OBJECT

public:
    explicit Calendar(QWidget* parent = nullptr);

    QList<QDate> getSelectedDates() const;
    QPair<QDate, QDate> getVisibleRange() const;

    int rowCount() const;
    int columnCount() const;

    QDate getDate(int row, int col) const;

    CalendarCell* getCell(int row, int col);
    CalendarCell* getCell(const QDate&);

    void setWeekStart(Qt::DayOfWeek);

    void setNormalColor(const QColor&);
    void setTodayColor(const QColor&);

signals:
    void pageChanged();
    void focusChanged(CalendarCell*);

public slots:
    virtual void setFocus(CalendarCell*);

    void setToday();

private slots:
    void setPage(int year, int month);
    void setDate(const QDate&);

private:
    enum { NumDays = 7, NumWeeks = 5 };

    void updateCells();

    CalendarSwitch* m_switch;

    QVBoxLayout m_calendarLayout;
    QHBoxLayout m_switchLayout;
    QHBoxLayout m_head;
    QGridLayout m_grid;

    QList<CalendarCell*> m_focused;
    QLabel* m_days[NumDays];

    QColor m_normalColor;
    QColor m_todayColor;

    int m_year;
    int m_month;
    int m_offset;
    int m_weekStart;
};

} // namespace tagberry::widgets
