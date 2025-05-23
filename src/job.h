/*
 * BluezQt - Asynchronous BlueZ wrapper library
 *
 * SPDX-FileCopyrightText: 2014 Alejandro Fiestas Olivares <afiestas@kde.org>
 * SPDX-FileCopyrightText: 2014-2015 David Rosca <nowrep@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef BLUEZQT_JOB_H
#define BLUEZQT_JOB_H

#include <QObject>

#include "bluezqt_export.h"

#include <memory>

namespace BluezQt
{
class JobPrivate;

/*!
 * \inmodule BluezQt
 * \class BluezQt::Job
 * \inheaderfile BluezQt/Job
 * \brief Async job.
 *
 * This class represents an asynchronous job performed by BluezQt,
 * it is usually not used directly but instead it is inherit by some
 * other class.
 *
 * There are two ways of using this class, one is via exec() which will block
 * the thread until a result is fetched, the other is via connecting to the
 * signal result()
 *
 * Please, think twice before using exec(), it should be used only in either
 * unittest or cli apps.
 *
 * \note Job and its subclasses are meant to be used in a fire-and-forget way.
 * Jobs will delete themselves when they finish using deleteLater().
 *
 * \note Even given their asynchronous nature, Jobs are still executed in the
 * main thread, so any blocking code executed in it will block the app calling it.
 *
 * \sa InitManagerJob
 * \sa InitObexManagerJob
 */
class BLUEZQT_EXPORT Job : public QObject
{
    Q_OBJECT
    /*! \property BluezQt::Job::error */
    Q_PROPERTY(int error READ error)
    /*! \property BluezQt::Job::errorText */
    Q_PROPERTY(QString errorText READ errorText)
    /*! \property BluezQt::Job::running */
    Q_PROPERTY(bool running READ isRunning)
    /*! \property BluezQt::Job::finished */
    Q_PROPERTY(bool finished READ isFinished)

public:
    /*!
     * Creates a new Job object as a child of \a parent.
     */
    explicit Job(QObject *parent = nullptr);

    ~Job() override;

    /*!
     * \enum BluezQt::Job::Error
     * \value NoError
     *        Indicates there is no error.
     * \value UserDefinedError
     *        Subclasses should define error codes starting at this value.
     * \sa error()
     */
    enum Error {
        NoError = 0,
        UserDefinedError = 100
    };
    Q_ENUM(Error)

    /*!
     * Executes the job synchronously.
     *
     * This will start a nested QEventLoop internally. Nested event loop can be dangerous and
     * can have unintended side effects, you should avoid calling exec() whenever you can and use the
     * asynchronous interface of Job instead.
     *
     * Should you indeed call this method, you need to make sure that all callers are reentrant,
     * so that events delivered by the inner event loop don't cause non-reentrant functions to be
     * called, which usually wreaks havoc.
     *
     * Note that the event loop started by this method does not process user input events, which means
     * your user interface will effectively be blocked. Other events like paint or network events are
     * still being processed. The advantage of not processing user input events is that the chance of
     * accidental reentrancy is greatly reduced. Still you should avoid calling this function.
     *
     * \warning This method blocks until the job finishes!
     *
     * Returns \c true if the job has been executed without error, \c false otherwise.
     */
    bool exec();

    /*!
     * Returns the error code for this job if there has been an error.
     *
     * Make sure to call this once result() has been emitted.
     */
    int error() const;

    /*!
     * Returns the error text if there has been an error.
     *
     * Only call if error is not 0.
     *
     * This is usually some extra data associated with the error,
     * such as a URL.  Use errorString() to get a human-readable,
     * translated message.
     */
    QString errorText() const;

    /*!
     * Returns whether the job is currently running.
     */
    bool isRunning() const;

    /*!
     * Returns whether the job has already finished.
     */
    bool isFinished() const;

public Q_SLOTS:
    /*!
     * Starts the job asynchronously.
     *
     * This method will schedule doStart() to be executed in the next
     * loop. This is done so this method returns as soon as possible.
     *
     * When the job is finished, result() is emitted.
     */
    void start();

    /*!
     * Kills the job.
     *
     * This method will kill the job and then call deleteLater().
     * Only jobs started with start() can be killed.
     *
     * It will not emit the result signal.
     */
    void kill();

protected Q_SLOTS:
    /*!
     * Implementation for start() that will be executed in next loop
     *
     * This slot is always called in the next loop, triggered by start().
     *
     * When implementing this method is important to remember that jobs
     * are not executed on a different thread (unless done that way), so any
     * blocking task has to be done in a different thread or process.
     */
    virtual void doStart() = 0;

protected:
    /*!
     * Sets the \a errorCode.
     *
     * It should be called when an error
     * is encountered in the job, just before calling emitResult().
     *
     * You should define an enum of error codes,
     * with values starting at Job::UserDefinedError, and use
     * those. For example:
     * \code
     * enum ExampleErrors{
     *   InvalidFoo = UserDefinedError,
     *   BarNotFound
     * };
     * \endcode
     *
     * \sa emitResult()
     */
    void setError(int errorCode);

    /*!
     * Sets the \a errorText.
     *
     * It should be called when an error
     * is encountered in the job, just before calling emitResult().
     *
     * Provides extra information about the error that cannot be
     * determined directly from the error code.  For example, a
     * URL or filename.  This string is not normally translatable.
     *
     * \sa emitResult(), setError()
     */
    void setErrorText(const QString &errorText);

    /*!
     * Utility function to emit the result signal, and remove this job.
     *
     * \note Deletes this job using deleteLater().
     * \sa result()
     */
    void emitResult();

    /*!
     * Implementation for emitting the result signal.
     *
     * This function is needed to be able to emit result() signal
     * with the job pointer's type being subclass.
     */
    virtual void doEmitResult() = 0;

private:
    std::unique_ptr<JobPrivate> const d_ptr;

    Q_DECLARE_PRIVATE(Job)
};

} // namespace BluezQt

#endif // BLUEZQT_JOB_H
