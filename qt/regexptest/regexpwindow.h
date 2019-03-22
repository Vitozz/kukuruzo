/*
 * regexpwindow.h
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

#ifndef RegexpWindow_H
#define RegexpWindow_H

#include <QMainWindow>
#include <QRegularExpression>

namespace Ui {
class RegexpWindow;
}

enum checkType {
    CHECK_ALL = 1,
    CHECK_POS = 2
};

class RegexpWindow : public QMainWindow {
    Q_OBJECT
public:
    RegexpWindow(QWidget *parent = nullptr);
    ~RegexpWindow();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);

private:
    Ui::RegexpWindow *ui;
    void RunOnlineHelp();
    QString LoadTextFile(const QString &filename);
    QList<QStringList> CheckExpression(const QString &regexp, const QString &text) const;
    QString GetRegexpList(const QList <QStringList> &matches, const int &parm, const int &pos) const;
    void readSettings();
    void writeSettings();
    QString unquoteText(const QString &text) const;
    QString quoteText(const QString &text) const;

    QRegularExpression::PatternOptions patternOptions_;
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
    void on_isinvgreed_toggled(bool checked);
    void on_isextpattern_toggled(bool checked);
    void on_dontcapture_toggled(bool checked);
    void on_multiline_toggled(bool checked);
    void on_iscase_toggled(bool checked);
    void on_dotMatchEverithing_toggled(bool checked);
    void on_unquotebox_toggled(bool checked);
};

#endif // RegexpWindow_H
