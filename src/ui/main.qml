/*
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM

KCM.AbstractKCM {
    id: root

    implicitWidth: Kirigami.Units.gridUnit * 35
    implicitHeight: Kirigami.Units.gridUnit * 25

    headerPaddingEnabled: false

    header: ColumnLayout {
        spacing: Kirigami.Units.smallSpacing
        Kirigami.InlineMessage {
            id: errorMessage

            Layout.fillWidth: true
            position: Kirigami.InlineMessage.Position.Header
            type: Kirigami.MessageType.Error
            showCloseButton: true

            onVisibleChanged: {
                if (!visible) {
                    errorMessage.text = "";
                }
            }
        }

        RowLayout {
            spacing: Kirigami.Units.smallSpacing
            Layout.margins: Kirigami.Units.largeSpacing

            QQC2.Label {
                text: i18nc("@text", "Show:")
            }

            QQC2.RadioButton {
                text: i18nc("@title:tab", "User Schedule")
                checked: kcm.isPersonalUse
                onToggled: kcm.isPersonalUse = true
            }

            QQC2.RadioButton {
                text: i18nc("@title:tab", "System Schedule")
                checked: !kcm.isPersonalUse
                onToggled: kcm.isPersonalUse = false
            }
        }
    }

    actions: [
        Kirigami.Action {
            text: i18nc("@action:button", "Print")
            icon.name: "cups"
            onTriggered: kcm.print()
        }
    ]

    contentItem: QQC2.SplitView {
        id: view
        orientation: Qt.Vertical

        TasksComponent {
            clip: true
            QQC2.SplitView.fillWidth: true
            QQC2.SplitView.fillHeight: true
        }

        VariablesComponent {
            clip: true
            QQC2.SplitView.fillWidth: true
        }
    }

    Settings {
        id: persistentSettings

        property var viewState
    }

    Component.onCompleted: view.restoreState(persistentSettings.viewState)
    Component.onDestruction: persistentSettings.viewState = view.saveState()

    Connections {
        target: kcm

        function onShowOnboarding() {
            kcm.push("Onboarding.qml")
        }

        function onShowError(errorString, details) {
            errorMessage.text = "";
            errorMessage.text += errorString;
            errorMessage.text += "";
            errorMessage.text += details;
            errorMessage.visible = true;
        }
    }

    function pushPage(pageName, properties): void {
        kcm.refreshPages();
        kcm.push(pageName, properties);
    }
}
