/****************************************************************************
   Copyright (C) 2015, Jacob Dawid <jacob@omg-it.works>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

// Own includes
#include "messageshighlighter.h"

MessagesHighlighter::MessagesHighlighter(QTextDocument *document) :
    QSyntaxHighlighter(document) {
    HighlightingRule rule;

    QStringList errorPatterns;
    errorPatterns
            << ".*[Cc]{1}ould not.*"
            << ".*[Cc]{1}annot.*"
            << ".*[Ff]{1}ail.*"
            << ".*[Ee]{1}rror.*";

    QStringList keywordPatterns;
    keywordPatterns
            << "\\bALSA:"
            << "Started JACK server successfully."
            << "Stopped JACK server successfully."
            << "Server is not running"
            << "\\brealtime mode\\b"
            << "\\bnon-realtime mode\\b"
            << "bit\\b"
            << "Hz\\b"
            << "\\bperiod\\b"
            << "\\bperiods\\b"
            << "\\bframes\\b"
            << "\\bbuffer\\b"
            << "\\b .*\\|.*\\b";

    _errorFormat.setForeground(Qt::red);
    foreach(QString pattern, errorPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = _errorFormat;
        highlightingRules.append(rule);
    }

    _keywordFormat.setFontWeight(QFont::Bold);
    foreach(QString pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = _keywordFormat;
        highlightingRules.append(rule);
    }

    _numberFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("\\b[-0-9\\.\\,]+");
    rule.format = _numberFormat;
    highlightingRules.append(rule);

    _quotationFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("\".*\"");
    rule.format = _quotationFormat;
    highlightingRules.append(rule);

    _timestampFormat.setForeground(Qt::gray);
    rule.pattern = QRegExp("\\[.*\\]");
    rule.format = _timestampFormat;
    highlightingRules.append(rule);
}

MessagesHighlighter::~MessagesHighlighter() {
}

void MessagesHighlighter::highlightBlock(const QString &text) {
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}

