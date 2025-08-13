/*
 * barrier -- mouse and keyboard sharing utility
 * Copyright (C) 2014-2016 Symless Ltd.
 *
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file LICENSE that should have accompanied this file.
 *
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ZeroconfRecord.h"

#include <QtCore/QObject>
#define _MSL_STDINT_H
#include <stdint.h>

#if defined(Q_OS_WIN)
// DNS SD support is optional on Windows
#define BARRIER_USE_DNSSD 0
#else
#define BARRIER_USE_DNSSD 1
#endif

#if BARRIER_USE_DNSSD
#include <dns_sd.h>
#endif

class QSocketNotifier;

class ZeroconfBrowser : public QObject
{
    Q_OBJECT

public:
    ZeroconfBrowser(QObject* parent = 0);
    ~ZeroconfBrowser();
    void browseForType(const QString& type);
    inline QList<ZeroconfRecord> currentRecords() const { return m_Records; }
    inline QString serviceType() const { return m_BrowsingType; }

signals:
    void currentRecordsChanged(const QList<ZeroconfRecord>& list);
#if BARRIER_USE_DNSSD
    void error(DNSServiceErrorType err);
#else
    void error(int err);
#endif

private slots:
    void socketReadyRead();

private:
#if BARRIER_USE_DNSSD
    static void DNSSD_API browseReply(DNSServiceRef, DNSServiceFlags flags,
            quint32, DNSServiceErrorType errorCode, const char* serviceName,
            const char* regType, const char* replyDomain, void* context);

    DNSServiceRef m_DnsServiceRef;
    QSocketNotifier* m_pSocket;
#endif
    QList<ZeroconfRecord> m_Records;
    QString m_BrowsingType;
};
