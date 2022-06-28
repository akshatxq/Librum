import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Librum.style
import Librum.icons


Item
{
    id: root
    property int selectedAmountOfBooks: 12
    signal selectedBookCount(int amount)
    
    implicitWidth: 58
    implicitHeight: 32
    
    
    Rectangle
    {
        id: container
        anchors.fill: parent
        color: Style.colorBackground
        border.color: Style.colorLightBorder
        radius: 4
        antialiasing: true
        
        
        RowLayout
        {
            id: layout
            anchors.centerIn: parent
            spacing: 5
            
            
            Label
            {
                id: bookCountLabel
                text: root.selectedAmountOfBooks.toString()
                color: Style.colorBaseText
                font.family: Style.defaultFontFamily
                font.pointSize: 11
                font.bold: true
            }
            
            Image
            {
                id: dropDownArrowImage
                source: Icons.dropdownGray
                sourceSize.width: 8
                fillMode: Image.PreserveAspectFit
            }
        }
    }
    
    MouseArea
    {
        anchors.fill: parent
        
        onClicked: selectorPopup.open()
    }
    
    MBookCountSelectorPopup
    {
        id: selectorPopup
        y: -implicitHeight - 6
        closePolicy: Popup.CloseOnReleaseOutsideParent | Popup.CloseOnEscape
        
        onSelected:
            (content) =>
            {
                root.selectedAmountOfBooks = content;
                root.selectedBookCount(content);
                close();
            }
    }
    
    
    function giveFocus()
    {
        root.forceActiveFocus();
    }
}
