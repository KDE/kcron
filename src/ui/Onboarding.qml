/*
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM

KCM.SimpleKCM {
    id: root

    implicitWidth: Kirigami.Units.gridUnit * 35
    implicitHeight: Kirigami.Units.gridUnit * 25

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: Math.min(fontMetrics.averageCharacterWidth * 70, root.width - Kirigami.Units.largeSpacing * 4)
        text: i18n("Welcome to Task Scheduler!")
        icon.name: "preferences-system-time"
        explanation: xi18nc("@info", "Schedule programs to run in the background. Set tasks to run on a fixed schedule and automate repetitive tasks like downloading files or sending emails. <link url='%1'>Learn more</link>", "https://man7.org/linux/man-pages/man8/cron.8.html")

        helpfulAction: QQC2.Action {
            text: i18nc("@action:button", "Go to Dashboard")
            onTriggered: kcm.pop()
        }

        onLinkActivated: (link) => Qt.openUrlExternally(link);
    }

    FontMetrics {
        id: fontMetrics
    }
}
