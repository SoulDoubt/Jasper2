#include "Sphere.h"

namespace Jasper
{

#undef PHI	// take the reciprocal of phi, to obtain an icosahedron that fits a unit cube
#define PHI (1.0f / ((1.0f + sqrtf(5.0f)) / 2.0f))
#define PI 3.14159265359f


Sphere::Sphere(const std::string& name, float radius) : Mesh(name), m_radius(radius)
{
}

Sphere::~Sphere()
{
}

float Sphere::icosPositions[12*3] = {
    -PHI, 1.0f, 0.0f,    PHI, 1.0f, 0.0f,   -PHI,-1.0f, 0.0f,    PHI,-1.0f, 0.0f,
    0.0f, -PHI, 1.0f,   0.0f,  PHI, 1.0f,   0.0f, -PHI,-1.0f,   0.0f,  PHI,-1.0f,
    1.0f, 0.0f, -PHI,   1.0f, 0.0f,  PHI,  -1.0f, 0.0f, -PHI,  -1.0f, 0.0f,  PHI
};

float Sphere::icosTexCoords[60 * 2] = {
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

    1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

    1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,

    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f
};

uint32_t Sphere::icosIndices[60] = {
    0,11, 5, 0, 5, 1, 0, 1, 7, 0, 7,10, 0,10,11,
    5,11, 4, 1, 5, 9, 7, 1, 8,10, 7, 6,11,10, 2,
    3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
    4, 9, 5, 2, 4,11, 6, 2,10, 8, 6, 7, 9, 8, 1
};






void Sphere::LoadIcosphere()
{
    Positions.reserve( 60 );
    Normals.resize( 60 ); // needs to be resize rather than reserve
    Colors.reserve( 60 );
    TexCoords.reserve( 60 );
    Indices.reserve( 60 );


    for( size_t i = 0; i < 60; ++i ) {
        Positions.emplace_back( *reinterpret_cast<const Vector3*>(&icosPositions[icosIndices[i]*3]) );
        Normals.emplace_back( *reinterpret_cast<const Vector3*>(&icosPositions[icosIndices[i]*3]) );
        TexCoords.emplace_back( *reinterpret_cast<const Vector2*>(&icosTexCoords[i*2]) );
        Indices.push_back( (uint32_t)i );
    }

    // calculate the face normal for each triangle
    size_t numTriangles = Indices.size() / 3;
    for( size_t i = 0; i < numTriangles; ++i ) {
        const uint32_t index0 = (Indices)[i*3+0];
        const uint32_t index1 = (Indices)[i*3+1];
        const uint32_t index2 = (Indices)[i*3+2];

        const Vector3 &v0 = Positions[index0];
        const Vector3 &v1 = Positions[index1];
        const Vector3 &v2 = Positions[index2];

        Vector3 e0 = v1 - v0;
        Vector3 e1 = v2 - v0;
        Vector3 n = Normalize( Cross( e0, e1 ) );

        Normals[index0] = Normals[index1] = Normals[index2] = n;
    }

    // color
    size_t numPositions = Positions.size();
    for( size_t i = 0; i < numPositions; ++i )
        Colors.emplace_back( (Positions)[i] * 0.5f + Vector3( 0.5f ) );

    size_t pb = Positions.size();
    // now subdivide
    Subdivide(2);
    size_t pa = Positions.size();
    size_t pai = Indices.size();

    // spherize
    for( auto &pos : Positions )
        pos = Normalize( pos );
    for( auto &normal : Normals )
        normal = Normalize( normal );

    RecalcTexCoords();
    size_t pat = TexCoords.size();

    for (const auto& tc : TexCoords) {
        printf ("%s\n", tc.ToString().c_str());
    }
    //CalculateTangentSpace();
    CalculateExtents();
    Mesh::Initialize();
}

void Sphere::LoadUVSphere()
{
    int numSegments = m_longLines;
    int numRings = m_latLines;
    Vector3 mCenter = {0.f, 0.f, 0.f};

    Positions.resize( numSegments * numRings );
    Normals.resize( numSegments * numRings );
    TexCoords.resize( numSegments * numRings );
    Colors.resize( numSegments * numRings );
    Indices.resize( (numSegments - 1) * (numRings - 1) * 6 );

    float ringIncr = 1.0f / (float)( numRings - 1 );
    float segIncr = 1.0f / (float)( numSegments - 1 );
    float radius = m_radius;

    auto vertIt = Positions.begin();
    auto normIt = Normals.begin();
    auto texIt = TexCoords.begin();
    auto colorIt = Colors.begin();
    for( int r = 0; r < numRings; r++ ) {
        float v = r * ringIncr;
        for( int s = 0; s < numSegments; s++ ) {
            float u = 1.0f - s * segIncr;
            float x = sinf( float(M_PI * 2) * u ) * sinf( float(M_PI) * v );
            float y = sinf( float(M_PI) * (v - 0.5f) );
            float z = cosf( float(M_PI * 2) * u ) * sinf( float(M_PI) * v );

            *vertIt++ = Vector3( x * radius + mCenter.x, y * radius + mCenter.y, z * radius + mCenter.z );

            *normIt++ = Vector3( x, y, z );
            *texIt++ = Vector2( u, v );
            *colorIt++ = Vector3( x * 0.5f + 0.5f, y * 0.5f + 0.5f, z * 0.5f + 0.5f );
        }
    }

    auto indexIt = Indices.begin();
    for( int r = 0; r < numRings - 1; r++ ) {
        for( int s = 0; s < numSegments - 1 ; s++ ) {
            *indexIt++ = (uint32_t)(r * numSegments + ( s + 1 ));
            *indexIt++ = (uint32_t)(r * numSegments + s);
            *indexIt++ = (uint32_t)(( r + 1 ) * numSegments + ( s + 1 ));

            *indexIt++ = (uint32_t)(( r + 1 ) * numSegments + s);
            *indexIt++ = (uint32_t)(( r + 1 ) * numSegments + ( s + 1 ));
            *indexIt++ = (uint32_t)(r * numSegments + s);
        }
    }
}

void Sphere::Initialize()
{
    LoadUVSphere();
    CalculateExtents();
    CalculateTangentSpace();
    Mesh::Initialize();
}

void Sphere::Subdivide(int subdivisions)
{
    for( int j = 0; j < subdivisions; ++j ) {
        Positions.reserve( Positions.size() + Indices.size() );
        Normals.reserve( Normals.size() + Indices.size() );
        Indices.reserve( Indices.size() * 4 );

        const size_t numTriangles = Indices.size() / 3;
        for( uint32_t i = 0; i < numTriangles; ++i ) {
            uint32_t index0 = Indices[i * 3 + 0];
            uint32_t index1 = Indices[i * 3 + 1];
            uint32_t index2 = Indices[i * 3 + 2];

            uint32_t index3 = (uint32_t)Positions.size();
            uint32_t index4 = index3 + 1;
            uint32_t index5 = index4 + 1;

            // add new positions
            Positions.push_back( 0.5f * (Positions[index0] + Positions[index1]) );
            Positions.push_back( 0.5f * (Positions[index1] + Positions[index2]) );
            Positions.push_back( 0.5f * (Positions[index2] + Positions[index0]) );

            // add new normals
            Normals.push_back( 0.5f * (Normals[index0] + Normals[index1]) );
            Normals.push_back( 0.5f * (Normals[index1] + Normals[index2]) );
            Normals.push_back( 0.5f * (Normals[index2] + Normals[index0]) );

            // add new triangles
            Indices[i * 3 + 1] = index3;
            Indices[i * 3 + 2] = index5;

            Indices.push_back( index3 );
            Indices.push_back( index1 );
            Indices.push_back( index4 );

            Indices.push_back( index5 );
            Indices.push_back( index3 );
            Indices.push_back( index4 );

            Indices.push_back( index5 );
            Indices.push_back( index4 );
            Indices.push_back( index2 );


        }
    }
}

void Sphere::RecalcTexCoords()
{
    // calculate texture coords
    TexCoords.clear();
    TexCoords.resize( Normals.size(), Vector2() );
    for( size_t i = 0; i < Normals.size(); ++i ) {
        const Vector3& normal = Normals[i];
        Vector2 uv;
        uv.x = 0.5f - 0.5f * atanf( normal.x / -normal.z ) / float( M_PI );
        uv.y = 1.0f - acosf( normal.y ) / float( M_PI );
        TexCoords[i] = uv;
    }

    //lambda closure to easily add a vertex with unique texture coordinate to our mesh
    auto addVertex = [&] ( size_t i, const Vector2 &uv ) {
        const uint32_t index = Indices[i];
        Indices[i] = (uint32_t)Positions.size();
        Positions.push_back( Positions[index] );
        Normals.push_back( Normals[index] );
        TexCoords.push_back( uv );
        Colors.push_back( Colors[index] );
    };

    //fix texture seams (this is where the magic happens)
    size_t numTriangles = Indices.size() / 3;
    for( size_t i = 0; i < numTriangles; ++i ) {
        const Vector2 &uv0 = TexCoords[ Indices[i * 3 + 0] ];
        const Vector2 &uv1 = TexCoords[ Indices[i * 3 + 1] ];
        const Vector2 &uv2 = TexCoords[ Indices[i * 3 + 2] ];

        const float d1 = uv1.x - uv0.x;
        const float d2 = uv2.x - uv0.x;

        if( fabs(d1) > 0.5f && fabs(d2) > 0.5f )
            addVertex( i * 3 + 0, uv0 + Vector2( (d1 > 0.0f) ? 1.0f : -1.0f, 0.0f ) );
        else if( fabs(d1) > 0.5f )
            addVertex( i * 3 + 1, uv1 + Vector2( (d1 < 0.0f) ? 1.0f : -1.0f, 0.0f ) );
        else if( fabs(d2) > 0.5f )
            addVertex( i * 3 + 2, uv2 + Vector2( (d2 < 0.0f) ? 1.0f : -1.0f, 0.0f ) );
    }
}

void Sphere::Destroy()
{
}


// ------------------------ Cylinder -------------------------//

Cylinder::Cylinder(const std::string& name, float height, float radius)
    : Mesh(name), m_height(height), m_radius(radius)
{

}

Cylinder::~Cylinder()
{

}

float lerp(float x, float y, float a)
{
    return x * (1.0 - a) + y * a;
}

void Cylinder::GenerateCaps(bool flip, float height){    
    const size_t index = Positions.size();
	const Vector3 n = flip ? -m_direction : m_direction;
	Normals.resize( index + m_capSubdivisions * m_segments * 2, n );
	Colors.resize( index + m_capSubdivisions * m_segments * 2, Vector3( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f ) );

	const Quaternion axis = Rotation( Vector3( 0, 1, 0 ), m_direction );

	// vertices
	const float segmentIncr = 1.0f / ( m_segments - 1 );
	for( int r = 0; r < m_capSubdivisions; ++r ) {
		for( int i = 0; i < m_segments; ++i ) {
			float cosPhi = -cosf( i * segmentIncr * float( M_PI * 2 ) );
			float sinPhi = sinf( i * segmentIncr * float( M_PI * 2 ) );

			// inner point
			float x = ( m_radius * cosPhi * float( r ) ) / m_capSubdivisions;
			float y = m_height;
			float z = ( m_radius * sinPhi * float( r ) ) / m_capSubdivisions;

			Positions.emplace_back( Vector3(0.f, 0.f, 0.f) + axis * Vector3( x, y, z ) );
			TexCoords.emplace_back( i * segmentIncr, 1.0f - height / m_height );

			// outer point
			x = ( m_radius * cosPhi * float( r + 1 ) ) / m_capSubdivisions;
			y = m_height;
			z = ( m_radius * sinPhi * float( r + 1 ) ) / m_capSubdivisions;

			Positions.emplace_back( Vector3(0.f, 0.f, 0.f) + axis * Vector3( x, y, z ) );
			TexCoords.emplace_back( i * segmentIncr, 1.0f - height / m_height );
		}
	}

	// index buffer
	Indices.reserve( Indices.size() + 6 * m_segments * m_capSubdivisions );

	for( int r = 0; r < m_capSubdivisions; ++r ) {
		for( int i = 0; i < ( m_segments - 1 ); ++i ) {
			if( flip ) {
				Indices.push_back( (uint32_t)( index + r * m_segments * 2 + i * 2 + 0 ) );
				Indices.push_back( (uint32_t)( index + r * m_segments * 2 + i * 2 + 2 ) );
				Indices.push_back( (uint32_t)( index + r * m_segments * 2 + i * 2 + 3 ) );
                       
				Indices.push_back( (uint32_t)( index + r * m_segments * 2 + i * 2 + 0 ) );
				Indices.push_back( (uint32_t)( index + r * m_segments * 2 + i * 2 + 3 ) );
				Indices.push_back( (uint32_t)( index + r * m_segments * 2 + i * 2 + 1 ) );
			}
			else {
				Indices.push_back( (uint32_t)( index + r * m_segments * 2 + i * 2 + 0 ) );
				Indices.push_back( (uint32_t)( index + r * m_segments * 2 + i * 2 + 3 ) );
				Indices.push_back( (uint32_t)( index + r * m_segments * 2 + i * 2 + 2 ) );
                                                         
				Indices.push_back( (uint32_t)( index + r * m_segments * 2 + i * 2 + 0 ) );
				Indices.push_back( (uint32_t)( index + r * m_segments * 2 + i * 2 + 1 ) );
				Indices.push_back( (uint32_t)( index + r * m_segments * 2 + i * 2 + 3 ) );
			}
		}
	}
}

void Cylinder::LoadVerts()
{
    Positions.reserve( m_segments * m_slices );
    Normals.reserve( m_segments * m_slices );
    TexCoords.reserve( m_segments * m_slices );
    Indices.reserve( (m_segments - 1) * (m_slices - 1) * 6 );

    Colors.reserve( m_segments * m_slices );

    const float segmentIncr = 1.0f / (m_segments - 1);
    const float ringIncr = 1.0f / (m_slices - 1);
    const Quaternion axis = Quaternion::FromAxisAndAngle( m_direction, 0.f);


    // vertex, normal, tex coord and color buffers
    for( int i = 0; i < m_segments; ++i ) {
        for( int j = 0; j < m_slices; ++j ) {
            float cosPhi = -cosf( i * segmentIncr * float(M_PI * 2) );
            float sinPhi =  sinf( i * segmentIncr * float(M_PI * 2) );

            float r = lerp( m_radiusBase, m_radiusApex, j * ringIncr );
            float x = r * cosPhi;
            float y = m_height * j * ringIncr;
            float z = r * sinPhi;
            const Vector3 n = Normalize( Vector3( m_height * cosPhi, m_radiusBase - m_radiusApex, m_height * sinPhi ) );

            Positions.emplace_back( Vector3(0.f, 0.f, 0.f) + axis * Vector3( x, y, z ) );
            TexCoords.emplace_back( Vector2(i * segmentIncr, 1.0f - j * ringIncr) );
            Normals.emplace_back( axis * n );
            Colors.emplace_back( n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f );
        }
    }

    // index buffer
    for ( int j = 0; j < m_slices - 1; ++j ) {
        for( int i = 0; i < m_segments - 1; ++i ) {
            Indices.push_back( (uint32_t)((i + 0) * m_slices + (j + 0)) );
            Indices.push_back( (uint32_t)((i + 1) * m_slices + (j + 0)) );
            Indices.push_back( (uint32_t)((i + 1) * m_slices + (j + 1)) );

            Indices.push_back( (uint32_t)((i + 0) * m_slices + (j + 0)) );
            Indices.push_back( (uint32_t)((i + 1) * m_slices + (j + 1)) );
            Indices.push_back( (uint32_t)((i + 0) * m_slices + (j + 1)) );
        }
    }

    // caps
    if( m_radiusBase > 0.0f )
        GenerateCaps( true, 0.f);// 0.0f, mRadiusBase, positions, normals, texCoords, colors, indices );

    if( m_radiusApex > 0.0f )
        GenerateCaps( false, m_height);//, mHeight, mRadiusApex, positions, normals, texCoords, colors, indices );
}

void Cylinder::Initialize()
{
    LoadVerts();
    CalculateExtents();
    CalculateTangentSpace();
    Mesh::Initialize();

}

}
