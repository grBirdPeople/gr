#ifndef _H_GRPARTICLESYSTEM_
#define _H_GRPARTICLESYSTEM_

#include "grAlgo.h"
#include "grParticleData.h"

template<typename T>
using GenOpt = void( T::* )( const sizeT startIdx, const sizeT endIdx );
template<typename T>
using UpdOpt = void( T::* )( const sizeT alive, const float dt );


struct grSEmitSystem
{
	grCParticleData& rData;

	grSEmitSystem( grCParticleData& rParticleData )
		: rData( rParticleData )
	{}
	grSEmitSystem( const grSEmitSystem& ) = default;
	grSEmitSystem& operator=( const grSEmitSystem& ) = default;

	void Generate()
	{
		rData.EmitData.SpawnTimeAcc += rData.EmitData.Dt;
		rData.EmitData.EmitAcc = 0;
		while ( rData.EmitData.SpawnTimeAcc >= rData.EmitData.EmitRateMs )
		{
			rData.EmitData.SpawnTimeAcc -= rData.EmitData.EmitRateMs;
			rData.EmitData.EmitAcc += 1;
		}

		if ( rData.EmitData.EmitAcc > 0 )
		{
			sizeT last{ rData.EmitData.Size - 1 };
			rData.EmitData.StartIdx = rData.EmitData.Alive;
			rData.EmitData.EndIdx = grMath::Min<sizeT>( rData.EmitData.StartIdx + rData.EmitData.EmitAcc, last );
			if ( rData.EmitData.StartIdx == rData.EmitData.EndIdx )
				return;

			rData.EmitData.Alive += rData.EmitData.EndIdx - rData.EmitData.StartIdx;
		}
	}
};


struct grSBaseSystem // Some helper functions and as some are shared between systems this struct is inherited
{
	void EqualCheck( grV2f& rMinMax, EEqualValue& rEqual )
	{
		rEqual = grMath::CmpF( rMinMax.x, rMinMax.y ) ? EEqualValue::YES : EEqualValue::NO;
	}

	void EqualCheckColor( grColor::Rgba arrCol[], EEqualValue& rStartEqual, EEqualValue& rEndEqual, EEqualValue& rLerpEqual )
	{
		rStartEqual = arrCol[ 0 ].Cmp( arrCol[ 1 ] ) ? EEqualValue::YES : EEqualValue::NO;
		rEndEqual = arrCol[ 2 ].Cmp( arrCol[ 3 ] ) ? EEqualValue::YES : EEqualValue::NO;

		if ( rStartEqual == EEqualValue::YES && rEndEqual == EEqualValue::YES )
			rLerpEqual = arrCol[ 1 ].Cmp( arrCol[ 2 ] ) ? EEqualValue::YES : EEqualValue::NO;
	}

	void EqualCheckScale( const grV2f arrV2f[], EEqualValue& rStartEqual, EEqualValue& rEndEqual )
	{
		rStartEqual = grMath::CmpV2f( arrV2f[ 0 ], arrV2f[ 1 ] ) ? EEqualValue::YES : EEqualValue::NO;
		rEndEqual = grMath::CmpV2f( arrV2f[ 2 ], arrV2f[ 3 ] ) ? EEqualValue::YES : EEqualValue::NO;
	}

	void EqualCheck( const grV2f arrV2f[], EEqualValue& rEqualX, EEqualValue& rEqualY )
	{
		rEqualX = grMath::CmpF( arrV2f[ 0 ].x, arrV2f[ 1 ].x ) ? EEqualValue::YES : EEqualValue::NO;
		rEqualY = grMath::CmpF( arrV2f[ 0 ].y, arrV2f[ 1 ].y ) ? EEqualValue::YES : EEqualValue::NO;
	}

	void SwapCheckColor( grColor::Rgba arr[], const sizeT arrIdx1, const sizeT arrIdx2 )
	{
		if ( arr[ arrIdx1 ].R > arr[ arrIdx2 ].R )
			grAlgo::Swap<uint8_t>( arr[ arrIdx1 ].R, arr[ arrIdx2 ].R );

		if ( arr[ arrIdx1 ].G > arr[ arrIdx2 ].G )
			grAlgo::Swap<uint8_t>( arr[ arrIdx1 ].G, arr[ arrIdx2 ].G );

		if ( arr[ arrIdx1 ].B > arr[ arrIdx2 ].B )
			grAlgo::Swap<uint8_t>( arr[ arrIdx1 ].B, arr[ arrIdx2 ].B );

		if ( arr[ arrIdx1 ].A > arr[ arrIdx2 ].A )
			grAlgo::Swap<uint8_t>( arr[ arrIdx1 ].A, arr[ arrIdx2 ].A );
	}

	void SwapCheck( grV2f& rMinMax )
	{
		if ( rMinMax.x > rMinMax.y )
			grAlgo::Swap( rMinMax.x, rMinMax.y );
	}

	void SwapCheck( grV2f& rMin, grV2f& rMax )
	{
		if ( rMin.x > rMax.x )
			grAlgo::Swap( rMin.x, rMax.x );
		if ( rMin.y > rMax.y )
			grAlgo::Swap( rMin.y, rMax.y );
	}
};


struct grSColorSystem : public grSBaseSystem
{
	grCParticleData& rData;
	GenOpt<grSColorSystem> GenOption;
	UpdOpt<grSColorSystem> UpdOption; // Lerp HSV or RGB

	grSColorSystem( grCParticleData& rParticleData )
		: rData( rParticleData )
		, GenOption( &grSColorSystem::GenOpt3 )
		, UpdOption( &grSColorSystem::UpdOpt1 )
	{}
	grSColorSystem( const grSColorSystem& ) = default;
	grSColorSystem& operator=( const grSColorSystem& ) = default;

