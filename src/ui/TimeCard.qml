/*
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

Kirigami.AbstractCard {
    id: card

    property string title
    property var model
    property real delegateWidth: Kirigami.Units.gridUnit * 5

    property alias columns: grid.columns

    signal changed(int index)
    signal allChanged(bool checked)

    Layout.fillWidth: false
    Layout.alignment: Qt.AlignHCenter

    header: RowLayout {
        Layout.fillWidth: true
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            level: 3
            text: card.title
            type: Kirigami.Heading.Type.Primary
            Layout.fillWidth: true
        }

        QQC2.CheckBox {
            text: i18nc("@label:checkbox", "Select all")
            tristate: true
            checkState: card.model.getCheckState()

            onClicked: {
                card.model.toggleAll()
                card.allChanged(checked)
            }
        }
    }

    // NOTE: Without this item the whole TimeCard breaks and the GridLayout shifts
    contentItem: Item {
        implicitHeight: grid.implicitHeight

        GridLayout {
            id: grid

            anchors.horizontalCenter: parent.horizontalCenter
            columnSpacing: Kirigami.Units.smallSpacing
            rowSpacing: Kirigami.Units.smallSpacing

            Repeater {
                model: card.model
                delegate: Kirigami.Chip {
                    closable: false
                    implicitWidth: card.delegateWidth
                    text: model.text
                    checked: model.checked
                    visible: model.visible ?? true
                    checkable: true
                    onToggled: {
                        card.model.toogle(index)
                        card.changed(index)
                    }
                }
            }
        }
    }
}
