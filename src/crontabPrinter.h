/*
    --------------------------------------------------------------------
    KDE\QT Printing class
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Robert Berry <rjmber@ntlworld.com>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CRONTABPRINTER_H
#define CRONTABPRINTER_H

#include <QList>

#include "crontabPrinterWidget.h"

class QString;

class CrontabWidget;

class CrontabPrinterPrivate;
class QPrinter;
/**
 *Provides a wrapper for simple printing of text.
 */
class CrontabPrinter
{
public:

    /**
     * Constructor
     */
    explicit CrontabPrinter(CrontabWidget *crontabWidget);

    /**
     * Destructor
     */
    ~CrontabPrinter();

    bool start();
    void finish();
    void printTasks();
    void printVariables();

    /**
     * Whether crontab should be printed
     */
    bool isPrintCrontab() const;

    /**
     * Whether all users should be printed (root only)
     */
    bool isAllUsers() const;

private:

    /**
     *Disable the copy constructor and the assignment operator
     */
    CrontabPrinter &operator=(const CrontabPrinter &)
    {
        return *this;
    }

    void printPageNumber();

    void drawMainTitle();
    void drawTitle(const QString &title);

    void drawHeader(const QList<int> &columnWidths, const QStringList &headers);
    void drawContentRow(const QList<int> &columnWidths, const QStringList &contents);

    void drawTable(const QList<int> &columnWidths);

    void needNewPage();

    void changeRow(int x, int y);
    int computeMargin() const;
    int computeStringHeight(const QString &text) const;

    QList<int> findMaxWidths(const QList<QStringList> &tasksContent, int columnCount);
    QList<int> findColumnWidths(const QList<QStringList> &tasksContent, int columnCount);

    /**
     * Pointer a printer options object
     */
    CrontabPrinterWidget *mCrontabPrinterWidget = nullptr;

    /**
     * Pointer to parent widget
     */
    CrontabWidget *mCrontabWidget = nullptr;

    QPainter *mPainter = nullptr;

    QPrinter *mPrinter = nullptr;

    QRect *mPrintView = nullptr;

    int mPage = 0;
    int mCurrentRowPosition = 0;
};

#endif
