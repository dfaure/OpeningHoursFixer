# SPDX-FileCopyrightText: 2021 David Faure <faure@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.5)

project(OpeningHoursFixer VERSION 1.0)

find_package(ECM 5.44 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDEInstallDirs)
include(KDECMakeSettings)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(REQUIRED_QT_VERSION 5.9)
find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Core Gui Widgets)

find_package(KOpeningHours REQUIRED CONFIG)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050900)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x054F00)

add_subdirectory(src)

