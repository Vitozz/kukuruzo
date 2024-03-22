/*
 * syntaxhighlighter.cpp
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

#include "syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent, const QString& pattern)
    : QSyntaxHighlighter(parent)
{
    textFormat.setFontWeight(QFont::Bold);
    textFormat.setForeground(Qt::red);
    rule_.pattern = QRegularExpression(pattern, QRegularExpression::UseUnicodePropertiesOption);
    rule_.format = textFormat;
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    QRegularExpressionMatchIterator matchIterator = rule_.pattern.globalMatch(text);
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), rule_.format);
    }
    setCurrentBlockState(0);
}