	void Init( const grColor::Rgba& rStartMin, const grColor::Rgba& rStartMax, const grColor::Rgba& rEndMin, const grColor::Rgba& rEndMax, const bool hsv )
	{
		rData.ColorData.ArrMinMax[ 0 ] = rStartMin;
		rData.ColorData.ArrMinMax[ 1 ] = rStartMax;
		rData.ColorData.ArrMinMax[ 2 ] = rEndMin;
		rData.ColorData.ArrMinMax[ 3 ] = rEndMax;
		rData.ColorData.bHsv = hsv;

		SwapCheckColor( rData.ColorData.ArrMinMax, 0, 1 );
		SwapCheckColor( rData.ColorData.ArrMinMax, 2, 3 );

		rData.ColorData.LerpEqual = EEqualValue::NO; // A cheat if the default values set in the data struct would be used it doesn't trigger lerping
		EqualCheckColor( rData.ColorData.ArrMinMax, rData.ColorData.StartEqual, rData.ColorData.EndEqual, rData.ColorData.LerpEqual );

		UpdOption = rData.ColorData.bHsv ? &grSColorSystem::UpdOpt0 : &grSColorSystem::UpdOpt1;

		// Dislike all below code but I don't wan't to bloat with states or more weird looking functions so it will do for now/ever
		if ( rData.ColorData.StartEqual == EEqualValue::NO && rData.ColorData.EndEqual == EEqualValue::NO )
		{
			InitDist( 0, 0, 1 );
			InitDist( 4, 2, 3 );
			GenOption = &grSColorSystem::GenOpt0;
			return;
		}

		if ( rData.ColorData.StartEqual == EEqualValue::NO && rData.ColorData.EndEqual == EEqualValue::YES )
		{
			InitDist( 0, 0, 1 );
			GenOption = &grSColorSystem::GenOpt1;
			return;
		}

		if ( rData.ColorData.StartEqual == EEqualValue::YES && rData.ColorData.EndEqual == EEqualValue::NO )
		{
			InitDist( 4, 2, 3 );
			GenOption = &grSColorSystem::GenOpt2;
			return;
		}

		// rColData.StartEqual == EEqualValue::YES && rColData.EndEqual == EEqualValue::YES
		GenOption = &grSColorSystem::GenOpt3;
	}

	void InitDist( const sizeT arrDistIdx, const sizeT arrMinMaxIdx1, const sizeT arrMinMaxIdx2 )
	{
		rData.ColorData.ArrDist[ arrDistIdx ] = rData.ColorData.Rand.DistU( rData.ColorData.ArrMinMax[ arrMinMaxIdx1 ].R, rData.ColorData.ArrMinMax[ arrMinMaxIdx2 ].R );
		rData.ColorData.ArrDist[ arrDistIdx + 1 ] = rData.ColorData.Rand.DistU( rData.ColorData.ArrMinMax[ arrMinMaxIdx1 ].G, rData.ColorData.ArrMinMax[ arrMinMaxIdx2 ].G );
		rData.ColorData.ArrDist[ arrDistIdx + 2 ] = rData.ColorData.Rand.DistU( rData.ColorData.ArrMinMax[ arrMinMaxIdx1 ].B, rData.ColorData.ArrMinMax[ arrMinMaxIdx2 ].B );
		rData.ColorData.ArrDist[ arrDistIdx + 3 ] = rData.ColorData.Rand.DistU( rData.ColorData.ArrMinMax[ arrMinMaxIdx1 ].A, rData.ColorData.ArrMinMax[ arrMinMaxIdx2 ].A );
	}

	void InitRandColor( pU<grColor::Rgba[]>& rArr, const sizeT arrColorIdx, const sizeT arrDistIdx )
	{
		rArr[ arrColorIdx ].R = ( uint8_t )( rData.ColorData.Rand.IntU( rData.ColorData.ArrDist[ arrDistIdx ] ) );
		rArr[ arrColorIdx ].G = ( uint8_t )( rData.ColorData.Rand.IntU( rData.ColorData.ArrDist[ arrDistIdx + 1 ] ) );
		rArr[ arrColorIdx ].B = ( uint8_t )( rData.ColorData.Rand.IntU( rData.ColorData.ArrDist[ arrDistIdx + 2 ] ) );
		rArr[ arrColorIdx ].A = ( uint8_t )( rData.ColorData.Rand.IntU( rData.ColorData.ArrDist[ arrDistIdx + 3 ] ) );
	}

	void GenOpt0( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			InitRandColor( rData.ArrayData.ColorStart, i, 0 );

		for ( sizeT i = startIdx; i < endIdx; ++i )
			InitRandColor( rData.ArrayData.ColorEnd, i, 4 );
	}

	void GenOpt1( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			InitRandColor( rData.ArrayData.ColorStart, i, 0 );

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.ColorEnd[ i ] = rData.ColorData.ArrMinMax[ 2 ];
	}

	void GenOpt2( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.ColorStart[ i ] = rData.ColorData.ArrMinMax[ 0 ];

		for ( sizeT i = startIdx; i < endIdx; ++i )
			InitRandColor( rData.ArrayData.ColorEnd, i, 4 );
	}

	void GenOpt3( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.ColorStart[ i ] = rData.ColorData.ArrMinMax[ 1 ];

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.ColorEnd[ i ] = rData.ColorData.ArrMinMax[ 2 ];
	}

