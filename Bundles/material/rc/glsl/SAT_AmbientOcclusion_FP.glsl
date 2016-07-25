#version 420

uniform sampler3D u_sat;

uniform vec3 u_lightDir;
uniform int u_sliceIndex;

uniform int u_nbShells;
uniform int u_shellRadius;

uniform float u_scatteringConeAngle;
uniform int   u_nbSamplesAlongCone;

out vec4 illuminationVal;

//-----------------------------------------------------------------------------

vec4 satLookup(in ivec3 min, in ivec3 max)
{
    return texelFetch(u_sat, max, 0)
            - texelFetch(u_sat, ivec3( max.x, max.y, min.z ), 0)
            - texelFetch(u_sat, ivec3( max.x, min.y, max.z ), 0)
            - texelFetch(u_sat, ivec3( min.x, max.y, max.z ), 0)
            + texelFetch(u_sat, ivec3( min.x, min.y, max.z ), 0)
            + texelFetch(u_sat, ivec3( min.x, max.y, min.z ), 0)
            + texelFetch(u_sat, ivec3( max.x, min.y, min.z ), 0)
            - texelFetch(u_sat, min, 0);
}

//-----------------------------------------------------------------------------

mat2 rotMat2D(in float angle)
{
    return mat2(cos(angle), -sin(angle),
                sin(angle),  cos(angle));
}

//-----------------------------------------------------------------------------

int getVoxelSecondaryCoord(in ivec2 lineOrigin, in vec2 lineVector, int x)
{
    float lineSlope = lineVector.y / lineVector.x;
    return int(lineSlope * (x - lineOrigin.x) + lineOrigin.y);
}

//-----------------------------------------------------------------------------

