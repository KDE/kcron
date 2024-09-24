/*
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.tableview as TableView

import org.kde.private.kcms.cron as Private

Table {
    id: main

    title: i18nc("@title:group", "Environment Variables")
    iconName: "text-plain-symbolic"
    model: kcm.variablesModel

    onCreate: root.pushPage("VariablePage.qml", {"edited": false, "variable": main.model.create()})
    onEdit: root.pushPage("VariablePage.qml", {"edited": true, "variable": main.model.modify()})


    TableView.HeaderComponent {
        width: Kirigami.Units.gridUnit * 2
        minimumWidth: Kirigami.Units.gridUnit * 2

        textRole: "enabled"
        draggable: false
        resizable: false

        itemDelegate: QQC2.CheckBox {
            checked: model?.enabled ?? false
            onToggled: model.enabled = !model.enabled
        }

        headerDelegate: QQC2.CheckBox {
            tristate: true
            checkState: main.model.enabledState
            onClicked: {
                if (main.model.enabledState === Qt.Checked) {
                    main.model.enabledState = Qt.Unchecked;
                } else {
                    main.model.enabledState = Qt.Checked;
                }
            }
        }
    }

    TableView.HeaderComponent {
        width: Kirigami.Units.gridUnit * 8
        minimumWidth: Kirigami.Units.gridUnit * 8
        role: Private.VariablesModel.UserLoginRole
        title: i18nc("@title:column", "User")
        textRole: "userLogin"
        visible: main.model.needUserColumn
    }

    TableView.HeaderComponent {
        width: Kirigami.Units.gridUnit * 8
        minimumWidth: Kirigami.Units.gridUnit * 8

        role: Private.VariablesModel.NameRole
        title: i18nc("@title:column", "Variable")
        textRole: "name"

        itemDelegate: Item {
            width: parent.width

            Kirigami.IconTitleSubtitle {
                id: iconTitleSubtitle
                x: Kirigami.Units.largeSpacing
                width: parent.width - Kirigami.Units.largeSpacing * 2
                anchors.verticalCenter: parent.verticalCenter
                title: modelData
                icon.name: model?.icon ?? ""
                icon.width: Kirigami.Units.iconSizes.small

                QQC2.ToolTip.visible: truncated && handler.hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                QQC2.ToolTip.text: title

                HoverHandler {
                    id: handler
                    enabled: iconTitleSubtitle.truncated
                }
            }
        }
    }

    TableView.HeaderComponent {
        width: Kirigami.Units.gridUnit * 12
        minimumWidth: Kirigami.Units.gridUnit * 12
        role: Private.VariablesModel.ValueRole
        title: i18nc("@title:column", "Value")
        textRole: "value"
    }

    TableView.HeaderComponent {
        width: Kirigami.Units.gridUnit * 12
        minimumWidth: Kirigami.Units.gridUnit * 12
        role: Private.VariablesModel.CommentRole
        title: i18nc("@title:column", "Comment")
        textRole: "comment"
    }

    contextMenuActions: [
        Kirigami.Action {
            text: i18nc("@action:button", "Add Variable…")
            icon.name: "document-new-symbolic"
            enabled: main.hasFocus
            onTriggered: main.createVariable()
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Edit Variable…")
            icon.name: "document-open-symbolic"
            enabled: main.hasSelection && main.hasFocus
            onTriggered: main.edit()
        },
        Kirigami.Separator {},
        Kirigami.Action {
            text: i18nc("@action:button", "Select All")
            icon.name: "edit-select-all-symbolic"
            shortcut: "Ctrl+A"
            enabled: main.hasFocus
            onTriggered: main.model.selectAll()
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Copy")
            icon.name: "edit-copy-symbolic"
            shortcut: "Ctrl+C"
            enabled: main.hasSelection && main.hasFocus
            onTriggered: main.model.copy()
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Cut")
            icon.name: "edit-cut-symbolic"
            shortcut: "Ctrl+X"
            enabled: main.hasSelection && main.hasFocus
            onTriggered: main.model.cut()
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Paste")
            icon.name: "edit-paste-symbolic"
            shortcut: "Ctrl+V"
            // TODO: enabled with qclipboard has content
            enabled: main.hasFocus
            onTriggered: main.model.paste()
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Delete")
            icon.name: "edit-delete-symbolic"
            shortcut: "Ctrl+Del"
            enabled: main.hasSelection && main.hasFocus
            onTriggered: main.model.removeSelected()
        }
    ]

    actions: [
        Kirigami.Action {
            icon.name: "document-new-symbolic"
            onTriggered: main.create()
            text: i18nc("@action:button", "Add…")
        },
        Kirigami.Action {
            icon.name: "document-edit-symbolic"
            enabled: main.hasSelection
            onTriggered: main.edit()
            text: i18nc("@action:button", "Edit…")
        },
        Kirigami.Action {
            icon.name: "edit-delete-symbolic"
            enabled: main.hasSelection
            onTriggered: main.model.removeSelected()
            text: i18nc("@action:button", "Delete")
        }
    ]
}
