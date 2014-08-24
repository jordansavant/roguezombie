// A simple blur shader, weighted on alpha

uniform sampler2D texture;

void main()
{
    // lol nicer looking
    gl_FragDepth = gl_FragCoord.z / 2;
    gl_FragColor = texture2D(texture, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t));
    return;
}