float coneShadowQuery(in ivec3 voxelPos)
{
    vec3  coneDir = -u_lightDir;
    float coneAngle = u_scatteringConeAngle;

    int nbConeSamples = u_nbSamplesAlongCone;

    // Find the axis (x, y or z) who has the smallest angle to the light direction.
    int primaryAxis = abs(coneDir.y) > abs(coneDir.x)            ? 1 : 0;
    primaryAxis     = abs(coneDir.z) > abs(coneDir[primaryAxis]) ? 2 : primaryAxis;

    // Two remaining axes.
    int secondaryAxis0 = (primaryAxis - 1) % 3;
    int secondaryAxis1 = (primaryAxis + 1) % 3;

    if(primaryAxis == 0){
        secondaryAxis0 = 2; secondaryAxis1 = 1;
    }
    else if(primaryAxis == 1){
        secondaryAxis0 = 0; secondaryAxis1 = 2;
    }
    else if(primaryAxis == 2){
        secondaryAxis0 = 0; secondaryAxis1 = 1;
    }

    ivec3 satSize = textureSize(u_sat, 0);

    // Coordinate on the primary axis where the cone exits the volume.
    const int outCoord = coneDir[primaryAxis] < 0 ? 0 : satSize[primaryAxis] - 1;

    // Number of voxels separating our current voxel to the exit.
    int coneVoxelHeight = abs(voxelPos[primaryAxis] - outCoord);

    // Distance (in voxels) between cuboid samples along the cone.
    int cuboidHeight = coneVoxelHeight / nbConeSamples;

    if(cuboidHeight <= 1)
    {
        nbConeSamples = coneVoxelHeight / 2;
        cuboidHeight = 2;
    }

    if(nbConeSamples <= 1)
    {
        return 1;
    }

    // Compute cone-plane projection. This projection consists of two half-lines starting at voxelPos.
    // We need to find their direction, this can be done by rotating the cone vector by it's angle in each plane.
    const vec2  projDir0   = vec2(coneDir[primaryAxis], coneDir[secondaryAxis0]);
    const float projAngle0 = atan(tan(coneAngle) / length(projDir0));

    const vec2  projDir1   = vec2(coneDir[primaryAxis], coneDir[secondaryAxis1]);
    const float projAngle1 = atan(tan(coneAngle) / length(projDir1));

    const vec2 v0 = rotMat2D( projAngle0) * projDir0;
    const vec2 v1 = rotMat2D(-projAngle0) * projDir0;
    const vec2 v2 = rotMat2D( projAngle1) * projDir1;
    const vec2 v3 = rotMat2D(-projAngle1) * projDir1;

    const ivec2 o1 = ivec2(voxelPos[primaryAxis], voxelPos[secondaryAxis0]);
    const ivec2 o2 = ivec2(voxelPos[primaryAxis], voxelPos[secondaryAxis1]);

    ivec3 queryCubeMin, queryCubeMax;

    // The way the cone is facing.
    const int coneOrientation = int(sign(coneDir[primaryAxis]));

    const int incr = cuboidHeight * coneOrientation;

    const int beginCoord = outCoord - coneOrientation * cuboidHeight/2;
    const int endCoord   = beginCoord - (nbConeSamples - 1) * incr;

    float coneSum = 0.f;
    int nbVoxels  = 0;

    for(int i = beginCoord; i != endCoord; i -= incr)
    {
        ivec3 p0, p1, p2, p3;
        p0[primaryAxis] = p1[primaryAxis] = p2[primaryAxis] = p3[primaryAxis] = i;

        p0[secondaryAxis0] = getVoxelSecondaryCoord(o1, v0, i);
        p1[secondaryAxis0] = getVoxelSecondaryCoord(o1, v1, i);
        p2[secondaryAxis1] = getVoxelSecondaryCoord(o2, v2, i);
        p3[secondaryAxis1] = getVoxelSecondaryCoord(o2, v3, i);

        queryCubeMin[secondaryAxis0] = min(p0[secondaryAxis0], p1[secondaryAxis0]);
        queryCubeMax[secondaryAxis0] = max(p0[secondaryAxis0], p1[secondaryAxis0]);

        queryCubeMin[secondaryAxis1] = min(p2[secondaryAxis1], p3[secondaryAxis1]);
        queryCubeMax[secondaryAxis1] = max(p2[secondaryAxis1], p3[secondaryAxis1]);

        queryCubeMin[primaryAxis] = i - cuboidHeight/2;
        queryCubeMax[primaryAxis] = i + cuboidHeight/2;

        queryCubeMin = max(queryCubeMin, ivec3(0));
        queryCubeMax = min(queryCubeMax, satSize - ivec3(1));

        ivec3 cubeDiff = queryCubeMax - queryCubeMin;
        nbVoxels += cubeDiff.x * cubeDiff.y * cubeDiff.z;

        coneSum += satLookup(queryCubeMin, queryCubeMax).a;
    }

    return coneSum / float(nbVoxels);
}

//-----------------------------------------------------------------------------

vec4 ambientOcclusionAndColourBleedingQuery(in ivec3 voxelPos)
{
    ivec3 satSize = textureSize(u_sat, 0) - ivec3(1);

    ivec3 shellMin = max(voxelPos - ivec3(u_shellRadius), ivec3(0));
    ivec3 shellMax = min(voxelPos + ivec3(u_shellRadius), satSize);

    int radius = u_shellRadius;

    vec4 satLookupVal = satLookup(shellMin, shellMax);
    vec4 aoFactor = satLookupVal / float(radius * radius);

    for(int i = 1; i < u_nbShells; ++i)
    {
        shellMin = max(shellMin - ivec3(u_shellRadius), ivec3(0));
        shellMax = min(shellMax + ivec3(u_shellRadius), satSize);

        vec4 lastLookup = satLookupVal;
        satLookupVal = satLookup(shellMin, shellMax);

        radius += u_shellRadius;

        aoFactor += (satLookupVal - lastLookup) / float(radius * radius);
    }

    return pow(u_nbShells * u_shellRadius, -2.f) * aoFactor;
}

//-----------------------------------------------------------------------------

void main(void)
{
    ivec3 voxelCoords = ivec3(gl_FragCoord.xy, u_sliceIndex);

    illuminationVal = ambientOcclusionAndColourBleedingQuery(voxelCoords);

    float shadowFactor = coneShadowQuery(voxelCoords);

    illuminationVal.a = shadowFactor;

    illuminationVal.a = exp(-illuminationVal.a);
}