	void UpdOpt0( const sizeT alive, const float dt )
	{
		for ( sizeT i = 0; i < alive; ++i )
		{
			float step{ 1.0f / rData.ArrayData.Life[ i ] * dt };

			// Faster to first store localy and then lerp instead of passing to lerp directly by indexing the array // Measured with std::chronos
			grColor::Hsva start{ grColor::Rgba2Hsva( rData.ArrayData.ColorStart[ i ] ) };
			grColor::Hsva end{ grColor::Rgba2Hsva( rData.ArrayData.ColorEnd[ i ] ) };

			start.H = grMath::Lerp( start.H, end.H, step );
			start.S = grMath::Lerp( start.S, end.S, step );
			start.V = grMath::Lerp( start.V, end.V, step );
			start.A = grMath::Lerp( start.A, end.A, step );

			rData.ArrayData.ColorStart[ i ] = grColor::Hsva2Rgba( start );
		}
	}

	void UpdOpt1( const sizeT alive, const float dt )
	{
		for ( sizeT i = 0; i < alive; ++i )
		{
			float step{ 1.0f / rData.ArrayData.Life[ i ] * dt };

			grColor::Rgba start{ rData.ArrayData.ColorStart[ i ] };
			grColor::Rgba end{ rData.ArrayData.ColorEnd[ i ] };

			rData.ArrayData.ColorStart[ i ].R = ( uint8_t )grMath::Lerp( ( float )start.R, ( float )end.R, step );
			rData.ArrayData.ColorStart[ i ].G = ( uint8_t )grMath::Lerp( ( float )start.G, ( float )end.G, step );
			rData.ArrayData.ColorStart[ i ].B = ( uint8_t )grMath::Lerp( ( float )start.B, ( float )end.B, step );
			rData.ArrayData.ColorStart[ i ].A = ( uint8_t )grMath::Lerp( ( float )start.A, ( float )end.A, step );
		}
	}

	void Generate()
	{
		sizeT startIdx{ rData.EmitData.StartIdx }, endIdx{ rData.EmitData.EndIdx };
		( this->*GenOption )( startIdx, endIdx );
	}

	void Update()
	{
		if ( rData.ColorData.LerpEqual == EEqualValue::YES )
			return;

		float dt{ rData.EmitData.Dt };
		sizeT alive{ rData.EmitData.Alive };
		( this->*UpdOption )( alive, dt );
	}
};


struct grSScaleSystem : public grSBaseSystem
{
	grCParticleData& rData;
	GenOpt<grSScaleSystem> GenOption;
	UpdOpt<grSScaleSystem> UpdOption; // Lerp or not

	grSScaleSystem( grCParticleData& rParticleData )
		: rData( rParticleData )
		, GenOption( &grSScaleSystem::GenOpt3 )
		, UpdOption( &grSScaleSystem::UpdOpt1 )
	{}
	grSScaleSystem( const grSScaleSystem& ) = default;
	grSScaleSystem& operator=( const grSScaleSystem& ) = default;

	void Init( const grV2f& rStartMin, const grV2f& rStartMax, const grV2f& rEndMin, const grV2f& rEndMax )
	{
		rData.ScaleData.ArrMinMax[ 0 ] = rStartMin;
		rData.ScaleData.ArrMinMax[ 1 ] = rStartMax;
		rData.ScaleData.ArrMinMax[ 2 ] = rEndMin;
		rData.ScaleData.ArrMinMax[ 3 ] = rEndMax;

		EqualCheckScale( rData.ScaleData.ArrMinMax, rData.ScaleData.StartEqual, rData.ScaleData.EndEqual );

		if ( rData.ScaleData.StartEqual == EEqualValue::NO && rData.ScaleData.EndEqual == EEqualValue::NO ||
			 rData.ScaleData.StartEqual == EEqualValue::NO && rData.ScaleData.EndEqual == EEqualValue::YES ||
			 rData.ScaleData.StartEqual == EEqualValue::YES && rData.ScaleData.EndEqual == EEqualValue::NO )
		{
			rData.ScaleData.ArrDist[ 0 ] = InitDist( rData.ScaleData.ArrMinMax[ 0 ].x, rData.ScaleData.ArrMinMax[ 1 ].x );
			rData.ScaleData.ArrDist[ 1 ] = InitDist( rData.ScaleData.ArrMinMax[ 0 ].y, rData.ScaleData.ArrMinMax[ 1 ].y );
			rData.ScaleData.ArrDist[ 2 ] = InitDist( rData.ScaleData.ArrMinMax[ 2 ].x, rData.ScaleData.ArrMinMax[ 3 ].x );
			rData.ScaleData.ArrDist[ 3 ] = InitDist( rData.ScaleData.ArrMinMax[ 2 ].y, rData.ScaleData.ArrMinMax[ 3 ].y );
		}

		GenOption =
			rData.ScaleData.StartEqual == EEqualValue::NO && rData.ScaleData.EndEqual == EEqualValue::NO ?
			&grSScaleSystem::GenOpt0 :
			rData.ScaleData.StartEqual == EEqualValue::NO && rData.ScaleData.EndEqual == EEqualValue::YES ?
			&grSScaleSystem::GenOpt1 :
			rData.ScaleData.StartEqual == EEqualValue::YES && rData.ScaleData.EndEqual == EEqualValue::NO ?
			&grSScaleSystem::GenOpt2 :
			&grSScaleSystem::GenOpt3;

		UpdOption =
			rData.ScaleData.StartEqual == EEqualValue::NO || rData.ScaleData.EndEqual == EEqualValue::NO ?
			&grSScaleSystem::UpdOpt0 :
			&grSScaleSystem::UpdOpt1;
	}

