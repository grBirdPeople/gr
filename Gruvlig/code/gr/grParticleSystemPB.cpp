#include	"grParticleSystemPB.h"

#include    "grRandom.h"
#include	"grV2.h"
//#include    "grParticleManagerPB.h"
#include    "grParticlePB.h"
#include    "grAlgo.h"


// cTor
//////////////////////////////////////////////////
grParticleSystemPB::grParticleSystemPB( const uInt id, const uInt partQuantity )
    : uPArrDeactivateId ( new uInt[ partQuantity ] )
    , uPRand            ( new grRandom )
    , Id                ( id )
{}


// dTor
//////////////////////////////////////////////////
grParticleSystemPB::~grParticleSystemPB( void )
{
    if ( uPArrDeactivateId != nullptr )
        delete[] uPArrDeactivateId.release();

    if( uPRand  != nullptr)
        delete uPRand.release();
}


// Activate
//////////////////////////////////////////////////
void
grParticleSystemPB::Activate( SParticleEmitter& rPartBlock, const float deltaT )
{
    rPartBlock.SpawnCounter -= deltaT;
    if ( rPartBlock.SpawnCounter <= 0.0f )
    {
        rPartBlock.SpawnCounter += rPartBlock.SpawnInMilliSec;
        if ( rPartBlock.PartActive >= rPartBlock.PartSize )
            return;

        // Think writing data should maybe be done elsewhere
        {
            grParticlePB& rPart = *rPartBlock.uPArrParticle[ rPartBlock.PartActive ];
            grParticlAttributePB& rAtt = *rPartBlock.uPAttribute.get();
            rPart.Color = rAtt.Color;
            rPart.Position = rAtt.Position;
            rPart.Velocity = rAtt.Velocity;
            rPart.Lifetime = rAtt.Lifetime;
        }

        ++rPartBlock.PartActive;
    }


    {
        //grParticleAttributePB& rAttribute = *rParticleSetup.pAttribute;
        //rAttribute.SpawnCounter -= fixedT;
        //if ( rAttribute.SpawnCounter < 0.0f )
        //{
        //    rAttribute.SpawnCounter += rAttribute.SpawnInMilliSec;
        //    if ( rParticleSetup.ParticlesActive >= PARTICLE_PER_SETUP )
        //        return;

        //    grParticlePB& rPart = *rParticleSetup.arrParticle[ rParticleSetup.ParticlesActive ].get();

        //    rPart.Position = rAttribute.Position;
        //    rPart.Mass = rAttribute.Mass;


        //    // Speed
        //    float speed = 0.0f;
        //    if ( rAttribute.bSpeedRange == true )
        //    {
        //        speed = pRand->Float( rAttribute.SpeedRange.x, rAttribute.SpeedRange.y );
        //    }
        //    else
        //    {
        //        speed = rAttribute.SpeedRange.x;
        //    }

        //    // Direction
        //    grV2f dir = grV2f( 0.0f, -1.0f );
        //    if ( rAttribute.bDirectionRange == true )
        //    {
        //        grV2f range = rAttribute.DirectionRange;
        //        float randDeg = 0.0f;
        //        if ( range.x > range.y )
        //        {
        //            range.x = grMath::Abs( range.x - 360.0f );
        //            randDeg = range.x + range.y;
        //            randDeg = pRand->Float( 0.0f, randDeg );
        //            randDeg -= range.x;
        //        }
        //        else
        //        {
        //            randDeg = range.y - range.x;
        //            randDeg = pRand->Float( 0.0f, randDeg );
        //            randDeg += range.x;
        //        }

        //        grMath::RotatePoint( &dir, randDeg * grMath::DegToRad );
        //    }
        //    else
        //    {
        //        grMath::RotatePoint( &dir, rAttribute.DirectionRange.x * grMath::DegToRad );
        //    }

        //    // Lifetime
        //    if ( rAttribute.bLifetimeRange == true )
        //    {
        //        rPart.Lifetime = pRand->Float( rAttribute.LifetimeRange.x, rAttribute.LifetimeRange.y );
        //    }
        //    else
        //    {
        //        rPart.Lifetime = rAttribute.LifetimeRange.x;
        //    }

        //    // Accelerations
        //    rPart.Acceleration += ( dir * speed ) / rPart.Mass;


        //    ++rParticleSetup.ParticlesActive;
        //}
    }
}


