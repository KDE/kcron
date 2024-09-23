/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class CTVariable;
class Variable final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged FINAL)
    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY commentChanged FINAL)
    Q_PROPERTY(QString userLogin READ userLogin WRITE setUserLogin NOTIFY userLoginChanged FINAL)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged FINAL)

    Q_PROPERTY(QString icon READ icon NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString information READ information NOTIFY nameChanged FINAL)

public:
    explicit Variable(CTVariable *variable, QObject *parent = nullptr) noexcept;
    ~Variable() override;

    QString name() const noexcept;
    bool setName(const QString &name) noexcept;

    QString value() const noexcept;
    bool setValue(const QString &value) noexcept;

    QString comment() const noexcept;
    bool setComment(const QString &comment) noexcept;

    QString userLogin() const noexcept;
    bool setUserLogin(const QString &userLogin) noexcept;

    bool enabled() const noexcept;
    bool setEnabled(bool enabled) noexcept;

    QString icon() const noexcept;

    QString information() const noexcept;

    QString exportVariable();

    CTVariable *variable() const noexcept;

    void updateVariable(const CTVariable &variable) noexcept;
    void updateVariable(CTVariable *variable) noexcept;

public:
    Q_INVOKABLE void apply();
    Q_INVOKABLE void cancel();

    Q_INVOKABLE bool hasChanges() const;

Q_SIGNALS:
    void nameChanged();
    void valueChanged();
    void commentChanged();
    void userLoginChanged();
    void enabledChanged();

    void applyed();
    void canceled();

private:
    CTVariable *const mVariable;
};
