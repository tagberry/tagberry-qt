/*
 * Copyright (C) 2019 Tagberry authors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "widgets/Calendar.hpp"

#include <QDebug>
#include <QLocale>

namespace tagberry::widgets {

Calendar::Calendar(QWidget* parent)
    : QWidget(parent)
    , m_switch(new CalendarSwitch)
    , m_year(-1)
    , m_month(-1)
    , m_offset(-1)
    , m_weekStart(-1)
{
    setLayout(&m_calendarLayout);

    m_calendarLayout.addLayout(&m_switchLayout, 0);
    m_calendarLayout.addLayout(&m_head, 0);
    m_calendarLayout.addLayout(&m_grid, 1);

    m_switchLayout.addStretch(1);
    m_switchLayout.addWidget(m_switch);
    m_switchLayout.addStretch(1);

    for (int col = 0; col < columnCount(); col++) {
        m_head.addSpacing(3);
        m_head.addWidget((m_days[col] = new QLabel));

        QFont font;
        font.setPointSize(10);
        m_days[col]->setFont(font);

        m_grid.setColumnStretch(col, 1);

        for (int row = 0; row < rowCount(); row++) {
            if (col == 0) {
                m_grid.setRowStretch(row, 1);
            }

            auto cell = new CalendarCell(nullptr, row, col);

            connect(cell, &CalendarCell::clicked, this, &Calendar::setFocus);

            m_grid.addWidget(cell, row, col);
        }
    }

    setWeekStart(Qt::Monday);

    connect(m_switch, &CalendarSwitch::switched, this, &Calendar::setPage);
    connect(m_switch, &CalendarSwitch::today, this, &Calendar::setToday);

    connect(&m_midnightTimer, &QTimer::timeout, this, &Calendar::handleTimer);
    connect(&m_minuteTimer, &QTimer::timeout, this, &Calendar::handleTimer);

    scheduleTimer();
}

QVector<QDate> Calendar::getSelectedDates() const
{
    QVector<QDate> ret;

    for (auto cell : m_focused) {
        if (cell) {
            ret.append(getDate(cell->row(), cell->column()));
        }
    }

    return ret;
}

QPair<QDate, QDate> Calendar::getVisibleRange() const
{
    return qMakePair(getDate(0, 0), getDate(rowCount() - 1, columnCount() - 1));
}

int Calendar::rowCount() const
{
    return NumWeeks;
}

int Calendar::columnCount() const
{
    return NumDays;
}

int Calendar::headerHeight()
{
    return m_switch->height() + m_days[0]->height();
}

void Calendar::setWeekStart(Qt::DayOfWeek day)
{
    QLocale locale;
    for (int col = 0; col < columnCount(); col++) {
        m_days[col]->setText(
            locale.dayName((day + col - 1) % NumDays + 1, QLocale::ShortFormat));
    }
    m_weekStart = day;
}

void Calendar::setPage(int year, int month)
{
    if (year == m_year && month == m_month) {
        return;
    }

    if (year < 1970 || year > 9999) {
        qCritical() << "bad year value, should be in range [1970; 9999]";
        return;
    }

    if (month < 1 || month > 12) {
        qCritical() << "bad month value, should be in range [1; 12]";
        return;
    }

    m_year = year;
    m_month = month;

    const int firstDay = QDate(m_year, m_month, 1).dayOfWeek();
    if (firstDay >= m_weekStart) {
        m_offset = (firstDay - m_weekStart);
    } else {
        m_offset = (firstDay - m_weekStart + NumDays);
    }

    updateCells();
    m_switch->setYearMonth(m_year, m_month);

    pageChanged();
}

void Calendar::setDate(const QDate& date)
{
    setPage(date.year(), date.month());
    setFocus(getCell(date));
}

void Calendar::setToday()
{
    setDate(QDate::currentDate());
}

void Calendar::setFocus(CalendarCell* newCell)
{
    for (auto cell : m_focused) {
        if (cell) {
            cell->setFocused(false);
        }
    }

    m_focused.clear();

    if (newCell) {
        newCell->setFocused(true);
        m_focused.append(newCell);
    }

    focusChanged(newCell);
}

void Calendar::setColors(QHash<QString, QColor> colors)
{
    for (int col = 0; col < columnCount(); col++) {
        for (int row = 0; row < rowCount(); row++) {
            auto cell = getCell(row, col);
            cell->setColors(colors);
        }
    }
}

QDate Calendar::getDate(int row, int col) const
{
    return QDate(m_year, m_month, 1).addDays(row * NumDays + col - m_offset);
}

CalendarCell* Calendar::getCell(int row, int col)
{
    if (QLayoutItem* item = m_grid.itemAtPosition(row, col)) {
        if (QWidget* widget = item->widget()) {
            return static_cast<CalendarCell*>(widget);
        }
    }
    qCritical() << "invalid calendar row/col" << row << col;
    return nullptr;
}

CalendarCell* Calendar::getCell(const QDate& date)
{
    const QDate start = getDate(0, 0);
    const qint64 diff = start.daysTo(date);

    if (diff < 0 || diff >= rowCount() * columnCount()) {
        return nullptr;
    } else {
        const int row = int(diff / columnCount());
        const int col = int(diff % columnCount());
        return getCell(row, col);
    }
}

void Calendar::updateCells()
{
    for (int col = 0; col < columnCount(); col++) {
        for (int row = 0; row < rowCount(); row++) {
            auto cell = getCell(row, col);
            if (!cell) {
                continue;
            }

            QDate date = getDate(row, col);

            cell->setMonth(date.day() == 1 ? date.month() : -1);
            cell->setDay(date.day());
            cell->setDimmed(date.month() != m_month);
            cell->setToday(date == QDate::currentDate());
        }
    }
}

void Calendar::scheduleTimer()
{
    auto msecs = QTime::currentTime().msecsTo(QTime(23, 59, 59, 999)) + 1;

    // update current day exactly at midnight
    m_midnightTimer.setSingleShot(true);
    m_midnightTimer.start(msecs);

    // also update current day every minute to handle system date change
    m_minuteTimer.setSingleShot(false);
    m_midnightTimer.start(60000);
}

void Calendar::handleTimer()
{
    updateCells();
    scheduleTimer();
}

} // namespace tagberry::widgets
