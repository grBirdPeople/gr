#ifndef _H_GRPARTICLESYSTEM_
#define _H_GRPARTICLESYSTEM_

#include "grAlgo.h"
#include "grColor.h"
#include "grParticleData.h"

typedef std::uniform_real_distribution<float> FloatDist;


struct grSBaseSystem
{
	void InitColor( grColor::SRgba& rStart, grColor::SRgba& rEnd )
	{
		rStart.R = grMath::Clamp<float>( rStart.R, 0.0f, 255.0f );
		rStart.G = grMath::Clamp<float>( rStart.G, 0.0f, 255.0f );
		rStart.B = grMath::Clamp<float>( rStart.B, 0.0f, 255.0f );
		rStart.A = grMath::Clamp<float>( rStart.A, 0.0f, 255.0f );

		rEnd.R = grMath::Clamp<float>( rEnd.R, 0.0f, 255.0f );
		rEnd.G = grMath::Clamp<float>( rEnd.G, 0.0f, 255.0f );
		rEnd.B = grMath::Clamp<float>( rEnd.B, 0.0f, 255.0f );
		rEnd.A = grMath::Clamp<float>( rEnd.A, 0.0f, 255.0f );
	}

	void SetMinMax( grV2f& rMinMax, EEqualValue& rEqual, const bool swap = true )
	{
		if ( swap )
		{
			if ( rMinMax.x > rMinMax.y )
				grAlgo::Swap( rMinMax.x, rMinMax.y );
		}
		rEqual = grMath::CmpF( rMinMax.x, rMinMax.y ) ? EEqualValue::YES : EEqualValue::NO;
	}

	void SetMinMax( grV2f& rMin, grV2f& rMax, EEqualValue& rEqual, const bool swap = true )
	{
		if ( swap )
		{
			if ( rMin.x > rMax.x )
				grAlgo::Swap( rMin.x, rMax.x );
			if ( rMin.y > rMax.y )
				grAlgo::Swap( rMin.y, rMax.y );
		}
		rEqual = grMath::CmpV2f( rMin, rMax ) ? EEqualValue::YES : EEqualValue::NO;
	}

	void SetStartEnd( const grV2f& rStartMin, const grV2f& rStartMax, const grV2f& rEndMin, const grV2f& rEndMax, EEqualValue& rStartEqual, EEqualValue& rEndEqual )
	{
		rStartEqual = grMath::CmpV2f( rStartMin, rStartMax ) ? EEqualValue::YES : EEqualValue::NO;
		rEndEqual = grMath::CmpV2f( rEndMin, rEndMax ) ? EEqualValue::YES : EEqualValue::NO;
	}
};


struct grSScaleSystem : public grSBaseSystem
{
	typedef void( grSScaleSystem::*ScaleOption )( const sizeT startIdx, const sizeT endIdx );

	grSEmitData& rEmit;
	grSScaleData& rScale;
	grSArrayData& rArray;

	FloatDist DistStartX;
	FloatDist DistStartY;
	FloatDist DistEndX;
	FloatDist DistEndY;

	ScaleOption Option;

	grSScaleSystem( const grSParticleData& rData )
		: rEmit( *rData.puEmit )
		, rScale( *rData.puScale )
		, rArray( *rData.puArray )
		, Option( &grSScaleSystem::Option0 )
	{}
	grSScaleSystem( const grSScaleSystem& ) = default;
	grSScaleSystem& operator=( const grSScaleSystem& ) = default;

