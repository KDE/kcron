/*
    --------------------------------------------------------------------
    KDE\QT printing implementation.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Robert Berry <rjmber@ntlworld.com>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "crontabPrinter.h"

#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>

#include <KLocalizedString>

#include "crontabWidget.h"

#include "taskWidget.h"

#include "ctcron.h"
#include "cttask.h"
#include "ctvariable.h"

#include "logging.h"

CrontabPrinter::CrontabPrinter(CrontabWidget *crontabWidget)
{
    mCrontabWidget = crontabWidget;
}

CrontabPrinter::~CrontabPrinter()
{
    delete mCrontabPrinterWidget;

    delete mPainter;
    delete mPrinter;
    delete mPrintView;
}

bool CrontabPrinter::start()
{
    logDebug() << "Printing selection...";

    if (!mPrinter) {
        mPrinter = new QPrinter();
    }

    // do some printer initialization
    mPrinter->setFullPage(true);

    /*
    CrontabPrinterWidget* dialogPage = new CrontabPrinterWidget(crontabWidge);
    printer->addDialogPage(dialogPage);
    */

    // initialize the printer using the print dialog
    QPrintDialog *printDialog = new QPrintDialog(mPrinter, nullptr);
    printDialog->setOptionTabs(QList<QWidget *>() << mCrontabWidget);
    printDialog->setEnabledOptions(QAbstractPrintDialog::PrintToFile);
    if (printDialog->exec() == QDialog::Rejected) {
        logDebug() << "Printing canceled";
        delete printDialog;
        return false;
    }

    delete printDialog;

    // create a painter to paint on the printer object
    mPainter = new QPainter();

    // start painting
    mPainter->begin(mPrinter);

    const int margin = computeMargin();
    mPrintView = new QRect(margin, margin, mPainter->device()->width() - 2*margin, mPainter->device()->height() - 2*margin);

    mPage = 1;
    mCurrentRowPosition = 0;

    drawMainTitle();

    return true;
}

void CrontabPrinter::printTasks()
{
    CTCron *cron = mCrontabWidget->currentCron();

    drawTitle(i18n("Scheduled Tasks"));

    QList<QStringList> tasksContent;
    tasksContent.reserve(cron->tasks().count());
    const auto tasks = cron->tasks();
    for (CTTask *task : tasks) {
        QStringList values;
        values << task->schedulingCronFormat();
        values << task->command;
        values << task->comment;

        tasksContent.append(values);
    }

    QList<int> tasksColumnWidths = findColumnWidths(tasksContent, 3);

    QStringList taskHeaders;
    taskHeaders << i18n("Scheduling") << i18n("Command") << i18n("Description");
    drawHeader(tasksColumnWidths, taskHeaders);

    for (const QStringList &contents : qAsConst(tasksContent)) {
        drawContentRow(tasksColumnWidths, contents);

        needNewPage();
    }

    drawTable(tasksColumnWidths);
}

void CrontabPrinter::printVariables()
{
    CTCron *cron = mCrontabWidget->currentCron();

    mPainter->translate(0, 20);
    mCurrentRowPosition = 0;

    //Environment Variables

    drawTitle(i18n("Environment Variables"));

    //QList<QStringList> variablesContent;
    const auto variables = cron->variables();
    for (CTVariable *variable : variables) {
        mPainter->drawText(*(mPrintView), Qt::AlignLeft | Qt::TextWordWrap, variable->variable + QLatin1String(" = ") + variable->value);

        const int moveBy = computeStringHeight(variable->variable);
        mPainter->translate(0, moveBy);
    }
}

void CrontabPrinter::drawMainTitle()
{
    CTCron *cron = mCrontabWidget->currentCron();

    QFont originalFont = mPainter->font();
    QFont titleFont(originalFont);
    titleFont.setPixelSize(20);
    titleFont.setBold(true);

    mPainter->setFont(titleFont);

    QString mainTitle;
    if (cron->isSystemCron()) {
        mainTitle = i18n("System Crontab");
    } else if (cron->isMultiUserCron()) {
        mainTitle = i18n("All Users Crontabs");
    } else {
        mainTitle = i18nc("Crontab of user login", "Crontab of user %1", cron->userLogin());
    }

    mPainter->drawText(*(mPrintView), Qt::AlignHCenter | Qt::TextWordWrap, mainTitle);

    mPainter->translate(0, computeStringHeight(mainTitle));

    mPainter->setFont(originalFont);
}

void CrontabPrinter::drawTitle(const QString &title)
{
    QFont originalFont = mPainter->font();
    QFont titleFont(originalFont);
    titleFont.setPixelSize(16);
    titleFont.setBold(true);

    mPainter->setFont(titleFont);

    mPainter->drawText(*(mPrintView), Qt::AlignLeft | Qt::TextWordWrap, title);

    mPainter->translate(0, computeStringHeight(title));

    mPainter->setFont(originalFont);
}

void CrontabPrinter::drawHeader(const QList<int> &columnWidths, const QStringList &headers)
{
    QFont originalFont = mPainter->font();
    QFont titleFont(originalFont);
    titleFont.setBold(true);

    mPainter->setFont(titleFont);

    drawContentRow(columnWidths, headers);

    mPainter->setFont(originalFont);
}

