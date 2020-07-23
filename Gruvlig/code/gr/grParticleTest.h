#ifndef		_GRPARTICLETEST_H_
#define		_GRPARTICLETEST_H_

#include	"grCommon.h"
#include	"grV2.h"
#include	"grRandom.h"
#include	"grAlgo.h"
#include	"grColor.h"
#include	"grStruct.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
typedef grColor::SRgba SRgba;

////////////////////////////////////////////////////////////////////////////////////////////////////
struct grSParticle
{
	grSParticle( const sizeT size )
		: puColorStart		( std::make_unique<SRgba[]>( size ) )
		, puColorEnd		( std::make_unique<SRgba[]>( size ) )
		, puScaleStart		( std::make_unique<grV2f[]>( size ) )
		, puScaleEnd		( std::make_unique<grV2f[]>( size ) )
		, puAcceleration	( std::make_unique<grV2f[]>( size ) )
		, puVelocity		( std::make_unique<grV2f[]>( size ) )
		, puPosition		( std::make_unique<grV2f[]>( size ) )
		, puMass			( std::make_unique<float[]>( size ) )
		, puLife			( std::make_unique<float[]>( size ) )
		, Alive				( 0 )
	{}
	grSParticle( const grSParticle& ) = delete;
	grSParticle& operator=( const grSParticle& ) = delete;
	grSParticle( grSParticle&& ) noexcept = delete;
	grSParticle& operator=( grSParticle&& ) noexcept = delete;

	inline void Kill( const sizeT nowIdx )
	{
		sizeT last = Alive - 1;

		//for( sizeT i = 0; i < rParticle->Alive + 1; ++i )
		//	printf( "%d %3s %g \n", i, "", rParticle->puLife[ i ] );

		grAlgo::Swap( puColorStart[ nowIdx ], puColorStart[ last ] );
		grAlgo::Swap( puColorEnd[ nowIdx ], puColorEnd[ last ] );
		grAlgo::Swap( puScaleStart[ nowIdx ], puScaleStart[ last ] );
		grAlgo::Swap( puScaleEnd[ nowIdx ], puScaleEnd[ last ] );
		grAlgo::Swap( puAcceleration[ nowIdx ], puAcceleration[ last ] );
		grAlgo::Swap( puVelocity[ nowIdx ], puVelocity[ last ] );
		grAlgo::Swap( puPosition[ nowIdx ], puPosition[ last ] );
		grAlgo::Swap( puMass[ nowIdx ], puMass[ last ] );
		grAlgo::Swap( puLife[ nowIdx ], puLife[ last ] );

		// Most values are generated by operator= and does not need zeroing
		// Values that are generated with operator+= needs resetting obviously

		puAcceleration[ last ] = grV2f();

		//for ( sizeT i = 0; i < rParticle->Alive + 1; ++i )
		//	printf( "%d %3s %g \n", i, "", rParticle->puLife[ i ] );

		--Alive;
	}

	pU<SRgba[]> puColorStart;
	pU<SRgba[]> puColorEnd;
	pU<grV2f[]> puScaleStart;
	pU<grV2f[]> puScaleEnd;
	pU<grV2f[]> puAcceleration;
	pU<grV2f[]> puVelocity;
	pU<grV2f[]> puPosition;
	pU<float[]> puMass;
	pU<float[]> puLife;

	sizeT Alive;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
enum class EParticleEqual
{
	NO = 0,
	YES = 1
};

struct grSBaseGenerate
{
	inline void SetBaseColor( grColor::SRgba& rStart, grColor::SRgba& rEnd )
	{
		rStart.R = grMath::Clamp( rStart.R, 0, 255 );
		rStart.G = grMath::Clamp( rStart.G, 0, 255 );
		rStart.B = grMath::Clamp( rStart.B, 0, 255 );
		rStart.A = grMath::Clamp( rStart.A, 0, 255 );

		rEnd.R = grMath::Clamp( rEnd.R, 0, 255 );
		rEnd.G = grMath::Clamp( rEnd.G, 0, 255 );
		rEnd.B = grMath::Clamp( rEnd.B, 0, 255 );
		rEnd.A = grMath::Clamp( rEnd.A, 0, 255 );

		if ( grMath::CmpFloat( rStart.R, rEnd.R ) &&
			 grMath::CmpFloat( rStart.G, rEnd.G ) &&
			 grMath::CmpFloat( rStart.B, rEnd.B ) &&
			 grMath::CmpFloat( rStart.A, rEnd.A ) )
		{
			Equal = EParticleEqual::YES;
		}
	}

