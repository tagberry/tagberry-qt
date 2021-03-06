/*
 * Copyright (C) 2019 Tagberry authors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "presenters/CalendarArea.hpp"

namespace tagberry::presenters {

CalendarArea::CalendarArea(storage::LocalStorage& storage, models::Root& root)
    : m_layout(new QHBoxLayout)
    , m_calendar(new widgets::TagCalendar)
    , m_storage(storage)
    , m_root(root)
{
    m_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    m_layout->addWidget(m_calendar);

    setLayout(m_layout);

    connect(
        m_calendar, &widgets::TagCalendar::pageChanged, this, &CalendarArea::refreshPage);

    connect(m_calendar, &widgets::TagCalendar::currentDateChanged, this,
        &CalendarArea::changeCurrentDate);

    connect(m_calendar, &widgets::TagCalendar::tagFocusCleared, this,
        &CalendarArea::clearTagFocus);

    connect(m_calendar, &widgets::TagCalendar::tagFocusChanged, this,
        &CalendarArea::changeTagFocus);

    connect(
        m_calendar, &widgets::TagCalendar::focusTaken, this, &CalendarArea::focusTaken);

    connect(&m_root.colorScheme(), &models::ColorScheme::widgetColorsChanged, m_calendar,
        &widgets::TagCalendar::setColors);

    m_calendar->setColors(m_root.colorScheme().widgetColors());
    m_calendar->setToday();
}

int CalendarArea::headerHeight()
{
    return m_calendar->headerHeight();
}

void CalendarArea::changeCurrentDate(QDate date)
{
    m_root.setCurrentDate(date);
}

void CalendarArea::clearTagFocus()
{
    m_root.tags().focusTag(nullptr);
}

void CalendarArea::changeTagFocus(widgets::TagLabel* label)
{
    if (!label) {
        return;
    }

    auto tag = m_root.tags().getTagByName(label->text());

    m_root.tags().focusTag(tag);
}

void CalendarArea::refreshPage()
{
    m_calendar->clearTags();

    auto range = m_calendar->getVisibleRange();

    m_root.resetCurrentPage(range);

    for (auto date = range.first; date != range.second.addDays(1);
         date = date.addDays(1)) {
        auto recSet = m_root.currentPage().recordsByDate(date);

        connect(recSet.get(), &models::RecordSet::recordTagsChanged,
            [=] { rebuildCell(date); });

        connect(recSet.get(), &models::RecordSet::recordStatesChanged,
            [=] { updateCellStates(date); });
    }

    m_storage.readPage(range, m_root.currentPage(), m_root.tags());
}

void CalendarArea::rebuildCell(QDate date)
{
    m_calendar->clearTags(date);

    auto recSet = m_root.currentPage().recordsByDate(date);

    for (auto tag : recSet->getAllTags()) {
        auto label = new widgets::TagLabel;

        label->setFocused(tag->isFocused());
        label->setText(tag->name());
        label->setComplete(recSet->checkAllRecordsWithTagComplete(tag));
        label->setCustomIndicator(QString("%1").arg(recSet->numRecordsWithTag(tag)));

        connect(tag.get(), &models::Tag::nameChanged, label, &widgets::TagLabel::setText);

        connect(
            tag.get(), &models::Tag::focusChanged, label, &widgets::TagLabel::setFocused);

        connect(
            tag.get(), &models::Tag::colorsChanged, label, &widgets::TagLabel::setColors);

        label->setColors(tag->getColors());

        m_calendar->addTag(date, label);
    }
}

void CalendarArea::updateCellStates(QDate date)
{
    auto recSet = m_root.currentPage().recordsByDate(date);

    for (auto label : m_calendar->getTags(date)) {
        auto tag = m_root.tags().getTagByName(label->text());
        label->setComplete(recSet->checkAllRecordsWithTagComplete(tag));
    }
}

} // namespace tagberry::presenters
