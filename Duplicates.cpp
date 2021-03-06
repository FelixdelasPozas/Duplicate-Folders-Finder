/*
 File: Duplicates.cpp
 Created on: 30/10/2018
 Author: Felix de las Pozas Alvarez

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Project
#include "Duplicates.h"
#include "AboutDialog.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QVariant>
#include <QDesktopServices>
#include <QDebug>

const QString Duplicates::FOLDER{"Folder"}; /** Selected folder settings key. */

const unsigned long long MEGABYTE{1024*1024};

//-----------------------------------------------------------------
Duplicates::Duplicates()
{
  setupUi(this);

  m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  m_table->setContextMenuPolicy(Qt::CustomContextMenu);
  m_table->horizontalHeader()->resizeSections(QHeaderView::ResizeMode::ResizeToContents);

  connectSignals();

  loadSettings();
}

//--------------------------------------------------------------------
Duplicates::~Duplicates()
{
  saveSettings();
}

//-----------------------------------------------------------------
void Duplicates::connectSignals()
{
  connect(m_about,      SIGNAL(pressed()), this, SLOT(onAboutPressed()));
  connect(m_quit,       SIGNAL(pressed()), this, SLOT(close()));
  connect(m_folderPick, SIGNAL(pressed()), this, SLOT(openFolderSelectionDialog()));
  connect(m_search,     SIGNAL(pressed()), this, SLOT(scan()));
  connect(m_table,      SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onMenuRequested(const QPoint &)));
}

//-----------------------------------------------------------------
void Duplicates::onAboutPressed()
{
  AboutDialog dialog{this};
  dialog.exec();
}

//-----------------------------------------------------------------
void Duplicates::openFolderSelectionDialog()
{
  QDir currentDir{m_folder->text()};
  if(!currentDir.exists() || !currentDir.isReadable()) currentDir = QApplication::applicationDirPath();

  QFileDialog dialog{this};
  dialog.setWindowIcon(QIcon(":/Duplicates/folder.svg"));
  dialog.setWindowTitle(tr("Open folder to scan"));
  dialog.setFileMode(QFileDialog::DirectoryOnly);
  dialog.setFilter(QDir::AllDirs | QDir::Readable);
  dialog.setDirectory(currentDir.absolutePath());

  bool invalid = true;
  while(invalid && (dialog.exec() == QDialog::Accepted))
  {
    auto selection = dialog.selectedFiles();
    if(selection.size() == 1)
    {
      QDir selectedDir{selection.first()};

      if(selectedDir.exists() && selectedDir.isReadable())
      {
        m_folder->setText(QDir::toNativeSeparators(selectedDir.absolutePath()));
        invalid = false;
      }
    }

    if(invalid)
    {
      QMessageBox msgBox;
      msgBox.setWindowTitle(tr("Error"));
      msgBox.setWindowIcon(QIcon(":/Duplicates/folder.svg"));
      msgBox.setText(tr("The selected directory is invalid"));
      msgBox.setIcon(QMessageBox::Icon::Critical);
      msgBox.exec();
    }
  }

  m_folderPick->setDown(false);
}

//--------------------------------------------------------------------
void Duplicates::scan()
{
  m_table->clearContents();
  m_table->setRowCount(0);
  m_inserted->setText("0");

  m_progress->setEnabled(true);
  m_progress->setValue(0);

  m_search->setEnabled(false);

  QDir directory{m_folder->text()};

  auto thread = new ScanThread(directory, this);
  connect(thread, SIGNAL(progress(int)), m_progress, SLOT(setValue(int)));
  connect(thread, SIGNAL(found(const QString &, const QString &, const float, const QString &, const float)), this, SLOT(onFound(const QString &, const QString &, const float, const QString &, const float)));
  connect(thread, SIGNAL(finished()), this, SLOT(onThreadFinished()));

  thread->start();
}

//--------------------------------------------------------------------
void Duplicates::loadSettings()
{
  QSettings settings("Felix de las Pozas Alvarez", "DuplicatesFinder");

  QDir directory{settings.value(FOLDER, QApplication::applicationDirPath()).toString()};

  m_folder->setText(QDir::toNativeSeparators(directory.absolutePath()));
}

//--------------------------------------------------------------------
void Duplicates::onThreadFinished()
{
  auto thread = qobject_cast<ScanThread *>(sender());
  if(thread)
  {
    QApplication::restoreOverrideCursor();

    m_search->setEnabled(true);

    m_progress->setValue(0);
    m_progress->setEnabled(false);

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Icon::Information);
    msgBox.setWindowTitle(tr("Search results"));
    msgBox.setWindowIcon(QIcon(":/Duplicates/magnifying-glass.svg"));

    if(m_table->rowCount() == 0)
    {
      msgBox.setText(tr("No duplicates found!"));
    }
    else
    {
      msgBox.setText(tr("Found %1 duplicates!").arg(m_table->rowCount()));
      for (int c = 0; c < m_table->horizontalHeader()->count(); ++c)
      {
        m_table->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);
      }
    }

    msgBox.exec();

    m_search->setDown(false);

    m_inspected->setText(QString::number(thread->inspected()));

    thread->deleteLater();
  }
  else
  {
    qWarning() << "Unable to cast to ScanThread";
  }
}

