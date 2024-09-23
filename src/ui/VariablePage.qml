/*
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM

KCM.AbstractKCM {
    id: main

    title: main.edited ? i18nc("@title", "Edit Variable") :
                         i18nc("@title", "New Variable")

    property bool edited
    property var variable

    headerPaddingEnabled: false

    actions: [
        Kirigami.Action {
            text: i18nc("@action:button", "Edit Variable")
            Accessible.name: i18nc("@action:button accessible", "Edit Variable")
            icon.name: "document-open-symbolic"
            enabled: !errorMessage.visible
            visible: main.edited
            onTriggered: {
                kcm.pop();
                kcm.variablesModel.applyModify();
            }
        },
        Kirigami.Action {
            text: i18nc("@action:button", "New Variable")
            Accessible.name: i18nc("@action:button accessible", "New Variable")
            icon.name: "document-new-symbolic"
            enabled: !errorMessage.visible
            visible: !main.edited
            onTriggered: {
                kcm.pop();
                kcm.variablesModel.applyCreate();
            }
        }
    ]

    header: Kirigami.InlineMessage {
        id: errorMessage
        visible: text !== ""
        position: Kirigami.InlineMessage.Position.Header
        type: Kirigami.MessageType.Error
        text: {
            if (variableName.editText.trim() === "") {
                return xi18nc("@info", "<message>Please enter the variable name</message>");
            } else if (variableValue.text.trim() === "") {
                return xi18nc("@info", "<message>Please enter the variable value</message>");
            }

            return ""
        }
    }

    contentItem: Kirigami.FormLayout {
        Kirigami.IconTitleSubtitle {
            icon.source: main.variable.icon
            title: main.variable.information
        }

        QQC2.ComboBox {
            id: variableName
            editable: true
            wheelEnabled: false
            Kirigami.FormData.label: i18nc("@label:listbox", "Variable:")
            Layout.preferredWidth: Kirigami.Units.gridUnit * 16
            currentIndex: -1
            model: [
                "HOME",
                "MAILTO",
                "SHELL",
                "PATH",
                "LD_CONFIG_PATH",
            ]

            Component.onCompleted: variableName.editText = main.variable.name
            onActivated: main.variable.name = variableName.editText
            onEditTextChanged: main.variable.name = variableName.editText
        }

        QQC2.ComboBox {
            id: variableUser
            editable: false
            wheelEnabled: false
            model: kcm?.userList ?? []
            Kirigami.FormData.label: i18nc("@label:listbox", "User:")
            Layout.preferredWidth: Kirigami.Units.gridUnit * 16
            visible: kcm?.variablesModel.needUserColumn ?? false

            Component.onCompleted: {
                if (main.variable) {
                    variableUser.currentIndex = variableUser.find(main.variable.userLogin)
                }
            }

            onActivated: main.variable.userLogin = variableName.currentText
        }

        QQC2.TextField {
            id: variableValue
            Kirigami.FormData.label: i18nc("@label:textbox", "Value:")
            text: main.variable.value
            onTextChanged: main.variable.value = text
        }

        QQC2.TextField {
            id: variableComment
            Kirigami.FormData.label: i18nc("@label:textbox", "Comment:")
            text: main.variable.comment
            onTextChanged: main.variable.comment = text
        }

        QQC2.CheckBox {
            id: variableEnabled
            text: i18nc("@label:checkbox", "Enable this variable")
            checked: main.variable.enabled
            onToggled: main.variable.enabled = checked
        }
    }
}
