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
#include <kdeprintdialog.h>

#include <klocale.h>

#include "logging.h"

const int CrontabPrinter::alignTextLeft(1000);
const int CrontabPrinter::alignTextRight(2000);
const int CrontabPrinter::alignTextCenter(3000);

const int CrontabPrinter::defaultLeftMargin(20);
const int CrontabPrinter::defaultRightMargin(20);
const int CrontabPrinter::defaultTopMargin(30);
const int CrontabPrinter::defaultBottomMargin(30);

/*
 I had to add this enum to get the file to compile. Since I ported
 this class to QT2.1.1 the compiler throws errors about these flags. 
 When I figure why  I will fix this ugly solution. 
 */

enum AlignmentFlags {AlignLeft = 0x0001, AlignRight = 0x0002,
	AlignHCenter = 0x0004, AlignTop = 0x0008,
	AlignBottom = 0x0010, AlignVCenter = 0x0020,
	AlignCenter = AlignVCenter | AlignHCenter,
	SingleLine = 0x0040, DontClip = 0x0080,
	ExpandTabs = 0x0100, ShowPrefix = 0x0200,
	WordBreak = 0x0400, DontPrint = 0x1000};

CrontabPrinter::~CrontabPrinter() {
	delete prnt;
	delete prntOpt;
	if (columns.isEmpty()==false)
		columns.erase(columns.begin(), columns.end());
}

void CrontabPrinter::createColumns(unsigned num_columns) {
	//Construct all of the columns to be equal in size
	//I am going to add a function which works on percentages

	Column *col;
	int col_width(width / num_columns);

	if (columns.isEmpty()==false)
		columns.erase(columns.begin(), columns.end());

	for (unsigned i = 0, start = leftMargin; i < num_columns; i++) {
		col = new Column;
		col->start = start;
		col->finish = start + (col_width - 5);
		col->height = topMargin;
		columns.append(col);
		start+=col_width;
	}

}

bool CrontabPrinter::start() {
	prnt->setDocName("Cron Tab");

	QPrintDialog *printDialog = KdePrint::createPrintDialog(prnt, QList<QWidget*>() << prntOpt, parent);
	printDialog->setWindowTitle(i18n("Print Cron Tab"));
	if (printDialog->exec()) {
		//Setup a printer
		if (paint!=NULL)
			delete paint;
		paint = new QPainter ();
		paint->begin(prnt);
#ifdef __GNUC__
		logDebug() << "kde4: port settabstops" << endl;
#endif
		//paint->setTabStops(20); // Setup a defualt tab stop size

		//Get the information about the page size
		width = prnt->width() - (leftMargin + rightMargin);
		height = prnt->height() - (topMargin + bottomMargin);
		return true;
	} else
		return false;
}

void CrontabPrinter::setFont(const QFont &font) {
	paint->setFont(font);
}

QFont CrontabPrinter::getFont() const {
	return paint->font();
}

void CrontabPrinter::print(const QString &str, int col, int alignment, bool wordWrap) {
	//Prints the string str into the column col using
	//the remaining arguments as format flags

	int format = 0;

	if (paint==NULL)
		return;

	//Setup the alignment
	switch (alignment) {
	case alignTextLeft:
		format = AlignLeft | AlignTop | DontClip | ExpandTabs;
		break;
	case alignTextCenter:
		format = AlignHCenter | AlignTop | DontClip | ExpandTabs;
		break;
	case alignTextRight:
		format = AlignRight | AlignTop | DontClip | ExpandTabs;
		break;
	default:
		//add error trap
		break;

	}

	//Check if we are wrapping words
	if (wordWrap)
		format = format | WordBreak;

	//Whats left of the page
	int remainder(height - columns[col-1]->height);
	QRect rect = paint->boundingRect(columns[col-1]->start, columns[col-1]->height, columns[col-1]->width(), remainder, format, str);

	if (rect.height() <= remainder) {
		//Draw the text
		paint->drawText(columns[col-1]->start, columns[col-1]->height, columns[col-1]->width(), remainder, format, str);
		//Reset the columns height
		columns[col-1]->height += rect.height();
	} else {
		newPage();
		remainder = height - columns[col-1]->height;
		rect = paint->boundingRect(columns[col-1]->start, columns[col-1]->height, columns[col-1]->width(), remainder, format, str);
		paint->drawText(columns[col-1]->start, columns[col-1]->height, columns[col-1]->width(), remainder, format, str);
		columns[col-1]->height += rect.height();
	}
}

void CrontabPrinter::levelColumns(int space) {
	int ht(0);

	//Find the heighest height
	for (int i = 0; i < columns.size(); i++) {
		if (ht < columns[i]->height)
			ht = columns[i]->height;
	}

	//Set all the columns to that height and then add the space argument
	for (int i = 0; i < columns.size(); i++)
		columns[i]->height = ht+space;
}

void CrontabPrinter::finished() {
	if (paint!=NULL) {
		paint->end(); //Send to printer or file
		delete paint;
	}
}

void CrontabPrinter::newPage() {
	prnt->newPage();
	for (int i = 0; i < columns.size(); i++)
		columns[i]->height = topMargin;
}

int CrontabPrinter::numCopies() const {
	return prnt->numCopies();
}