void CrontabPrinter::drawContentRow(const QList<int> &columnWidths, const QStringList &contents)
{
    QString firstColumn;

    int totalWidths = 0;
    int index = 0;
    for (const QString &content : contents) {
        if (index == 0) {
            firstColumn = content;
        }

        mPainter->drawText(*(mPrintView), Qt::AlignLeft | Qt::TextWordWrap, QLatin1String(" ") + content);

        mPainter->translate(columnWidths[index], 0);

        totalWidths += columnWidths[index];

        index++;
    }

    int moveBy = computeStringHeight(firstColumn);

    changeRow(-totalWidths, moveBy);
}

void CrontabPrinter::finish()
{
    // stop painting, this will automatically send the print data to the printer
    mPainter->end();
}

void CrontabPrinter::printPageNumber()
{
    logDebug() << "Printing page number...";

    mPainter->translate(0, -mCurrentRowPosition);
    mPrintView->moveTo(QPoint(0, mPrintView->height()));
    mPainter->translate(0, -mPrintView->height());
    mPainter->drawText(mPrintView->right() - mPainter->fontMetrics().boundingRect(QString::number(mPage)).width(), mPrintView->bottom()+ mPainter->fontMetrics().ascent() + 5, QString::number(mPage));
}

void CrontabPrinter::changeRow(int x, int y)
{
    mPainter->translate(x, y);
    mCurrentRowPosition = mCurrentRowPosition + y;
}

int CrontabPrinter::computeMargin() const
{
    const int dpiy = mPainter->device()->logicalDpiY();
    const int margin = (int)((2/2.54)*dpiy);    // 2 cm margins

    return margin;
}

int CrontabPrinter::computeStringHeight(const QString &text) const
{
    const int fontHeight = mPainter->fontMetrics().height();
    const int lines = mPainter->fontMetrics().boundingRect(text).width() / mPrintView->width() + 1;
    const int moveBy = (fontHeight + 2) * lines;

    return moveBy;
}

/**
 * Whether crontab should be printed
 */
bool CrontabPrinter::isPrintCrontab() const
{
    return mCrontabPrinterWidget->printCrontab();
}

/**
 * Whether all users should be printed (root only)
 */
bool CrontabPrinter::isAllUsers() const
{
    return mCrontabPrinterWidget->printAllUsers();
}

void CrontabPrinter::drawTable(const QList<int> &columnWidths)
{
    mPainter->translate(0, -mCurrentRowPosition + computeMargin());

    int columnWidthsTotal = 0;
    for (int columnWidth : columnWidths) {
        columnWidthsTotal += columnWidth;
    }

    const int margin = computeMargin();
    int linePositionX = margin;

    QPen originalPen = mPainter->pen();
    QPen pen(originalPen);

    pen.setWidth(1);

    mPainter->setPen(pen);

    //First horizontal line
    mPainter->drawLine(QPoint(margin, 0), QPoint(margin + columnWidthsTotal, 0));

    //Second horizontal line
    mPainter->drawLine(QPoint(margin, 0+computeStringHeight(QStringLiteral(" "))), QPoint(margin + columnWidthsTotal, 0+computeStringHeight(QStringLiteral(" "))));

    //First vertical line
    mPainter->drawLine(QPoint(linePositionX, 0), QPoint(linePositionX, mCurrentRowPosition));

    for (int columnWidth : columnWidths) {
        linePositionX += columnWidth;
        mPainter->drawLine(QPoint(linePositionX, 0), QPoint(linePositionX, mCurrentRowPosition));
    }

    //Last horizontal line
    mPainter->drawLine(QPoint(margin, mCurrentRowPosition), QPoint(margin + columnWidthsTotal, mCurrentRowPosition));

    mPainter->setPen(originalPen);

    mPainter->translate(0, mCurrentRowPosition - computeMargin());
}

QList<int> CrontabPrinter::findMaxWidths(const QList<QStringList> &contents, int columnCount)
{
    QList<int> columnWidths;
    for (int i = 0; i < columnCount; ++i) {
        columnWidths.append(0);
    }

    for (const QStringList &content : contents) {
        int columnIndex = 0;
        while (columnIndex < columnWidths.count()) {
            const int valueWidth = mPainter->fontMetrics().boundingRect(content.at(columnIndex)).width();
            if (columnWidths[columnIndex] < valueWidth) {
                columnWidths[columnIndex] = valueWidth;
            }

            columnIndex++;
        }
    }

    return columnWidths;
}

QList<int> CrontabPrinter::findColumnWidths(const QList<QStringList> &contents, int columnCount)
{
    QList<int> columnWidths = findMaxWidths(contents, columnCount);

    int margin = computeMargin();
    int pageWidth = mPainter->device()->width() - 2*margin;

    int columnWidthSum = 0;
    for (int width : qAsConst(columnWidths)) {
        logDebug() << "Column : " << width;
        columnWidthSum += width;
    }

    if (columnWidthSum >= pageWidth) {
        logDebug() << "The printing could be out of the page";
        return columnWidths;
    }

    int additionalSpace = (pageWidth - columnWidthSum) / columnWidths.count();

    int columnIndex = 0;
    while (columnIndex < columnWidths.count()) {
        columnWidths[columnIndex] = columnWidths[columnIndex] + additionalSpace;

        columnIndex++;
    }

    return columnWidths;
}

void CrontabPrinter::needNewPage()
{
    const int margin = computeMargin();
    if (mCurrentRowPosition + margin >= mPrintView->height()) {
        printPageNumber();
        mPrinter->newPage();
        mPage++;
        mCurrentRowPosition = 0;
    }
}
