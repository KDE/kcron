/**************************************************************************
 *   KT main GUI view implementation                                       *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktview.h"

#include <stdlib.h>
#include <unistd.h>

#include <config-kcron.h>

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream.h>
#endif

#include <string>


#include <q3header.h>
#include <qfileinfo.h>
#include <QFile>
#include <qdatetime.h>
//Added by qt3to4:
#include <QResizeEvent>

#include <kglobalsettings.h>
#include <klocale.h>
#include <kglobal.h>

#include "cthost.h"
#include "ctcron.h"
#include "ctvariable.h"
#include "cttask.h"

#include "kticon.h"
#include "ktlistcron.h"
#include "ktlistvars.h"
#include "ktlistvar.h"
#include "ktlisttasks.h"
#include "ktlisttask.h"
#include "ktapp.h"
#include "ktvariable.h"
#include "kttask.h"
#include "ktprint.h"

KTView::KTView(QWidget *parent) :
  QWidget(parent),
  listView(0),
  currentCTCron(0),
  currentIsTask(true),
  currentCTTask(0),
  currentCTVariable(0),
  clipboardIsTask(true),
  clipboardCTTask(0),
  clipboardCTVariable(0)
{
  ktapp = (KTApp*)parentWidget();
  setBackgroundRole(QPalette::Base);
  refresh();
  listView->setSelected(listView->firstChild(), true);
  
  connect(this, SIGNAL(enableModificationButtons(bool)), parent,
    SLOT(slotEnableModificationButtons(bool)));
    
  connect(this, SIGNAL(enablePaste(bool)), parent,
    SLOT(slotEnablePaste(bool)));
    
  connect(this, SIGNAL(enableRunNow(bool)), parent,
    SLOT(slotEnableRunNow(bool)));
  
  connect(this, SIGNAL(enableEnabled(bool)), parent,
    SLOT(slotEnableEnabled(bool)));
  
  emit(enableModificationButtons(false));
  emit(enablePaste(clipboardCTVariable));
  emit(enableRunNow(false));
            
}

void KTView::refresh()
{
  Q3ListView* tmpListView(listView);

  listView = new Q3ListView(this);

  listView->setFrameStyle(Q3ListView::Panel | Q3ListView::Sunken);
  listView->setRootIsDecorated(true);
  listView->setAllColumnsShowFocus(true);
  listView->setShowSortIndicator(true);

  const CTHost& cth(ktapp->getCTHost());

  if (cth.root())
    listView->addColumn(i18n("Users/Tasks/Variable"));
  else
    listView->addColumn(i18n("Tasks/Variables"));

  listView->addColumn(i18n("Value"));
  listView->addColumn(i18n("Description"));

  for (int item = 0; item <=2; item++)
    listView->setColumnWidthMode(item, Q3ListView::Maximum);

  // for each user
  for (CTCronIterator i = const_cast<CTHost&>(cth).cron.begin();
    i != cth.cron.end(); i++)
  {
    CTCron* ctcron((CTCron*)*i);

    KTListVars* variables(0);
    KTListTasks* tasks(0);

    if (cth.root())
    {
      KTListCron* user = new KTListCron(listView, ctcron);
      if (currentCTCron == ctcron)
      {
        listView->setSelected(user, true);
      }

      variables = new KTListVars(user, ctcron);
      tasks = new KTListTasks(user, ctcron);
    }
    else
    {
      variables = new KTListVars(listView, ctcron);
      tasks = new KTListTasks(listView, ctcron);
    }

    if (currentIsTask)
    {
      listView->setSelected(tasks, true);
      slotSetCurrent(tasks);
    }
    else
    {
      listView->setSelected(variables, true);
      slotSetCurrent(variables);
    }

    for (CTVariableIterator j = (CTVariableIterator)ctcron->variable.begin();
      j != ctcron->variable.end(); j++)
    {
      new KTListVar(variables, ctcron, (CTVariable*)*j);
    }

    for (CTTaskIterator k = (CTTaskIterator)ctcron->task.begin();
      k != ctcron->task.end(); k++)
    {
      new KTListTask(tasks, ctcron, (CTTask*)*k);
    }
  }

  resizeEvent();

  connect(listView, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint&, int)),
    SLOT(slotMenu(Q3ListViewItem*, const QPoint&, int)));

  connect(listView, SIGNAL(selectionChanged(Q3ListViewItem*)),
    SLOT(slotSetCurrent(Q3ListViewItem*)));

  connect(listView, SIGNAL(doubleClicked(Q3ListViewItem*)),
    SLOT(slotEdit(Q3ListViewItem*)));

  connect(listView, SIGNAL(returnPressed(Q3ListViewItem*)),
    SLOT(slotEdit(Q3ListViewItem*)));

  // switch to the new view
  if (tmpListView)
  {
    tmpListView->hide();
    listView->show();
    listView->setFocus();
    delete tmpListView;
  }
}

void KTView::print () const
{
  bool crontab, allUsers;
  KTListItem *ktli, *user;

  const CTHost& cth(ktapp->getCTHost());

  KTPrint printer(cth.root());

  if (printer.start())
  {
    crontab = printer.crontab();
    allUsers = printer.allUsers();

    printer.createColumns(3);

    int copies = printer.numCopies();
    while (copies != 0)
    {
      if (allUsers || !cth.root())
      {
        ktli = (KTListItem*)listView->firstChild();
      }
      else
      {
        ktli = (KTListItem*)listView->currentItem();
      }

      //Check that the correct item is selected, they must
      //select the top level entry for all items to print
      while (ktli->depth() != 0)
        ktli = (KTListItem*)ktli->parent();

      user = ktli; //Used to store user's name

      if (allUsers)
      {
        while (ktli)
        {
          pageHeading(ktli, printer);
          ktli->print(printer);
          if (crontab)
            pageFooter(ktli, printer);
          ktli = (KTListItem*)ktli->nextSibling();
          if (ktli)
            printer.newPage();
        }
      }
      else
      {
        //ktli goes out of range here hence the need for user
        pageHeading(user, printer);
        if (!cth.root())
        {
          while (ktli)
          {
            ktli->print(printer);
            ktli = (KTListItem*)ktli->nextSibling();
          }
        }
        else
          ktli->print(printer);

        if (crontab)
          pageFooter(user, printer);

      }

    copies--; //Keep a track of how many copies we have printed
    if (copies != 0) printer.newPage();
    }//End printing loop (for more than one copy)

    printer.finished(); //End the print

  }//End Printing if
}

void KTView :: pageHeading (KTListItem* user, KTPrint &printer) const
{
  QFont stnd;
  QString logonInfo;
  QDateTime now (QDateTime::currentDateTime());
  char hostName[20];

  gethostname(hostName, 20);
  // SSA : Fix Me user name, logon name and host name must be
  // SSA : not only in us-ascii ??
  logonInfo = i18nc("user on host", "%1 <placeholder>%2</placeholder> on %3",
     QString::fromLocal8Bit(user->getCTCron()->name.c_str()),
     QString::fromLocal8Bit(user->getCTCron()->login.c_str()),
     QString::fromLocal8Bit(hostName));

  stnd = printer.getFont();
  printer.setFont(QFont( KGlobalSettings::generalFont().family(), 14, QFont::Bold ));

  printer.print (i18n("Scheduled Tasks"), 2, KTPrint::alignTextCenter, false);
  printer.print (logonInfo, 2, KTPrint::alignTextCenter, false);
  printer.print (KGlobal::locale()->formatDateTime(now), 2, KTPrint::alignTextCenter, false);
  printer.setFont(stnd);

  printer.levelColumns(20);

}

void KTView :: pageFooter (KTListItem* user, KTPrint &printer) const
{
#ifdef HAVE_SSTREAM
  ostringstream oss;
#else
  char buffer[4096];
  ostrstream oss(buffer, sizeof(buffer));
#endif

  oss<<*(user->getCTCron())<<ends;

  if (oss) {
    string crontab(oss.str());
    printer.print(crontab.c_str(), 1, KTPrint::alignTextLeft, false);
  }
}

KTView::~KTView()
{
  delete listView;
}

void KTView::resizeEvent (QResizeEvent*)
{
  listView->setFixedWidth(width());
  listView->setFixedHeight(height());
  repaint();
}

void KTView::copy()
{
  if (clipboardCTTask)
  {
    delete clipboardCTTask;
    clipboardCTTask = 0;
  }

  if (clipboardCTVariable)
  {
    delete clipboardCTVariable;
    clipboardCTVariable = 0;
  }

  if (currentIsTask)
  {
    clipboardCTTask = new CTTask(*currentCTTask);
  }
  else
  {
    clipboardCTVariable = new CTVariable(*currentCTVariable);
  }
  clipboardIsTask = currentIsTask;
}

void KTView::paste()
{
  KTListItem* qlvi = (KTListItem*)listView->currentItem();

  if (currentIsTask)
  {
    CTTask* temptask = new CTTask(*clipboardCTTask);
    currentCTCron->task.push_back(temptask);
    KTListTask* ktlt = new KTListTask(qlvi, currentCTCron, temptask);
    listView->setSelected(ktlt, true);
  }
  else
  {
    CTVariable* tempvar = new CTVariable(*clipboardCTVariable);
    currentCTCron->variable.push_back(tempvar);
    KTListVar* ktlv = new KTListVar(qlvi, currentCTCron, tempvar);
    listView->setSelected(ktlv, true);
  }
}

void KTView::create()
{
  KTListItem* ktli = (KTListItem*)listView->currentItem();
  if ((ktli->text(0) != KTListTasks::getDescription()) && (ktli->text(0) != KTListVars::getDescription())) {
    ktli = (KTListItem*)ktli->parent();
  }
  Q_CHECK_PTR(ktli);
  ktli->create();
  listView->triggerUpdate();
}

void KTView::edit()
{
  KTListItem* ktli = (KTListItem*)listView->currentItem();
  Q_CHECK_PTR(ktli);
  ktli->edit();
  listView->triggerUpdate();
}

void KTView::remove()
{
  if (currentIsTask)
  {
    if (currentCTTask)
    {
      for (CTTaskIterator i = (CTTaskIterator)
        (currentCTCron->task.begin());
        i != currentCTCron->task.end(); i++)
      {
        if (*i == currentCTTask)
        {
          currentCTCron->task.erase(i);
          delete currentCTTask;
          currentCTTask = 0;
          break;
        }
      }
    }
  }
  else
  {
    if (currentCTVariable)
    {
      for (CTVariableIterator j = (CTVariableIterator)
        (currentCTCron->variable.begin());
        j != currentCTCron->variable.end(); j++)
      {
        if (*j == currentCTVariable)
        {
          currentCTCron->variable.erase(j);
          delete currentCTVariable;
          currentCTVariable = 0;
          break;
        }
      }
    }
  }
  refresh();
}

QString KTView::absolute() const
{
  QString fullCommand = QString::fromLocal8Bit(currentCTTask->command.c_str());
  QString command(fullCommand);
  int pos(command.indexOf(" "));
  if (pos > 0) command = command.left(pos);

  if (command.indexOf("/") == 0)
   return fullCommand;

  QString path = QString::fromLocal8Bit(currentCTCron->path().c_str());
  int begin(0);
  int end(0);
  QString prefix;
  QString full;

  while ((end = path.indexOf(":", begin)) > 0)
  {
    prefix = path.mid(begin, begin + end) + '/';
    full = prefix + command;
    if (QFileInfo(full).isExecutable())
      return QString(prefix + fullCommand);
    begin = end + 1;
  }
  prefix = path.mid(begin, begin + end) + '/';
  full = prefix + command;
  if (QFileInfo(full).isExecutable())
    return QString(prefix + fullCommand);
  return QString("");
}

void KTView::run() const
{
  QString command(absolute() + " &");
  system(QFile::encodeName(command));
}

void KTView::enable(bool enable)
{
  Q3ListViewItem* qlvi = listView->currentItem();

  if (currentIsTask)
  {
    if (currentCTTask)
    {
      currentCTTask->enabled = enable;
      ((KTListTask*)(qlvi))->refresh();
    }
  }
  else
  {
    if (currentCTVariable)
    {
      currentCTVariable->enabled = enable;
      ((KTListVar*)(qlvi))->refresh();
    }
  }
}

void KTView::slotMenu(Q3ListViewItem* qlvi, const QPoint& qp, int /*i*/)
{
  if (qlvi !=0 ) {
    listView->setSelected(qlvi, true);
    slotSetCurrent(qlvi);
    ktapp->slotEdit(qp);
  }
}

