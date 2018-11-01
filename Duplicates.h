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
#include <QHash>

// Boost
#include <boost/functional/hash.hpp>

class QAction;
class QMenu;
class QPlainTextEdit;

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

	  /** \brief Process a directories recursively and returns the size in megabytes.
	   * \param[in] directoryPath Directory path.
	   *
	   */
	  const float processDirectory(const QString &directoryPath);

	  static const QString FOLDER; /** Folder settings text key. */

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

	  QHash<std::size_t, QList<struct DirectoryInfo>> m_directories; /** stores the directory information mapped with its hash. */
};

#endif /* DUPLICATES_H_ */