	inline void SetBaseStartEnd( grV2f& rStart, grV2f& rEnd )
	{
		rStart = grMath::Abs( rStart );
		rEnd = grMath::Abs( rEnd );
		Equal = grMath::CmpV2f( rStart, rEnd ) ? EParticleEqual::YES : EParticleEqual::NO;
	}

	inline void SetBaseMinMaxX2( grV2f& rMin, grV2f& rMax )
	{
		grMath::RangeCheckV2fx2( rMin, rMax );
		Equal = grMath::CmpV2f( rMin, rMax ) ? EParticleEqual::YES : EParticleEqual::NO;
	}

	inline void SetBaseMinMax( grV2f& rMinMax )
	{
		grMath::RangeCheckV2f( rMinMax );
		Equal = grMath::CmpFloat( rMinMax.x, rMinMax.y ) ? EParticleEqual::YES : EParticleEqual::NO;
	}

	EParticleEqual Equal;
};

struct grSColorGenerate : public grSBaseGenerate
{
	inline void Set( const grColor::SRgba& rStart, const grColor::SRgba& rEnd, const bool randomize )
	{
		LocalStart = rStart;
		LocalEnd = rEnd;
		Rand = randomize;
		SetBaseColor( LocalStart, LocalEnd );
	}

	inline void Generate( pU<grSParticle>& rParticle, const sizeT startIdx, const sizeT endIdx, const pU<grRandom>& rRand )
	{
		if ( Rand )
		{
			float midR = grMath::Abs( LocalStart.R - LocalEnd.R ) * 0.5f;
			float midG = grMath::Abs( LocalStart.G - LocalEnd.G ) * 0.5f;
			float midB = grMath::Abs( LocalStart.B - LocalEnd.B ) * 0.5f;
			//float midA = grMath::Abs( LocalStart.A - LocalEnd.A ) * 0.5f;		// Unsure if random alpha is desired

			for ( sizeT i = startIdx; i < endIdx; ++i )
			{
				grColor::SRgba result( LocalStart.R > midR ? ( uint16_t )rRand->Float( midR, LocalStart.R ) : ( uint16_t )rRand->Float( LocalStart.R, midR ),
									   LocalStart.G > midG ? ( uint16_t )rRand->Float( midG, LocalStart.G ) : ( uint16_t )rRand->Float( LocalStart.G, midG ),
									   LocalStart.B > midB ? ( uint16_t )rRand->Float( midB, LocalStart.B ) : ( uint16_t )rRand->Float( LocalStart.B, midB ),
									   ( uint16_t )LocalStart.A );

				rParticle->puColorStart[ i ] = result;
			}

			for ( sizeT i = startIdx; i < endIdx; ++i )
			{
				grColor::SRgba result( LocalEnd.R > midR ? ( uint16_t )rRand->Float( midR, LocalEnd.R ) : ( uint16_t )rRand->Float( LocalEnd.R, midR ),
									   LocalEnd.G > midG ? ( uint16_t )rRand->Float( midG, LocalEnd.G ) : ( uint16_t )rRand->Float( LocalEnd.G, midG ) ,
									   LocalEnd.B > midB ? ( uint16_t )rRand->Float( midB, LocalEnd.B ) : ( uint16_t )rRand->Float( LocalEnd.B, midB ),
									   ( uint16_t )LocalEnd.A );

				rParticle->puColorEnd[ i ] = result;
			}

			return;
		}
		
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rParticle->puColorStart[ i ] = LocalStart;

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rParticle->puColorEnd[ i ] = LocalEnd;
	}

	SRgba LocalStart, LocalEnd;
	bool Rand;
};

struct grSScaleGenerate : public grSBaseGenerate
{
	inline void Set( const grV2f& start, const grV2f& end )
	{
		LocalStart = start;
		LocalEnd = end;
		SetBaseStartEnd( LocalStart, LocalEnd );
	}

