
// out
varying vec3 LightIntensity;

// uniform
struct LightInfo {
    vec4 Position;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
};
uniform LightInfo Light;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Tr;
uniform float Ns;

void main()
{
    vec3 tnorm = normalize( gl_NormalMatrix * gl_Normal);
    vec4 eyeCoords = gl_ModelViewMatrix * vec4(gl_Vertex);
    vec3 s = normalize(vec3(Light.Position - eyeCoords));
    vec3 v = normalize(-eyeCoords.xyz);
    vec3 r = reflect( -s, tnorm );
    vec3 ambient = Light.La * Ka;
    float sDotN = max(dot(s,tnorm), 0.0);
    vec3 diffuse = Light.Ld * Kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 ) {
        spec = Light.Ls * Ks * pow(max(dot(r,v),0.0), Ns);
    }
    LightIntensity = ambient + diffuse + spec;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex);
}
