
varying vec3 Normal;
varying vec3 Vertex;

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

void main(void) {
    vec4 eyeCoords = gl_ModelViewMatrix * vec4(Vertex, 1);
    vec4 LightPos = (Light.Position - eyeCoords) * gl_ModelViewMatrix;
    vec3 s = normalize(vec3(LightPos));
    vec3 v = normalize(-eyeCoords.xyz);
    vec3 r = reflect(-s, Normal);
    vec3 ambient = Light.La * Ka;
    float sDotN = max(dot(s,Normal), 0.0);
    vec3 diffuse = Light.Ld * Kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 ) {
        spec = Light.Ls * Ks * pow(max(dot(r,v),0.0), Ns);
    }
    gl_FragColor = vec4(ambient + diffuse + spec, 1.0);
}
