import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    width: 50
    height: 50
    border.color: "#333"
    color: isFirstRow || isFirstCol ? "#888" : (isEditing ? "#333" : "#555")

    property string label: "Default"
    property int idx: -1
    property bool isEditing: false
    property bool isFirstCol: false
    property bool isFirstRow: false
    property int colIdx: -1
    property int rowIdx: -1

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
              windowUtils.col_idx_to_letter(colIdx) : 
              (isFirstCol ?
                  rowIdx :
                  colIdx + ";" + rowIdx))
        color: "#FFF8E7"
        font.pixelSize: isFirstRow || isFirstCol ? 20 : 16
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
