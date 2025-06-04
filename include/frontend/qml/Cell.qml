import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {

  property int colIdx
  property int rowIdx
  property int col: 0
  property int row: 0
  property bool isFirstCol: colIdx === 0
  property bool isFirstRow: rowIdx === 0

  property string label: ""
  property bool isEditing: {
    return windowState.editingCol === col && windowState.editingRow === row
  }

  border.color: "#333"
  color: isFirstRow || isFirstCol ? "#888" : (isEditing ? "#333" : "#555")

  MouseArea {
    anchors.fill: parent
    onClicked: {
      if (!isFirstCol && !isFirstRow) {
        windowState.editingCol = col
        windowState.editingRow = row
      }
    }
  }

  Text {
    visible: !isEditing
    anchors.centerIn: parent
    text: {
      if (index === 0)
        return ""
      if (isFirstRow)
        return windowUtils.col_idx_to_letter(col)
      if (isFirstCol)
        return row
      if (label === "" || label === "\"\"")
        return windowUtils.col_idx_to_letter(col) + "" + row;
      return label
    }
    color: {
      if (text === "Nil")
        return "#F66";
      else if (isFirstCol || isFirstRow || label !== "" && label !== "\"\"") 
        return "#FFF8E7";
      return "#777" ;
    }
    font.pixelSize: 16
  }

  TextInput {
    visible: isEditing
    anchors.fill: parent
    text: windowState.get_raw_content_by_pos(col, row)
    color: "#FFF8E7"
    font.pixelSize: 16
    focus: isEditing
    selectByMouse: true
    cursorVisible: true

    function finishEdit() {
      if (text !== "" || (text === "" && label != "")) {
        windowState.eval_save(text, col, row)
        label = windowState.get_content_by_pos(col, row)
      }
      windowState.editingCol = -1
      windowState.editingRow = -1
    }

    onEditingFinished: {
      finishEdit()
    }

    Keys.onEscapePressed: {
      finishEdit()
    }

    Keys.onPressed: (event) => {
        if (event.key === Qt.Key_Tab || event.key === Qt.Key_Right) {
            keyNavigationEvent(col + 1, row)
            event.accepted = true
        } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter || event.key === Qt.Key_Down) {
            keyNavigationEvent(col, row+1)
            event.accepted = true
        } else if (event.key === Qt.Key_Left) {
            keyNavigationEvent(col-1, row)
            event.accepted = true
        } else if (event.key === Qt.Key_Up) {
            keyNavigationEvent(col, row-1)
            event.accepted = true
        }
    }

    function keyNavigationEvent(newCol, newRow) {
        finishEdit()
        windowState.editingCol = newCol
        windowState.editingRow = newRow
    }
  }

  function updateLabel() {
    const content = windowState.get_content_by_pos(col, row)
    if (content !== "") {
      label = content
    } else {
      label = ""
    }
  }
}
