#! /usr/bin/env bash
$XGETTEXT `find . -name \*.qml -o -name "*.cpp"` -o $podir/kcron.pot
