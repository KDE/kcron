/***************************************************************************
 *   --------------------------------------------------------------------  *
 *   KDE\QT printing implementation.                                       *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Robert Berry <rjmber@ntlworld.com>                *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

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

class CrontabPrinterPrivate {
public:
	/**
	 * Pointer a printer options object
	 */
	CrontabPrinterWidget* crontabPrinterWidget;

	/**
	 * Pointer to parent widget
	 */
	CrontabWidget* crontabWidget;

	QPainter* painter;

	QPrinter* printer;

	QRect* printView;

	int page;
	int currentRowPosition;

};

CrontabPrinter::CrontabPrinter(CrontabWidget* crontabWidget) :
	d(new CrontabPrinterPrivate()) {

	d->crontabPrinterWidget = NULL;
	d->crontabWidget = crontabWidget;

	d->painter = NULL;
	d->printer = NULL;
	d->printView = NULL;

	d->page = 0;
	d->currentRowPosition = 0;
}

CrontabPrinter::~CrontabPrinter() {
	delete d->crontabPrinterWidget;

	delete d->painter;
	delete d->printer;
	delete d->printView;

	delete d;
}

bool CrontabPrinter::start() {
	logDebug() << "Printing selection..." << endl;

        if (d->printer == NULL) {
            d->printer = new QPrinter();
        }

	// do some printer initialization
	d->printer->setFullPage( true);

	/*
	CrontabPrinterWidget* dialogPage = new CrontabPrinterWidget(d->crontabWidge);
	d->printer->addDialogPage(dialogPage);
	*/

	// initialize the printer using the print dialog
	QPrintDialog *printDialog = new QPrintDialog(d->printer, 0);
	printDialog->setOptionTabs(QList<QWidget *>() << d->crontabWidget);
	printDialog->setEnabledOptions(QAbstractPrintDialog::PrintToFile);
	if (printDialog->exec() == QDialog::Rejected) {
		logDebug() << "Printing canceled" << endl;
		delete printDialog;
		return false;
	}

	delete printDialog;

	// create a painter to paint on the printer object
	d->painter = new QPainter();

	// start painting
	d->painter->begin(d->printer);

	int margin = computeMargin();
	d->printView = new QRect(margin, margin, d->painter->device()->width() - 2*margin, d->painter->device()->height() - 2*margin );

	d->page = 1;
	d->currentRowPosition = 0;

	drawMainTitle();


	return true;
}

