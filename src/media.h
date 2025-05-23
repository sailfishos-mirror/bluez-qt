/*
 * BluezQt - Asynchronous BlueZ wrapper library
 *
 * SPDX-FileCopyrightText: 2018 Manuel Weichselbaumer <mincequi@web.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef BLUEZQT_MEDIA_H
#define BLUEZQT_MEDIA_H

#include <QObject>

#include "bluezqt_export.h"

#include <memory>

namespace BluezQt
{
class MediaEndpoint;
class PendingCall;

/*!
 * \inmodule BluezQt
 * \class BluezQt::Media
 * \inheaderfile BluezQt/Media
 * \brief Bluetooth Media.
 *
 * This allows media endpoints to be established in accordance with the
 * capabilities of a specific media service profile.
 *
 * For example, an A2DP media endpoint could be created allowing data from a
 * remote device to be streamed to/from the sender.
 *
 * Each media endpoint is associated with a service object instance that
 * implements the required behaviours of the endpoint. The service object
 * must be created at a given path before it is registered.
 *
 * \sa MediaEndpoint
 */
class BLUEZQT_EXPORT Media : public QObject
{
    Q_OBJECT

public:
    ~Media() override;

    /*!
     * Registers an \a endpoint.
     *
     * Register a local end point to sender, the sender can register as many end points as it likes.
     *
     * \note If the sender disconnects the end points are automatically unregistered.
     *
     * Possible errors:
     *
     * \list
     * \li PendingCall::InvalidArguments
     * \li PendingCall::NotSupported
     * \endlist
     *
     * Returns void pending call.
     */
    PendingCall *registerEndpoint(MediaEndpoint *endpoint);

    /*!
     * Unregisters an \a endpoint.
     *
     * Returns void pending call.
     */
    PendingCall *unregisterEndpoint(MediaEndpoint *endpoint);

private:
    BLUEZQT_NO_EXPORT explicit Media(const QString &path, QObject *parent = nullptr);

    std::unique_ptr<class MediaPrivate> const d;

    friend class AdapterPrivate;
};

} // namespace BluezQt

#endif // BLUEZQT_MEDIA_H
