
/***************************************************************************
 *   ktprint.h                                                             *
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

#include <vector>
#include <kprinter.h>

class QPainter;
class KPrinter;
class QString;
class QFont;

/**
  *Provides a wrapper for simple printing of text.
  */
class KTPrint{
public:

/**
  *Static formatting constants
  */
  static const int alignTextLeft;
  static const int alignTextRight;
  static const int alignTextCenter;

/**
  *Static Default Margin Constants
  */
  static const int defaultLeftMargin;
  static const int defaultRightMargin;
  static const int defaultTopMargin;
  static const int defaultBottomMargin;

/**
  * Contructor
  */
  KTPrint(bool _root) :
                leftMargin(defaultLeftMargin), 
                rightMargin (defaultRightMargin), 
                topMargin(defaultTopMargin),
                bottomMargin(defaultBottomMargin), 
                paint(0),
                root(_root)
  {
    prnt = new KPrinter;
    prnt->setOption("crontab","true");
    createColumns(1);
  }

/**
  * Destructor
  */
  ~KTPrint();
	
/**
  * Start the printing process, gain page info
  */
  bool start ();
	
/**
  * Send the information to the printer
  */
  void finished ();
	
/**
  * Set the font to be used
  */
  void setFont (const QFont &font);

/**
  * Get the current font
  */
  QFont getFont ()const;
	
/**
  * Print text
  */
  void print (const QString &str, int col =1, int alignment=KTPrint::defaultLeftMargin, bool wordWrap = true);

/**
  * Constructs the columns
  */
  void createColumns(unsigned num_columns =1);	

/**
  * Make sure that all columns start printing on the same line
  */
  void levelColumns(int space = 5);

/**
  * Insert a new page
  */
  void newPage();

/**
  *
  */
  int numCopies () const;  
  
/**
  * Whether crontab should be printed
  */
  bool crontab() const
  {
     return (prnt->option("crontab") == "true");
  }
  
/**  
  * Whether all users should be printed (root only)
  */
  bool allUsers() const
  {
     return (prnt->option("allusers") == "true");
  }
private:

 /**
   *Disable the copy constructor and the assignment operator
   */
  KTPrint (const KTPrint&) {}
  KTPrint& operator=(const KTPrint&) {return *this;}

/**
  * Left boundary
  */	
  int       leftMargin;

/**
  * Right boundary
  */
  int       rightMargin;

/**
  * Top boundary
  */
  int       topMargin;

/**
  * Bottom boundary
  */
  int       bottomMargin;
	
/**
  * Width of the page
  */
  int       width;

/**
  * Height of the page
  */
  int       height;

/**
  * Pointer to a painter object
  */
  QPainter *paint;

/**
  * Whether we are root or not
  */
  bool root;

/**
  * Pointer a printer object
  */
  KPrinter *prnt;

/**
  *Nest a column class to make text layout nicer
  */

  class Column {
    public:

     /**
       *Starting X co-ord
       */
      int start;

     /**
       *Finishing X co-ord
       */
      int finish;

     /**
       *Current Y co-ord
       */
      int height;


    /**
      *Return the width of the column
      */
      int width () const {return finish-start;}
  };

/**
  * Keep a vector of the columns
  */
  std::vector <Column*> columns;

};
	
#endif
