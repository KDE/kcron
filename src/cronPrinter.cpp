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

#include <KLazyLocalizedString>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextEdit>

namespace HTML
{
static constexpr QLatin1StringView html("<!DOCTYPE html><html>%1</html>");
static constexpr QLatin1StringView documentBody("<body>%1</body>");
static constexpr QLatin1StringView documentHead(
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

static constexpr QLatin1StringView beginTable("<table class=\"table\">");
static constexpr QLatin1StringView beginTableHeader("<thead>");
static constexpr QLatin1StringView endTableHeader("</thead>");
static constexpr QLatin1StringView beginTableBody("<tbody>");
static constexpr QLatin1StringView endTableBody("</tbody>");
static constexpr QLatin1StringView endTable("</table>");
static constexpr QLatin1StringView tableHeader("<th>%1</th>");
static constexpr QLatin1StringView tableRow("<tr>%1</tr>");
static constexpr QLatin1StringView tableData("<td><pre>%1</pre></td>");

const static auto tasks = kli18nc("@title Note: that is an HTML tag", "<b>Tasks</b>");
const static auto variables = kli18nc("@title Note: that is an HTML tag", "<b>Environment Variables</b>");
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
        textEdit.setHtml(HTML::html.arg(HTML::documentHead
                                        + HTML::documentBody.arg(title + HTML::tasks.toString() + tasksTable + HTML::variables.toString() + variablesTable)));
        textEdit.print(printer);
    }

    delete dlg;
    delete printer;
}

QString CronPrinter::getTasksTable(CTCron *cron)
{
    QString header = HTML::tableRow.arg(HTML::tableHeader.arg(i18nc("@title:column", "Scheduling")) + HTML::tableHeader.arg(i18nc("@title:column", "Command"))
                                        + HTML::tableHeader.arg(i18nc("@title:column", "Description")));

    QString table = HTML::beginTable + HTML::beginTableHeader + header + HTML::endTableHeader + HTML::beginTableBody;

    for (CTTask *task : cron->tasks()) {
        table.append(getRow(task));
    }

    return table.append(HTML::endTableBody + HTML::endTable);
}

QString CronPrinter::getVariablesTable(CTCron *cron)
{
    QString header = HTML::tableRow.arg(HTML::tableHeader.arg(i18nc("@title:column", "Variable")) + HTML::tableHeader.arg(i18nc("@title:column", "Value"))
                                        + HTML::tableHeader.arg(i18nc("@title:column", "Comment")));

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
        title.append(i18nc("@title", "System Crontab"));
    } else {
        title.append(i18nc("@title", "Crontab of user %1", cron->userLogin()));
    }

    title.append(QStringLiteral("\n</b></pre>"));

    return title;
}

#include "cronPrinter.moc"

#include "moc_cronPrinter.cpp"
