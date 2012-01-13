
#include <QtGui>

#ifndef NN_GETPASSWORDDIALOG_H
#define NN_GETPASSWORDDIALOG_H

class getPasswordDialog : public QDialog
{
	Q_OBJECT
	private:
	
	QLabel *plab1, *plab2;
	QLineEdit *pass1, *pass2;
	QVBoxLayout *vbox;
	QHBoxLayout *hbox1, *hbox2, *hbox3;
	QPushButton *accbutton, *decbutton;

	public:
	QString getPass1() const { return pass1->text(); }
	QString getPass2() const { return pass2->text(); }

	getPasswordDialog(QWidget *par) : QDialog(par)
	{
		std::string diatitle = "Enter Password";

		plab1 = new QLabel("Password:");
		plab2 = new QLabel("Confirm:");
		pass1 = new QLineEdit();
		pass1->setEchoMode(QLineEdit::Password);
		pass2 = new QLineEdit();
		pass2->setEchoMode(QLineEdit::Password);


		vbox = new QVBoxLayout();
		hbox1 = new QHBoxLayout();
		hbox2 = new QHBoxLayout();
		hbox3 = new QHBoxLayout();


		accbutton = new QPushButton("Accept");
		decbutton = new QPushButton("Cancel");

		hbox1->addWidget(plab1);
		hbox1->addWidget(pass1);

		hbox2->addWidget(plab2);
		hbox2->addWidget(pass2);

		hbox3->addWidget(accbutton);
		hbox3->addWidget(decbutton);

		vbox->addLayout(hbox1);
		vbox->addLayout(hbox2);
		vbox->addLayout(hbox3);

		connect(accbutton, SIGNAL(clicked()), this, SLOT(accept()));
		connect(decbutton, SIGNAL(clicked()), this, SLOT(reject()));

		setLayout(vbox);
		setWindowTitle(diatitle.c_str());
	}
};

#endif

