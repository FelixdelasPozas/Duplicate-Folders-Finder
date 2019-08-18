/*
 File: Duplicates.h
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

#ifndef DUPLICATES_H_
#define DUPLICATES_H_

// Project
#include <ui_Duplicates.h>

// Qt
#include <QMainWindow>
#include <QThread>
#include <QHash>
#include <QDir>

// Boost
#include <boost/functional/hash/extensions.hpp>
#include <boost/functional/hash.hpp>

// C++
#include <crtdefs.h>
#include <string.h>

class QAction;
class QMenu;
class QPlainTextEdit;

/** \class ScanThread
 * \brief Thread for scanning a path.
 *
 */
class ScanThread
: public QThread
{
    Q_OBJECT
  public:
    /** \brief ScanThread class constructor.
     * \param[in] directory Starting directory.
     * \param[in] parent Raw pointer of the object parent of this one.
     *
     */
    explicit ScanThread(const QDir &directory, QObject *parent = nullptr);

    /** \brief ScanThread class virtual destructor.
     *
     */
    virtual ~ScanThread()
    {};

    /** \brief Returns the number of inspected directories.
     *
     */
    int inspected() const
    { return m_directories.size(); }

  signals:
    void progress(int);
    void found(const QString &name, const QString &parent1, const float size1, const QString &parent2, const float size2);

  protected:
    virtual void run() override;

  private:
    /** \brief Process a directories recursively and returns the size in megabytes.
     * \param[in] directoryPath Directory path.
     *
     */
    const float processDirectory(const QString &directoryPath);

    /** \struct DirectoryInfo
     * \brief Holds directory information.
     *
     */
    struct DirectoryInfo
    {
      QString name; /** directory basename.  */
      QString path; /** directory full path. */
      float   size; /** size in megabytes.   */
    };

    /** \brief Returns the hash of the given DirectoryInfo struct.
     * \param[in] info DirectoryInfo struct reference.
     *
     */
    std::size_t hash(const DirectoryInfo &info)
    {
      boost::hash<std::string> hasher;
      return hasher(info.name.toStdString());
    };

    const QDir m_directory; /** starting directory. */
    QHash<std::size_t, QList<struct DirectoryInfo>> m_directories; /** stores the directory information mapped with its hash. */
};

/** \class Duplicates
 * \brief Main dialog implementation.
 *
 */
class Duplicates
: public QMainWindow
, private Ui::Duplicates
{
    Q_OBJECT
  public:
    /** \brief Duplicates class constructor.
     *
     */
	  explicit Duplicates();

	  /** \brief Duplicates class virtual destructor.
	   *
	   */
	  virtual ~Duplicates();

	private slots:
	  /** \brief Shows the egocentric about dialog.
	   *
	   */
	  void onAboutPressed();

	  /** \brief Dialog to get the folder to search for duplicates.
	   *
	   */
	  void openFolderSelectionDialog();

	  /** \brief Scans the contents of folder for duplicates.
	   *
	   */
	  void scan();

	  /** \brief Shows context menu for the selected position of the table widget.
	   * \param[in] pos Click position of the request.
	   *
	   */
	  void onMenuRequested(const QPoint &pos);

	  /** \brief Helper method to open the folder specified in the custom context menu of the table.
	   *
	   */
	  void onActionTriggered();

	  /** \brief Inserts a row in the table with the given info.
	   * \param[in] name Directory name.
	   * \param[in] parent1 First parent name.
	   * \param[in] size1 First parent size in megabytes.
     * \param[in] parent2 Second parent name.
     * \param[in] size2 Second parent size in megabytes.
     *
	   */
	  void onFound(const QString &name, const QString &parent1, const float size1, const QString &parent2, const float size2);

	  /** \brief Updates the GUI when the scan thread finishes.
	   *
	   */
	  void onThreadFinished();

	private:
	  /** \brief Helper method to connect UI signals.
	   *
	   */
	  void connectSignals();

	  /** \brief Helper method to load the settings from the registry.
	   *
	   */
	  void loadSettings();

	  /** \brief Helper method to save the settings to the registry.
	   *
	   */
	  void saveSettings();

	  static const QString FOLDER; /** Folder settings text key. */
};

#endif /* DUPLICATES_H_ */
