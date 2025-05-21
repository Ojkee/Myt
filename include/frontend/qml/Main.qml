import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

ApplicationWindow {
    visible: true
    width: 1400
    height: 600
    title: qsTr("Myt")

    GridView {
        id: gridView
        anchors.fill: parent
        cellWidth: 50
        cellHeight: 50
        model: 100000
        flow: GridView.LeftToRight
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        delegate: Cell {
            idx: index
            isFirstCol: index % columnCount === 0
            isFirstRow: index < columnCount
            colIdx: index % columnCount
            rowIdx: Math.floor(index / columnCount)
        }

        ScrollBar.horizontal: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        WheelHandler {
            id: wheelHandler
            target: gridView
            onWheel: (event) => {
                if (event.modifiers & Qt.ControlModifier) {
                    gridView.contentX -= event.angleDelta.y
                } else {
                    gridView.contentY -= event.angleDelta.y
                }
            }
        }
    }
}