// Update
//////////////////////////////////////////////////
void
grParticleSystemPB::Update( SParticleEmitter& rPartBlock, const float deltaT )
{
    grParticlAttributePB& rAtt = *rPartBlock.uPAttribute.get();
    for ( sizeT idx = 0; idx < rPartBlock.PartActive; ++idx )
    {
        rPartBlock.uPArrParticle[ idx ]->Lifetime -= deltaT;
        if ( rPartBlock.uPArrParticle[ idx ]->Lifetime <= grMath::Epsilon )
        {
            rPartBlock.uPPartDeactivateQue->Push( idx );
            continue;
        }

        rPartBlock.uPArrParticle[ idx ]->Position += rPartBlock.uPArrParticle[ idx ]->Velocity * deltaT;
    }





    {
        //grParticleAttributePB& rAttribute = *rParticleSetup.pAttribute.get();
        //for ( sizeT idx = 0; idx < rParticleSetup.ParticlesActive; ++idx )
        //{
        //    grParticlePB& rPart = *rParticleSetup.arrParticle[ idx ].get();


        //    if ( rAttribute.bSpeedOsc == true )
        //    {
        //        if ( rPart.Velocity.x != NULL && rPart.Velocity.y != NULL )
        //        {
        //            grV2f norm = rPart.Velocity.Normalized();
        //            rPart.Acceleration += ( norm * rAttribute.SpeedOsc ) / rPart.Mass;
        //        }
        //    }



        //    rPart.Acceleration += rAttribute.Gravity / rPart.Mass;

        //    rPart.Velocity += rPart.Acceleration * fixedT;
        //    rPart.Position += rPart.Velocity * fixedT;

        //    rPart.Lifetime -= fixedT;
        //    rPart.Acceleration = grV2f();

        //    // TEST
        //    //printf( "%s %d \n", "Particles Active: ", rParticleSetup.ParticlesActive );
        //    grBBox box( grV2f( 20.0f, 20.0f ), rPart.Position );
        //    grDebugManager::Instance().AddBBox( box, sf::Color::White );
        //    // TEST
        //}
    }
}


// Deactivate
//////////////////////////////////////////////////
void
grParticleSystemPB::Deactivate( SParticleEmitter& rPartBlock )
{
    if ( rPartBlock.uPPartDeactivateQue->GetQuantity() > 0 )
    {
        uInt size = rPartBlock.uPPartDeactivateQue->GetQuantity();
        for ( uInt i = 0; i < size; ++i )
            uPArrDeactivateId[ i ] = rPartBlock.uPPartDeactivateQue->Pull();

        grAlgo::InsrtSort<uInt>( uPArrDeactivateId.get(), size );

        for ( uInt idx = 0; idx < size; ++idx )
        {
            grParticlePB& rTooPart = *rPartBlock.uPArrParticle[ uPArrDeactivateId[ idx ] ];
            grParticlePB& rFromPart = *rPartBlock.uPArrParticle[ rPartBlock.PartActive - 1 ];

            rTooPart = rFromPart;

            // Should not need this
            //rFromPart.Position = 0.0f;
            //rFromPart.Velocity = 0.0f;
            //rFromPart.Lifetime = 0.0f;

            --rPartBlock.PartActive;
        }
    }



    

    {
        //sInt active = ( sInt )rParticleSetup.ParticlesActive;
        //for ( sInt idx = 0; idx < active; ++idx )
        //{
        //    if ( rParticleSetup.arrParticle[ idx ]->Lifetime < 0.0f )
        //    {
        //        grParticlePB& rFrom = *rParticleSetup.arrParticle[ --active ].get();
        //        grParticlePB& rToo = *rParticleSetup.arrParticle[ idx ].get();

        //        rToo.Position = rFrom.Position;
        //        rToo.Velocity = rFrom.Velocity;
        //        rToo.Mass = rFrom.Mass;
        //        rToo.Lifetime = rFrom.Lifetime;

        //        // TODO: This reset block should not be needed but without it things blow up. Investigate more.
        //        // Hint: If idx equals active it writes to itself
        //        rFrom.Position = grV2f();
        //        rFrom.Velocity = grV2f();
        //        rFrom.Mass = 0.0f;
        //        rFrom.Lifetime = 0.0f;

        //        --idx;
        //    }
        //}
        //rParticleSetup.ParticlesActive = active;
    }
}