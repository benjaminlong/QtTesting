/*=========================================================================

   Program: ParaView
   Module:    pqPlayBackEventsDialog.cxx

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "pqEventDispatcher.h"
#include "pqEventPlayer.h"
#include "pqPlayBackEventsDialog.h"
#include "pqTestUtility.h"

#include "ui_pqPlayBackEventsDialog.h"

#include <QFile>
#include <QFileDialog>
#include <QTableWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QStringListModel>
#include <QTimer>

#include <QDebug>

//////////////////////////////////////////////////////////////////////////////////
// pqImplementation

struct pqPlayBackEventsDialog::pqImplementation
{
public:
  pqImplementation(pqEventPlayer& player,
                   pqEventDispatcher& dispatcher,
                   pqTestUtility* testUtility)
    : Player(player), Dispatcher(dispatcher), TestUtility(testUtility)
  {
    this->CurrentRow = 0;
  }
  
  ~pqImplementation()
  {
  }

  Ui::pqPlayBackEventsDialog Ui;

  pqEventPlayer& Player;
  pqEventDispatcher& Dispatcher;
  pqTestUtility* TestUtility;

  double CurrentLine;
  double MaxLines;
  int CurrentRow;
  QStringList filenames;

  bool ActivePlayBack;
};

///////////////////////////////////////////////////////////////////////////////////
// pqPlayBackEventsDialog

// ----------------------------------------------------------------------------
pqPlayBackEventsDialog::pqPlayBackEventsDialog(pqEventPlayer& Player,
                                               pqEventDispatcher& Dispatcher,
                                               pqTestUtility* TestUtility,
                                               QWidget* Parent)
  : QDialog(Parent),
    Implementation(new pqImplementation(Player, Dispatcher, TestUtility))
{
  this->Implementation->Ui.setupUi(this);

  QObject::connect(&this->Implementation->Player,
                   SIGNAL(eventAboutToBePlayed(const QString&,const QString&, const QString&)),
                   this,
                   SLOT(onEventAboutToBePlayed(const QString&, const QString&, const QString&)));

  QObject::connect(this->Implementation->Ui.timeStepSpinBox, SIGNAL(valueChanged(const int&)),
                   &this->Implementation->Dispatcher, SLOT(changeTimeStep(const int&)));

  QObject::connect(this->Implementation->Ui.loadFileButton, SIGNAL(clicked()),
                   this, SLOT(onLoadFile()));

  QObject::connect(this->Implementation->Ui.playPauseButton, SIGNAL(clicked(bool)),
                   this, SLOT(onPlayButtonClicked(bool)));

  QObject::connect(this->Implementation->Ui.stopButton, SIGNAL(clicked()),
                   this, SLOT(onStopButtonClicked()));

  QObject::connect(this->Implementation->Ui.stepButton, SIGNAL(clicked()),
                   this, SLOT(onStepButtonClicked()));

  QObject::connect(this->Implementation->TestUtility, SIGNAL(stopped()),
                   this, SLOT(onStopButtonClicked()));

  QObject::connect(&this->Implementation->Dispatcher, SIGNAL(started()),
                   this, SLOT(onStarted()));

  this->onLoadFile();
}

// ----------------------------------------------------------------------------
pqPlayBackEventsDialog::~pqPlayBackEventsDialog()
{
  delete Implementation;
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::accept()
{
  QDialog::accept();
  QTimer::singleShot(0, this, SLOT(onAutoDelete()));
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::reject()
{
  QDialog::reject();
  QTimer::singleShot(0, this, SLOT(onAutoDelete()));
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::onAutoDelete()
{
  //stop the playback ...
  this->Implementation->Dispatcher.stop();
  delete this;
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::onEventAboutToBePlayed(const QString& Object,
                                                    const QString& Command,
                                                    const QString& Arguments)
{
  this->Implementation->CurrentLine++;
  QString command;
  QString arguments;
  QString object = Object.split('/').last();

  command = Command.length() > 20 ?
              Command.left(9) + "..." + Command.right(8) : Command;
  arguments = Arguments.length() > 30 ?
                Arguments.left(13) + "..." + Arguments.right(14) : Arguments;

  this->Implementation->Ui.commandLabel->setText(QString(tr("Command : %1 ")).arg(command));
  this->Implementation->Ui.argumentsLabel->setText(QString(tr("Argument(s) : %1 ")).arg(arguments));
  this->Implementation->Ui.objectLabel->setText(QString(tr("Object : %1 ")).arg(object));

  QWidget* widget = this->Implementation->Ui.tableWidget->cellWidget(
      this->Implementation->Ui.tableWidget->currentRow(), 1);
  QProgressBar* progressBar = qobject_cast<QProgressBar*>(widget);
  progressBar->setValue(
      static_cast<int>((this->Implementation->CurrentLine/this->Implementation->MaxLines)*100));
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::onLoadFile()
{
  QStringList filenames = QFileDialog::getOpenFileNames(this, "Macro File Name",
    QString(), "XML Files (*.xml)");
  if (!filenames.isEmpty())
    {
    this->Implementation->filenames = QStringList();
    this->Implementation->filenames = filenames;
    for(int i = 0 ; i < filenames.count() ; i++)
      {
      QFileInfo info(filenames[i]);
      QTableWidgetItem* item = new QTableWidgetItem(info.fileName());
      qDebug() << "TableWidget ... add " << item << i;
      this->Implementation->Ui.tableWidget->setRowCount(i+1);
      this->Implementation->Ui.tableWidget->setItem(i, 0, item);
      QProgressBar* bar = new QProgressBar();
      bar->setValue(0);
      this->Implementation->Ui.tableWidget->setCellWidget(i, 1, bar);
      }
    this->Implementation->Ui.tableWidget->update();
//    this->Implementation->Ui.loadFileView->setModel(new QStringListModel(fileNamesModel));

    this->Implementation->Ui.playPauseButton->setEnabled(true);
    this->Implementation->Ui.timeStepSpinBox->setEnabled(true);
    this->Implementation->Ui.stepButton->setEnabled(false);
    }
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::onPlayButtonClicked(bool value)
{
  if(value)
    {
    this->updateUiToPlayMode();
    if(!this->Implementation->ActivePlayBack)
      {
      this->Implementation->ActivePlayBack = true;
      this->Implementation->TestUtility->playTests(this->Implementation->filenames);
      }
    else
      {
      this->Implementation->Dispatcher.restart();
      }
    }
  else
    {
    this->Implementation->Dispatcher.pause();
    this->updateUiToPauseMode();
    }
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::onStopButtonClicked()
{
  this->Implementation->Dispatcher.stop();
  this->updateUiToStopMode();
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::onStepButtonClicked()
{
  this->Implementation->Dispatcher.oneStep();
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::onStarted()
{
  this->Implementation->Ui.tableWidget->setCurrentCell(this->Implementation->CurrentRow, 0);

  this->Implementation->MaxLines = 0;
  this->Implementation->CurrentLine = 0;

  QFile file(this->Implementation->filenames[this->Implementation->CurrentRow]);
  file.open(QIODevice::ReadOnly);
  char line[20000];
  while (file.readLine(line, 20000) != -1)
    {
    this->Implementation->MaxLines++;
    }
  this->Implementation->MaxLines = this->Implementation->MaxLines - 12;

  this->Implementation->CurrentRow ++;
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::updateUiToPlayMode()
{
  this->Implementation->Ui.loadFileButton->setEnabled(false);
  this->Implementation->Ui.stepButton->setEnabled(false);
  this->Implementation->Ui.stopButton->setEnabled(true);
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::updateUiToPauseMode()
{
  this->Implementation->Ui.stepButton->setEnabled(true);
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::updateUiToStopMode()
{
  this->Implementation->ActivePlayBack = false;
  this->Implementation->Ui.loadFileButton->setEnabled(true);
  this->Implementation->Ui.playPauseButton->setChecked(false);
  this->Implementation->Ui.stopButton->setEnabled(false);
  this->Implementation->Ui.commandLabel->setText(QString(tr("Command : ")));
  this->Implementation->Ui.argumentsLabel->setText(QString(tr("Argument(s) :")));
  this->Implementation->Ui.objectLabel->setText(QString(tr("Object : ")));
  this->Implementation->CurrentRow = 0;
}


