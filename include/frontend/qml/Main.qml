import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

ApplicationWindow {
  title: qsTr("Myt")
  visible: true
  width: 1056
  height: 576

  GridView {
    id: gridView
    interactive: false
    anchors.fill: parent
    cellWidth: 96
    cellHeight: 36

    property int colCount: Math.floor(parent.width / cellWidth)
    property int rowCount: Math.floor(parent.height / cellHeight)
    property int colOffset: 0
    property int rowOffset: 0

    model: colCount * rowCount

    function moveCells(c, r, dc, dr) {
      var x =  c + dc
      var y = r + dr
      windowState.editingCol += dc
      windowState.editingRow += dr

      var padSize = 3
      if (c >= colOffset - padSize + colCount && dc > 0)           // RIGHT
        gridView.colOffset += dc
      else if (c > padSize && c <= colOffset + padSize && dc < 0)  // LEFT
        gridView.colOffset += dc

      if (r >= rowOffset - padSize + rowCount && dr > 0)           // DOWN
        gridView.rowOffset += dr
      else if (r > padSize && r <= rowOffset + padSize && dr < 0)  // UP
        gridView.rowOffset += dr

      updateVisibleCells()
    }

    function updateVisibleCells() {
      for (let i = 0; i < gridView.count; ++i) {
        const item = gridView.itemAtIndex(i)
        if (item && item.updateLabel) {
          item.updateLabel()
        }
      }
    }

    delegate: Cell {
      width: gridView.cellWidth
      height: gridView.cellHeight
      colIdx: index % gridView.colCount
      rowIdx: Math.floor(index / gridView.colCount)
      onMoveRequested: (c, r, dc, dr) => {
        gridView.moveCells(c, r, dc, dr)
      }
    }

    WheelHandler {
      id: wheelHandler
      acceptedDevices: PointerDevice.Mouse
      onWheel: (event) => {
        if (event.modifiers & Qt.ControlModifier) {
          gridView.colOffset = Math.max(0, gridView.colOffset - Math.sign(event.angleDelta.y))
        } else {
          gridView.rowOffset = Math.max(0, gridView.rowOffset - Math.sign(event.angleDelta.y))
        }
        event.accepted = true
      }
    }
  } 

  Connections {
    target: windowState
    function onRequestCellUpdate(col, row) {
      for (let i = 0; i < gridView.count; ++i) {
        const item = gridView.itemAtIndex(i)
        if (item && item.col === col && item.row === row) {
          item.label = windowState.get_content_by_pos(col, row)
        }
      }
    }
  }
}