	DistF InitDist( const float a, const float b )
	{
		return a < b ? rData.ScaleData.Rand.DistF( a, b ) : rData.ScaleData.Rand.DistF( b, a );
	}

	void GenOpt0( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.ScaleStart[ i ] = { rData.ScaleData.Rand.Float( rData.ScaleData.ArrDist[ 0 ] ), rData.ScaleData.Rand.Float( rData.ScaleData.ArrDist[ 1 ] ) };

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.ScaleEnd[ i ] = { rData.ScaleData.Rand.Float( rData.ScaleData.ArrDist[ 2 ] ), rData.ScaleData.Rand.Float( rData.ScaleData.ArrDist[ 3 ] ) };
	}

	void GenOpt1( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.ScaleStart[ i ] = { rData.ScaleData.Rand.Float( rData.ScaleData.ArrDist[ 0 ] ), rData.ScaleData.Rand.Float( rData.ScaleData.ArrDist[ 1 ] ) };

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.ScaleEnd[ i ] = rData.ScaleData.ArrMinMax[ 2 ];
	}

	void GenOpt2( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.ScaleStart[ i ] = rData.ScaleData.ArrMinMax[ 1 ];

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.ScaleEnd[ i ] = { rData.ScaleData.Rand.Float( rData.ScaleData.ArrDist[ 2 ] ), rData.ScaleData.Rand.Float( rData.ScaleData.ArrDist[ 3 ] ) };
	}

	void GenOpt3( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.ScaleStart[ i ] = rData.ScaleData.ArrMinMax[ 1 ];

		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.ScaleEnd[ i ] = rData.ScaleData.ArrMinMax[ 2 ];
	}

	void UpdOpt0( const sizeT alive, const float dt )
	{
		for ( sizeT i = 0; i < alive; ++i )
		{
			float step{ ( 1.0f / rData.ArrayData.Life[ i ] ) * dt };
			grV2f start{ rData.ArrayData.ScaleStart[ i ] };
			grV2f end{ rData.ArrayData.ScaleEnd[ i ] };
			rData.ArrayData.ScaleStart[ i ] = grMath::LerpV2f( start, end, step );
		}
	}

	void UpdOpt1( const sizeT alive, const float dt )
	{
		for ( sizeT i = 0; i < alive; ++i )
			rData.ArrayData.ScaleStart[ i ] = rData.ArrayData.ScaleStart[ i ];
	}

	void Generate()
	{
		sizeT startIdx{ rData.EmitData.StartIdx }, endIdx{ rData.EmitData.EndIdx };
		( this->*GenOption )( startIdx, endIdx );
	}

	void Update()
	{
		float dt{ rData.EmitData.Dt };
		sizeT alive{ rData.EmitData.Alive };
		( this->*UpdOption )( alive, dt );
	}
};


struct grSMassSystem : public grSBaseSystem
{
	grCParticleData& rData;
	GenOpt<grSMassSystem> GenOption;

	grSMassSystem( grCParticleData& rParticleData )
		: rData( rParticleData )
		, GenOption( &grSMassSystem::GenOpt1 )
	{}
	grSMassSystem( const grSMassSystem& ) = default;
	grSMassSystem& operator=( const grSMassSystem& ) = default;

	void Init( const grV2f& rMinMax )
	{
		rData.MassData.MinMax.x = grMath::Max( rMinMax.x, 1.0f );
		rData.MassData.MinMax.y = grMath::Max( rMinMax.y, 1.0f );

		SwapCheck( rData.MassData.MinMax );
		EqualCheck( rData.MassData.MinMax, rData.MassData.Equal );

		rData.MassData.Dist = rData.MassData.Rand.DistF( rData.MassData.MinMax.x, rData.MassData.MinMax.y );

		GenOption =
			rData.MassData.Equal == EEqualValue::NO ?
			&grSMassSystem::GenOpt0 :
			&grSMassSystem::GenOpt1;
	}

	void GenOpt0( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.Mass[ i ] = rData.MassData.Rand.Float( rData.MassData.Dist );
	}

	void GenOpt1( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.Mass[ i ] = rData.MassData.MinMax.x;
	}

	void Generate()
	{
		sizeT startIdx{ rData.EmitData.StartIdx }, endIdx{ rData.EmitData.EndIdx };
		( this->*GenOption )( startIdx, endIdx );
	}
};


struct grSVelocitySystem : public grSBaseSystem
{
	grCParticleData& rData;

	grSVelocitySystem( grCParticleData& rParticleData )
		: rData( rParticleData )
	{}
	grSVelocitySystem( const grSVelocitySystem& ) = default;
	grSVelocitySystem& operator=( const grSVelocitySystem& ) = default;

	void Init( const grV2f& rDegreeMinMax, const grV2f& rForceMinMax )
	{
		rData.VelocityData.DegreeMinMax = grV2f( grMath::Clamp<float>( rDegreeMinMax.x, 0.0f, 359.9f ), grMath::Clamp<float>( rDegreeMinMax.y, 0.0f, 359.9f ) );
		rData.VelocityData.ForceMinMax = rForceMinMax;

		SwapCheck( rData.VelocityData.ForceMinMax );
		EqualCheck( rData.VelocityData.DegreeMinMax, rData.VelocityData.DegreeEqual );
		EqualCheck( rData.VelocityData.ForceMinMax, rData.VelocityData.ForceEqual );
	}

