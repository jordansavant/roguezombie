// A simple blur shader, weighted on alpha

uniform sampler2D texture;

void main()
{
    float radius = 0.00175;
    vec4 accum = vec4(0.0);
    vec4 normal = vec4(0.0);
        
    // get current color
    normal = texture2D(texture, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t));
    
    // get radius range of color surrounding current fragment
    accum += texture2D(texture, vec2(gl_TexCoord[0].s - radius, gl_TexCoord[0].t - radius));
    accum += texture2D(texture, vec2(gl_TexCoord[0].s + radius, gl_TexCoord[0].t - radius));
    accum += texture2D(texture, vec2(gl_TexCoord[0].s + radius, gl_TexCoord[0].t + radius));
    accum += texture2D(texture, vec2(gl_TexCoord[0].s - radius, gl_TexCoord[0].t + radius));
    
    // final opacity
    accum *= .5;
    
    // Target outline color
    accum.r = 0.0;
    accum.g = 1.0;
    accum.b = 0.0;
    
    // if the accumulated area is 100% opaque the left hand side will change accum to be 0, zeroing all values
    // if the accumulated area is 100% transparent the left hand side will let the target colors appear at full strength
    // the resulting value is added onto the current color times the current alpha
    normal = (accum * (1.0 - normal.a)) + (normal * normal.a);
    
    // If the new color matches our target outline (if we are an outline pixel)
    if(all(equal(normal, accum)))
    {
        gl_FragDepth = 0;
        gl_FragColor = normal;
    }
    else
    {
        gl_FragDepth = gl_FragCoord.z;
    }
}