//--------------------------------------------------------------------
void Duplicates::saveSettings()
{
  QDir directory{m_folder->text()};

  if(directory.exists() && directory.isReadable())
  {
    QSettings settings("Felix de las Pozas Alvarez", "DuplicatesFinder");

    settings.setValue(FOLDER, directory.absolutePath());
    settings.sync();
  }
}

//--------------------------------------------------------------------
void Duplicates::onActionTriggered()
{
  auto object = qobject_cast<QAction *>(sender());

  if(object)
  {
    auto folder = object->data().toString();
    if(!folder.isEmpty())
    {
      QDesktopServices::openUrl(QUrl::fromLocalFile(folder));
    }
  }
}

//--------------------------------------------------------------------
const float ScanThread::processDirectory(const QString& directoryPath)
{
  const QString separator{"/"};
  float size = 0.f;

  const QDir directory{directoryPath};
  Q_ASSERT(directory.exists());

  const auto dirEntries = directory.entryInfoList(QDir::Filter::NoDotAndDotDot|QDir::Filter::AllDirs);
  for(auto entry: dirEntries)
  {
    size += processDirectory(entry.absoluteFilePath());
  }

  const auto fileEntries = directory.entryInfoList(QDir::Filter::Files);
  for(auto entry: fileEntries)
  {
    size += static_cast<float>(entry.size())/MEGABYTE;
  }

  DirectoryInfo info;
  info.path = directory.absolutePath();
  info.name = info.path.split(separator).last().toLower();
  info.size = size;

  if((info.name.compare("Variado", Qt::CaseInsensitive) == 0) ||
     (info.name.compare("Various", Qt::CaseInsensitive) == 0) ||
     (info.name.startsWith("CD ", Qt::CaseInsensitive))       ||
     size == 0)
  {
    return 0;
  }

  auto hashValue = hash(info);

  if(m_directories.keys().contains(hashValue))
  {
    for(const auto entry: m_directories.value(hashValue))
    {
      if(entry.name == info.name)
      {
        auto parts = entry.path.split(separator);
        auto name  = parts.last();

        auto parent1 = QDir::toNativeSeparators(entry.path.left(entry.path.size()-entry.name.size()));
        parts = info.path.split(separator);

        auto parent2 = QDir::toNativeSeparators(info.path.left(info.path.size()-info.name.size()));

        emit found(name, parent1, entry.size, parent2, info.size);

        break;
      }
    }
  }

  m_directories[hashValue] << info;

  return size;
}

//--------------------------------------------------------------------
void Duplicates::onMenuRequested(const QPoint& pos)
{
  auto item = m_table->itemAt(pos);
  if(item)
  {
    auto row = item->row();
    auto original = m_table->item(row,1)->text() + m_table->item(row,0)->text() + QDir::separator();
    auto duplicate = m_table->item(row,3)->text() + m_table->item(row,0)->text() + QDir::separator();

    QMenu menu{this};

    auto action = menu.addAction(tr("Open original folder '%1'").arg(original));
    action->setData(original);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered()));

    action = menu.addAction(tr("Open duplicated folder '%1'").arg(duplicate));
    action->setData(duplicate);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered()));

    menu.exec(m_table->viewport()->mapToGlobal(pos));
  }
}

//--------------------------------------------------------------------
ScanThread::ScanThread(const QDir& directory, QObject* parent)
: QThread(parent)
, m_directory{directory}
{
}

//--------------------------------------------------------------------
void ScanThread::run()
{
  if(m_directory.exists() && m_directory.isReadable())
  {
    const auto dirEntries = m_directory.entryInfoList(QDir::Filter::NoDotAndDotDot|QDir::Filter::AllDirs);
    for(int i = 0; i < dirEntries.size(); ++i)
    {
      auto entry = dirEntries.at(i);
      processDirectory(entry.absoluteFilePath());

      emit progress(100*i/dirEntries.size());
    }
  }
}

//--------------------------------------------------------------------
void Duplicates::onFound(const QString& name, const QString& parent1, const float size1, const QString& parent2, const float size2)
{
  auto thread = qobject_cast<ScanThread *>(sender());
  if(thread) m_inspected->setText(QString::number(thread->inspected()));

  const auto rows = m_table->rowCount();
  m_table->insertRow(rows);

  auto item = new QTableWidgetItem {tr("%1").arg(name)}; //Name
  m_table->setItem(rows, 0, item);

  item = new QTableWidgetItem{tr("%1").arg(parent1)}; //Parent 1
  m_table->setItem(rows, 1, item);

  item = new QTableWidgetItem{tr("%1").arg(size1)}; //Size 1
  m_table->setItem(rows, 2, item);

  item = new QTableWidgetItem{tr("%1").arg(parent2)}; //Parent 2
  m_table->setItem(rows, 3, item);

  item = new QTableWidgetItem{tr("%1").arg(size2)}; //Size 2
  m_table->setItem(rows, 4, item);

  m_inserted->setText(tr("%1").arg(m_inserted->text().toInt() + 1));
}
