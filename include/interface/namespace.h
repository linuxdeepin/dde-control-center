// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
// clang-format off
#ifndef DCC_NAMESPACE_H
#define DCC_NAMESPACE_H

#define DCC_NAMESPACE dccV23

//#define } using namespace DCC_NAMESPACE;

//#define namespace DCC_NAMESPACE { namespace DCC_NAMESPACE {
//#define } }
////////////////////////////////
#define DCC_DECLARE_PRIVATE(Class) \
private: \
    QScopedPointer<Class##Private> d_ptr##Class; \
    Q_DECLARE_PRIVATE_D(d_ptr##Class, Class)\
    Q_DISABLE_COPY(Class)

#define DCC_INIT_PRIVATE(Class) d_ptr##Class(new Class##Private(this))

#endif // DCC_NAMESPACE_H
// clang-format on