	inline void Generate( pU<grSParticle>& rParticle, const sizeT startIdx, const sizeT endIdx, const pU<grRandom>& rRand )
	{
		if ( Equal == EParticleEqual::NO )
		{			
			float midX = grMath::Abs( LocalStart.x - LocalEnd.x ) * 0.5f;
			float midY = grMath::Abs( LocalStart.y - LocalEnd.y ) * 0.5f;

			for ( sizeT i = startIdx; i < endIdx; ++i )
			{
				grV2f result( LocalStart.x > midX ? rRand->Float( midX, LocalStart.x ) : rRand->Float( LocalStart.x, midX ),
							  LocalStart.y > midY ? rRand->Float( midY, LocalStart.y ) : rRand->Float( LocalStart.y, midY ) );

				rParticle->puScaleStart[ i ] = result;
			}

			for ( sizeT i = startIdx; i < endIdx; ++i )
			{
				grV2f result( LocalEnd.x > midX ? rRand->Float( midX, LocalEnd.x ) : rRand->Float( LocalEnd.x, midX ),
							  LocalEnd.y > midY ? rRand->Float( midY, LocalEnd.y ) : rRand->Float( LocalEnd.y, midY ) );

				rParticle->puScaleEnd[ i ] = result;
			}

			return;
		}

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rParticle->puScaleStart[ i ] = LocalStart;
	}

	grV2f LocalStart, LocalEnd;
};

struct grSForceBasicGenerate : public grSBaseGenerate
{
	inline void Set( const grV2f& min, const grV2f& max )
	{
		LocalMin = min;
		LocalMax = max;
		SetBaseMinMaxX2( LocalMin, LocalMax );
	}

	inline void Generate( pU<grSParticle>& rParticle, const sizeT startIdx, const sizeT endIdx, const pU<grRandom>& rRand )
	{
		if ( Equal == EParticleEqual::NO )
		{
			for ( sizeT i = startIdx; i < endIdx; ++i )
				rParticle->puAcceleration[ i ] += rRand->V2fx2( LocalMin, LocalMax );

			return;
		}

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rParticle->puAcceleration[ i ] += LocalMin;
	}

	grV2f LocalMin, LocalMax;
};

struct grSPositionGenerate : public grSBaseGenerate
{
	inline void Set( const grV2f& min, const grV2f& max )
	{
		LocalMin = min;
		LocalMax = max;
		SetBaseMinMaxX2( LocalMin, LocalMax );
	}

	inline void Generate( pU<grSParticle>& rParticle, const grV2f& positionSys, const sizeT startIdx, const sizeT endIdx, const pU<grRandom>& rRand )
	{
		if ( Equal == EParticleEqual::NO )
		{
			for ( sizeT i = startIdx; i < endIdx; ++i )
				rParticle->puPosition[ i ] = grV2f( rRand->V2fx2( LocalMin, LocalMax ) ) + positionSys;

			return;
		}

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rParticle->puPosition[ i ] = LocalMin + positionSys;
	}

	grV2f LocalMin, LocalMax;
};

struct grSMassGenerate : public grSBaseGenerate
{
	inline void Set( const grV2f& rMinMax )
	{
		LocalMinMax = rMinMax;
		if ( LocalMinMax.x < 1.0f )
			LocalMinMax.x = 1.0f;

		SetBaseMinMax( LocalMinMax );
	}

	inline void Generate( pU<grSParticle>& rParticle, const sizeT startIdx, const sizeT endIdx, const pU<grRandom>& rRand )
	{
		if ( Equal == EParticleEqual::NO )
		{
			for ( sizeT i = startIdx; i < endIdx; ++i )
				rParticle->puMass[ i ] = rRand->Float( LocalMinMax.x, LocalMinMax.y );

			return;
		}

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rParticle->puMass[ i ] = LocalMinMax.x;
	}

	grV2f LocalMinMax;
};

struct grSLifeGenerate : public grSBaseGenerate
{
	inline void Set( const grV2f& minMax )
	{
		LocalMinMax = minMax;
		if ( LocalMinMax.x < 0.0f )
			LocalMinMax.x = 0.0f;

		SetBaseMinMax( LocalMinMax );
	}

	inline void Generate( pU<grSParticle>& rParticle, const sizeT startIdx, const sizeT endIdx, const pU<grRandom>& rRand )
	{
		if ( Equal == EParticleEqual::NO )
		{
			for ( sizeT i = startIdx; i < endIdx; ++i )
				rParticle->puLife[ i ] = rRand->Float( LocalMinMax.x, LocalMinMax.y );

			return;
		}

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rParticle->puLife[ i ] = LocalMinMax.x;
	}

