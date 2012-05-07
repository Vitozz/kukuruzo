#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRegExp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *e);

private:
	Ui::MainWindow *ui;
	void RunOnlineHelp();
	QString LoadTextFile(const QString &filename);
	QList <QStringList> CheckExpression(const QString &regexp, const QString &text) const;
	QString GetRegexpList(const QList <QStringList> &matches, const int &parm, const int &pos) const;
	void readSettings();
	void writeSettings();
	QString replaceNotGreedy(QString text) const;
	QString unquoteText(QString &text) const;
	QString quoteText(QString &text) const;
	QString getDirName(const QString &filename) const;

	Qt::CaseSensitivity sens_;
	QRegExp::PatternSyntax pattern_;
	QString fileName_;
	QString inputText_;
	QString regExpText_;
	QString originRegExpText_;
	QString dir_;
	bool isLoaded_;

private slots:
	void on_RegExp_textChanged(QString );
	void on_openFile_clicked();
	void on_spinBox_valueChanged(int );
	void on_runit_clicked();
	void on_actionOnline_Help_activated();
	void on_inputText_textChanged();
	void on_fileName_textChanged(QString );
	void on_iswildcardunix_toggled(bool checked);
	void on_iswildcard_toggled(bool checked);
	void on_isregexp2_toggled(bool checked);
	void on_isregexp_toggled(bool checked);
	void on_iscase_toggled(bool checked);
	void on_isw3c_toggled(bool checked);
	void on_unquotebox_toggled(bool checked);
};

#endif // MAINWINDOW_H
