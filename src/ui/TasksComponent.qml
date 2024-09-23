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

    title: i18nc("@title:group", "Scheduled Tasks")
    iconName: "system-run-symbolic"
    model: kcm.tasksModel

    onCreate: root.pushPage("TaskPage.qml", {"edited": false, "task": main.model.create()})
    onEdit: root.pushPage("TaskPage.qml", {"edited": true, "task": main.model.modify()})

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
        role: Private.TasksModel.UserLoginRole
        title: i18nc("@title:column", "User")
        textRole: "userLogin"
        visible: main.model.needUserColumn
    }

    TableView.HeaderComponent {
        width: Kirigami.Units.gridUnit * 8
        minimumWidth: Kirigami.Units.gridUnit * 8

        role: Private.TasksModel.CronFormatRole
        title: i18nc("@title:column", "Scheduling")
        textRole: "cronFormat"
    }

    TableView.HeaderComponent {
        width: Kirigami.Units.gridUnit * 12
        minimumWidth: Kirigami.Units.gridUnit * 12

        role: Private.TasksModel.CommandRole
        title: i18nc("@title:column", "Command")
        textRole: "command"
    }

    TableView.HeaderComponent {
        width: Kirigami.Units.gridUnit * 12
        minimumWidth: Kirigami.Units.gridUnit * 12

        role: Private.TasksModel.CommentRole
        title: i18nc("@title:column", "Comment")
        textRole: "comment"
    }

    TableView.HeaderComponent {
        width: Kirigami.Units.gridUnit * 10
        minimumWidth: Kirigami.Units.gridUnit * 10
        role: Private.TasksModel.DescriptionRole
        title: i18nc("@title:column", "Scheduling Details")
        textRole: "description"
    }

    contextMenuActions: [
        Kirigami.Action {
            text: i18nc("@action:button", "Add Task…")
            icon.name: "document-new-symbolic"
            shortcut: "Ctrl+N"
            enabled: main.hasFocus
            onTriggered: main.createTask()
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Edit Task…")
            icon.name: "document-open-symbolic"
            shortcut: "Ctrl+M"
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
        },
        Kirigami.Separator {},
        Kirigami.Action {
            text: i18nc("@action:button", "Run task")
            icon.name: "system-run-symbolic"
            shortcut: "Ctrl+R"
            enabled: main.hasSelection && main.hasFocus
            onTriggered: main.model.run()
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
        },
        Kirigami.Action {
            icon.name: "system-run-symbolic"
            enabled: main.hasSelection
            onTriggered: main.model.run()
            text: i18nc("@action:button", "Run")
        }
    ]
}