	grV2f LocalMinMax;
};

struct grSEmitter
{
	grSEmitter( const sizeT size, const float emitRate, const grV2f& position )
		: puRand		( std::make_unique<grRandom>() )
		, PositionSys	( position )
		, EmitRate		( emitRate )
		, SpawnAccT		( 0.0f )
		, EmitAcc		( 0 )
		, Size			( size )
	{}
	grSEmitter( const grSEmitter& ) = delete;
	grSEmitter& operator=( const grSEmitter& ) = delete;
	grSEmitter( grSEmitter&& ) noexcept = delete;
	grSEmitter& operator=( grSEmitter&& ) noexcept = delete;

	inline void Emit( pU<grSParticle>& rParticle, const float deltaT ) noexcept
	{
		// This could be done smoother ex; NewSpawns = dt * SpawnRate
		// Problem is I haven't figured out how to make the spawnrate 'API' call non arbitrary in relation to real time, for example seconds

		// In case spawns per frame would be greater than frame time
		SpawnAccT += deltaT;
		while ( SpawnAccT >= EmitRate )
		{
			SpawnAccT -= EmitRate;
			EmitAcc += 1;
		}

		if ( EmitAcc > 0 )
		{
			sizeT startIdx = rParticle->Alive;
			sizeT endIdx = grMath::Min( rParticle->Alive + EmitAcc + 1, Size - 1 );

			// Increment particles alive
			rParticle->Alive += EmitAcc + 1;	// TODO: +1 should be needed so find bug

			if ( puColor )		puColor->Generate( rParticle, startIdx, endIdx, puRand );
			if ( puScale )		puScale->Generate( rParticle, startIdx, endIdx, puRand );
			if ( puForceBasic )	puForceBasic->Generate( rParticle, startIdx, endIdx, puRand );
			if ( puPosition )	puPosition->Generate( rParticle,  PositionSys, startIdx, endIdx, puRand );
			if ( puMass )		puMass->Generate( rParticle, startIdx, endIdx, puRand );
			if ( puLife )		puLife->Generate( rParticle, startIdx, endIdx, puRand );
		}

		EmitAcc = 0;
	}

	// All types of generators goes here
	// No slow virtual stuff allowed so each generator has it's own place
	// TODO: If multiple generators of the same type would be allowed and produce cool results perhaps change to arrays
	pU<grSColorGenerate> puColor;
	pU<grSScaleGenerate> puScale;
	pU<grSForceBasicGenerate> puForceBasic;
	pU<grSPositionGenerate> puPosition;
	pU<grSMassGenerate> puMass;
	pU<grSLifeGenerate> puLife;

	pU<grRandom> puRand;	// Is slightly greater than 5mb so instead of each generator containing one it's passed

	grV2f PositionSys;
	float EmitRate, SpawnAccT;
	sizeT EmitAcc, Size;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
struct grSColorUpdate
{
	// TODO: Only create updater instance if colors are NOT equal

	grSColorUpdate( const bool hsv )
		: Hsv	( hsv )
	{}

	inline void Update( pU<grSParticle>& rParticle, const float deltaT )
	{
		if ( Hsv )
		{
			for ( sizeT i = 0; i < rParticle->Alive; ++i )
			{
				// RGB -> HSV lerp -> RGB
				grColor::SHsva start = grColor::Rgba2Hsva( rParticle->puColorStart[ i ] );
				grColor::SHsva end = grColor::Rgba2Hsva( rParticle->puColorEnd[ i ] );

				float lerpValue = 1.0f / rParticle->puLife[ i ] * deltaT;

				start.H = grMath::Lerp( start.H, end.H, lerpValue );
				start.S = grMath::Lerp( start.S, end.S, lerpValue );
				start.V = grMath::Lerp( start.V, end.V, lerpValue );
				start.A = grMath::Lerp( start.A, end.A, lerpValue );

				rParticle->puColorStart[ i ] = grColor::Hsva2Rgba( start );
			}

			return;
		}

		for ( sizeT i = 0; i < rParticle->Alive; ++i )
		{
			// Actually faster to store in block and than lerp then passing to lerp by directly indexing the array // Measured with std::chronos
			grColor::SRgba start = rParticle->puColorStart[ i ];
			grColor::SRgba end = rParticle->puColorEnd[ i ];

			float lerpValue = 1.0f / rParticle->puLife[ i ] * deltaT;

			rParticle->puColorStart[ i ].R = grMath::Lerp( start.R, end.R, lerpValue );
			rParticle->puColorStart[ i ].G = grMath::Lerp( start.G, end.G, lerpValue );
			rParticle->puColorStart[ i ].B = grMath::Lerp( start.B, end.B, lerpValue );
			rParticle->puColorStart[ i ].A = grMath::Lerp( start.A, end.A, lerpValue );
		}
	}