	float FindDegrees()
	{
		if ( rData.VelocityData.DegreeEqual == EEqualValue::NO )
		{
			if ( rData.VelocityData.DegreeMinMax.x > rData.VelocityData.DegreeMinMax.y )
			{
				float d{ 0.0f };
				float diff{ 359.9f - rData.VelocityData.DegreeMinMax.x };
				rData.VelocityData.Dist = rData.VelocityData.Rand.DistF( 0.0f, diff + rData.VelocityData.DegreeMinMax.y );
				d = rData.VelocityData.Rand.Float( rData.VelocityData.Dist ) - diff;
				d += d < 0.0f ? 359.9f : 0.0f;
				return d;
			}

			rData.VelocityData.Dist = rData.VelocityData.Rand.DistF( rData.VelocityData.DegreeMinMax.x, rData.VelocityData.DegreeMinMax.y );
			return rData.VelocityData.Rand.Float( rData.VelocityData.Dist );
		}

		// VelData.DegreeEqual == EEqualValue::YES
		return rData.VelocityData.DegreeMinMax.x;
	}

	float FindForce()
	{
		if ( rData.VelocityData.ForceEqual == EEqualValue::NO )
		{
			auto distForce{ rData.VelocityData.Rand.DistF( rData.VelocityData.ForceMinMax.x, rData.VelocityData.ForceMinMax.y ) };
			return rData.VelocityData.Rand.Float( distForce );
		}

		// VelData.ForceEqual == EEqualValue::YES
		return rData.VelocityData.ForceMinMax.x;
	}

	void Generate()
	{
		// This is retarded but I couldn't figure out how to do it without using 6 different loops which would be better but read worse
		// It shall remain retarded for the moment(s)

		sizeT startIdx{ rData.EmitData.StartIdx }, endIdx{ rData.EmitData.EndIdx };
		for ( sizeT i = startIdx; i < endIdx; ++i )
		{
			float d{ FindDegrees() };
			float f{ FindForce() };
			grV2f v{ grMath::DegToVec( d ) * f };
			rData.ArrayData.Velocity[ i ] = v / rData.ArrayData.Mass[ i ];
		}
	}

	void Update()
	{
		float dt{ rData.EmitData.Dt };
		sizeT alive{ rData.EmitData.Alive };
		for ( sizeT i = 0; i < alive; ++i )
			rData.ArrayData.Velocity[ i ] += rData.ArrayData.Acceleration[ i ].x * dt;
	}
};


struct grSPositionSystem : public grSBaseSystem // Position system doubles as spawn shape and position update
{
	grCParticleData& rData;
	GenOpt<grSPositionSystem> GenOption;

	grSPositionSystem( grCParticleData& rParticleData )
		: rData( rParticleData )
		, GenOption( &grSPositionSystem::BoxFilledGenOpt3 )
	{}
	grSPositionSystem( const grSPositionSystem& ) = default;
	grSPositionSystem& operator=( const grSPositionSystem& ) = default;

