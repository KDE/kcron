/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "cronPrinter.h"

#include "ctcron.h"
#include "cttask.h"
#include "ctvariable.h"

#include "kcm_cron_debug.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QTextEdit>

namespace HTML
{
const static QString html = QStringLiteral("<!DOCTYPE html><html>%1</html>");
const static QString documentBody = QStringLiteral("<body>%1</body>");
const static QString documentHead = QStringLiteral(
    "<head>"
    "<style>"
    ".table {"
    "        width: 100%;"
    "        margin-bottom: 20px;"
    "        border: 1px solid #dddddd;"
    "        border-collapse: collapse;"
    "}"
    ".table th {"
    "        font-weight: bold;"
    "        padding: 5px;"
    "        background: #efefef;"
    "        border: 1px solid #dddddd;"
    "}"
    ".table td {"
    "        border: 1px solid #dddddd;"
    "        padding: 5px;"
    "}"
    "pre {"
    "    white-space: pre-wrap;"
    "    word-wrap: break-word;"
    "}"
    "</style>"
    "</head>");

const static QString beginTable = QStringLiteral("<table class=\"table\">");
const static QString beginTableHeader = QStringLiteral("<thead>");
const static QString endTableHeader = QStringLiteral("</thead>");
const static QString beginTableBody = QStringLiteral("<tbody>");
const static QString endTableBody = QStringLiteral("</tbody>");
const static QString endTable = QStringLiteral("</table>");
const static QString tableHeader = QStringLiteral("<th>%1</th>");
const static QString tableRow = QStringLiteral("<tr>%1</tr>");
const static QString tableData = QStringLiteral("<td><pre>%1</pre></td>");

const static QString tasks = QStringLiteral("<b>Tasks</b>");
const static QString variables = QStringLiteral("<b>Environment Variables</b>");
}

CronPrinter::CronPrinter(QObject *parent)
    : QObject(parent)
{
}

void CronPrinter::print(CTCron *cron)
{
    if (!cron) {
        qCWarning(KCM_CRON_LOG) << "Invalid cron for print";
        return;
    }

    QString title = getTitle(cron);
    QString tasksTable = getTasksTable(cron);
    QString variablesTable = getVariablesTable(cron);

    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    printer->setFullPage(true);
    printer->setOutputFormat(QPrinter::PdfFormat);

    QPrintDialog *dlg = new QPrintDialog(printer);
    dlg->setOptions(QAbstractPrintDialog::PrintToFile);

    if (dlg->exec() == QDialog::Accepted) {
        QTextEdit textEdit;
        textEdit.setHtml(HTML::html.arg(HTML::documentHead + HTML::documentBody.arg(title + HTML::tasks + tasksTable + HTML::variables + variablesTable)));
        textEdit.print(printer);
    }

    delete dlg;
    delete printer;
}

QString CronPrinter::getTasksTable(CTCron *cron)
{
    QString header = HTML::tableRow.arg(HTML::tableHeader.arg(QStringLiteral("Scheduling")) + HTML::tableHeader.arg(QStringLiteral("Command"))
                                        + HTML::tableHeader.arg(QStringLiteral("Description")));

    QString table = HTML::beginTable + HTML::beginTableHeader + header + HTML::endTableHeader + HTML::beginTableBody;

    for (CTTask *task : cron->tasks()) {
        table.append(getRow(task));
    }

    return table.append(HTML::endTableBody + HTML::endTable);
}

QString CronPrinter::getVariablesTable(CTCron *cron)
{
    QString header = HTML::tableRow.arg(HTML::tableHeader.arg(QStringLiteral("Variable")) + HTML::tableHeader.arg(QStringLiteral("Value"))
                                        + HTML::tableHeader.arg(QStringLiteral("Comment")));

    QString table = HTML::beginTable + HTML::beginTableHeader + header + HTML::endTableHeader + HTML::beginTableBody;
    for (CTVariable *variable : cron->variables()) {
        table.append(getRow(variable));
    }

    return table.append(HTML::endTableBody + HTML::endTable);
}

QString CronPrinter::getRow(CTTask *task)
{
    return HTML::tableRow.arg(HTML::tableData.arg(task->schedulingCronFormat()) + HTML::tableData.arg(task->command) + HTML::tableData.arg(task->comment));
}

QString CronPrinter::getRow(CTVariable *variable)
{
    return HTML::tableRow.arg(HTML::tableData.arg(variable->variable) + HTML::tableData.arg(variable->value) + HTML::tableData.arg(variable->comment));
}

QString CronPrinter::getTitle(CTCron *cron)
{
    QString title;

    title.append(QStringLiteral("<pre><b>"));

    if (cron->isSystemCron()) {
        title.append(QStringLiteral("System Crontab"));
    } else {
        title.append(QStringLiteral("Crontab of user "));
        title.append(cron->userLogin());
    }

    title.append(QStringLiteral("\n</b></pre>"));

    return title;
}

#include "cronPrinter.moc"

#include "moc_cronPrinter.cpp"
