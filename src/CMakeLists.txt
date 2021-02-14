# SPDX-FileCopyrightText: 2021 David Faure <faure@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

add_executable(KOpeningHoursFixer
    main.cpp
    mainwidget.cpp
    editdata.cpp
    entrywidget.cpp
)

target_link_libraries(KOpeningHoursFixer
    Qt5::Widgets
    KOpeningHours
)
