/**************************************************************************
 *   ktview.cpp                                                            *
 *   --------------------------------------------------------------------  *
 *   KT main GUI view implementation                                       *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include <stdlib.h>
#include <qstring.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <kapp.h>
#include <klocale.h>

#include "ktview.h"
#include "kticon.h"
#include "ktlistcron.h"
#include "ktlistvars.h"
#include "ktlistvar.h"
#include "ktlisttasks.h"
#include "ktlisttask.h"
#include "ktapp.h"
#include "ktvariable.h"
#include "kttask.h"
#include "resource.h"

#include "cthost.h"
#include "ctcron.h"
#include "ctvariable.h"
#include "cttask.h"

KTView::KTView(QWidget *parent, const char* name) :
  QWidget(parent, name),
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
  setBackgroundMode(PaletteBase);
  refresh();
  listView->setSelected(listView->firstChild(), true);
}

void KTView::refresh()
{
  QListView* tmpListView(listView);

  listView = new QListView(this);

  listView->setFrameStyle(QListView::Panel | QListView::Sunken);
  listView->setRootIsDecorated(true);
  listView->setAllColumnsShowFocus(true);

  const CTHost& cth(ktapp->getCTHost());

  if (cth.root())
    listView->addColumn(i18n("Users/Tasks/Variables"));
  else
    listView->addColumn(i18n("Tasks/Variables"));

  listView->addColumn(i18n("Value"));
  listView->addColumn(i18n("Description"));

  for (int item = 0; item <=2; item++)
    listView->setColumnWidthMode(item, QListView::Maximum);

  // for each user
  for (CTCronIterator i = (CTCronIterator)cth.cron.begin();
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

  connect(listView, SIGNAL(rightButtonClicked(QListViewItem*, const QPoint&, int)),
    SLOT(slotMenu(QListViewItem*, const QPoint&, int)));

  connect(listView, SIGNAL(selectionChanged(QListViewItem*)),
    SLOT(slotSetCurrent(QListViewItem*)));

  connect(listView, SIGNAL(doubleClicked(QListViewItem*)),
    SLOT(slotEdit(QListViewItem*)));

  connect(listView, SIGNAL(returnPressed(QListViewItem*)),
    SLOT(slotEdit(QListViewItem*)));

  // switch to the new view
  if (tmpListView)
  {
    tmpListView->hide();
    listView->show();
    listView->setFocus();
    delete tmpListView;
  }
}

KTView::~KTView()
{
  delete listView;
}

void KTView::print(QPrinter* m_pPrinter) const
{
  QPainter printpainter;
  printpainter.begin(m_pPrinter);

  // TODO: add your printing code here

  printpainter.end();
}

void KTView::resizeEvent (QResizeEvent* event)
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
  QListViewItem* qlvi = listView->currentItem();

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
  QListViewItem* qlvi = listView->currentItem();

  if (currentIsTask)
  {
    CTTask* temptask = new CTTask("", "", currentCTCron->syscron);

    // default user to root for syscron task
    /*
    if (temptask->system())
    {
      temptask->user="root";
    }
    */

    // edit task
    KTTask* kttask = new KTTask(temptask);
    kttask->exec();
    delete kttask;

    if (temptask->dirty())
    {
      currentCTCron->task.push_back(temptask);
      KTListTask* ktlt = new KTListTask(qlvi, currentCTCron, temptask);
      listView->setSelected(ktlt, true);
    }
    else
    {
      // cancelled
      delete temptask;
    }
  }
  else
  {
    CTVariable* tempvar = new CTVariable();

    // edit variable
    KTVariable* ktvar = new KTVariable(tempvar);
    ktvar->exec();
    delete ktvar;

    if (tempvar->dirty())
    {
      currentCTCron->variable.push_back(tempvar);
      KTListVar* ktlv = new KTListVar(qlvi, currentCTCron, tempvar);
      qlvi->sortChildItems(1, true);
      listView->setSelected(ktlv, true);
    }
    else
    {
      // cancelled
      delete tempvar;
    }
  }
}

