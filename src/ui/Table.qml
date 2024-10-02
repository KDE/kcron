/*
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.tableview as TableView

ColumnLayout {
    id: main

    spacing: Kirigami.Units.smallSpacing

    property string title
    property string iconName
    property var model

    default property alias columns: table.headerComponents
    property alias contextMenuActions: table.actions
    property list<Kirigami.Action> actions

    readonly property bool hasSelection: table.selectionModel.hasSelection
    readonly property bool hasFocus: table.activeFocus || table.focus

    signal create()
    signal edit()

    Kirigami.IconTitleSubtitle {
        title: main.title
        icon.name: main.iconName
        font.bold: true
    }

    RowLayout {
        spacing: Kirigami.Units.smallSpacing

        QQC2.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Component.onCompleted: {
                if (background) {
                    background.visible = true
                }
            }

            TableView.ListTableView {
                id: table

                clip: true

                selectionModel: main.model.selectionModel
                model: main.model.proxyModel

                onRowDoubleClicked: (row, mouse) => main.edit()

                sortOrder: Qt.AscendingOrder
                sortRole: table.model.sortRole

                onColumnClicked: function (index, headerComponent) {
                    if (table.model.sortRole === headerComponent.role) {
                        table.sortOrder = table.sortOrder === Qt.DescendingOrder ? Qt.AscendingOrder : Qt.DescendingOrder;
                    }

                    table.model.sortRole = headerComponent.role;
                    table.model.sort(0, table.sortOrder);
                    __resetSelection();
                }

                function __getSourceIndex(row) {
                    let proxyIndex = table.model.index(row, 0);
                    return table.model.mapToSource(proxyIndex);
                }

                function __resetSelection(): void {
                    // NOTE: Making a forced copy of the list
                    let selectedIndexes = Array(...table.selectionModel.selectedIndexes);
                    let currentIndex = table.selectionModel.currentIndex.row;
                    table.selectionModel.clear();

                    for (let i in selectedIndexes) {
                        table.selectionModel.select(selectedIndexes[i], ItemSelectionModel.Select);
                    }

                    table.selectionModel.setCurrentIndex(table.model.index(currentIndex, 0), ItemSelectionModel.Select);
                }
            }
        }

        ColumnLayout {
            spacing: Kirigami.Units.smallSpacing
            Layout.alignment: Qt.AlignTop

            Repeater {
                model: main.actions

                delegate: QQC2.Button {
                    action: modelData
                }
            }
        }
    }

    Connections {
        target: main.model

        function onNeedUserColumnChanged() {
            table.updateModel()
        }
    }
}
