/*
 * mainwindow.h
 * Copyright (C) 2013-2019 Vitaly Tonkacheyev
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QRegExp;

namespace Ui {
class MainWindow;
}

enum checkType {
    CHECK_ALL = 1,
    CHECK_POS = 2
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
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
    QString replaceNotGreedy(const QString &text) const;
    QString unquoteText(const QString &text) const;
    QString quoteText(const QString &text) const;
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
    void on_actionOnline_Help_triggered();
    void on_actionE_xit_triggered();
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
