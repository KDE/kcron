/*
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

ColumnLayout {
    id: card

    property alias title: header.title
    property var model
    property real delegateWidth: Kirigami.Units.gridUnit * 5

    property alias columns: grid.columns
    property alias footer: formcard.delegates

    signal changed(int index)
    signal allChanged(bool checked)

    spacing: 0

    FormCard.FormHeader {
        id: header

        trailing: QQC2.CheckBox {
            text: i18nc("@label:checkbox", "Select all")
            tristate: true
            checkState: card.model.getCheckState()

            onClicked: {
                card.model.toggleAll()
                card.allChanged(checked)
            }
        }
    }

    FormCard.FormCard {
        id: formcard

        FormCard.AbstractFormDelegate {
            background: null
            contentItem: Item {
                implicitHeight: grid.implicitHeight

                GridLayout {
                    id: grid

                    width: parent.width
                    columnSpacing: Kirigami.Units.smallSpacing
                    rowSpacing: Kirigami.Units.smallSpacing

                    Repeater {
                        model: card.model
                        delegate: Kirigami.Chip {
                            closable: false
                            Layout.fillWidth: true
                            text: model.text
                            checked: model.checked
                            visible: model.visible ?? true
                            checkable: true
                            onToggled: {
                                card.model.toggle(index)
                                card.changed(index)
                            }
                        }
                    }
                }
            }
        }
    }
}