void KTView::slotEdit(Q3ListViewItem* /*qlvi*/)
{
  edit();
}

void KTView::slotSetCurrent (Q3ListViewItem* qlvi)
{
  Q3ListViewItem* parent = qlvi->parent();

  if (qlvi->text(0) == KTListVars::getDescription())
  {
    // variable label
    currentCTCron     = ((KTListCron*)qlvi)->getCTCron();
    currentCTVariable = 0;
    currentCTTask     = 0;
    currentIsTask     = false;
    emit(enableModificationButtons(false));
    emit(enablePaste(clipboardCTVariable));
    emit(enableRunNow(false));
    emit(enableEnabled(false));
    
  }
  else if (qlvi->text(0) == KTListTasks::getDescription())
  {
    // task label
    currentCTCron     = ((KTListCron*)qlvi)->getCTCron();
    currentCTVariable = 0;
    currentCTTask     = 0;
    currentIsTask     = true;
    
    emit(enableModificationButtons(false));
    emit(enablePaste(clipboardCTTask));
    emit(enableRunNow(false));
    emit(enableEnabled(false));
    
  }
  else if (parent)
  {
    if (parent->text(0) == KTListVars::getDescription())
    {
      // variable
      currentCTCron     = ((KTListVar*)qlvi)->getCTCron();
      currentCTVariable = ((KTListVar*)qlvi)->getCTVariable();
      currentCTTask     = 0;
      currentIsTask     = false;
      
      emit(enableModificationButtons(true));
      emit(enableRunNow(false));
      emit(enableEnabled(currentCTVariable->enabled));

    }
    else if (parent->text(0) == KTListTasks::getDescription())
    {
      // task
      currentCTCron     = ((KTListTask*)qlvi)->getCTCron();
      currentCTVariable = 0;
      currentCTTask     = ((KTListTask*)qlvi)->getCTTask();
      currentIsTask     = true;

      emit(enableModificationButtons(true));
      emit(enableRunNow((currentCTTask->enabled) &&
              (!absolute().isEmpty())));
      emit(enableEnabled(currentCTTask->enabled));
              
    }
  }
  else
  {
    // user
    currentCTCron     = ((KTListCron*)qlvi)->getCTCron();
    currentCTVariable = 0;
    currentCTTask     = 0;
    currentIsTask     = true;
    
    emit(enableModificationButtons(true));
    emit(enablePaste(false));
    emit(enableRunNow(false));
    emit(enableEnabled(false));
    
  }
}

#include "ktview.moc"