	void InitBox( const grV2f& rBoxOffsetMin, const grV2f& rBoxOffsetMax, const float frameThickness ) // Radius == 0.0f equals filled box // Radius != 0.0f equals framed box
	{
		rData.PositionData.ArrMinMax[ 0 ] = rBoxOffsetMin;
		rData.PositionData.ArrMinMax[ 1 ] = rBoxOffsetMax;
		rData.PositionData.BoxFrameThickness = grMath::AbsF( frameThickness );

		EqualCheck( rData.PositionData.ArrMinMax, rData.PositionData.EqualBoxX, rData.PositionData.EqualBoxY );
		SwapCheck( rData.PositionData.ArrMinMax[ 0 ], rData.PositionData.ArrMinMax[ 1 ] );

		rData.PositionData.PositionType = rData.PositionData.BoxFrameThickness == 0.0f ? EPositionType::BOX_FILLED : EPositionType::BOX_FRAMED;

		if ( rData.PositionData.PositionType == EPositionType::BOX_FILLED )
		{
			if ( rData.PositionData.EqualBoxX == EEqualValue::NO && rData.PositionData.EqualBoxY == EEqualValue::NO )
			{
				GenOption = &grSPositionSystem::BoxFilledGenOpt0;
				rData.PositionData.ArrDistBox[ 0 ] = rData.PositionData.Rand.DistF( rData.PositionData.ArrMinMax[ 0 ].x, rData.PositionData.ArrMinMax[ 1 ].x );
				rData.PositionData.ArrDistBox[ 1 ] = rData.PositionData.Rand.DistF( rData.PositionData.ArrMinMax[ 0 ].y, rData.PositionData.ArrMinMax[ 1 ].y );
				return;
			}

			if ( rData.PositionData.EqualBoxX == EEqualValue::NO && rData.PositionData.EqualBoxY == EEqualValue::YES )
			{
				GenOption = &grSPositionSystem::BoxFilledGenOpt1;
				rData.PositionData.ArrDistBox[ 0 ] = rData.PositionData.Rand.DistF( rData.PositionData.ArrMinMax[ 0 ].x, rData.PositionData.ArrMinMax[ 1 ].x );
				return;
			}

			if ( rData.PositionData.EqualBoxX == EEqualValue::YES && rData.PositionData.EqualBoxY == EEqualValue::NO )
			{
				GenOption = &grSPositionSystem::BoxFilledGenOpt2;
				rData.PositionData.ArrDistBox[ 1 ] = rData.PositionData.Rand.DistF( rData.PositionData.ArrMinMax[ 0 ].y, rData.PositionData.ArrMinMax[ 1 ].y );
				return;
			}

			// rPosData.EqualBoxX == EEqualValue::YES && rPosData.EqualBoxY == EEqualValue::YES
			GenOption = &grSPositionSystem::BoxFilledGenOpt3;
			return;
		}

		// rPosData.PositionType == EPositionType::BOX_FRAMED

		// This is really stupid but it works // Commented as it's not really obvious what's going on
		// Final position is dependent of the system position, the box dimension and the potential box offset relative to the system position

		// Have to get the offset somehow so I came up with this elegant beauty
		rData.PositionData.BoxFrameOffset.x =
			rData.PositionData.ArrMinMax[ 0 ].x <= 0.0f && rData.PositionData.ArrMinMax[ 1 ].x <= 0.0f ?
			rData.PositionData.ArrMinMax[ 0 ].x - rData.PositionData.ArrMinMax[ 1 ].x :
			rData.PositionData.ArrMinMax[ 0 ].x > 0.0f && rData.PositionData.ArrMinMax[ 1 ].x > 0.0f ?
			rData.PositionData.ArrMinMax[ 1 ].x - rData.PositionData.ArrMinMax[ 0 ].x :
			rData.PositionData.ArrMinMax[ 0 ].x + rData.PositionData.ArrMinMax[ 1 ].x;

		rData.PositionData.BoxFrameOffset.y =
			rData.PositionData.ArrMinMax[ 0 ].y <= 0.0f && rData.PositionData.ArrMinMax[ 1 ].y <= 0.0f ?
			rData.PositionData.ArrMinMax[ 0 ].y - rData.PositionData.ArrMinMax[ 1 ].y :
			rData.PositionData.ArrMinMax[ 0 ].y > 0.0f && rData.PositionData.ArrMinMax[ 1 ].y > 0.0f ?
			rData.PositionData.ArrMinMax[ 1 ].y - rData.PositionData.ArrMinMax[ 0 ].y :
			rData.PositionData.ArrMinMax[ 0 ].y + rData.PositionData.ArrMinMax[ 1 ].y;

		rData.PositionData.BoxFrameOffset *= 0.5f;

		// Box frames(top, bottom, left, right) can either have a positive or a negative offset relative to the system position in either or both x and y dimensions
		// A radius for x and y is used going from the box origo outwards
		float radX =
			rData.PositionData.ArrMinMax[ 0 ].x < 0.0f ?
			grMath::AbsF( rData.PositionData.ArrMinMax[ 0 ].x - rData.PositionData.ArrMinMax[ 1 ].x ) * 0.5f :
			grMath::AbsF( rData.PositionData.ArrMinMax[ 1 ].x - rData.PositionData.ArrMinMax[ 0 ].x ) * 0.5f;

		float radY =
			rData.PositionData.ArrMinMax[ 0 ].y < 0.0f ?
			grMath::AbsF( rData.PositionData.ArrMinMax[ 0 ].y - rData.PositionData.ArrMinMax[ 1 ].y ) * 0.5f :
			grMath::AbsF( rData.PositionData.ArrMinMax[ 1 ].y - rData.PositionData.ArrMinMax[ 0 ].y ) * 0.5f;

		// Needs clamp so the box frames doesn't overflow themself x<->x and/or y<->y which will later break rand if thickness is greater then x<->x and/or y<->y
		rData.PositionData.BoxFrameThickness = radX < radY ?
			grMath::Clamp<float>( rData.PositionData.BoxFrameThickness, 0.0f, radX ) :
			grMath::Clamp<float>( rData.PositionData.BoxFrameThickness, 0.0f, radY );

		// How much the vector going out from origo should deviate in length based on frame thickness which results in the, uh, frames thickness
		// Thickness is calculated from x and y box didmensions and offsetted inwards dependent of the thickness var
		rData.PositionData.ArrDistBox[ 0 ] = rData.PositionData.Rand.DistF( radY - rData.PositionData.BoxFrameThickness, radY );
		rData.PositionData.ArrDistBox[ 1 ] = rData.PositionData.Rand.DistF( radX - rData.PositionData.BoxFrameThickness, radX );

		// Length of the frames for x and y
		// Y is modded so the corners of x and y frames doesn't overlap
		rData.PositionData.ArrDistBox[ 2 ] = rData.PositionData.Rand.DistF( -radX, radX );
		rData.PositionData.ArrDistBox[ 3 ] = rData.PositionData.Rand.DistF( -radY + rData.PositionData.BoxFrameThickness, radY - rData.PositionData.BoxFrameThickness );
		GenOption = &grSPositionSystem::BoxFramedGenOpt;
	}

	void InitCircle( const grV2f& rRadiusMinMax )
	{
		rData.PositionData.ArrMinMax[ 0 ] = rRadiusMinMax;
		EqualCheck( rData.PositionData.ArrMinMax[ 0 ], rData.PositionData.EqualCircle );
		SwapCheck( rData.PositionData.ArrMinMax[ 0 ] );

		GenOption =
			rData.PositionData.EqualCircle == EEqualValue::NO ?
			&grSPositionSystem::CircleGenOption0 :
			&grSPositionSystem::CircleGenOption1;
	}

	void CircleGenOption0( const sizeT startIdx, const sizeT endIdx )
	{
		grV2f sysPos{ rData.EmitData.SystemPosition };
		DistF distRad{ rData.PositionData.Rand.DistF( rData.PositionData.ArrMinMax[ 0 ] ) };
		DistF distDeg{ rData.PositionData.Rand.DistF( 0.0f, 359.9f ) };
		for ( sizeT i = startIdx; i < endIdx; ++i )
		{
			grV2f v{ grMath::DegToVec( rData.PositionData.Rand.Float( distDeg ) ) };
			v *= rData.PositionData.Rand.Float( distRad );
			rData.ArrayData.Position[ i ] = v + sysPos;
		}
	}

