import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
  width: 100
  height: 40
  border.color: "#333"
  color: isFirstRow || isFirstCol ? "#888" : (isEditing ? "#333" : "#555")

  property string label: "Default"
  property bool isEditing: false

  property int colIdx
  property int rowIdx
  property int col
  property int row
  property bool isFirstCol: colIdx === 0
  property bool isFirstRow: rowIdx === 0

  MouseArea {
    anchors.fill: parent
    onClicked: {
      isEditing = isFirstRow || isFirstCol ? false : true
    }
  }

  Text {
    visible: !isEditing
    anchors.centerIn: parent
    text: index === 0 ? 
      "" :
      (isFirstRow ?
        windowUtils.col_idx_to_letter(col) : 
        (isFirstCol ?
          row :
          col + ";" + row))
    color: "#FFF8E7"
    font.pixelSize: 16
  }

  TextInput {
    visible: isEditing
    anchors.fill: parent
    text: label
    color: "#FFF8E7"
    font.pixelSize: 16
    focus: isEditing
    selectByMouse: true
    cursorVisible: true

    onEditingFinished: {
        label = text
        isEditing = false
    }

    Keys.onReturnPressed: {
        label = text
        isEditing = false
    }

    Keys.onEscapePressed: {
        label = text
        isEditing = false
    }
  }
}