	void Init( const grV2f& rStartMin, const grV2f& rStartMax, const grV2f& rEndMin, const grV2f& rEndMax )
	{		
		rScale.ScaleStartMin = rStartMin;
		rScale.ScaleStartMax = rStartMax;
		rScale.ScaleEndMin = rEndMin;
		rScale.ScaleEndMax = rEndMax;
		SetStartEnd( rScale.ScaleStartMin, rScale.ScaleStartMax, rScale.ScaleEndMin, rScale.ScaleEndMax, rScale.EqualScaleStart, rScale.EqualScaleEnd );

		Option = rScale.EqualScaleStart == EEqualValue::NO && rScale.EqualScaleEnd == EEqualValue::NO ?
			&grSScaleSystem::Option0 :
			rScale.EqualScaleStart == EEqualValue::NO && rScale.EqualScaleEnd == EEqualValue::YES ?
			&grSScaleSystem::Option1 :
			rScale.EqualScaleStart == EEqualValue::YES && rScale.EqualScaleEnd == EEqualValue::NO ?
			&grSScaleSystem::Option2 :
			&grSScaleSystem::Option3;

		DistStartX = rScale.ScaleStartMin.x < rScale.ScaleStartMax.x ?
			rScale.Rand.DistF( rScale.ScaleStartMin.x, rScale.ScaleStartMax.x ) :
			rScale.Rand.DistF( rScale.ScaleStartMax.x, rScale.ScaleStartMin.x );

		DistStartY = rScale.ScaleStartMin.y < rScale.ScaleStartMax.y ?
			rScale.Rand.DistF( rScale.ScaleStartMin.y, rScale.ScaleStartMax.y ) :
			rScale.Rand.DistF( rScale.ScaleStartMax.y, rScale.ScaleStartMin.y );

		DistEndX = rScale.ScaleEndMin.x < rScale.ScaleEndMax.x ?
			rScale.Rand.DistF( rScale.ScaleEndMin.x, rScale.ScaleEndMax.x ) :
			rScale.Rand.DistF( rScale.ScaleEndMax.x, rScale.ScaleEndMin.x );

		DistEndY = rScale.ScaleEndMin.y < rScale.ScaleEndMax.y ?
			rScale.Rand.DistF( rScale.ScaleEndMin.y, rScale.ScaleEndMax.y ) :
			rScale.Rand.DistF( rScale.ScaleEndMax.y, rScale.ScaleEndMin.y );
	}

	void Generate()
	{
		sizeT startIdx{ rEmit.StartIdx }, endIdx{ rEmit.EndIdx };
		( this->*Option )( startIdx, endIdx );
	}

	void Option0( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rArray.ScaleStart[ i ] = { rScale.Rand.Float( DistStartX ), rScale.Rand.Float( DistStartY ) };

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rArray.ScaleEnd[ i ] = { rScale.Rand.Float( DistEndX ), rScale.Rand.Float( DistEndY ) };
	}

	void Option1( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rArray.ScaleStart[ i ] = { rScale.Rand.Float( DistStartX ), rScale.Rand.Float( DistStartY ) };

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rArray.ScaleEnd[ i ] = rScale.ScaleEndMin;
	}

	void Option2( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rArray.ScaleStart[ i ] = rScale.ScaleStartMin;

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rArray.ScaleEnd[ i ] = { rScale.Rand.Float( DistEndX ), rScale.Rand.Float( DistEndY ) };
	}

	void Option3( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rArray.ScaleStart[ i ] = rScale.ScaleStartMin;

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rArray.ScaleEnd[ i ] = rScale.ScaleEndMin;
	}

	void Update()
	{
		for ( sizeT i = 0; i < rEmit.Alive; ++i )
		{
			grV2f start{ rArray.ScaleStart[ i ] };
			grV2f end{ rArray.ScaleEnd[ i ] };
			float step{ ( 1.0f / rArray.Life[ i ] ) * rEmit.Dt };
			rArray.ScaleStart[ i ] = grMath::LerpV2f( start, end, step );
		}
	}
};


struct grSMassSystem : public grSBaseSystem
{
	typedef std::uniform_real_distribution<float> MassDist;

	grSEmitData& rEmit;
	grSMassData& rMass;
	grSArrayData& rArray;

	FloatDist DistMass;

	grSMassSystem( const grSParticleData& rData )
		: rEmit( *rData.puEmit )
		, rMass( *rData.puMass )
		, rArray( *rData.puArray )
	{}
	grSMassSystem( const grSMassSystem& ) = default;
	grSMassSystem& operator=( const grSMassSystem& ) = default;

	void Init( const grV2f& rMassMinMax )
	{
		rMass.MassMinMax.x = grMath::Max( rMassMinMax.x, 1.0f );
		rMass.MassMinMax.y = grMath::Max( rMassMinMax.y, 1.0f );
		SetMinMax( rMass.MassMinMax, rMass.EqualMass );
		DistMass = rMass.Rand.DistF( rMass.MassMinMax.x, rMass.MassMinMax.y );
	}

	void Generate()
	{
		sizeT startIdx{ rEmit.StartIdx }, endIdx{ rEmit.EndIdx };
		if ( rMass.EqualMass == EEqualValue::NO )
		{
			for ( sizeT i = startIdx; i < endIdx; ++i )
				rArray.Mass[ i ] = rMass.Rand.Float( DistMass );

			return;
		}

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rArray.Mass[ i ] = rMass.MassMinMax.x;
	}
};


