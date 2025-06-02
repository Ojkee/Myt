import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
  border.color: "#333"
  color: isFirstRow || isFirstCol ? "#888" : (isEditing ? "#333" : "#555")


  property int colIdx
  property int rowIdx
  property int col: 0
  property int row: 0
  property bool isFirstCol: colIdx === 0
  property bool isFirstRow: rowIdx === 0

  property string label: ""
  property bool isEditing: false

  MouseArea {
    anchors.fill: parent
    onClicked: {
      isEditing = isFirstRow || isFirstCol ? false : true
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
      if (label === "")
        return windowUtils.col_idx_to_letter(col) + "" + row;
      return label
    }
    color: (isFirstCol || isFirstRow || label !== "") ? 
      "#FFF8E7" :
      "#777" 
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

    onEditingFinished: {
      if (text !== "") {
        windowState.eval_save(text, col, row)
        label = windowState.get_content_by_pos(col, row)
      }
      isEditing = false
    }

    Keys.onReturnPressed: {
      if (text !== "") {
        windowState.eval_save(text, col, row)
        label = windowState.get_content_by_pos(col, row)
      }
      isEditing = false
    }

    Keys.onEscapePressed: {
      if (text !== "") {
        windowState.eval_save(text, col, row)
        label = windowState.get_content_by_pos(col, row)
      }
      isEditing = false
    }
  }
}
