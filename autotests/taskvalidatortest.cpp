/*
    SPDX-FileCopyrightText: 2025 Alexander Wilms <f.alexander.wilms@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QDir>
#include <QFileInfo>
#include <QObject>
#include <QString>
#include <QTemporaryFile>
#include <QTest>

#include "cttask.h"
#include "task.h"
#include "taskvalidator.h"

class TaskValidatorTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testEmptyCommand();
    void testAbsolutePathExecutable();
    void testAbsolutePathNotFound();
    void testAbsolutePathNotExecutable();
    void testAbsolutePathWithArguments();
    void testCommandInPath();
    void testCommandInPathWithArguments();
    void testCommandNotInPath();
    void testSpecialValidCommand();

private:
    bool validate(const QString &command, QString *error = nullptr)
    {
        QString crontabLine = command.isEmpty() ? QString() : QStringLiteral("@reboot ") + command;
        CTTask ctTask(crontabLine, QString(), QLatin1String("user"));
        Task task(&ctTask, this);
        TaskValidator validator;
        validator.setTask(&task);
        bool valid = validator.validate();
        if (error) {
            *error = validator.errorString();
        }
        return valid;
    }
};

void TaskValidatorTest::testEmptyCommand()
{
    QString error;
    QVERIFY(!validate(QString(), &error));
    QVERIFY(error.contains(QLatin1String("Choose a program to run")));
}

void TaskValidatorTest::testAbsolutePathExecutable()
{
    QVERIFY(validate(QLatin1String("/usr/bin/true")));
}

void TaskValidatorTest::testAbsolutePathNotFound()
{
    QString error;
    QVERIFY(!validate(QLatin1String("/nonexistent/binary/path"), &error));
    QVERIFY(error.contains(QLatin1String("File not found")));
}

void TaskValidatorTest::testAbsolutePathNotExecutable()
{
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    QVERIFY(tempFile.open());
    QString fileName = tempFile.fileName();
    tempFile.close();
    QFile::setPermissions(fileName, QFile::ReadOwner | QFile::WriteOwner);

    QString error;
    QVERIFY(!validate(fileName, &error));
    QVERIFY(error.contains(QLatin1String("not executable")));

    QFile::remove(fileName);
}

void TaskValidatorTest::testAbsolutePathWithArguments()
{
    // Quoted paths work with arguments
    QVERIFY(validate(QLatin1String("\"/usr/bin/printf\" hello world")));
}

void TaskValidatorTest::testCommandInPath()
{
    QVERIFY(validate(QLatin1String("ls")));
}

void TaskValidatorTest::testCommandInPathWithArguments()
{
    QVERIFY(validate(QLatin1String("ls -la /tmp")));
}

void TaskValidatorTest::testCommandNotInPath()
{
    QString error;
    QVERIFY(!validate(QLatin1String("nonexistentcommand123"), &error));
    QVERIFY(error.contains(QLatin1String("Command not found")));
}

void TaskValidatorTest::testSpecialValidCommand()
{
    // "cd" is in mSpecialValidCommands
    QVERIFY(validate(QLatin1String("cd /tmp")));
}

QTEST_MAIN(TaskValidatorTest)
#include "taskvalidatortest.moc"