	void CircleGenOption1( const sizeT startIdx, const sizeT endIdx )
	{
		grV2f sysPos{ rData.EmitData.SystemPosition };
		DistF distDeg{ rData.PositionData.Rand.DistF( 0.0f, 359.9f ) };
		for ( sizeT i = startIdx; i < endIdx; ++i )
		{
			grV2f v{ grMath::DegToVec( rData.PositionData.Rand.Float( distDeg ) ) };
			v *= rData.PositionData.ArrMinMax[ 0 ].x;
			rData.ArrayData.Position[ i ] = v + sysPos;
		}
	}


// position = Vector2( centerPos.x + ( radX * Mathf.Sin( Mathf.Deg2Rad * alpha ) ),
// 					   centerPos.y + ( radY * Mathf.Cos( Mathf.Deg2Rad * alpha ) ) );

// alpha += 5f;//can be used as speed

// position = Vector2( centerPos.x + ( radX * MCos( alpha ) * MCos( tilt ) ) - ( radY * MSin( alpha ) * MSin( tilt ) ),
// 					   centerPos.y + ( radX * MCos( alpha ) * MSin( tilt ) ) + ( radY * MSin( alpha ) * MCos( tilt ) ) );
// alpha += 5f;


	void BoxFilledGenOpt0( const sizeT startIdx, const sizeT endIdx )
	{
		grV2f sysPos{ rData.EmitData.SystemPosition };
		for ( sizeT i = startIdx; i < endIdx; ++i )
		{
			grV2f v{ rData.PositionData.Rand.Float( rData.PositionData.ArrDistBox[ 0 ] ), rData.PositionData.Rand.Float( rData.PositionData.ArrDistBox[ 1 ] ) };
			rData.ArrayData.Position[ i ] = v + sysPos;
		}
	}

	void BoxFilledGenOpt1( const sizeT startIdx, const sizeT endIdx )
	{
		grV2f v{ grV2f( 0.0f, rData.PositionData.ArrMinMax[ 0 ].y ) + rData.EmitData.SystemPosition };
		for ( sizeT i = startIdx; i < endIdx; ++i )
		{
			v.x += rData.PositionData.Rand.Float( rData.PositionData.ArrDistBox[ 0 ] );
			rData.ArrayData.Position[ i ] = v;
		}
	}

	void BoxFilledGenOpt2( const sizeT startIdx, const sizeT endIdx )
	{
		grV2f v{ grV2f( rData.PositionData.ArrMinMax[ 0 ].x, 0.0f ) + rData.EmitData.SystemPosition };
		for ( sizeT i = startIdx; i < endIdx; ++i )
		{
			v.y += rData.PositionData.Rand.Float( rData.PositionData.ArrDistBox[ 1 ] );
			rData.ArrayData.Position[ i ] = v;
		}
	}

	void BoxFilledGenOpt3( const sizeT startIdx, const sizeT endIdx )
	{
		grV2f v{ grV2f( rData.PositionData.ArrMinMax[ 0 ] ) + rData.EmitData.SystemPosition };
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.Position[ i ] = v;
	}

	void BoxFramedGenOpt( const sizeT startIdx, const sizeT endIdx )
	{
		grV2f sysPos{ rData.EmitData.SystemPosition };
		for ( sizeT i = startIdx; i < endIdx; ++i )
		{
			rData.PositionData.BoxFrameDegAcc = rData.PositionData.BoxFrameDegAcc >= 270.0f ? 0.0f : rData.PositionData.BoxFrameDegAcc + 90.0f;

			grV2f frame =
				rData.PositionData.BoxFrameDegAcc == 0.0f ?
				( rData.PositionData.BoxOrigoSN * -1.0f * rData.PositionData.Rand.Float( rData.PositionData.ArrDistBox[ 0 ] ) ) +
				( rData.PositionData.BoxOrigoEW * rData.PositionData.Rand.Float( rData.PositionData.ArrDistBox[ 2 ] ) ) :
				rData.PositionData.BoxFrameDegAcc == 90.0f ?
				( rData.PositionData.BoxOrigoEW * rData.PositionData.Rand.Float( rData.PositionData.ArrDistBox[ 1 ] ) ) +
				( rData.PositionData.BoxOrigoSN * rData.PositionData.Rand.Float( rData.PositionData.ArrDistBox[ 3 ] ) ) :
				rData.PositionData.BoxFrameDegAcc == 180.0f ?
				( rData.PositionData.BoxOrigoSN * rData.PositionData.Rand.Float( rData.PositionData.ArrDistBox[ 0 ] ) ) +
				( rData.PositionData.BoxOrigoEW * rData.PositionData.Rand.Float( rData.PositionData.ArrDistBox[ 2 ] ) ) :
				( rData.PositionData.BoxOrigoEW * -1.0f * rData.PositionData.Rand.Float( rData.PositionData.ArrDistBox[ 1 ] ) ) +
				( rData.PositionData.BoxOrigoSN * rData.PositionData.Rand.Float( rData.PositionData.ArrDistBox[ 3 ] ) );

			rData.ArrayData.Position[ i ] = rData.PositionData.BoxFrameOffset + frame + sysPos;
		}
	}

	void Generate()
	{
		sizeT startIdx{ rData.EmitData.StartIdx }, endIdx{ rData.EmitData.EndIdx };
		( this->*GenOption )( startIdx, endIdx );
	}

	void Update()
	{
		float dt{ rData.EmitData.Dt };
		sizeT alive{ rData.EmitData.Alive };
		for ( sizeT i = 0; i < alive; ++i )
			rData.ArrayData.Position[ i ] += rData.ArrayData.Velocity[ i ] * dt;
	}
};


