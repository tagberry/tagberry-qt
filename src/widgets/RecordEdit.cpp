/*
 * Copyright (C) 2019 Tagberry authors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "widgets/RecordEdit.hpp"

namespace tagberry::widgets {

RecordEdit::RecordEdit(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_layout.setContentsMargins(QMargins(0, 0, 0, 0));
    m_layout.addWidget(&m_cell);

    setLayout(&m_layout);

    m_complete.setSize(24);

    m_title.setPlaceholderText("enter title");
    m_title.setFontWeight(QFont::Bold);

    m_titleRowLayout.setContentsMargins(QMargins(3, 3, 3, 3));
    m_titleRowLayout.setSpacing(4);
    m_titleRowLayout.addWidget(&m_complete);
    m_titleRowLayout.addWidget(&m_title);
    m_titleRowLayout.setAlignment(&m_complete, Qt::AlignTop);

    m_tagsRowLayout.setContentsMargins(QMargins(0, 0, 0, 0));
    m_tagsRowLayout.addWidget(&m_tagListEdit);

    m_textRowLayout.setContentsMargins(QMargins(0, 0, 0, 0));
    m_textRowLayout.addWidget(&m_textEdit);

    m_cell.setRowLayout(Row_Title, &m_titleRowLayout, 0);
    m_cell.setRowLayout(Row_Tags, &m_tagsRowLayout, 0);
    m_cell.setRowLayout(Row_Text, &m_textRowLayout, 0);

    connect(&m_cell, &MultirowCell::clicked, this, &RecordEdit::cellClicked);

    connect(&m_complete, &CheckBox::clicked, this, [=] { clicked(this); });
    connect(&m_complete, &CheckBox::stateChanged, this, &RecordEdit::completeChanged);

    connect(&m_title, &LineEdit::clicked, this, [=] { clicked(this); });
    connect(&m_title, &LineEdit::textChanged, this, &RecordEdit::titleChanged);
    connect(
        &m_title, &LineEdit::editingFinished, this, &RecordEdit::titleEditingFinished);

    connect(&m_tagListEdit, &TagListEdit::clicked, this, [=] { clicked(this); });
    connect(&m_tagListEdit, &TagListEdit::tagsChanged, this, &RecordEdit::tagsChanged);
    connect(&m_tagListEdit, &TagListEdit::tagAdded, this, &RecordEdit::tagAdded);
    connect(&m_tagListEdit, &TagListEdit::tagFocusChanged, this,
        &RecordEdit::tagFocusChanged);
    connect(&m_tagListEdit, &TagListEdit::tagFocusCleared, this,
        &RecordEdit::tagFocusCleared);
}

void RecordEdit::setTags(QList<TagLabel*> tags)
{
    m_tagListEdit.setTags(tags);
}

void RecordEdit::setComplete(bool complete)
{
    m_complete.setChecked(complete);
}

void RecordEdit::setTitle(QString str)
{
    m_title.setText(str);
}

void RecordEdit::setFocused(bool focused)
{
    m_cell.setFocused(focused);
    m_title.setFocused(focused);
}

void RecordEdit::startEditing()
{
    m_title.startEditing();
}

bool RecordEdit::complete() const
{
    return m_complete.isChecked();
}

QString RecordEdit::title() const
{
    return m_title.text();
}

QList<TagLabel*> RecordEdit::tags() const
{
    return m_tagListEdit.tags();
}

void RecordEdit::notifyRemoving()
{
    removing();
}

void RecordEdit::cellClicked()
{
    tagFocusCleared();
    clicked(this);
}

void RecordEdit::setColors(QHash<QString, QColor> colors)
{
    m_cell.setBorderColor(colors["border"]);
    m_cell.setRowColor(Row_Title, colors["background"]);
    m_cell.setRowColor(Row_Tags, colors["background"]);
    m_cell.setRowColor(Row_Text, colors["background"]);
    m_complete.setColors(colors["background-dimmed"], colors["border"]);
}

} // namespace tagberry::widgets
