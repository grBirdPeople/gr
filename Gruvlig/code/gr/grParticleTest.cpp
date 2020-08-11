#include	"grParticleTest.h"

#include	"grCore.h"


grCParticleSys::grCParticleSys( const intU size, const float emitRate, const grV2f& position )
	: m_puParticleArr( std::make_unique<grSParticleArr>( size ) )
	, m_puParticleVar( std::make_unique<grSParticleVar>() )
	, m_puEmit( std::make_unique<grSEmitter>() )
	, m_puUpdate( std::make_unique<grSUpdate>() )
{
	float e{ emitRate };
	if( e < 0.0f )
		e = 0.0f;

	m_puParticleVar->SystemPosition = position;
	m_puParticleVar->EmitRateSec = e;
	m_puParticleVar->EmitRate = 1.0f / e;
	m_puParticleVar->SpawnAccT = 0.0f;
	m_puParticleVar->Size = size;
}

float
grCParticleSys::GetEmitRate() const
{
	return m_puParticleVar->EmitRateSec;
}

void
grCParticleSys::SetEmitRate( const float emitRate )
{
	float e{ emitRate };
	if( e < 0.0f )
		e = 0.0f;

	m_puParticleVar->EmitRateSec = e;
	m_puParticleVar->EmitRate = 1.0f / e;
}

void
grCParticleSys::SetSystemPosition( const grV2f& position )
{
	m_puParticleVar->SystemPosition = position;
}

void
grCParticleSys::SetGravity( const float dirInDeg, const float force )
{
	float d{ grMath::Clamp( dirInDeg, 0.0f, 360.0f ) };
	m_puParticleVar->GravityV = { grMath::DegToVec( d ) * force };
	m_puParticleVar->GravityF = force;
}

void
grCParticleSys::SetColor( const grColor::SRgba& start, const grColor::SRgba& end, const bool hsv, const bool randomize )
{
	m_puEmit->AddGenerate( EParticleGenerateType::COLOR );
	m_puEmit->puColor->Init( start, end, randomize );

	m_puUpdate->AddUpdate( EParticleUpdateType::COLOR );
	m_puUpdate->puColor->SetHsv( hsv );
}

void
grCParticleSys::SetScale( const grV2f& start, const grV2f& end )
{
	m_puEmit->AddGenerate( EParticleGenerateType::SCALE );
	m_puEmit->puScale->Init( start, end );

	if ( m_puEmit->puScale->IsEqual == EPartValueEqual::NO )
		m_puUpdate->AddUpdate( EParticleUpdateType::SCALE );
}

void
grCParticleSys::SetMass( const grV2f& minMax )
{
	m_puEmit->AddGenerate( EParticleGenerateType::MASS );
	m_puEmit->puMass->Init( minMax );
}

void
grCParticleSys::SetLife( const grV2f& minMax )
{
	m_puEmit->AddGenerate( EParticleGenerateType::LIFE );
	m_puEmit->puLife->Init( minMax );

	m_puUpdate->AddUpdate( EParticleUpdateType::LIFE );
}

void
grCParticleSys::AddGeneratorPositionBox( const grV2f& min, const grV2f& max )
{
	m_puEmit->AddGenerate( EParticleGenerateType::POSITION );
	m_puEmit->puPosition->Init( min, max );
}

void
grCParticleSys::AddGeneratorPositionEllipse( const grV2f& min, const grV2f& max, const float step )
{
	m_puEmit->AddGenerate( EParticleGenerateType::POSITION );
	m_puEmit->puPosition->Init( min, max, step );
}

void
grCParticleSys::AddGeneratorDirectionForce( const grV2f& dirInDegMinMax, const grV2f& forceMinMax )
{
	m_puEmit->AddGenerate( EParticleGenerateType::FORCE );
	m_puEmit->puForce->Init( dirInDegMinMax, forceMinMax );
}


// TEST DRAW
#include	"grBBox.h"
#include	"grDebugManager.h"
// TEST DRAW

void
grCParticleSys::Update( const float deltaT )
{
	m_puEmit->Emit( m_puParticleVar, m_puParticleArr, deltaT );
	m_puUpdate->Update( m_puParticleVar, m_puParticleArr, deltaT );
}

void
grCParticleSys::Render( sf::RenderWindow& rRenderWin )
{
	rRenderWin.draw( &m_puParticleArr->puVerts.get()[ 0 ], m_puParticleVar->Alive, sf::PrimitiveType::Points );

	//printf( "Max: %d %2s Alive: %d \n", m_puParticleVar->Size, "", m_puParticleVar->Alive );
	




	//// TEST DRAW
	//printf( "Max: %d %2s Alive: %d \n", m_puEmit->Size, "", m_puParticle->Alive );
	//for ( sizeT i = 0; i < m_puParticle->Alive; ++i )
	//{
	//	grBBox box( m_puParticle->puScaleStart[ i ], m_puParticle->puPosition[ i ] );
	//	grDebugManager::Instance().AddBBox( box, sf::Color( ( sf::Uint8 )m_puParticle->puColorStart[ i ].R,
	//														( sf::Uint8 )m_puParticle->puColorStart[ i ].G,
	//														( sf::Uint8 )m_puParticle->puColorStart[ i ].B,
	//														( sf::Uint8 )m_puParticle->puColorStart[ i ].A ) );
	//}
	//// TEST DRAW
}