	bool Hsv;
};

struct grSScaleUpdate
{
	inline void Update( pU<grSParticle>& rParticle, const float deltaT )
	{
		for ( sizeT i = 0; i < rParticle->Alive; ++i )
		{
			grV2f start = rParticle->puScaleStart[ i ];
			grV2f end = rParticle->puScaleEnd[ i ];

			float lerpValue = 1.0f / rParticle->puLife[ i ] * deltaT;

			rParticle->puScaleStart[ i ] = grMath::Lerp( start, end, lerpValue );
		}
	}
};

struct grSForceBasicUpdate
{
	inline void Update( pU<grSParticle>& rParticle )
	{
		for ( sizeT i = 0; i < rParticle->Alive; ++i )
			rParticle->puVelocity[ i ] = rParticle->puAcceleration[ i ] / rParticle->puMass[ i ];
	}
};

struct grSPositionUpdate
{
	inline void Update( pU<grSParticle>& rParticle, const float deltaT )
	{
		for ( sizeT i = 0; i < rParticle->Alive; ++i )
			rParticle->puPosition[ i ] += rParticle->puVelocity[ i ] * deltaT;
	}
};

struct grSLifeUpdate
{
	inline void Update( pU<grSParticle>& rParticle, const float deltaT )
	{
		for ( sizeT i = 0; i < rParticle->Alive; ++i )
		{
			rParticle->puLife[ i ] -= deltaT;
			if ( rParticle->puLife[ i ] <= 0.0f )
				rParticle->Kill( i );
		}
	}
};

struct grSUpdate
{
	grSUpdate( void )
	: puPosition ( std::make_unique<grSPositionUpdate>() )
	{}
	grSUpdate( const grSUpdate& ) = delete;
	grSUpdate& operator=( const grSUpdate& ) = delete;
	grSUpdate( grSUpdate&& ) noexcept = delete;
	grSUpdate& operator=( grSUpdate&& ) noexcept = delete;

	inline void Update( pU<grSParticle>& rParticle, const float deltaT )
	{
		// TODO: Would be cool to not need the if's. Do not wan't the indirections from a virtual base so some type of eventlist might be an option
		// Update all updaters
		if ( puColor )		puColor->Update( rParticle, deltaT );
		if ( puScale )		puScale->Update( rParticle, deltaT );
		if ( puForceBasic )	puForceBasic->Update( rParticle );
		if ( puPosition )	puPosition->Update( rParticle, deltaT );
		if ( puLife )		puLife->Update( rParticle, deltaT );
	}

	// No slow virtual stuff allowed so each updater has it's own place
	// All types of updaters goes here
	pU<grSColorUpdate> puColor;
	pU<grSScaleUpdate> puScale;
	pU<grSForceBasicUpdate> puForceBasic;
	pU<grSPositionUpdate> puPosition;
	pU<grSLifeUpdate> puLife;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class grCParticleSys
{
public:
	grCParticleSys( const intU size = 2000, const float emitRate = 1000.0f, const grV2f& position = grV2f( 320.0f, 180.0f ) );
	grCParticleSys( const grCParticleSys& ) = delete;
	grCParticleSys& operator=( const grCParticleSys& ) = delete;
	grCParticleSys( grCParticleSys&& ) noexcept = delete;
	grCParticleSys& operator=( grCParticleSys&& ) noexcept = delete;

	void EmitRate( const float emitRate );
	void PositionSystem( const grV2f& position );

	// If min<->max are equal, the value will be that. If not equal, the value will be rand inbetween
	// TODO: Could be better to use injection here as that would make it possible store behaviours externaly
	void Color( const grColor::SRgba& start, const grColor::SRgba& end, const bool hsv = false, const bool randomize = false );
	void Scale( const grV2f& start, const grV2f& end );
	void ForceBasic( const grV2f& min, const grV2f& max );
	void Position( const grV2f& min, const grV2f& max );
	void Mass( const grV2f& minMax );
	void Life( const grV2f& minMax );

	// TODO: Remeber to make this call once in the particle manager whenever that class exists
	void Update( const float deltaT );

private:
	pU<grSParticle> m_puParticle;
	pU<grSEmitter> m_puEmit;
	pU<grSUpdate> m_puUpdate;

	// TODO: Needs activation choices for emission: start/stop/timer/infinite
};

#endif	// _GRPARTICLETEST_H_