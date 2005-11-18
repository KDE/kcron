
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

#include <qpainter.h>
#include <qstring.h>
#include <q3paintdevicemetrics.h>

#include <ktprintopt.h>
#include <klocale.h>
#include "ktprint.h"

const int KTPrint::alignTextLeft             (1000);
const int KTPrint::alignTextRight            (2000);
const int KTPrint::alignTextCenter           (3000);

const int KTPrint::defaultLeftMargin         (20);
const int KTPrint::defaultRightMargin        (20);
const int KTPrint::defaultTopMargin          (30);
const int KTPrint::defaultBottomMargin       (30);

/*
  I had to add this enum to get the file to compile. Since I ported
  this class to QT2.1.1 the compiler throws errors about these flags. 
  When I figure why  I will fix this ugly solution. 
*/

enum AlignmentFlags { AlignLeft = 0x0001, AlignRight = 0x0002, 
                      AlignHCenter = 0x0004, AlignTop = 0x0008, 
                      AlignBottom = 0x0010, AlignVCenter = 0x0020,
                      AlignCenter = AlignVCenter | AlignHCenter, 
                      SingleLine = 0x0040, DontClip = 0x0080, 
                      ExpandTabs = 0x0100, ShowPrefix = 0x0200, 
                      WordBreak = 0x0400, DontPrint = 0x1000 };

KTPrint::~KTPrint()
{
  delete prnt;
  if (columns.size()>0)
    columns.erase(columns.begin(), columns.end());

}

void KTPrint :: createColumns (unsigned num_columns)
{
//Construct all of the columns to be equal in size
//I am going to add a function which works on percentages

  int start;
  Column *col;
  int col_width(width / num_columns);

  if (columns.size()>0)
    columns.erase(columns.begin(), columns.end());

  for (unsigned i = 0, start = leftMargin; i < num_columns; i++) {
    col = new Column;
    col->start = start;
    col->finish = start + (col_width - 5);
    col->height = topMargin;
    columns.push_back(col);
    start+=col_width;
  }
	
}		
	
bool KTPrint:: start ()
{
  prnt->setDocName("Cron Tab");
  prnt->addDialogPage(new KTPrintOpt(root));

  if (prnt->setup(0L, i18n("Print Cron Tab"))) { //Setup a printer
    if (paint!=NULL) delete paint;
    paint = new  QPainter ();
    paint->begin(prnt);
#warning "kde4: port settabstops";
    //paint->setTabStops(20); // Setup a defualt tab stop size
    		
    //Get the information about the page size
    Q3PaintDeviceMetrics metrics (prnt);
    width = metrics.width () - (leftMargin + rightMargin);
    height = metrics.height () - (topMargin + bottomMargin);
    return true;
  }
  else
    return false;
}	

void KTPrint :: setFont (const QFont &font)
{
  paint->setFont(font);
}

QFont KTPrint :: getFont () const
{
  return paint->font();	
}

void KTPrint :: print (const QString &str, int col, int alignment, bool wordWrap)
{
//Prints the string str into the column col using
//the remaining arguments as format flags

  int format;
 
  if (paint==NULL) 
    return;
  
	
   //Setup the alignment
  switch (alignment) {
    case alignTextLeft : format = AlignLeft | AlignTop | DontClip | ExpandTabs; break;
    case alignTextCenter : format = AlignHCenter | AlignTop | DontClip | ExpandTabs; break;
    case alignTextRight : format = AlignRight | AlignTop | DontClip | ExpandTabs; break;
    default :
       //add error trap
       break;

  }
	
  //Check if we are wrapping words
  if (wordWrap)
    format = format | WordBreak;
	
  //Whats left of the page
  int remainder (height - columns[col-1]->height);
  QRect rect=paint->boundingRect(columns[col-1]->start,columns[col-1]->height, columns[col-1]->width(), remainder,format, str);
  
  if (rect.height() <= remainder)
  {
    //Draw the text
    paint->drawText(columns[col-1]->start,columns[col-1]->height, columns[col-1]->width(), remainder, format, str);
    //Reset the columns height
    columns[col-1]->height += rect.height();
  }
  else
  {
    newPage();
    remainder = height - columns[col-1]->height;
    rect=paint->boundingRect(columns[col-1]->start,columns[col-1]->height, columns[col-1]->width(), remainder,format, str);
    paint->drawText(columns[col-1]->start,columns[col-1]->height, columns[col-1]->width(), remainder, format, str);
    columns[col-1]->height += rect.height();
  }
}

void KTPrint :: levelColumns(int space)
{
  int ht(0);
	
  //Find the heighest height
  for (unsigned i(0); i < columns.size(); i++) {
   if (ht < columns[i]->height)
     ht = columns[i]->height;
  }
	
  //Set all the columns to that height and then add the space argument
  for (unsigned i(0); i < columns.size(); i++)
    columns[i]->height = ht+space;
}

void KTPrint :: finished ()
{
  if (paint!=NULL) {
    paint->end(); //Send to printer or file
    delete paint;
  }
}

void KTPrint :: newPage ()
{
  prnt->newPage();
  for (unsigned i(0); i < columns.size(); i++)
    columns[i]->height = topMargin;
}

int KTPrint :: numCopies () const
{
  return prnt->numCopies();
}

