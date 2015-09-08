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

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->label->setReadOnly(true);
	connect(ui->actionE_xit, SIGNAL(triggered()), SLOT(on_actionE_xit_triggered()));
	connect(ui->actionOnline_Help, SIGNAL(changed()), SLOT(on_actionOnline_Help_triggered()));
	connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(ui->actionOpen, SIGNAL(triggered()), SLOT(on_openFile_clicked()));
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
	const QList <QStringList> matches = CheckExpression(regExpText_,inputText_);
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
		if (QFile::exists(fileName_)) {
			ui->fileName->setText(fileName_);
			inputText_ = LoadTextFile(fileName_);
			ui->inputText->setPlainText(inputText_);
			QFileInfo fi(fileName_);
			dir_ = fi.absolutePath();
		}
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
	const QString matchWildcard = tr("<u><a style=\"color:red\">Match # </a><b>%1</b></u><br>");
	const QString groupWildcard = tr("<a style=\"color:blue\">Group # </a><b>%1: </b><i>%2</i><br>");
	if(!matches.isEmpty()){
		foreach(const QStringList &group, matches) {
			result += matchWildcard.arg(QString::number(matches.indexOf(group)+1));
			if(parm == CHECK_ALL){
				foreach (const QString &item, group) {
#ifdef HAVE_QT5
					result += groupWildcard.arg(QString::number(group.indexOf(item)),item.toHtmlEscaped());
#else
					result += groupWildcard.arg(QString::number(group.indexOf(item)), Qt::escape(item));
#endif
				}
			}
			else if(parm == CHECK_POS) {
				if(pos < group.length()) {
#ifdef HAVE_QT5
					result += groupWildcard.arg(QString::number(pos), group[pos].toHtmlEscaped());
#else
					result += groupWildcard.arg(QString::number(pos), Qt::escape(group[pos]));
#endif
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
		return result;
	}
	else if (!rx.isValid()) {
		QMessageBox::critical(0, tr("Regexp error"), rx.errorString());
	}
	return result;
}


void MainWindow::on_actionOnline_Help_triggered()
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
			return result;
		}
	}
	else {
		isLoaded_ = false;
	}
	return result;
}

void MainWindow::RunOnlineHelp()
{
	QString helpdir, url;
	const QStringList dirs = QStringList() << "/usr/share/doc/regexptest/html"
					       << "/usr/local/share/doc/regexptest/html"
					       << "/usr/share/doc/regexptest/html"
					       << QString("%1/.local/share/doc/regexptest/html").arg(QDir::homePath())
					       << QString("%1/inst/docs").arg(qApp->applicationDirPath())
					       << QString("%1/docs").arg(qApp->applicationDirPath());
	foreach (const QString &item, dirs) {
		QDir _dir(item);
		if (_dir.exists()) {
			helpdir = item;
			break;
		}
	}
#ifdef Q_OS_WIN
	url = QString("file:///%1/regexp_help.html").arg(helpdir);
#else
	url = QString("file://%1/regexp_help.html").arg(helpdir);
#endif
	if (!QDesktopServices::openUrl(QUrl(url,QUrl::TolerantMode))){
		QMessageBox::warning(this,tr("Error"),tr("Unable to open regexp_help.html"));
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
	if (!ui->inputText->document()->isEmpty()) {
		inputText_ = ui->inputText->document()->toPlainText();
		if (!isLoaded_) {
			ui->fileName->setText("");
			fileName_ = "";
		}
		else {
			isLoaded_ = false;
		}
	}
}

void MainWindow::on_spinBox_valueChanged(int value)
{
	const QList <QStringList> matches = CheckExpression(regExpText_,inputText_);
	ui->label->document()->setHtml(GetRegexpList(matches,CHECK_POS,value));

}

void MainWindow::on_openFile_clicked()
{
	if(dir_.isEmpty() || dir_.isNull()) {
		dir_ = QDir::homePath();
	}
	QString str_fileName = QFileDialog::getOpenFileName(this,
							    tr("Open File..."),
							    dir_,
							    tr("All Files (*);;Text Files (*.txt)"));
	if (!str_fileName.isEmpty())
	{
		const QFileInfo fi(str_fileName);
		dir_ = fi.absolutePath();
		ui->fileName->setText(str_fileName);
		ui->inputText->setPlainText(LoadTextFile(str_fileName));

	}
}

QString MainWindow::getDirName(const QString &filename) const
{
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

QString MainWindow::replaceNotGreedy(const QString &text) const
{
	QString greedy = text;
	if(!greedy.isEmpty()){
		greedy.replace("+?","+");
		greedy.replace("*?","*");
		greedy.replace("}?","}");
	}
	return greedy;
}

QString MainWindow::unquoteText(const QString &text) const
{
	QString quoted = text;
	if (!quoted.isEmpty()) {
		quoted.replace("\\\\", "\\");
	}
	return quoted;
}

QString MainWindow::quoteText(const QString &text) const
{
	QString unqouted = text;
	if (!unqouted.isEmpty()) {
		unqouted.replace("\\", "\\\\");
	}
	return unqouted;
}

void MainWindow::on_RegExp_textChanged(QString )
{
	if(!ui->RegExp->text().isEmpty()){
		originRegExpText_ = ui->RegExp->text();
		const QString tmp = replaceNotGreedy(originRegExpText_);
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

void MainWindow::on_actionE_xit_triggered()
{
	close();
}
