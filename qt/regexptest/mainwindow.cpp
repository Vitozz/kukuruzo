#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdlib.h"

#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QRegExp>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QDir>
#include <QSettings>

#define TEXT_OPT "text"
#define FILENAME_OPT "filename"
#define REGEXP_OPT "regexp"
#define ISQTSTYLE_OPT "qtstyle"
#define ISW3C_OPT "isw3c"
#define ISCASE_OPT "iscase"
#define ISREGEXP_OPT "isregexp"
#define ISREGEXP2_OPT "isregexp2"
#define ISWILDCARD_OPT "iswildcard"
#define ISWILDCARDLUNIX_OPT "iswildcardlinux"
#define ISEXACT_OPT "isexact"
#define ISMINIMAL_OPT "isminimal"

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
	inputText_ = settings.value(TEXT_OPT, "").toString();
	fileName_ = settings.value(FILENAME_OPT, "").toString();
	regExpText_ = settings.value(REGEXP_OPT, "").toString();
	if(!regExpText_.isEmpty()&&(!regExpText_.isNull())){
		ui->RegExp->setText(regExpText_);
	}
	if(!fileName_.isEmpty()){
		ui->fileName->setText(fileName_);
		inputText_ = LoadTextFile(fileName_);
		ui->inputText->setPlainText(inputText_);
		dir_ = getDirName(fileName_) + "/";
	}
	else{
		if(!inputText_.isEmpty()&&(!inputText_.isNull())){
			ui->inputText->setPlainText(inputText_);
		}
	}
	ui->unquotebox->setChecked(settings.value(ISQTSTYLE_OPT, false).toBool());
	ui->iscase->setChecked(settings.value(ISCASE_OPT, false).toBool());
	ui->isexactmatch->setChecked(settings.value(ISEXACT_OPT, false).toBool());
	ui->isminimal->setChecked(settings.value(ISMINIMAL_OPT, false).toBool());
	ui->isregexp->setChecked(settings.value(ISREGEXP_OPT, false).toBool());
	ui->isregexp2->setChecked(settings.value(ISREGEXP2_OPT, false).toBool());
	ui->isw3c->setChecked(settings.value(ISW3C_OPT, false).toBool());
	ui->iswildcard->setChecked(settings.value(ISWILDCARD_OPT, false).toBool());
	ui->iswildcardunix->setChecked(settings.value(ISWILDCARDLUNIX_OPT, false).toBool());
}

void MainWindow::writeSettings(){
	QSettings settings;
	if(!fileName_.isEmpty()){
		settings.setValue(FILENAME_OPT, QVariant(fileName_));
		settings.setValue(TEXT_OPT, "");
	}else{
		settings.setValue(TEXT_OPT, QVariant(inputText_));
		settings.setValue(FILENAME_OPT, "");
	}
	settings.setValue(REGEXP_OPT, QVariant(regExpText_));
	settings.setValue(ISQTSTYLE_OPT, QVariant(ui->unquotebox->isChecked()));
	settings.setValue(ISW3C_OPT, QVariant(ui->isw3c->isChecked()));
	settings.setValue(ISCASE_OPT, QVariant(ui->iscase->isChecked()));
	settings.setValue(ISREGEXP_OPT, QVariant(ui->isregexp->isChecked()));
	settings.setValue(ISREGEXP2_OPT, QVariant(ui->isregexp2->isChecked()));
	settings.setValue(ISWILDCARD_OPT, QVariant(ui->iswildcard->isChecked()));
	settings.setValue(ISWILDCARDLUNIX_OPT, QVariant(ui->iswildcardunix->isChecked()));
	settings.setValue(ISEXACT_OPT, QVariant(ui->isexactmatch->isChecked()));
	settings.setValue(ISMINIMAL_OPT, QVariant(ui->isminimal->isChecked()));
}

QString MainWindow::GetRegexpList(const QList<QStringList> &matches, const int &parm, const int &pos) const
{
	QString result;
	if(!matches.isEmpty()){
		foreach(QStringList group, matches) {
			result += QString("<u><a style=\"color:red\">Match # </a>") + "<b>"+ QString::number(matches.indexOf(group)+1) + "</b></u><br>";
			if(parm == CHECK_ALL){
				for(int i=0; i<group.length();i++){
					result += QString("<a style=\"color:blue\">Group # </a>")+ "<b>"+QString::number(i)+ ": </b>"+"<i>" + Qt::escape(group[i]) + "</i><br>";
				}
			}
			else if(parm == CHECK_POS) {
				if(pos < group.length()) {
					result += QString("<a style=\"color:blue\">Group # </a>")+ "<b>"+ QString::number(pos)+ ": </b>"+"<i>" + Qt::escape(group[pos]) + "</i><br>";
				}
			}
		}
	}
	return result;
}

QList <QStringList> MainWindow::CheckExpression(const QString &regexp, const QString &text) const{
	QRegExp rx(regexp, sens_, pattern_);
	QList <QStringList> result;
	if(rx.isValid() && !rx.isEmpty()) {
		if(ui->isminimal->isChecked()) {
			rx.setMinimal(true);
		}
		else {
			rx.setMinimal(false);
		}
		int pos = rx.indexIn(text);
		while(((pos = rx.indexIn(text, pos)) != -1)&&(pos < text.length())){
			result << rx.capturedTexts();
			pos += rx.matchedLength();
		}
		ui->spinBox->setMaximum(rx.captureCount());
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
			isLoaded_ = true;
		}
	}else{
		isLoaded_ = false;
	}
	return result;
}

void MainWindow::RunOnlineHelp()
{
	QString helpdir;
#ifdef Q_OS_WIN32
	helpdir = qApp->applicationDirPath() + "/docs";
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
		if(!isLoaded_){
			ui->fileName->setText("");
			fileName_ = "";
		}else{
			isLoaded_ = false;
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

QString MainWindow::getDirName(const QString &filename) const{
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

QString MainWindow::replaceNotGreedy(QString text) const{
	if(!text.isEmpty()){
		text.replace("+?","+");
		text.replace("*?","*");
		text.replace("}?","}");
	}
	return text;
}

QString MainWindow::unquoteText(QString &text) const
{
	if (!text.isEmpty()) {
		text.replace("\\\\", "\\");
	}
	return text;
}

QString MainWindow::quoteText(QString &text) const
{
	if (!text.isEmpty()) {
		text.replace("\\", "\\\\");
	}
	return text;
}

void MainWindow::on_RegExp_textChanged(QString )
{
	if(!ui->RegExp->text().isEmpty()){
		originRegExpText_ = ui->RegExp->text();
		QString tmp = replaceNotGreedy(originRegExpText_);
		regExpText_ = ui->unquotebox->isChecked() ? unquoteText(tmp) : tmp;
	}
}

void MainWindow::on_unquotebox_toggled(bool checked)
{
	if (checked) {
		ui->RegExp->setText(quoteText(originRegExpText_));
	}
	else {
		ui->RegExp->setText(unquoteText(originRegExpText_));
	}
}
