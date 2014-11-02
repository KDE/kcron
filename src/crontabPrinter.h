/***************************************************************************
 *   --------------------------------------------------------------------  *
 *   KDE\QT Printing class                                                 *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Robert Berry <rjmber@ntlworld.com>                *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KTPRINT_H
#define KTPRINT_H

#include <QList>

#include "crontabPrinterWidget.h"

class QString;

class CrontabWidget;

class CrontabPrinterPrivate;

/**
 *Provides a wrapper for simple printing of text.
 */
class CrontabPrinter {
public:

	/**
	 * Contructor
	 */
	CrontabPrinter(CrontabWidget* crontabWidget);

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
	CrontabPrinter& operator=(const CrontabPrinter&) {
		return *this;
	}
	
	void printPageNumber();
	
	void drawMainTitle();
	void drawTitle(const QString& title);
	
	void drawHeader(const QList<int>& columnWidths, const QStringList& headers);
	void drawContentRow(const QList<int>& columnWidths, const QStringList& contents);
	
	void drawTable(const QList<int>& columnWidths);
	
	void needNewPage();
	int maxWidth();

	void changeRow(int x, int y);
	int computeMargin() const;
	int computeStringHeight(const QString& text) const;

	QList<int> findMaxWidths(const QList<QStringList>& tasksContent, int columnCount);
	QList<int> findColumnWidths(const QList<QStringList>& tasksContent, int columnCount);
	
	CrontabPrinterPrivate* const d;

};

#endif
