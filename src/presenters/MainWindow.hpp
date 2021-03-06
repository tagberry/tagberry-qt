/*
 * Copyright (C) 2019 Tagberry authors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#pragma once

#include "models/Root.hpp"
#include "presenters/CalendarArea.hpp"
#include "presenters/RecordsArea.hpp"
#include "storage/LocalStorage.hpp"

#include <QHBoxLayout>
#include <QMainWindow>

namespace tagberry::presenters {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(storage::LocalStorage& storage);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    storage::LocalStorage& m_storage;

    models::Root m_root;

    QHBoxLayout* m_layout;
    QWidget* m_widget;

    CalendarArea* m_calendarArea {};
    RecordsArea* m_recordsArea {};
};

} // namespace tagberry::presenters
