/*
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtCore
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import QtQuick.Dialogs as QtDialogs

import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.private.kcms.cron 1.0 as Private

// Use AbstractKCM to avoid Kirigami.Theme.View background
KCM.AbstractKCM {
    id: main

    title: main.edited ? i18nc("@title", "Edit Task") :
                         i18nc("@title", "New Task")

    property bool edited
    property var task

    headerPaddingEnabled: false
    framedView: false

    actions: [
        Kirigami.Action {
            text: i18nc("@action:button", "Save")
            Accessible.name: i18nc("@action:button accessible", "Save")
            icon.name: "document-save-symbolic"
            enabled: !errorMessage.visible
            visible: main.edited
            onTriggered: {
                kcm.pop();
                kcm.tasksModel.applyModify();
            }
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Create")
            Accessible.name: i18nc("@action:button accessible", "Create")
            icon.name: "document-new-symbolic"
            enabled: !errorMessage.visible
            visible: !main.edited
            onTriggered: {
                kcm.pop();
                kcm.tasksModel.applyCreate();
            }
        }
    ]

    Private.TaskValidator {
        id: validator
        task: main.task
    }

    header: Kirigami.InlineMessage {
        id: errorMessage
        visible: text !== ""
        type: Kirigami.MessageType.Error
        position: Kirigami.InlineMessage.Position.Header
        text: validator.errorString
    }

    contentItem: QQC2.ScrollView {
        Component.onCompleted: {
            if (background) {
                background.visible = false;
            }
        }

        contentItem: Flickable {
            bottomMargin: Kirigami.Units.largeSpacing
            contentHeight: content.height
            clip: true

            ColumnLayout {
                id: content
                x: Kirigami.Units.largeSpacing
                width: parent.width - Kirigami.Units.largeSpacing * 2
                spacing: Kirigami.Units.smallSpacing

                Kirigami.FormLayout {
                    Kirigami.ActionTextField {
                        id: taskCommand
                        Kirigami.FormData.label: i18nc("@label:textbox", "Command:")
                        text: main.task.command
                        onTextChanged: main.task.command = text

                        rightActions: [
                            Kirigami.Action {
                                icon.name: "document-open-symbolic"
                                text: i18nc("@action:button", "Browse the Program")
                                onTriggered: fileDialogComponent.incubateObject(main)
                            }
                        ]
                    }

                    QQC2.CheckBox {
                        checked: main.task.enabled
                        text: i18nc("@label:checkbox", "Enable this task")
                    }

                    QQC2.ComboBox {
                        id: taskUser
                        Kirigami.FormData.label: i18nc("@label:listbox", "User:")
                        editable: false
                        wheelEnabled: false
                        model: kcm?.userList ?? []
                        Layout.fillWidth: true
                        visible: kcm?.tasksModel.needUserColumn ?? false

                        onActivated: main.task.userLogin = taskUser.currentText

                        Component.onCompleted: {
                            if (main.task) {
                                const found = taskUser.find(main.task.userLogin);
                                taskUser.currentIndex = found >= 0 ? found : 0;
                            }
                        }
                    }

                    QQC2.TextField {
                        id: taskComment
                        Layout.fillWidth: true
                        Kirigami.FormData.label: i18nc("@label:textbox", "Comment:")
                        text: main.task.comment
                        onTextChanged: main.task.comment = text
                    }

                    QQC2.CheckBox {
                        id: taskSystemBootup
                        Kirigami.FormData.label: i18nc("@label:checkbox", "Run:")
                        checked: main.task.reboot
                        onToggled: main.task.reboot = taskSystemBootup.checked
                        text: i18nc("@label:checkbox", "When system starts")
                    }

                    QQC2.CheckBox {
                        id: taskEveryDay
                        checked: main.task.isEveryDay()
                        onToggled: main.__runEveryDay(checked)
                        text: i18nc("@label:checkbox", "Every day")
                    }
                }

                TimeCard {
                    enabled: !taskSystemBootup.checked && !taskEveryDay.checked
                    title: i18nc("@title:group", "Months")
                    model: monthsModel
                    columns: 4
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 25

                    onAllChanged: {
                        for (let i = 0; i < monthsModel.count; i++) {
                            main.task.setMonthEnabled(i + 1, monthsModel.get(i)["checked"]);
                        }
                        validator.validate();
                    }

                    onChanged: index => {
                        main.task.setMonthEnabled(index + 1, monthsModel.get(index)["checked"]);
                        validator.validate();
                    }
                }

                TimeCard {
                    enabled: !taskSystemBootup.checked && !taskEveryDay.checked
                    title: i18nc("@title:group", "Days of Month")
                    model: monthModel
                    delegateWidth: Kirigami.Units.gridUnit * 3
                    columns: 7
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 25

                    onAllChanged: {
                        for (let i = 0; i < monthModel.count; i++) {
                            main.task.setDayOfMonthEnabled(i + 1, monthModel.get(i)["checked"]);
                        }
                        validator.validate();
                    }

                    onChanged: index => {
                        main.task.setDayOfMonthEnabled(index + 1, monthModel.get(index)["checked"]);
                        validator.validate();
                    }
                }

                TimeCard {
                    enabled: !taskSystemBootup.checked && !taskEveryDay.checked
                    title: i18nc("@title:group", "Days of Week")
                    model: daysModel
                    columns: 4
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 25

                    onAllChanged: {
                        for (let i = 0; i < daysModel.count; i++) {
                            main.task.setDayOfWeekEnabled(i + 1, daysModel.get(i)["checked"]);
                        }
                        validator.validate();
                    }

                    onChanged: index => {
                        main.task.setDayOfWeekEnabled(index + 1, daysModel.get(index)["checked"]);
                        validator.validate();
                    }
                }

                TimeCard {
                    enabled: !taskSystemBootup.checked
                    title: i18nc("@title:group", "Hours")
                    model: hoursModel
                    delegateWidth: Kirigami.Units.gridUnit * 3
                    columns: 5
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 25

                    onAllChanged: {
                        for (let i = 0; i < hoursModel.count; i++) {
                            main.task.setHourEnabled(i, hoursModel.get(i)["checked"]);
                        }
                        validator.validate();
                    }

                    onChanged: index => {
                        main.task.setHourEnabled(index, hoursModel.get(index)["checked"]);
                        validator.validate();
                    }
                }

                TimeCard {
                    enabled: !taskSystemBootup.checked
                    title: i18nc("@title:group", "Minutes")
                    model: minutesModel
                    delegateWidth: Kirigami.Units.gridUnit * 3
                    columns: 6
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 25

                    onAllChanged: checked => {
                        for (let i = 0; i < minutesModel.count; i++) {
                            main.task.setMinuteEnabled(i, minutesModel.get(i)["checked"]);
                        }
                        validator.validate();

                        if (checked) {
                            minutesModel.showAll();
                        } else {
                            minutesModel.showLess();
                        }
                    }

                    onChanged: index => {
                        main.task.setMinuteEnabled(index, minutesModel.get(index)["checked"]);
                        validator.validate();
                    }

                    footer: FormCard.FormComboBoxDelegate {
                        textRole: "text"
                        valueRole: "value"

                        text: i18nc("@label:listbox", "Selection:")

                        currentIndex: 0

                        model: [
                            {"text": i18n("Custom selection"), "value": 0},
                            {"text": i18n("Each minute"), "value": 1},
                            {"text": i18n("Every 2 minutes"), "value": 2},
                            {"text": i18n("Every 5 minutes"), "value": 5},
                            {"text": i18n("Every 10 minutes"), "value": 10},
                            {"text": i18n("Every 15 minutes"), "value": 15},
                            {"text": i18n("Every 20 minutes"), "value": 20},
                            {"text": i18n("Every 30 minutes"), "value": 30},
                        ]

                        onActivated: {
                            if (currentValue === 0 || currentValue === 1 || currentValue === 2) {
                                minutesModel.showAll();
                            } else {
                                minutesModel.showLess();
                            }

                            for (let i = 0; i < minutesModel.count; i++) {
                                minutesModel.setProperty(i, "checked", i % currentValue === 0);
                                main.task.setMinuteEnabled(i, i % currentValue === 0);
                            }
                            validator.validate();
                        }
                    }
                }
            }
        }
    }

    Component {
        id: fileDialogComponent

        QtDialogs.FileDialog {
            id: fileDialog
            currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]

            onAccepted: {
                taskCommand.text = Qt.resolvedUrl(selectedFile).toString().replace("file://", "")
                destroy()
            }
            onRejected: destroy()
            Component.onCompleted: open()
        }
    }

    CheckedModel {
        id: monthsModel
        Component.onCompleted: {
            append({
                       "text": i18nc("@action:button", "January"),
                       "checked": main.task.isMonthEnabled(1)
                   });
            append({
                       "text": i18nc("@action:button", "February"),
                       "checked": main.task.isMonthEnabled(2)
                   });
            append({
                       "text": i18nc("@action:button", "March"),
                       "checked": main.task.isMonthEnabled(3)
                   });
            append({
                       "text": i18nc("@action:button", "April"),
                       "checked": main.task.isMonthEnabled(4)
                   });
            append({
                       "text": i18nc("@action:button", "May"),
                       "checked": main.task.isMonthEnabled(5)
                   });
            append({
                       "text": i18nc("@action:button", "June"),
                       "checked": main.task.isMonthEnabled(6)
                   });
            append({
                       "text": i18nc("@action:button", "July"),
                       "checked": main.task.isMonthEnabled(7)
                   });
            append({
                       "text": i18nc("@action:button", "August"),
                       "checked": main.task.isMonthEnabled(8)
                   });
            append({
                       "text": i18nc("@action:button", "September"),
                       "checked": main.task.isMonthEnabled(9)
                   });
            append({
                       "text": i18nc("@action:button", "October"),
                       "checked": main.task.isMonthEnabled(10)
                   });
            append({
                       "text": i18nc("@action:button", "November"),
                       "checked": main.task.isMonthEnabled(11)
                   });
            append({
                       "text": i18nc("@action:button", "December"),
                       "checked": main.task.isMonthEnabled(12)
                   });
        }
    }

    CheckedModel {
        id: monthModel
        Component.onCompleted: {
            for (var i = 1; i < 32; i++) {
                append({
                           "text": i,
                           "checked": main.task.isDayOfMonthEnabled(i)
                       });
            }
        }
    }

    CheckedModel {
        id: daysModel
        Component.onCompleted: {
            append({
                       "text": i18nc("@action:button", "Monday"),
                       "checked": main.task.isWeekDayEnabled(1)
                   });
            append({
                       "text": i18nc("@action:button", "Tuesday"),
                       "checked": main.task.isWeekDayEnabled(2)
                   });
            append({
                       "text": i18nc("@action:button", "Wednesday"),
                       "checked": main.task.isWeekDayEnabled(3)
                   });
            append({
                       "text": i18nc("@action:button", "Thursday"),
                       "checked": main.task.isWeekDayEnabled(4)
                   });
            append({
                       "text": i18nc("@action:button", "Friday"),
                       "checked": main.task.isWeekDayEnabled(5)
                   });
            append({
                       "text": i18nc("@action:button", "Saturday"),
                       "checked": main.task.isWeekDayEnabled(6)
                   });
            append({
                       "text": i18nc("@action:button", "Sunday"),
                       "checked": main.task.isWeekDayEnabled(7)
                   });
        }
    }

    CheckedModel {
        id: hoursModel
        Component.onCompleted: {
            for (let i = 0; i < 24; i++) {
                append({
                           "text": i,
                           "checked": main.task.isHourEnabled(i)
                       });
            }
        }
    }

    CheckedModel {
        id: minutesModel
        Component.onCompleted: {
            for (let i = 0; i < 60; i++) {
                append({
                           "text": i,
                           "checked": main.task.isMinuteEnabled(i),
                           "visible": true
                       });
            }

            showLess();
        }

        function showAll(): void {
            for (let i = 0; i < 60; i++) {
                setProperty(i, "visible", true);
            }
        }
        function showLess(): void {
            for (let i = 0; i < 60; i++) {
                setProperty(i, "visible", i % 5 === 0);
            }
        }
    }

    function __runEveryDay(flag): void {
        if (flag) {
            monthsModel.setAllChecked();
            monthModel.setAllChecked();
            daysModel.setAllChecked();
        } else {
            monthsModel.setAllUnchecked();
            monthModel.setAllUnchecked();
            daysModel.setAllUnchecked();
        }

        for (let i = 0; i < monthsModel.count; i++) {
            main.task.setMonthEnabled(i + 1, monthsModel.get(i)["checked"]);
        }

        for (let i = 0; i < monthModel.count; i++) {
            main.task.setDayOfMonthEnabled(i + 1, monthModel.get(i)["checked"]);
        }

        for (let i = 0; i < daysModel.count; i++) {
            main.task.setDayOfWeekEnabled(i + 1, daysModel.get(i)["checked"]);
        }

        validator.validate();
    }
}
