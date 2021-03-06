#ifndef		_GRMAP_H_
#define		_GRMAP_H_

#include	"grCommon.h"
#include	"grV2.h"

class		grNavMesh;
class		grNavNode;


// grMap
//////////////////////////////////////////////////
class grMap
{
private:

	struct sMapData
	{
		str			Name;
		str			StrMap;
		grV2f		TileSize;
		intU		TilesX;
		intU		TilesY;
	};
	
	//////////////////////////////////////////////////

public:

				grMap			( const str& uniqueName,
								const str& strMap,
								const grV2f& tileSize,
								const intU numTilesX,
								const intU numTilesY );

				~grMap			( void );
	
	//////////////////////////////////////////////////
	
	sMapData*	GetMapData		( void )					{ return m_pMapData; }
	grNavMesh*	GetNavMesh		( void )					{ return m_pNavMesh; }
	
	/////////////////////////////////////////////////
	
	void		CreateNavMesh	( void );
	grNavNode*	FindValidPos	( const grV2f& pos );
	
	//////////////////////////////////////////////////

private:

	sMapData*	m_pMapData;
	grNavMesh*	m_pNavMesh;

};

#endif		//_GRMAP_H_

