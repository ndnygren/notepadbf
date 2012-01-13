/* notepadbf - simple text editor with Blowfish encryption
* Copyright (C) 2012 Nick Nygren
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>. */

#include <QtGui>
#include <fstream>
#include <sstream>
#include "../blowfishStream.h"
#include "getPasswordDialog.h"

#ifndef NN_MAINNPBFWINDOW_H
#define NN_MAINNPBFWINDOW_H

class mainNPBFWindow : public QWidget
{
	Q_OBJECT
	private:
	QString titlebar;
	QString filename;
	std::string password;
	QTextEdit *textbox;
	QVBoxLayout *vbox;

	QMenuBar *menuBar;
	QMenu *fileMenu;
	QAction *newAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *exitAct;

	public slots:
	void newFile()
	{
		filename = "";
		setWindowTitle(titlebar);
		textbox->setText("");
	}

	void saveFile()
	{
		if (filename == "") { saveFileAs(); return; }

		blowfishStream bf(password);
		std::ofstream ofs;
		std::stringstream ss;

		ss.str(textbox->toPlainText().toStdString());

		ofs.open(filename.toStdString().c_str(), std::ofstream::out | std::ofstream::binary);

		bf.setOutStream(ofs);
		bf.encrypt(ss);
		
		ofs.close();
	}

	void saveFileAs()
	{
		QMessageBox *m1 = new QMessageBox(this);
		getPasswordDialog dia(this);
		filename = QFileDialog::getSaveFileName(this, tr("Save File As"), "", tr("Files (*.*)"));
		if (!dia.exec())
		{
			filename = "";
			return;
		}
		
		if (dia.getPass1() != dia.getPass2())
		{
			filename = "";
			m1->setWindowTitle("Error");
			m1->setText("Error: Passwords do not match.");
			m1->exec();
			return;
		}

		if (dia.getPass1().length() == 0)
		{
			filename = "";
			m1->setWindowTitle("Error");
			m1->setText("Error: Passwords is blank.");
			m1->exec();
			return;
		}

		password = dia.getPass1().toStdString();

		blowfishStream bf(password);
		std::ofstream ofs;
		std::stringstream ss;

		ss.str(textbox->toPlainText().toStdString());

		ofs.open(filename.toStdString().c_str(), std::ofstream::out | std::ofstream::binary);

		bf.setOutStream(ofs);
		bf.encrypt(ss);

		setWindowTitle(titlebar + filename);
		
		ofs.close();
	}

	void openFile()
	{
		QMessageBox m1;
		getPasswordDialog dia(this);
		filename = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.*)"));

		if (!dia.exec())
		{
			filename = "";
			return;
		}
		
		if (dia.getPass1() != dia.getPass2())
		{
			filename = "";
			m1.setWindowTitle("Error");
			m1.setText("Error: Passwords do not match.");
			m1.exec();
			return;
		}

		if (dia.getPass1().length() == 0)
		{
			filename = "";
			m1.setWindowTitle("Error");
			m1.setText("Error: Passwords is blank.");
			m1.exec();
			return;
		}

		password = dia.getPass1().toStdString();

		blowfishStream bf(password);
		std::fstream ifs;
		std::stringstream ss;

		ifs.open(filename.toStdString().c_str(), std::ios::in | std::ios::binary | std::ios::ate);
		if (ifs.fail() || ifs.bad())
		{
			filename = "";
			m1.setWindowTitle("Error");
			m1.setText("Error: Could not open file.");
			m1.exec();
			return;
		}
		else
		{
			ifs.seekg(0, std::ios::beg);
		}

		bf.setOutStream(ss);
		bf.decrypt(ifs);

		setWindowTitle(titlebar + filename);
		
		ifs.close();
		
		textbox->setText(ss.str().c_str());
	}

	public:

	mainNPBFWindow(QWidget *par = 0) : QWidget(par)
	{
		filename = "";
		titlebar = "notepadbf v0.1 ";
		setWindowTitle(titlebar);
		textbox = new QTextEdit();
		vbox = new QVBoxLayout();

		menuBar = new QMenuBar(this);

		fileMenu = menuBar->addMenu(tr("&File"));

		newAct = new QAction(tr("&New"), this);
		newAct->setShortcuts(QKeySequence::New);
		newAct->setStatusTip(tr("Create a new file"));
		connect(newAct,SIGNAL(triggered()),this,SLOT(newFile()));

		openAct = new QAction(tr("&Open"), this);
		openAct->setShortcuts(QKeySequence::Open);
		openAct->setStatusTip(tr("Open an existing file"));
		connect(openAct,SIGNAL(triggered()),this,SLOT(openFile()));

		saveAct = new QAction(tr("&Save"), this);
		saveAct->setShortcuts(QKeySequence::Save);
		saveAct->setStatusTip(tr("Save file"));
		connect(saveAct,SIGNAL(triggered()),this,SLOT(saveFile()));

		saveAsAct = new QAction(tr("Save &As"), this);
		saveAsAct->setStatusTip(tr("Save file under new location"));
		connect(saveAsAct,SIGNAL(triggered()),this,SLOT(saveFileAs()));

		exitAct = new QAction(tr("E&xit"), this);
		exitAct->setStatusTip(tr("Close program"));
		connect(exitAct,SIGNAL(triggered()),this,SLOT(close()));

		fileMenu->addAction(newAct);
		fileMenu->addSeparator();
		fileMenu->addAction(openAct);
		fileMenu->addAction(saveAct);
		fileMenu->addAction(saveAsAct);
		fileMenu->addSeparator();
		fileMenu->addAction(exitAct);

		vbox->addWidget(textbox);
		vbox->setMenuBar(menuBar);
		setLayout(vbox);
	}
};

#endif