void CrontabPrinter::printTasks() {
	CTCron* cron = d->crontabWidget->currentCron();

	drawTitle(i18n("Scheduled Tasks"));

	QList<QStringList> tasksContent;
	foreach(CTTask* task, cron->tasks()) {
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

	foreach(const QStringList& contents, tasksContent) {

		drawContentRow(tasksColumnWidths, contents);

		needNewPage();

	}

	drawTable(tasksColumnWidths);


}

void CrontabPrinter::printVariables() {
	CTCron* cron = d->crontabWidget->currentCron();

	d->painter->translate(0, 20);
	d->currentRowPosition = 0;

	//Environment Variables

	drawTitle(i18n("Environment Variables"));

	//QList<QStringList> variablesContent;
	foreach(CTVariable* variable, cron->variables()) {
		d->painter->drawText(*(d->printView), Qt::AlignLeft | Qt::TextWordWrap, variable->variable + QLatin1String( " = " ) + variable->value);

		int moveBy = computeStringHeight(variable->variable);
		d->painter->translate(0, moveBy);
	}
}

void CrontabPrinter::drawMainTitle() {
	CTCron* cron = d->crontabWidget->currentCron();

	QFont originalFont = d->painter->font();
	QFont titleFont(originalFont);
	titleFont.setPixelSize(20);
	titleFont.setBold(true);

	d->painter->setFont(titleFont);

	QString mainTitle;
	if (cron->isSystemCron())
		mainTitle = i18n("System Crontab");
	else if (cron->isMultiUserCron())
		mainTitle = i18n("All Users Crontabs");
	else
		mainTitle = i18nc("Crontab of user login", "Crontab of user %1", cron->userLogin());

	d->painter->drawText(*(d->printView), Qt::AlignHCenter | Qt::TextWordWrap, mainTitle);

	d->painter->translate(0, computeStringHeight(mainTitle));

	d->painter->setFont(originalFont);

}

void CrontabPrinter::drawTitle(const QString& title) {

	QFont originalFont = d->painter->font();
	QFont titleFont(originalFont);
	titleFont.setPixelSize(16);
	titleFont.setBold(true);


	d->painter->setFont(titleFont);

	d->painter->drawText(*(d->printView), Qt::AlignLeft | Qt::TextWordWrap, title);


	d->painter->translate(0, computeStringHeight(title));

	d->painter->setFont(originalFont);
}

void CrontabPrinter::drawHeader(const QList<int>& columnWidths, const QStringList& headers) {

	QFont originalFont = d->painter->font();
	QFont titleFont(originalFont);
	titleFont.setBold(true);

	d->painter->setFont(titleFont);

	drawContentRow(columnWidths, headers);

	d->painter->setFont(originalFont);
}


void CrontabPrinter::drawContentRow(const QList<int>& columnWidths, const QStringList& contents) {


	QString firstColumn;

	int totalWidths = 0;
	int index=0;
	foreach(const QString& content, contents) {
		if (index==0)
			firstColumn = content;

		d->painter->drawText(*(d->printView), Qt::AlignLeft | Qt::TextWordWrap, QLatin1String( " " ) + content);

		d->painter->translate(columnWidths[index], 0);

		totalWidths += columnWidths[index];

		index++;
	}

	int moveBy = computeStringHeight(firstColumn);


	changeRow( -totalWidths, moveBy);
}


void CrontabPrinter::finish() {
	// stop painting, this will automatically send the print data to the printer
	d->painter->end();

}

void CrontabPrinter::printPageNumber() {
	logDebug() << "Printing page number..." << endl;

	d->painter->translate(0, - d->currentRowPosition);
	d->printView->moveTo(QPoint(0, d->printView->height()) );
	d->painter->translate( 0, - d->printView->height() );
	d->painter->drawText(d->printView->right() - d->painter->fontMetrics().width(QString::number(d->page) ), d->printView->bottom()+ d->painter->fontMetrics().ascent() + 5, QString::number(d->page) );

}

void CrontabPrinter::changeRow(int x, int y) {
	d->painter->translate(x, y);
	d->currentRowPosition = d->currentRowPosition + y;
}

int CrontabPrinter::computeMargin() const {
	int dpiy = d->painter->device()->logicalDpiY();
	int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins

	return margin;
}

int CrontabPrinter::computeStringHeight(const QString& text) const {

	int fontHeight = d->painter->fontMetrics().height();
	int lines = d->painter->fontMetrics().width(text) / d->printView->width() + 1;
	int moveBy = (fontHeight + 2) * lines;

	return moveBy;
}

/**
 * Whether crontab should be printed
 */
bool CrontabPrinter::isPrintCrontab() const {
	return d->crontabPrinterWidget->printCrontab();
}

/**
 * Whether all users should be printed (root only)
 */
bool CrontabPrinter::isAllUsers() const {
	return d->crontabPrinterWidget->printAllUsers();
}


void CrontabPrinter::drawTable(const QList<int>& columnWidths) {
	d->painter->translate(0, - d->currentRowPosition + computeMargin());

	int columnWidthsTotal = 0;
	foreach(int columnWidth, columnWidths) {
		columnWidthsTotal += columnWidth;
	}


	int margin = computeMargin();
	int linePositionX = margin;

	QPen originalPen = d->painter->pen();
	QPen pen(originalPen);

	pen.setWidth(1);

	d->painter->setPen(pen);


	//First horizontal line
	d->painter->drawLine(QPoint(margin, 0), QPoint(margin + columnWidthsTotal, 0));

	//Second horizontal line
	d->painter->drawLine(QPoint(margin, 0+computeStringHeight(QLatin1String( " " ))), QPoint(margin + columnWidthsTotal, 0+computeStringHeight(QLatin1String( " " ))));

	//First vertical line
	d->painter->drawLine(QPoint(linePositionX, 0), QPoint(linePositionX, d->currentRowPosition));

	foreach(int columnWidth, columnWidths) {
		linePositionX += columnWidth;
		d->painter->drawLine(QPoint(linePositionX, 0), QPoint(linePositionX, d->currentRowPosition));
	}

	//Last horizontal line
	d->painter->drawLine(QPoint(margin, d->currentRowPosition), QPoint(margin + columnWidthsTotal, d->currentRowPosition));

	d->painter->setPen(originalPen);

	d->painter->translate(0, d->currentRowPosition - computeMargin());
}

QList<int> CrontabPrinter::findMaxWidths(const QList<QStringList>& contents, int columnCount) {
	QList<int> columnWidths;
	for (int i=0; i<columnCount; ++i) {
		columnWidths.append(0);
	}

	foreach(const QStringList& content, contents) {

		int columnIndex = 0;
		while (columnIndex < columnWidths.count()) {

			int valueWidth = d->painter->fontMetrics().width(content.at(columnIndex));
			if (columnWidths[columnIndex] < valueWidth) {
				columnWidths[columnIndex] = valueWidth;
			}

			columnIndex++;
		}


	}

	return columnWidths;
}

QList<int> CrontabPrinter::findColumnWidths(const QList<QStringList>& contents, int columnCount) {
	QList<int> columnWidths = findMaxWidths(contents, columnCount);

	int margin = computeMargin();
	int pageWidth = d->painter->device()->width() - 2*margin;

	int columnWidthSum = 0;
	foreach(int width, columnWidths) {
		logDebug() << "Column : " << width << endl;
		columnWidthSum += width;
	}

	if (columnWidthSum >= pageWidth) {
		logDebug() << "The printing could be out of the page" << endl;
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



void CrontabPrinter::needNewPage() {
	int margin = computeMargin();
	if (d->currentRowPosition + margin >= d->printView->height()) {
		printPageNumber();
		d->printer->newPage();
		d->page++;
		d->currentRowPosition = 0;
	}
}
