/*
 * regexpwindow.cpp
 * Copyright (C) 2013-2022 Vitaly Tonkacheyev
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
#include "regexpwindow.h"
#include "ui_regexpwindow.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QIODevice>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>
#include <QTextStream>
#include <QUrl>
#ifdef IS_DEBUG
#include <QDebug>
#endif

#define TEXT_OPT "text"
#define FILENAME_OPT "filename"
#define REGEXP_OPT "regexp"
#define ISQTSTYLE_OPT "qtstyle"
#define DOTME_OPT "dotme"
#define ISCASE_OPT "iscase"
#define MULTIL_OPT "ismulti"
#define ISEXTPAT_OPT "isextpat"
#define ISWILDCARD_OPT "iswildcard"
#define ISINVGREED_OPT "isinvgreed"
#define DONTCAP_OPT "dontcap"
#define ISMINIMAL_OPT "isminimal"

RegexpWindow::RegexpWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::RegexpWindow)
    , isLoaded_(false)
{
    ui->setupUi(this);
    ui->label->setReadOnly(true);
    patternOptions_ = QRegularExpression::NoPatternOption;
    connect(ui->actionE_xit, &QAction::triggered, this, &RegexpWindow::on_actionE_xit_triggered);
    connect(ui->actionOnline_Help, &QAction::changed, this, &RegexpWindow::on_actionOnline_Help_triggered);
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionOpen, &QAction::triggered, this, &RegexpWindow::on_openFile_clicked);
    readSettings();
}

RegexpWindow::~RegexpWindow()
{
    if (highlighter_)
        delete highlighter_;
    delete ui;
}

void RegexpWindow::changeEvent(QEvent* e)
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

void RegexpWindow::closeEvent(QCloseEvent*)
{
    writeSettings();
}

void RegexpWindow::on_runit_clicked()
{
    const QList<QStringList> matches(CheckExpression(regExpText_, inputText_));
    if (!matches.isEmpty()) {
        ui->label->document()->setHtml(GetRegexpList(matches, CHECK_ALL, 0));
        if (highlighter_)
            delete highlighter_;
        highlighter_ = new SyntaxHighlighter(ui->inputText->document(), regExpText_);
    }
}

void RegexpWindow::readSettings()
{
    QSettings settings;
    inputText_ = settings.value(TEXT_OPT, "").toString();
    fileName_ = settings.value(FILENAME_OPT, "").toString();
    regExpText_ = settings.value(REGEXP_OPT, "").toString();
    if (!regExpText_.isEmpty() && (!regExpText_.isNull())) {
        ui->RegExp->setText(regExpText_);
    }
    if (!fileName_.isEmpty()) {
        if (QFile::exists(fileName_)) {
            ui->fileName->setText(fileName_);
            inputText_ = LoadTextFile(fileName_);
            ui->inputText->setPlainText(inputText_);
            QFileInfo fi(fileName_);
            dir_ = fi.absolutePath();
        }
    } else {
        if (!inputText_.isEmpty() && (!inputText_.isNull())) {
            ui->inputText->setPlainText(inputText_);
        }
    }
    ui->unquotebox->setChecked(settings.value(ISQTSTYLE_OPT, false).toBool());
    ui->iscase->setChecked(settings.value(ISCASE_OPT, false).toBool());
    ui->dontcapture->setChecked(settings.value(DONTCAP_OPT, false).toBool());
    ui->isminimal->setChecked(settings.value(ISMINIMAL_OPT, false).toBool());
    ui->multiline->setChecked(settings.value(MULTIL_OPT, false).toBool());
    ui->isextpattern->setChecked(settings.value(ISEXTPAT_OPT, false).toBool());
    ui->dotMatchEverithing->setChecked(settings.value(DOTME_OPT, false).toBool());
    ui->iswildcard->setChecked(settings.value(ISWILDCARD_OPT, false).toBool());
    ui->isinvgreed->setChecked(settings.value(ISINVGREED_OPT, false).toBool());
}

void RegexpWindow::writeSettings()
{
    QSettings settings;
    if (!fileName_.isEmpty()) {
        settings.setValue(FILENAME_OPT, QVariant(fileName_));
        settings.setValue(TEXT_OPT, "");
    } else {
        settings.setValue(TEXT_OPT, QVariant(inputText_));
        settings.setValue(FILENAME_OPT, "");
    }
    settings.setValue(REGEXP_OPT, QVariant(regExpText_));
    settings.setValue(ISQTSTYLE_OPT, QVariant(ui->unquotebox->isChecked()));
    settings.setValue(DOTME_OPT, QVariant(ui->dotMatchEverithing->isChecked()));
    settings.setValue(ISCASE_OPT, QVariant(ui->iscase->isChecked()));
    settings.setValue(MULTIL_OPT, QVariant(ui->multiline->isChecked()));
    settings.setValue(ISEXTPAT_OPT, QVariant(ui->isextpattern->isChecked()));
    settings.setValue(ISWILDCARD_OPT, QVariant(ui->iswildcard->isChecked()));
    settings.setValue(ISINVGREED_OPT, QVariant(ui->isinvgreed->isChecked()));
    settings.setValue(DONTCAP_OPT, QVariant(ui->dontcapture->isChecked()));
    settings.setValue(ISMINIMAL_OPT, QVariant(ui->isminimal->isChecked()));
}

QString RegexpWindow::GetRegexpList(const QVector<QStringList>& matches, const int& parm, const int& pos) const
{
    QString result;
    const QString matchWildcard(tr("<u><a style=\"color:red\">Match # </a><b>%1</b></u><br>"));
    const QString groupWildcard(tr("<a style=\"color:blue\">Group # </a><b>%1: </b><i>%2</i><br>"));
    if (!matches.isEmpty()) {
        for (const QStringList& groups : matches) {
            result += matchWildcard.arg(QString::number(matches.indexOf(groups)));
            switch (parm) {
            case CHECK_ALL:
                for (const QString& item : groups)
                    result += groupWildcard.arg(QString::number(groups.indexOf(item)), item.toHtmlEscaped());
                break;
            case CHECK_POS:
                if (pos < groups.length())
                    result += groupWildcard.arg(QString::number(pos), groups[pos].toHtmlEscaped());
                break;
            default:
                continue;
            }
        }
    }
    return result;
}

QVector<QStringList> RegexpWindow::CheckExpression(const QString& regexp, const QString& text) const
{
    QString pattern = regexp;
    if (ui->iswildcard->isChecked()) {
        pattern = QRegularExpression::wildcardToRegularExpression(regexp);
    }
    QRegularExpression rx(pattern, patternOptions_);
    QVector<QStringList> result;
    if (rx.isValid()) {
        QRegularExpression::MatchType mType_;
        if (ui->isminimal->isChecked()) {
            mType_ = QRegularExpression::PartialPreferFirstMatch;
        } else {
            mType_ = QRegularExpression::NormalMatch;
        }
        QRegularExpressionMatchIterator iter = rx.globalMatch(text, 0, mType_);
        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            QStringList matches;
            if (match.hasMatch()) {
                for (int i = 0; i <= rx.captureCount(); ++i) {
                    matches << match.captured(i);
                }
                result << matches;
            }
        }
#ifdef IS_DEBUG
        qDebug() << "Pattern" << regexp;
        qDebug() << "Pattern options" << patternOptions_;
        qDebug() << "text" << text;
        qDebug() << "Result" << result;
        qDebug() << "Match type" << mType_;
#endif
        ui->spinBox->setMaximum(rx.captureCount());
        return result;
    } else if (!rx.isValid()) {
        QMessageBox::critical(nullptr, tr("Regexp error"), rx.errorString());
    }
    return result;
}

void RegexpWindow::on_actionOnline_Help_triggered()
{
    RunOnlineHelp();
}

QString RegexpWindow::LoadTextFile(const QString& filename)
{
    QString result;
    if (!filename.isEmpty()) {
        QFile file(fileName_);
        QTextStream in(&file);
        if (file.open(QIODevice::ReadOnly)) {
            result = in.readAll();
            isLoaded_ = true;
            return result;
        }
    } else {
        isLoaded_ = false;
    }
    return result;
}

void RegexpWindow::RunOnlineHelp()
{
    QString helpdir, url;
    const QStringList dirs({ QStringLiteral("/usr/share/doc/regexptest/html"),
        QStringLiteral("/usr/local/share/doc/regexptest/html"),
        QStringLiteral("/usr/share/doc/regexptest/html"),
        QString("%1/.local/share/doc/regexptest/html").arg(QDir::homePath()),
        QString("%1/inst/docs").arg(qApp->applicationDirPath()),
        QString("%1/docs").arg(qApp->applicationDirPath()) });
    for (const QString& item : dirs) {
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
    if (!QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode))) {
        QMessageBox::warning(this, tr("Error"), tr("Unable to open regexp_help.html"));
    }
}

void RegexpWindow::on_dotMatchEverithing_toggled(bool checked)
{
    if (checked) {
        patternOptions_ |= QRegularExpression::DotMatchesEverythingOption;
    } else {
        patternOptions_ &= ~QRegularExpression::DotMatchesEverythingOption;
    }
}

void RegexpWindow::on_iscase_toggled(bool checked)
{
    if (checked) {
        patternOptions_ |= QRegularExpression::CaseInsensitiveOption;
    } else {
        patternOptions_ &= ~QRegularExpression::CaseInsensitiveOption;
    }
}

void RegexpWindow::on_multiline_toggled(bool checked)
{
    if (checked) {
        patternOptions_ |= QRegularExpression::MultilineOption;
    } else {
        patternOptions_ &= ~QRegularExpression::MultilineOption;
    }
}

void RegexpWindow::on_isextpattern_toggled(bool checked)
{
    if (checked) {
        patternOptions_ |= QRegularExpression::ExtendedPatternSyntaxOption;
    } else {
        patternOptions_ &= ~QRegularExpression::ExtendedPatternSyntaxOption;
    }
}

void RegexpWindow::on_isinvgreed_toggled(bool checked)
{
    if (checked) {
        patternOptions_ |= QRegularExpression::InvertedGreedinessOption;
    } else {
        patternOptions_ &= ~QRegularExpression::InvertedGreedinessOption;
    }
}

void RegexpWindow::on_fileName_textChanged(QString)
{
    QString temptext;
    if (!ui->fileName->text().isEmpty()) {
        temptext = ui->fileName->text();
        temptext.replace("\\", "/");
    }
    fileName_ = temptext;
}

void RegexpWindow::on_dontcapture_toggled(bool checked)
{
    if (checked) {
        patternOptions_ |= QRegularExpression::DontCaptureOption;
    } else {
        patternOptions_ &= ~QRegularExpression::DontCaptureOption;
    }
}

void RegexpWindow::on_inputText_textChanged()
{
    if (!ui->inputText->document()->isEmpty()) {
        inputText_ = ui->inputText->document()->toPlainText();
        if (!isLoaded_) {
            ui->fileName->clear();
            fileName_ = QString();
        } else {
            isLoaded_ = false;
        }
    }
}

void RegexpWindow::on_spinBox_valueChanged(int value)
{
    const QVector<QStringList> matches = CheckExpression(regExpText_, inputText_);
    ui->label->document()->setHtml(GetRegexpList(matches, CHECK_POS, value));
}

void RegexpWindow::on_openFile_clicked()
{
    if (dir_.isEmpty() || dir_.isNull()) {
        dir_ = QDir::homePath();
    }
    QString str_fileName = QFileDialog::getOpenFileName(this,
        tr("Open File..."),
        dir_,
        tr("All Files (*);;Text Files (*.txt)"));
    if (!str_fileName.isEmpty()) {
        const QFileInfo fi(str_fileName);
        dir_ = fi.absolutePath();
        ui->fileName->setText(str_fileName);
        ui->inputText->setPlainText(LoadTextFile(str_fileName));
    }
}

QString RegexpWindow::unquoteText(const QString& text) const
{
    QString quoted = text;
    if (!quoted.isEmpty()) {
        quoted.replace("\\\\", "\\");
    }
    return quoted;
}

QString RegexpWindow::quoteText(const QString& text) const
{
    QString unqouted = text;
    if (!unqouted.isEmpty()) {
        unqouted.replace("\\", "\\\\");
    }
    return unqouted;
}

void RegexpWindow::on_RegExp_textChanged(QString)
{
    if (!ui->RegExp->text().isEmpty()) {
        originRegExpText_ = ui->RegExp->text();
        regExpText_ = ui->unquotebox->isChecked() ? unquoteText(originRegExpText_) : originRegExpText_;
    }
}

void RegexpWindow::on_unquotebox_toggled(bool checked)
{
    ui->RegExp->setText((checked) ? quoteText(originRegExpText_) : unquoteText(originRegExpText_));
}

void RegexpWindow::on_actionE_xit_triggered()
{
    close();
}
