#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdlib.h"

#include "QFile"
#include "QTextStream"
#include "QIODevice"
#include "QRegExp"
#include "QMessageBox"
#include "QDesktopServices"
#include "QUrl"
#include "QFileDialog"
#include "QDir"
#include "QSettings"

static const int CHECK_ALL = 1;
static const int CHECK_POS = 2;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->label->setReadOnly(true);
	readSettings();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::closeEvent(QCloseEvent *){
	writeSettings();
}

void MainWindow::on_runit_clicked()
{
	QList <QStringList> matches = CheckExpression(regExpText_,inputText_);
	ui->label->document()->setHtml(GetRegexpList(matches,CHECK_ALL,0));

}

void MainWindow::readSettings(){
	QSettings settings;
	inputText_ = settings.value("text", "").toString();
	fileName_ = settings.value("filename", "").toString();
	regExpText_ = settings.value("regexp", "").toString();
	if(!regExpText_.isEmpty()&&(!regExpText_.isNull())){
		ui->RegExp->setText(regExpText_);
	}
	if(!fileName_.isEmpty()){
		ui->fileName->setText(fileName_);
		inputText_ = LoadTextFile(fileName_);
		ui->inputText->setPlainText(inputText_);
		dir_ = getDirName(fileName_) + "/";
	}else{
		if(!inputText_.isEmpty()&&(!inputText_.isNull())){
			ui->inputText->setPlainText(inputText_);
		}
	}
}

void MainWindow::writeSettings(){
	QSettings settings;
	if(!fileName_.isEmpty()){
		settings.setValue("filename", fileName_);
		settings.setValue("text", "");
	}else{
		settings.setValue("text", inputText_);
		settings.setValue("filename", "");
	}
	settings.setValue("regexp", regExpText_);
}

QString MainWindow::GetRegexpList(const QList<QStringList> &matches, const int &parm, const int &pos){
	QString result;
	QStringList group;
	if(!matches.isEmpty()){
		foreach(group,matches){
			result += QString("<u><a style=\"color:red\">Match # </a>") + "<b>"+ QString::number(matches.indexOf(group)+1) + "</b></u><br>";
			if(parm == CHECK_ALL){
				for(int i=0; i<group.length();i++){
					result += QString("<a style=\"color:blue\">Group # </a>")+ "<b>"+QString::number(i)+ ": </b>"+"<i>" + Qt::escape(group[i]) + "</i><br>";
				}
			}else if(parm == CHECK_POS){
				if(pos < group.length()){
					result += QString("<a style=\"color:blue\">Group # </a>")+ "<b>"+ QString::number(pos)+ ": </b>"+"<i>" + Qt::escape(group[pos]) + "</i><br>";
				}
			}
		}
	}
	return result;
}

QList <QStringList> MainWindow::CheckExpression(const QString &regexp, const QString &text){
	QRegExp rx(regexp, sens_, pattern_);
	QList <QStringList> result;
	if(rx.isValid()){
		if(ui->isminimal->isChecked()){
			rx.setMinimal(true);
		}else{
			rx.setMinimal(false);
		}
		int pos = rx.indexIn(text);
		///qDebug("Pos %d", pos);
		while(((pos = rx.indexIn(text, pos)) != -1)&&(pos < text.length())){
			//qDebug("Pos %d", pos);
			ui->spinBox->setMaximum(rx.captureCount());
			result << rx.capturedTexts();
			pos += rx.matchedLength();
		}
	}
	return result;
}


void MainWindow::on_actionOnline_Help_activated()
{
	RunOnlineHelp();
}

QString MainWindow::LoadTextFile(const QString &filename){
	QString result;
	if(!filename.isEmpty()){
		QFile file(fileName_);
		QTextStream in(&file);
		if(file.open(QIODevice::ReadOnly)){
			result = in.readAll();
			isLoaded = true;
		}
	}else{
		isLoaded = false;
	}
	return result;
}