struct grSLifeSystem : public grSBaseSystem
{
	grCParticleData& rData;
	GenOpt<grSLifeSystem> GenOption;

	grSLifeSystem( grCParticleData& rParticleData )
		: rData( rParticleData )
		, GenOption( &grSLifeSystem::GenOpt1 )
	{}
	grSLifeSystem( const grSLifeSystem& ) = default;
	grSLifeSystem& operator=( const grSLifeSystem& ) = default;

	void Init( const grV2f& rMinMax )
	{
		rData.LifeData.MinMax = rMinMax;

		SwapCheck( rData.LifeData.MinMax );
		EqualCheck( rData.LifeData.MinMax, rData.LifeData.Equal );

		rData.LifeData.Dist = rData.LifeData.Rand.DistF( rData.LifeData.MinMax.x, rData.LifeData.MinMax.y );

		GenOption =
			rData.LifeData.Equal == EEqualValue::NO ?
			&grSLifeSystem::GenOpt0 :
			&grSLifeSystem::GenOpt1;
	}

	void Kill( const sizeT nowIdx, const sizeT last )
	{
		//grAlgo::Swap( Array.puVerts[ nowIdx ], Array.puVerts[ last ] );
		grAlgo::Swap<grColor::Rgba>( rData.ArrayData.ColorStart[ nowIdx ], rData.ArrayData.ColorStart[ last ] );
		grAlgo::Swap<grColor::Rgba>( rData.ArrayData.ColorEnd[ nowIdx ], rData.ArrayData.ColorEnd[ last ] );
		grAlgo::Swap<grV2f>( rData.ArrayData.ScaleStart[ nowIdx ], rData.ArrayData.ScaleStart[ last ] );
		grAlgo::Swap<grV2f>( rData.ArrayData.ScaleEnd[ nowIdx ], rData.ArrayData.ScaleEnd[ last ] );
		grAlgo::Swap<float>( rData.ArrayData.Mass[ nowIdx ], rData.ArrayData.Mass[ last ] );
		grAlgo::Swap<grV2f>( rData.ArrayData.Acceleration[ nowIdx ], rData.ArrayData.Acceleration[ last ] );
		grAlgo::Swap<grV2f>( rData.ArrayData.Velocity[ nowIdx ], rData.ArrayData.Velocity[ last ] );
		grAlgo::Swap<grV2f>( rData.ArrayData.Position[ nowIdx ], rData.ArrayData.Position[ last ] );
		grAlgo::Swap<float>( rData.ArrayData.Life[ nowIdx ], rData.ArrayData.Life[ last ] );

		// Most values are generated and set by operator= and does not need zeroing (all above)
		// Values that are generated and set by operator+= needs resetting (all below) with the exception of position
		rData.ArrayData.Acceleration[ last ] = { 0.0f, 0.0f };
	}

	void GenOpt0( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.Life[ i ] = rData.LifeData.Rand.Float( rData.LifeData.Dist );
	}

	void GenOpt1( const sizeT startIdx, const sizeT endIdx )
	{
		for ( sizeT i = startIdx; i < endIdx; ++i )
			rData.ArrayData.Life[ i ] = rData.LifeData.MinMax.x;
	}

	void Generate()
	{
		sizeT startIdx{ rData.EmitData.StartIdx }, endIdx{ rData.EmitData.EndIdx };
		( this->*GenOption )( startIdx, endIdx );
	}

	void Update()
	{
		for ( sizeT i = 0; i < rData.EmitData.Alive; ++i )
		{
			rData.ArrayData.Life[ i ] -= rData.EmitData.Dt;;
			if ( rData.ArrayData.Life[ i ] <= 0.0f )
				Kill( i, --rData.EmitData.Alive );
		}
	}
};


class grCParticleSystem
{
public:
	grCParticleSystem( grCParticleData& rParticleData )
		: EmitSystem( rParticleData )
		, ColorSystem( rParticleData )
		, ScaleSystem( rParticleData )
		, MassSystem( rParticleData )
		, VelocitySystem( rParticleData )
		, PositionSystem( rParticleData )
		, LifeSystem( rParticleData )
		, rData( rParticleData )
	{}
	grCParticleSystem( const grCParticleSystem& ) = delete;
	grCParticleSystem& operator=( const grCParticleSystem& ) = delete;
	grCParticleSystem( grCParticleSystem&& ) noexcept = delete;
	grCParticleSystem& operator=( grCParticleSystem&& ) noexcept = delete;

	void Run()
	{
		grStruct::grSTimer t( "Run", grStruct::grSTimer::ETimeType::NS );
		Generate();
		Update();
	}

	grSEmitSystem EmitSystem;
	grSColorSystem ColorSystem;
	grSScaleSystem ScaleSystem;
	grSMassSystem MassSystem;
	grSVelocitySystem VelocitySystem;
	grSPositionSystem PositionSystem;
	grSLifeSystem LifeSystem;

private:
	void Generate()
	{
		EmitSystem.Generate();
		if ( rData.EmitData.EmitAcc > 0 )
		{
			ColorSystem.Generate();
			ScaleSystem.Generate();
			MassSystem.Generate();
			VelocitySystem.Generate();
			PositionSystem.Generate();
			LifeSystem.Generate();
		}
	}

	void Update()
	{
		if ( rData.EmitData.Alive > 0 )
		{
			ColorSystem.Update();
			ScaleSystem.Update();
			VelocitySystem.Update();
			PositionSystem.Update();
			LifeSystem.Update();
		}
	}

	grCParticleData& rData;
};

#endif // _H_GRPARTICLESYSTEM_