struct grSVelocitySystem : public grSBaseSystem
{
	grSEmitData& rEmit;
	grSVelocityData& rVelocity;
	grSArrayData& rArray;

	FloatDist VelDist;

	grSVelocitySystem( const grSParticleData& rData )
		: rEmit( *rData.puEmit )
		, rVelocity( *rData.puVelocity )
		, rArray( *rData.puArray )
	{}
	grSVelocitySystem( const grSVelocitySystem& ) = default;
	grSVelocitySystem& operator=( const grSVelocitySystem& ) = default;

	void Init( const grV2f& rDegreeMinMax, const grV2f& rForceMinMax )
	{
		rVelocity.DegreeMinMax = grV2f( grMath::Clamp<float>( rDegreeMinMax.x, 0.0f, 359.9f ), grMath::Clamp<float>( rDegreeMinMax.y, 0.0f, 359.9f ) );
		rVelocity.ForceMinMax = rForceMinMax;
		SetMinMax( rVelocity.DegreeMinMax, rVelocity.EqualDegree, false );
		SetMinMax( rVelocity.ForceMinMax, rVelocity.EqualForce );
	}

	void Generate()
	{
		// This is retarded but I'm retarded and can't figure out how to do it without using 6 different loops which would be better but look more retarded
		// It shall remain retarded for the moment(s)

		sizeT startIdx{ rEmit.StartIdx }, endIdx{ rEmit.EndIdx };
		for ( sizeT i = startIdx; i < endIdx; ++i )
		{
			float d{ FindDegrees() };
			float f{ FindForce() };
			grV2f v{ grMath::DegToVec( d ) * f };
			rArray.Velocity[ i ] = v / rArray.Mass[ i ];
		}
	}

	float FindDegrees()
	{
		if ( rVelocity.EqualDegree == EEqualValue::NO )
		{
			if ( rVelocity.DegreeMinMax.x > rVelocity.DegreeMinMax.y )
			{
				float d{ 0.0f };
				float diff{ 359.9f - rVelocity.DegreeMinMax.x };
				VelDist = rVelocity.Rand.DistF( 0.0f, diff + rVelocity.DegreeMinMax.y );
				d = rVelocity.Rand.Float( VelDist ) - diff;
				if ( d < 0.0f )
					d += 359.9f;

				return d;
			}

			auto distDeg{ rVelocity.Rand.DistF( rVelocity.DegreeMinMax.x, rVelocity.DegreeMinMax.y ) };
			return rVelocity.Rand.Float( distDeg );
		}

		return rVelocity.DegreeMinMax.x;
	}

	float FindForce()
	{
		if ( rVelocity.EqualForce == EEqualValue::NO )
		{
			auto distForce{ rVelocity.Rand.DistF( rVelocity.ForceMinMax.x, rVelocity.ForceMinMax.y ) };
			return rVelocity.Rand.Float( distForce );
		}

		return rVelocity.ForceMinMax.x;
	}

	void Update()
	{
		for ( sizeT i = 0; i < rEmit.Alive; ++i )
			rArray.Velocity[ i ] += rArray.Acceleration[ i ].x * rEmit.Dt;
	}
};


struct grSPositionSystem : public grSBaseSystem
{
	grSEmitData& rEmit;
	grSPositionData& rPosition;
	grSArrayData& rArray;

	FloatDist DistPosX;
	FloatDist DistPosY;

	grSPositionSystem( const grSParticleData& rData )
		: rEmit( *rData.puEmit )
		, rPosition( *rData.puPosition )
		, rArray( *rData.puArray )
	{}
	grSPositionSystem( const grSPositionSystem& ) = default;
	grSPositionSystem& operator=( const grSPositionSystem& ) = default;

	void Init( const EPositionType positionType, const grV2f& rPositionOffsetMin, const grV2f& rPositionOffsetMax )
	{
		rPosition.PositionOffsetMin = rPositionOffsetMin;
		rPosition.PositionOffsetMax = rPositionOffsetMax;
		rPosition.TypePosition = positionType;
		SetMinMax( rPosition.PositionOffsetMin, rPosition.PositionOffsetMax, rPosition.EqualPosition );

		DistPosX = rPosition.Rand.DistF( rPosition.PositionOffsetMin.x, rPosition.PositionOffsetMax.x );
		DistPosY = rPosition.Rand.DistF( rPosition.PositionOffsetMin.y, rPosition.PositionOffsetMax.y );
	}

