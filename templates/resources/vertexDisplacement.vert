uniform sampler2D displacementMap;
uniform sampler2D velocityMap;

//uniform vec4 COI;

varying vec4 color;

void main()
{
    //using the displacement map to move vertices
	vec4 pos = texture2D( displacementMap, gl_MultiTexCoord0.xy );
    pos = vec4(pos.xyz, 1.0);
    color = texture2D( velocityMap, gl_MultiTexCoord0.xy );
    
    // add randomness according to how close the point is to the viewing plane (e.g. z=0), to simulate depth of field
    // color.xyz has random values that were created during setup

    // get the difference between the z value of the vertex and the z value of the camera's eye point (transformed to eye space)
//    vec4 posTransformed = gl_ModelViewMatrix * pos;
    
    // get the scale factor for the addition of the "noise"
//    float scaleFactor = abs(posTransformed.z - eyePointTransformed.z) * 0.1;
//    float scaleFactor = 0.0;
    
//	gl_Position = gl_ModelViewProjectionMatrix * vec4( pos.xyz + (color.xyz * scaleFactor), 1.0 ) ;
    
//    vec4 COIInScreenSpace = gl_ModelViewMatrix * COI;
//    vec4 posInScreenSpace = gl_ModelViewMatrix * pos;
//    vec4 posJittered = pos + vec4(color.xyz, 1.0);

//    float zDistFromCOI = abs(posInScreenSpace.z - COIInScreenSpace.z) / 200.0;
//    pos = zDistFromCOI > 100.0 ? pos : vec4(0.0,0.0,0.0,1.0);
//    vec4 finalPos = gl_ModelViewProjectionMatrix * vec4(pos.xyz + (color.xyz * scaleFactor), 1.0);
//    vec4 finalPos = pos * zDistFromCOI + posJittered * (1.0 - zDistFromCOI);
    vec4 finalPos = pos;
    
    gl_Position = gl_ModelViewProjectionMatrix * finalPos;
}