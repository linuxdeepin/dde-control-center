/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DCC_NAMESPACE_H
#define DCC_NAMESPACE_H

#define DCC_NAMESPACE dccV23

// #define } using namespace DCC_NAMESPACE;

// #define namespace DCC_NAMESPACE { namespace DCC_NAMESPACE {
// #define } }
////////////////////////////////
#define DCC_DECLARE_PRIVATE(Class)             \
  private:                                     \
  QScopedPointer<Class##Private> d_ptr##Class; \
  Q_DECLARE_PRIVATE_D(d_ptr##Class, Class)     \
  Q_DISABLE_COPY(Class)

#define DCC_INIT_PRIVATE(Class) d_ptr##Class(new Class##Private(this))

#endif // DCC_NAMESPACE_H