	void Init( const EPositionType positionType, const grV2f& rRadiusMin, const grV2f& rRadiusMax, const grV2f& rStepMinMax, const grV2f& rTiltMinMax )
	{
		rPosition.EllipseRadiusMin = rRadiusMin;
		rPosition.EllipseRadiusMax = rRadiusMax;
		rPosition.EllipseStepMinMax = rStepMinMax;
		rPosition.EllipseTiltMinMax = rTiltMinMax;
		rPosition.TypePosition = positionType;

		// position = Vector2( centerPos.x + ( radX * Mathf.Sin( Mathf.Deg2Rad * alpha ) ),
		// 					   centerPos.y + ( radY * Mathf.Cos( Mathf.Deg2Rad * alpha ) ) );
		   
		// alpha += 5f;//can be used as speed

		// position = Vector2( centerPos.x + ( radX * MCos( alpha ) * MCos( tilt ) ) - ( radY * MSin( alpha ) * MSin( tilt ) ),
		// 					   centerPos.y + ( radX * MCos( alpha ) * MSin( tilt ) ) + ( radY * MSin( alpha ) * MCos( tilt ) ) );
		// alpha += 5f;
	}

	void Generate()
	{
		//// Ellipse
		//grV2f v;
		//float radX{ 50.0f };
		//float radY{ 25.0f };

		//// Circular path pos
		//rPosition.EllipseStepCount += 4.0f * rEmit.Dt;
		//if( rPosition.EllipseStepCount > rPosition.Ellipse360 )
		//	rPosition.EllipseStepCount = 0.0f;

		//// Random circulr pos
		////auto distAlpha{ rPosition.Rand.DistF( 0.0f, rPosition.Ellipse360 ) };
		////rPosition.EllipseStepCount = rPosition.Rand.Float( distAlpha );

		//// Tilt
		//rPosition.EllipseTiltCount += grMath::DegToRad( 50.0f * rEmit.Dt );
		//if( rPosition.EllipseTiltCount > 360.0f )
		//	rPosition.EllipseTiltCount = 0.0f;
		//if( rPosition.EllipseTiltCount < 0.0f )
		//	rPosition.EllipseTiltCount = 360.0f;



		//// Regular
		//v.x = rEmit.SystemPosition.x + ( radX * std::sinf( rPosition.EllipseStepCount ) );
		//v.y = rEmit.SystemPosition.y + ( radY * std::cosf( rPosition.EllipseStepCount ) );

		//// Tilt
		////v.x = rEmit.SystemPosition.x + ( radX * std::cosf( rPosition.EllipseStepCount ) * std::cosf( rPosition.EllipseTiltCount ) ) -
		////	( radY * std::sinf( rPosition.EllipseStepCount ) * std::sinf( rPosition.EllipseTiltCount ) );

		////v.y = rEmit.SystemPosition.y + ( radX * std::cosf( rPosition.EllipseStepCount ) * std::sinf( rPosition.EllipseTiltCount ) ) +
		////	( radY * std::sinf( rPosition.EllipseStepCount ) * std::cosf( rPosition.EllipseTiltCount ) );

		//for ( sizeT i = rEmit.StartIdx; i < rEmit.EndIdx; ++i )
		//	rArray.Position[ i ] = v;



		// Box
		sizeT startIdx{ rEmit.StartIdx }, endIdx{ rEmit.EndIdx };
		if ( rPosition.EqualPosition == EEqualValue::NO )
		{
			if ( rPosition.TypePosition == EPositionType::BOX )
			{
				PositionNotEqualBox();
				return;
			}
			//PositionNotEqualEllipse( rSystemPosition, startIdx, endIdx, dt );
			return;
		}

		if ( rPosition.TypePosition == EPositionType::BOX )
		{
			PositionEqualBox();
			return;
		}
		//PositionEqualEllipse( rSystemPosition, startIdx, endIdx );
	}

	void PositionNotEqualBox()
	{
		for ( sizeT i = rEmit.StartIdx; i < rEmit.EndIdx; ++i )
		{
			grV2f v{ rPosition.Rand.Float( DistPosX ), rPosition.Rand.Float( DistPosY ) };
			rArray.Position[ i ] = v + rEmit.SystemPosition;
		}
	}

	void PositionNotEqualEllipse()
	{

	}

