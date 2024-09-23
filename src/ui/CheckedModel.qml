/*
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick

ListModel {
    function toggleAll(): void {
        const state = getCheckState();

        if (state === Qt.Checked)
            setAllUnchecked();
        else
            setAllChecked();
    }

    function getCheckState() {
        if (isAllUnchecked()) {
            return Qt.Unchecked;
        }

        if (isAllChecked()) {
            return Qt.Checked;
        }

        return Qt.PartiallyChecked;
    }

    function toogle(index): void {
        setProperty(index, "checked", !get(index)["checked"]);
    }

    function setAllChecked(): void {
        for (let i = 0; i < count; i++) {
            setProperty(i, "checked", true);
        }
    }

    function setAllUnchecked(): void {
        for (let i = 0; i < count; i++) {
            setProperty(i, "checked", false);
        }
    }

    function isAllChecked(): bool {
        for (let i = 0; i < count; i++) {
            if (!get(i)["checked"]) {
                return false;
            }
        }

        return true;
    }

    function isAllUnchecked(): bool {
        for (let i = 0; i < count; i++) {
            if (get(i)["checked"]) {
                return false;
            }
        }

        return true;
    }
}
