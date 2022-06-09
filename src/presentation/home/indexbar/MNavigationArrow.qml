import QtQuick
import QtQuick.Controls


Item
{
    id: root
    property bool toLeft : false
    signal clicked
    
    implicitWidth: image.implicitWidth + 5
    implicitHeight: image.implicitHeight
    
    Image
    {
        id: image
        anchors.right: parent.right
        rotation: (root.toLeft ? 180 : 0)
        source: properties.iconArrowGrayRightFilled
        sourceSize.width: 9
    }
    
    MouseArea
    {
        anchors.fill: parent
        
        onClicked: root.clicked()
    }
    
    
    function giveFocus()
    {
        root.forceActiveFocus();
    }
}