	void PositionEqualBox()
	{
		grV2f v{ rPosition.PositionOffsetMin + rEmit.SystemPosition };
		for ( sizeT i = rEmit.StartIdx; i < rEmit.EndIdx; ++i )
			rArray.Position[ i ] = v;
	}

	void PositionEqualEllipse()
	{

	}

	void Update()
	{
		for ( sizeT i = 0; i < rEmit.Alive; ++i )
			rArray.Position[ i ] += rArray.Velocity[ i ] * rEmit.Dt;
	}
};


struct grSLifeSystem : public grSBaseSystem
{
	grSEmitData& rEmit;
	grSLifeData& rLife;
	grSArrayData& rArray;

	grSLifeSystem( const grSParticleData& rData )
		: rEmit( *rData.puEmit )
		, rLife( *rData.puLife )
		, rArray( *rData.puArray )
	{}
	grSLifeSystem( const grSLifeSystem& ) = default;
	grSLifeSystem& operator=( const grSLifeSystem& ) = default;

	void Init( const grV2f& rLifeMinMax )
	{
		rLife.LifeMinMax = rLifeMinMax;
		SetMinMax( rLife.LifeMinMax, rLife.EqualLife );
	}

	void Generate()
	{
		sizeT startIdx{ rEmit.StartIdx }, endIdx{ rEmit.EndIdx };
		if ( rLife.EqualLife == EEqualValue::NO )
		{
			auto dist{ rLife.Rand.DistF( rLife.LifeMinMax.x, rLife.LifeMinMax.y ) };
			for ( sizeT i = startIdx; i < endIdx; ++i )
				rArray.Life[ i ] = rLife.Rand.Float( dist );

			return;
		}

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rArray.Life[ i ] = rLife.LifeMinMax.x;
	}

	void Update()
	{
		for ( sizeT i = 0; i < rEmit.Alive; ++i )
		{
			rArray.Life[ i ] -= rEmit.Dt;;
			if ( rArray.Life[ i ] <= 0.0f )
				Kill( i, --rEmit.Alive );
		}
	}

	void Kill( const sizeT nowIdx, const sizeT last )
	{
		//grAlgo::Swap( Array.puVerts[ nowIdx ], Array.puVerts[ last ] );
		//grAlgo::Swap( rParticleArr->puColorStart[ nowIdx ], rParticleArr->puColorStart[ last ] );
		//grAlgo::Swap( rParticleArr->puColorEnd[ nowIdx ], rParticleArr->puColorEnd[ last ] );
		grAlgo::Swap( rArray.ScaleStart[ nowIdx ], rArray.ScaleStart[ last ] );
		grAlgo::Swap( rArray.ScaleEnd[ nowIdx ], rArray.ScaleEnd[ last ] );
		grAlgo::Swap( rArray.Mass[ nowIdx ], rArray.Mass[ last ] );
		grAlgo::Swap( rArray.Acceleration[ nowIdx ], rArray.Acceleration[ last ] );
		grAlgo::Swap( rArray.Velocity[ nowIdx ], rArray.Velocity[ last ] );
		grAlgo::Swap( rArray.Position[ nowIdx ], rArray.Position[ last ] );
		grAlgo::Swap( rArray.Life[ nowIdx ], rArray.Life[ last ] );

		// Most values are generated and set by operator= and does not need zeroing (all above)
		// Values that are generated and set by operator+= needs resetting obviously (all below)
		rArray.Acceleration[ last ] = { 0.0f, 0.0f };
	}
};


struct grSParticleSystem
{
	pU<grSScaleSystem> puScale;
	pU<grSMassSystem> puMass;
	pU<grSVelocitySystem> puVelocity;
	pU<grSPositionSystem> puPosition;
	pU<grSLifeSystem> puLife;

	grSParticleSystem() = default;
	grSParticleSystem( const grSParticleSystem& ) = delete;
	grSParticleSystem& operator=( const grSParticleSystem& ) = delete;
	grSParticleSystem( grSParticleSystem&& ) noexcept = delete;
	grSParticleSystem& operator=( grSParticleSystem&& ) noexcept = delete;

	void Init( const grSParticleData& rData )
	{
		puScale = std::make_unique<grSScaleSystem>( rData );
		puMass = std::make_unique<grSMassSystem>( rData );
		puVelocity = std::make_unique<grSVelocitySystem>( rData );
		puPosition = std::make_unique<grSPositionSystem>( rData );
		puLife = std::make_unique<grSLifeSystem>( rData );
	}
};

#endif // _H_GRPARTICLESYSTEM_