import QtQuick 2.0

ShaderEffect {
    // radius, goes from 0 to width/2
    property real radius
    property alias sourceItem: cornerShaderSource.sourceItem
    property variant source: ShaderEffectSource {
        id: cornerShaderSource
        anchors.fill: parent
        hideSource: true
    }

    property size center: Qt.size(0.5, 0.5)
    property size start: Qt.size(0.5 - radius/width, 0.5 - radius/height)
    property real delta: 0.05
    property real active: 1
    vertexShader: "
                    uniform highp mat4 qt_Matrix;
                    attribute highp vec4 qt_Vertex;
                    attribute highp vec2 qt_MultiTexCoord0;
                    varying highp vec2 textureCoords;
                    void main() {
                        textureCoords = qt_MultiTexCoord0;
                        gl_Position = qt_Matrix * qt_Vertex;
                    }"
    fragmentShader: "
                varying highp vec2 textureCoords;
                uniform sampler2D source;
                uniform lowp float qt_Opacity;
                uniform highp vec2 start;
                uniform highp vec2 center;
                uniform highp float delta;
                uniform highp float active;
                void main() {
                    highp vec2 Coord  = max((abs(textureCoords - center) - start) / (center - start), vec2(0.0));
                    lowp float Alpha = smoothstep(1.0, 1.0 - delta, length(Coord));
                    gl_FragColor = vec4(texture2D(source, textureCoords).rgb * Alpha * active, Alpha) * qt_Opacity;
                }"
}
