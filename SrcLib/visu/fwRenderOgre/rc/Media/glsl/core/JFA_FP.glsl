/* Perform a jump flooding step */

#version 330

/* Current pass index */
uniform float u_passIndex;
/* Total number of passes required log2(max(window.x), max(window.y)) */
uniform float u_nbPasses;

/* Input texture: one of the initial textures of a previous ping-pong step */
/* Layout of the texture (fragCoord.x, fragCoord.y, rayPos.z, 1.0) */
uniform sampler2D u_inputTexture;

/* screen resolution: needed as gl_FragCoord is in window space */
uniform vec4 u_viewport;

/* Output fragment color */
out vec4 fragColor;

void main()
{
    vec2 coords = gl_FragCoord.xy * u_viewport.zw;
    coords = (coords - 0.5) * 2.;

    float stepwidth = exp2(u_nbPasses - u_passIndex - 1.0);

    /* as we are working with floating coordinates */
    /* we put the maximum distance to a reasonable amount */
    float dstMin = 10.0;
    vec3 closestPoint = vec3(0.0);
    vec2 currentSampleCoords = coords;

    /* Get the 8 "neighboring" samples for this step */
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            /* Compute the coordinate of the current sample */
            vec2 neighborSampleCoords = (gl_FragCoord.xy + vec2(x,y) * stepwidth) * u_viewport.zw;
            /* Get the value for the sample */
            vec4 neighborSampleValue = texture(u_inputTexture, neighborSampleCoords);

            /* Compute the distance from the current sample position */
            /* to the closest point we get for our neighboring sample */
            float d = distance(neighborSampleValue.xy, currentSampleCoords);

            /* Ensure that the current neighbor has already been initialized with a footprint sample (x != 0.0 && y != 0.0) */
            /* And that the distance is the closest */
            if ((neighborSampleValue.x != 0.0) && (neighborSampleValue.y != 0.0) && (d < dstMin))
            {
                dstMin = d;
                closestPoint = neighborSampleValue.xyz;
            }
        }
    }

    /* Finally we store the coordinates of the closest point we find */
    /* We also store the distance to the closest point in the blue channel */
    fragColor = vec4(closestPoint, dstMin);
}