void MainWindow::RunOnlineHelp()
{
	QString helpdir;
#ifdef Q_OS_WIN32
	helpdir = "/" + qApp->applicationDirPath() + "/docs";
#endif
#ifdef Q_OS_LINUX
	helpdir = "/usr/share/doc/regexptest/html";
	QDir folder(helpdir);
	if (!folder.exists()) {
		helpdir = QString::fromStdString(getenv("HOME")) + "/.local/share/doc/regexptest/html";
		if (!folder.cd(helpdir)) {
			helpdir = "/" + qApp->applicationDirPath() + "/docs";
		}
	}
#endif
	if (!QDesktopServices::openUrl(QUrl("file://"+helpdir+ "/regexp_help.html",QUrl::TolerantMode))){
		QMessageBox::warning(this,"Error","Unable to open regexp_help.html");
	}
}

void MainWindow::on_isw3c_toggled(bool checked)
{
	pattern_ = checked ? QRegExp::W3CXmlSchema11 : QRegExp::RegExp;
}

void MainWindow::on_iscase_toggled(bool checked)
{
	sens_ = checked ? Qt::CaseSensitive : Qt::CaseInsensitive;
}

void MainWindow::on_isregexp_toggled(bool checked)
{
	pattern_ = checked ? QRegExp::RegExp : QRegExp::RegExp;
}

void MainWindow::on_isregexp2_toggled(bool checked)
{
	pattern_ = checked ? QRegExp::RegExp2 : QRegExp::RegExp;
}

void MainWindow::on_iswildcard_toggled(bool checked)
{
	pattern_ = checked ? QRegExp::Wildcard: QRegExp::RegExp;
}

void MainWindow::on_iswildcardunix_toggled(bool checked)
{
	pattern_ = checked ? QRegExp::WildcardUnix : QRegExp::RegExp;
}

void MainWindow::on_fileName_textChanged(QString )
{
	QString temptext;
	if(!ui->fileName->text().isEmpty()){
		temptext = ui->fileName->text();
		temptext.replace("\\","/");
	}
	fileName_ = temptext;
}

void MainWindow::on_inputText_textChanged()
{
	if(!ui->inputText->document()->isEmpty()){
		inputText_ = ui->inputText->document()->toPlainText();
		if(!isLoaded){
			ui->fileName->setText("");
			fileName_ = "";
		}else{
			isLoaded = false;
		}
	}
}

void MainWindow::on_spinBox_valueChanged(int value)
{
	QList <QStringList> matches = CheckExpression(regExpText_,inputText_);
	ui->label->document()->setHtml(GetRegexpList(matches,CHECK_POS,value));

}

void MainWindow::on_openFile_clicked()
{
#ifdef Q_OS_WIN32
	if(dir_.isEmpty()||dir_.isNull()){
		dir_ = "C:/";
	}
#endif
#ifdef Q_OS_LINUX
	if(dir_.isEmpty()||dir_.isNull()){
		dir_ = "/";
	}
#endif
	QString str_fileName = QFileDialog::getOpenFileName(this,
							    tr("Open File..."),
							    dir_,
							    tr("All Files (*);;Text Files (*.txt)"));
	if (!str_fileName.isEmpty())
	{
		dir_ = getDirName(str_fileName)+ "/";
		ui->fileName->setText(str_fileName);
		ui->inputText->setPlainText(LoadTextFile(str_fileName));

	}
}

QString MainWindow::getDirName(const QString &filename){
	QString result;
	QRegExp dirregexp("(.*)/(?:.*|\\..*)$");
	if (dirregexp.isValid()){
		int pos = dirregexp.indexIn(filename);
		if(pos > -1){
			result = dirregexp.cap(1);
		}
	}
	return result;
}

QString MainWindow::replaceNotGreedy(QString text){
	if(!text.isEmpty()&&(!text.isNull())){
		text.replace("+?","+");
		text.replace("*?","*");
		text.replace("}?","}");
		return text;
	}
	return "";
}

void MainWindow::on_RegExp_textChanged(QString )
{
	if(!ui->RegExp->text().isEmpty()){
		QString tmp = ui->RegExp->text();
		regExpText_ = replaceNotGreedy(tmp);
	}
}