void KTView::edit()
{
  QListViewItem* qlvi = listView->currentItem();

  if (currentIsTask)
  {
    if (currentCTTask)
    {
      // edit task
      KTTask(currentCTTask).exec();
      ((KTListTask*)(qlvi))->refresh();
      qlvi->parent()->sortChildItems(1, true);
      listView->triggerUpdate();
    }
  }
  else
  {
    if (currentCTVariable)
    {
      KTVariable(currentCTVariable).exec();
      ((KTListVar*)(qlvi))->refresh();
      qlvi->parent()->sortChildItems(1, true);
      listView->triggerUpdate();
    }
  }
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
  QString fullCommand(currentCTTask->command.c_str());
  QString command(fullCommand);
  int pos(command.find(" "));
  if (pos > 0) command = command.left(pos);

  if (command.find("/") == 0)
   return fullCommand;

  QString path(currentCTCron->path().c_str());
  int begin(0);
  int end(0);
  QString prefix;
  QString full;

  while ((end = path.find(":", begin)) > 0)
  {
    prefix = path.mid(begin, begin + end) + "/";
    full = prefix + command;
    if (QFileInfo(full).isExecutable())
      return QString(prefix + fullCommand);
    begin = end + 1;
  }
  prefix = path.mid(begin, begin + end) + "/";
  full = prefix + command;
  if (QFileInfo(full).isExecutable())
    return QString(prefix + fullCommand);
  return QString("");
}

void KTView::run() const
{
  QString command(absolute() + " &");
  system(command);
}

void KTView::enable(bool enable)
{
  QListViewItem* qlvi = listView->currentItem();

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

void KTView::slotMenu(QListViewItem* qlvi, const QPoint& qp, int i)
{
  listView->setSelected(qlvi, true);
  slotSetCurrent(qlvi);
  ktapp->slotEdit(qp);
}

void KTView::slotEdit(QListViewItem* qlvi)
{
  edit();
}

void KTView::slotSetCurrent (QListViewItem* qlvi)
{
  QListViewItem* parent = qlvi->parent();

  if (qlvi->text(0) == KTListVars::getDescription())
  {
    // variable label
    currentCTCron = ((KTListCron*)qlvi)->getCTCron();
    currentCTVariable = 0;
    currentCTTask = 0;
    currentIsTask = false;
    ktapp->enableNew();
    ktapp->enableModify(false);
    ktapp->enableDelete(false);
    ktapp->enableCut(false);
    ktapp->enableCopy(false);
    ktapp->enablePaste(clipboardCTVariable);
    ktapp->enableRunNow(false);
    ktapp->enableEnable(false, false);
  }
  else if (qlvi->text(0) == KTListTasks::getDescription())
  {
    // task label
    currentCTCron = ((KTListCron*)qlvi)->getCTCron();
    currentCTVariable = 0;
    currentCTTask = 0;
    currentIsTask = true;
    ktapp->enableNew();
    ktapp->enableModify(false);
    ktapp->enableDelete(false);
    ktapp->enableCut(false);
    ktapp->enableCopy(false);
    ktapp->enablePaste(clipboardCTTask);
    ktapp->enableRunNow(false);
    ktapp->enableEnable(false, false);
  }
  else if (parent)
  {
    if (parent->text(0) == KTListVars::getDescription())
    {
      // variable
      currentCTCron = ((KTListVar*)qlvi)->getCTCron();
      currentCTVariable = ((KTListVar*)qlvi)->getCTVariable();
      currentCTTask = 0;
      currentIsTask = false;
      ktapp->enableNew(false);
      ktapp->enableModify();
      ktapp->enableDelete();
      ktapp->enableCut();
      ktapp->enableCopy();
      ktapp->enablePaste(false);
      ktapp->enableRunNow(false);
      ktapp->enableEnable(true, currentCTVariable->enabled);
    }
    else if (parent->text(0) == KTListTasks::getDescription())
    {
      // task
      currentCTCron = ((KTListTask*)qlvi)->getCTCron();
      currentCTVariable = 0;
      currentCTTask = ((KTListTask*)qlvi)->getCTTask();
      currentIsTask = true;
      ktapp->enableNew(false);
      ktapp->enableModify();
      ktapp->enableDelete();
      ktapp->enableCut();
      ktapp->enableCopy();
      ktapp->enablePaste(false);

      ktapp->enableRunNow(false);
      ktapp->enableRunNow((currentCTTask->enabled) &&
        (absolute() != ""));
      ktapp->enableEnable(true, currentCTTask->enabled);
    }
  }
  else
  {
    // user
    currentCTCron = ((KTListCron*)qlvi)->getCTCron();
    currentCTVariable = 0;
    currentCTTask = 0;
    currentIsTask = true;
    ktapp->enableNew(false);
    ktapp->enableModify(false);
    ktapp->enableDelete(false);
    ktapp->enableCut(false);
    ktapp->enableCopy(false);
    ktapp->enableRunNow(false);
      ktapp->enableEnable(true, currentCTTask->enabled);
    ktapp->enablePaste(false);
    ktapp->enableEnable(false, false);
  }
}









