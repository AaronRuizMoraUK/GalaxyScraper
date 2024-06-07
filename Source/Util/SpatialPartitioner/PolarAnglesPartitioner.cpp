#include "PolarAnglesPartitioner.h"
#include "Managers/PlanetManager.h"
#include "Object/Planet/Planet.h"
#include "Global/GlobalFunctions.h"
#include "Object/Object.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"

// Spatial Partition Dimensions
#define NUM_GRIDS_Y 10
#define NUM_GRIDS_X 20

PolarAnglesPartitioner * PolarAnglesPartitioner::polarAnglesPartitioner = NULL;

PolarAnglesPartitioner * PolarAnglesPartitioner::getPolarAnglesPartitioner() {
	if(polarAnglesPartitioner == NULL)
		polarAnglesPartitioner = new PolarAnglesPartitioner();

	return polarAnglesPartitioner;
}

PolarAnglesPartitioner::PolarAnglesPartitioner() :
ISpatialPartitioner< PAGridIndex >(),
gridsNumberY(NUM_GRIDS_Y), 
gridsNumberX(NUM_GRIDS_X)
{
	space = new Grid *[gridsNumberY];
	for(int i=0; i<gridsNumberY; ++i)
		space[i] = new Grid [gridsNumberX];
}

PolarAnglesPartitioner::~PolarAnglesPartitioner()
{
	OutputDebugString("Destroying PolarAnglesPartitioner...\n");

	cleanSpace();

	if( space ) {
		for(int i=0; i<gridsNumberY; ++i)
			delete [] space[i];
		delete []space;
	}
}

void PolarAnglesPartitioner::cleanSpace() {
	staticObjectsInSpace.clear();
	movableObjectsInSpace.clear();
	for(int i=0; i<gridsNumberY; ++i) {
		for(int j=0; j<gridsNumberX; ++j) {
			space[i][j].movableObjectSet.clear();
			space[i][j].staticObjectSet.clear();
		}
	}
}

bool PolarAnglesPartitioner::addStaticObject(Object *staticObject)
{
	/*bool registered = isRegistered(staticObject);
	if (registered)
	return false;*/

	std::vector<PAGridIndex> gridsIndex;
	whichGridsIndexWouldBe(staticObject, gridsIndex);

	if ( gridsIndex.empty() ) 
		return false;

	// Insert static object in space
	for(unsigned int numGrid=0; numGrid<gridsIndex.size(); ++numGrid) {
		// Get the grid
		int i = gridsIndex[numGrid][0];
		int j = gridsIndex[numGrid][1];

		space[i][j].staticObjectSet.insert(staticObject);
	}

	// Insert static object in the list
	staticObjectsInSpace[staticObject] = gridsIndex;

	return true;
}

bool PolarAnglesPartitioner::addMovableObject(IntelligentObject *movableObject)
{
	/*bool registered = isRegistered(movableObject);
	if (registered)
	return false;*/

	PAGridIndex gridsIndex;
	whichGridsIndexWouldBe(movableObject, gridsIndex);

	if( gridsIndex.empty() )
		return false;

	// Get the grid
	int i = gridsIndex[0];
	int j = gridsIndex[1];

	// Insert movable object in space
	space[i][j].movableObjectSet.insert(movableObject);

	// Insert movable object in the list
	movableObjectsInSpace[movableObject] = gridsIndex;

	// Update movable object
	movableObject->setSpatialGrid( gridsIndex );

	return true;
}

void PolarAnglesPartitioner::removeStaticObject(Object *staticObject)
{
	/*bool registered = isRegistered(staticObject);
	if (!registered)
	return;*/

	std::vector<PAGridIndex> gridsIndex;
	whereIs(staticObject, STATIC, gridsIndex);

	if(gridsIndex.empty())
		return;

	// Remove static object from space
	for(unsigned int numGrid=0; numGrid<gridsIndex.size(); ++numGrid) {
		int i = gridsIndex[numGrid][0];
		int j = gridsIndex[numGrid][1];

		space[i][j].staticObjectSet.erase(staticObject);
	}

	// Remove static object from list
	staticObjectsInSpace.erase(staticObject);
}

void PolarAnglesPartitioner::removeMovableObject(IntelligentObject *movableObject)
{
	/*bool registered = isRegistered(movableObject);
	if (!registered)
	return;*/

	PAGridIndex gridsIndex;
	whereIs(movableObject, gridsIndex);

	// If coordinates is not inside, return
	if( gridsIndex.size() != 2 )
		return;

	int i = gridsIndex[0];
	int j = gridsIndex[1];

	// Remove movable object from space
	space[i][j].movableObjectSet.erase(movableObject);

	// Remove movable object from list
	movableObjectsInSpace.erase(movableObject);
}

void PolarAnglesPartitioner::moveMovableObject(IntelligentObject *movableObject)
{
	// Get in which grid it is
	PAGridIndex gridsIndex;
	whereIs(movableObject, gridsIndex);

	// If coordinates is not inside, return
	if( gridsIndex.size() != 2 )
		return;

	moveMovableObject(movableObject, gridsIndex);
}

void PolarAnglesPartitioner::updateSpace() {

	// Update movable objects in space
	MovableObjectsInSpaceMap::iterator it = movableObjectsInSpace.begin();
	while( it != movableObjectsInSpace.end() ) {
		moveMovableObject(it->first, it->second);
		++it;
	}

	// Update player in space
	PAGridIndex gridIndex;
	Player *player = Player::getPlayer();
	whichGridsIndexWouldBe(player, gridIndex);
	player->setSpatialGrid(gridIndex);
}

void PolarAnglesPartitioner::whereIs(Object *object, TypeSeek typeSeek, std::vector<PAGridIndex> &output) const
{
	switch (typeSeek) {

		case STATIC: {
			StaticObjectsInSpaceMap::const_iterator it = staticObjectsInSpace.find(object);
			if( it != staticObjectsInSpace.end() ) {
				const std::vector<PAGridIndex> &gridsIndex= it->second;
				for( unsigned int i=0; i<gridsIndex.size(); ++i)
					output.push_back( gridsIndex[i] );
			}
			break;
					 }
		case MOVABLE: {
			assert( (IntelligentObject *)object );
			MovableObjectsInSpaceMap::const_iterator it = movableObjectsInSpace.find( (IntelligentObject *)object );
			if( it != movableObjectsInSpace.end() ) {
				const PAGridIndex &gridIndex = it->second;
				output.push_back( gridIndex );
			}
			break;
					  }
		case ANY: {
			bool isMovable=true;

			// Look for the object in static object list
			StaticObjectsInSpaceMap::const_iterator itStatic = staticObjectsInSpace.find(object);
			if( itStatic != staticObjectsInSpace.end() ) {
				const std::vector<PAGridIndex> &gridsIndex= itStatic->second;
				for( unsigned int i=0; i<gridsIndex.size(); ++i)
					output.push_back( gridsIndex[i] );
				isMovable = false; // It is an static object, not movable
			}

			if(isMovable) {
				// Look for the object in movable object list
				assert( (IntelligentObject *)object );
				MovableObjectsInSpaceMap::const_iterator itMoveble = movableObjectsInSpace.find( (IntelligentObject *)object );
				if( itMoveble != movableObjectsInSpace.end() ) {
					const PAGridIndex &gridIndex = itMoveble->second;
					output.push_back( gridIndex );
				}
			}
			break;
				  }
		default:
			assert(!"typeSeek no supported in PolarAnglesPartitioner"); // Warn if typeSeek is not any of the other cases
			return;
			break;
	}
}

void PolarAnglesPartitioner::whereIs(IntelligentObject *movableObject, PAGridIndex &output) const
{
	MovableObjectsInSpaceMap::const_iterator it = movableObjectsInSpace.find(movableObject);
	if( it != movableObjectsInSpace.end() ) {
		const PAGridIndex &gridIndex = it->second;
		output.push_back( gridIndex[0] );
		output.push_back( gridIndex[1] );
	}
}

void PolarAnglesPartitioner::whichGridsIndexWouldBe(Object *object, std::vector<PAGridIndex> &output) const
{
	// Current position of object
	const D3DXVECTOR3 &point = object->getPosition();

	// Current planet center
	const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();

	// Obtain the direction vector
	D3DXVECTOR3 direction = point - center;
	Global::normalizeVector3(direction, direction);

	// Obtain in Planet Local Coordinates from Global Coordinates of the direction
	const D3DXMATRIX & transformMatrix = planet->getTransformMatrix();
	D3DXMATRIX inverseTransform;
	D3DXMatrixInverse(&inverseTransform, NULL, &transformMatrix);

	D3DXVec3TransformNormal(&direction, &direction, &inverseTransform);

	// Obtain alpha and delta angles from local point
	float delta = asin( direction.y );
	float alpha = atan2( direction.x, direction.z );
	if(alpha<0)
		alpha += 2.0f*D3DX_PI;

	// Normalize alpha and delta angles to height map matrix dimensions
	float coordDelta = 0.0f;
	float coordAlpha = 0.0f;
	Global::normalize(coordDelta, delta, 0.0f, gridsNumberY, -D3DX_PI/2.0f, D3DX_PI/2.0f);
	Global::normalize(coordAlpha, alpha, 0.0f, gridsNumberX, 0.0f, 2.0f*D3DX_PI);

	coordDelta = Global::clamp(coordDelta, 0.0f, gridsNumberY-1.0f);
	coordAlpha = Global::clamp(coordAlpha, 0.0f, gridsNumberX-1.0f);

	// Obtain the space coordinates
	int coorY = floor(coordDelta);
	int coorX = floor(coordAlpha);

	PAGridIndex gridIndex;
	gridIndex.push_back(coorY);
	gridIndex.push_back(coorX);

	output.push_back(gridIndex);
}

void PolarAnglesPartitioner::whichGridsIndexWouldBe(AnimatedObject *object, PAGridIndex &output) const
{
	// Obtain alpha and delta angles from object
	const float delta = object->getDelta();
	const float alpha = object->getAlpha();

	// Normalize alpha and delta angles to height map matrix dimensions
	float coordDelta = 0.0f;
	float coordAlpha = 0.0f;
	Global::normalize(coordDelta, delta, 0.0f, gridsNumberY, -D3DX_PI/2.0f, D3DX_PI/2.0f);
	Global::normalize(coordAlpha, alpha, 0.0f, gridsNumberX, 0.0f, 2.0f*D3DX_PI);

	coordDelta = Global::clamp(coordDelta, 0.0f, gridsNumberY-1.0f);
	coordAlpha = Global::clamp(coordAlpha, 0.0f, gridsNumberX-1.0f);

	// Obtain the space coordinates
	int coorY = floor(coordDelta);
	int coorX = floor(coordAlpha);

	output.push_back(coorY);
	output.push_back(coorX);
}

void PolarAnglesPartitioner::getObjectsInGridIndex(const PAGridIndex &gridIndex, std::set<Object *> &output) const
{
	// If coordinates is not inside, return
	if( gridIndex.size() != 2 )
		return;

	int i = gridIndex[0];
	int j = gridIndex[1];

	if( i<0 || j<0 || i>gridsNumberY-1 || j>gridsNumberX-1 )
		return;	

	// If grid is not any pole
	if( i!=0 && i!=gridsNumberY-1 ) {
		std::set<Object *>::const_iterator itStatic = space[i][j].staticObjectSet.begin();
		while( itStatic != space[i][j].staticObjectSet.end() ) {
			output.insert(*itStatic);

			++itStatic;
		}

		std::set<IntelligentObject *>::const_iterator itMovable = space[i][j].movableObjectSet.begin();
		while( itMovable != space[i][j].movableObjectSet.end() ) {
			output.insert(*itMovable);

			++itMovable;
		}
	}
	else {
		// If it is in a pole, one pole is considered a unic grid
		for(int jPole=0; jPole<gridsNumberX; ++jPole) {
			std::set<Object *>::const_iterator itStatic = space[i][jPole].staticObjectSet.begin();
			while( itStatic != space[i][jPole].staticObjectSet.end() ) {
				output.insert( *itStatic );

				++itStatic;
			}

			std::set<IntelligentObject *>::const_iterator itMovable = space[i][jPole].movableObjectSet.begin();
			while( itMovable != space[i][jPole].movableObjectSet.end() ) {
				output.insert( *itMovable );

				++itMovable;
			}
		}
	}
}

void PolarAnglesPartitioner::getObjectsInSameGrid(IntelligentObject *object, std::set<Object *> &output) const
{
	PAGridIndex gridIndex;
	whereIs(object, gridIndex);

	if( gridIndex.size() != 2 )
		return;

	getObjectsInGridIndex(gridIndex, output);
}

void PolarAnglesPartitioner::getAdjacentObjects(const PAGridIndex &gridIndex, std::set<Object *> &output) const
{
	// First the own grid
	getObjectsInGridIndex(gridIndex, output);

	// Now the adjacent grids
	std::vector<PAGridIndex> adjacentGridIndex;
	getAdjacentGridsIndex(gridIndex, adjacentGridIndex);

	for(unsigned int numGrids=0; numGrids<adjacentGridIndex.size(); ++numGrids) {
		int i = adjacentGridIndex[numGrids][0];
		int j = adjacentGridIndex[numGrids][1];

		// If grid has not Pole grids adjancent
		if( i!=0 && i!=gridsNumberY-1 ) {
			getObjectsInGridIndex(adjacentGridIndex[numGrids], output);
		}
		else {
			// If grid has Pole grids adjancent, the pole in this case is not a unique grid
			std::set<Object *>::const_iterator itStatic = space[i][j].staticObjectSet.begin();
			while( itStatic != space[i][j].staticObjectSet.end() ) {
				output.insert(*itStatic);

				++itStatic;
			}

			std::set<IntelligentObject *>::const_iterator itMovable = space[i][j].movableObjectSet.begin();
			while( itMovable != space[i][j].movableObjectSet.end() ) {
				output.insert(*itMovable);

				++itMovable;
			}
		}
	}
}

void PolarAnglesPartitioner::getAdjacentObjectsTo(IntelligentObject *object, std::set<Object *> &output) const
{
	PAGridIndex gridIndex;
	whereIs(object, gridIndex);

	if( gridIndex.size() != 2 )
		return;

	getAdjacentObjects(gridIndex, output);
}

void PolarAnglesPartitioner::getMovableObjectsInGridIndex(const PAGridIndex &gridIndex, std::set<IntelligentObject *> &output) const
{
	// If coordinates is not inside, return
	if( gridIndex.size() != 2 )
		return;

	int i = gridIndex[0];
	int j = gridIndex[1];

	if( i<0 || j<0 || i>gridsNumberY-1 || j>gridsNumberX-1 )
		return;	

	// If grid is not any pole
	if( i!=0 && i!=gridsNumberY-1 ) {

		std::set<IntelligentObject *>::const_iterator itMovable = space[i][j].movableObjectSet.begin();
		while( itMovable != space[i][j].movableObjectSet.end() ) {
			output.insert(*itMovable);

			++itMovable;
		}
	}
	else {
		// If it is in a pole, one pole is considered a unic grid
		for(int jPole=0; jPole<gridsNumberX; ++jPole) {

			std::set<IntelligentObject *>::const_iterator itMovable = space[i][jPole].movableObjectSet.begin();
			while( itMovable != space[i][jPole].movableObjectSet.end() ) {
				output.insert( *itMovable );

				++itMovable;
			}
		}
	}
}

void PolarAnglesPartitioner::getMovableObjectsInSameGrid(IntelligentObject *object, std::set<IntelligentObject *> &output) const
{
	PAGridIndex gridIndex;
	whereIs(object, gridIndex);

	if( gridIndex.size() != 2 )
		return;

	getMovableObjectsInGridIndex(gridIndex, output);
}

void PolarAnglesPartitioner::getAdjacentMovableObjects(const PAGridIndex &gridIndex, std::set<IntelligentObject *> &output) const
{
	// First the own grid
	getMovableObjectsInGridIndex(gridIndex, output);

	// Now the adjacent grids
	std::vector<PAGridIndex> adjacentGridIndex;
	getAdjacentGridsIndex(gridIndex, adjacentGridIndex);

	for(unsigned int numGrids=0; numGrids<adjacentGridIndex.size(); ++numGrids) {
		int i = adjacentGridIndex[numGrids][0];
		int j = adjacentGridIndex[numGrids][1];

		// If grid has not Pole grids adjancent
		if( i!=0 && i!=gridsNumberY-1 ) {
			getMovableObjectsInGridIndex(adjacentGridIndex[numGrids], output);
		}
		else {
			// If grid has Pole grids adjancent, the pole in this case is not a unique grid
			std::set<IntelligentObject *>::const_iterator itMovable = space[i][j].movableObjectSet.begin();
			while( itMovable != space[i][j].movableObjectSet.end() ) {
				output.insert(*itMovable);

				++itMovable;
			}
		}
	}
}

void PolarAnglesPartitioner::getAdjacentMovalbeObjectsTo(IntelligentObject *object, std::set<IntelligentObject *> &output) const
{
	PAGridIndex gridIndex;
	whereIs(object, gridIndex);

	if( gridIndex.size() != 2 )
		return;

	getAdjacentMovableObjects(gridIndex, output);
}

void PolarAnglesPartitioner::getStaticObjectsInGridIndex(const PAGridIndex &gridIndex, std::set<Object *> &output) const
{
	// If coordinates is not inside, return
	if( gridIndex.size() != 2 )
		return;

	int i = gridIndex[0];
	int j = gridIndex[1];

	if( i<0 || j<0 || i>gridsNumberY-1 || j>gridsNumberX-1 )
		return;	

	// If grid is not any pole
	if( i!=0 && i!=gridsNumberY-1 ) {
		std::set<Object *>::const_iterator itStatic = space[i][j].staticObjectSet.begin();
		while( itStatic != space[i][j].staticObjectSet.end() ) {
			output.insert(*itStatic);

			++itStatic;
		}
	}
	else {
		// If it is in a pole, one pole is considered a unic grid
		for(int jPole=0; jPole<gridsNumberX; ++jPole) {
			std::set<Object *>::const_iterator itStatic = space[i][jPole].staticObjectSet.begin();
			while( itStatic != space[i][jPole].staticObjectSet.end() ) {
				output.insert( *itStatic );

				++itStatic;
			}
		}
	}
}

void PolarAnglesPartitioner::getStaticObjectsInSameGrid(IntelligentObject *object, std::set<Object *> &output) const
{
	PAGridIndex gridIndex;
	whereIs(object, gridIndex);

	if( gridIndex.size() != 2 )
		return;

	getStaticObjectsInGridIndex(gridIndex, output);
}

void PolarAnglesPartitioner::getAdjacentStaticObjects(const PAGridIndex &gridIndex, std::set<Object *> &output) const
{
	// First the own grid
	getStaticObjectsInGridIndex(gridIndex, output);

	// Now the adjacent grids
	std::vector<PAGridIndex> adjacentGridIndex;
	getAdjacentGridsIndex(gridIndex, adjacentGridIndex);

	for(unsigned int numGrids=0; numGrids<adjacentGridIndex.size(); ++numGrids) {
		int i = adjacentGridIndex[numGrids][0];
		int j = adjacentGridIndex[numGrids][1];

		// If grid has not Pole grids adjancent
		if( i!=0 && i!=gridsNumberY-1 ) {
			getStaticObjectsInGridIndex(adjacentGridIndex[numGrids], output);
		}
		else {
			// If grid has Pole grids adjancent, the pole in this case is not a unique grid
			std::set<Object *>::const_iterator itStatic = space[i][j].staticObjectSet.begin();
			while( itStatic != space[i][j].staticObjectSet.end() ) {
				output.insert(*itStatic);

				++itStatic;
			}
		}
	}
}

void PolarAnglesPartitioner::getAdjacentStaticObjectsTo(IntelligentObject *object, std::set<Object *> &output) const
{
	PAGridIndex gridIndex;
	whereIs(object, gridIndex);

	if( gridIndex.size() != 2 )
		return;

	getAdjacentStaticObjects(gridIndex, output);
}

void PolarAnglesPartitioner::getAdjacentGridsIndex(const PAGridIndex &gridIndex, std::vector<PAGridIndex> &output) const
{
	// If coordinates is not inside, return
	if( gridIndex.size() != 2 )
		return;

	int i = gridIndex[0];
	int j = gridIndex[1];

	if( i<0 || j<0 || i>gridsNumberY-1 || j>gridsNumberX-1 )
		return;

	// Up and down are not linked in a sphere
	int rowDown = i-1;
	int rowUp = i+1;

	// Left and right are linked in a sphere
	int columnLeft = j-1;
	if(columnLeft==-1)
		columnLeft = gridsNumberX-1;

	int columnRight = (j+1) % gridsNumberX;

	assert( rowDown>=-1 && rowDown<gridsNumberY &&
		rowUp>=0 && rowUp<gridsNumberY+1 &&
		columnLeft>=0 && columnLeft<gridsNumberX &&
		columnRight>=0 && columnRight<gridsNumberX );

	PAGridIndex grid;

	// If it is in south pole do not get down grids
	if( i!=0 ) {
		grid.push_back(rowDown), grid.push_back(columnLeft);
		output.push_back(grid), grid.clear();

		grid.push_back(rowDown), grid.push_back(j);
		output.push_back(grid), grid.clear();

		grid.push_back(rowDown), grid.push_back(columnRight);
		output.push_back(grid), grid.clear();
	}

	// If it is not in any pole, get right and left side grids
	if( i!=0 && i!=gridsNumberY-1 ) {
		grid.push_back(i), grid.push_back(columnLeft);
		output.push_back(grid), grid.clear();

		grid.push_back(i), grid.push_back(columnRight);
		output.push_back(grid), grid.clear();
	}

	// If it is in north pole do not get up grids
	if( i!=gridsNumberY-1 ) {
		grid.push_back(rowUp), grid.push_back(columnLeft); 
		output.push_back(grid), grid.clear();

		grid.push_back(rowUp), grid.push_back(j);
		output.push_back(grid), grid.clear();

		grid.push_back(rowUp), grid.push_back(columnRight);
		output.push_back(grid);
	}
}

void PolarAnglesPartitioner::getVisibleGridsIndex(const PAGridIndex &gridIndex, std::vector<PAGridIndex> &output) const {
	// If coordinates is not inside, return
	if( gridIndex.size() != 2 )
		return;

	int i = gridIndex[0];
	int j = gridIndex[1];

	if( i<0 || j<0 || i>gridsNumberY-1 || j>gridsNumberX-1 )
		return;

	// If grid index in a pole
	if( i == 0 || i == gridsNumberY-1 ) {
		output.push_back(gridIndex);

		if(i==0) {
			for(int y=1; y<=2; ++y) {
				for(int x=0; x<gridsNumberX; ++x) {
					PAGridIndex grid;
					grid.push_back(y);
					grid.push_back(x);

					output.push_back(grid);
				}
			}
		}
		else {
			for(int y=i-2; y<=i-1; ++y) {
				for(int x=0; x<gridsNumberX; ++x) {
					PAGridIndex grid;
					grid.push_back(y);
					grid.push_back(x);

					output.push_back(grid);
				}
			}
		}

		return;
	}
	// If grid index next to pole
	else if( i == 1 || i == gridsNumberY-2 ) {
		int iPole, i1, i2;
		int j1Deviation = 5;
		int j2Deviation = 3;
		if( i == 1 ) {
			iPole = i-1;
			i1 = i+1;
			i2 = i+2;
		}
		else {
			iPole = i+1;
			i1 = i-1;
			i2 = i-2;
		}

		// Insert pole
		PAGridIndex gridPole;
		gridPole.push_back(iPole);
		gridPole.push_back(0);
		output.push_back(gridPole);

		// Insert all index next to pole
		for(int x=0; x<gridsNumberX; ++x) {
			PAGridIndex grid;
			grid.push_back(i);
			grid.push_back(x);

			output.push_back(grid);
		}

		for(int xIt=j-j1Deviation ; xIt<=j+j1Deviation; ++xIt) {
			int x=xIt;
			if(x<0)
				x+=gridsNumberX;
			else
				x=x%gridsNumberX;

			PAGridIndex grid;
			grid.push_back(i1);
			grid.push_back(x);

			output.push_back(grid);
		}
		for(int xIt=j-j2Deviation ; xIt<=j+j2Deviation; ++xIt) {
			int x=xIt;
			if(x<0)
				x+=gridsNumberX;
			else
				x=x%gridsNumberX;

			PAGridIndex grid;
			grid.push_back(i2);
			grid.push_back(x);

			output.push_back(grid);
		}

		return;
	}
	// If grid index two times next to pole
	else if( i == 2 || i == gridsNumberY-3 ) {
		int iPole, i1, i2, i3;
		int jDeviation = 4;
		int j1Deviation = 5;
		int j2Deviation = 3;
		int j3Deviation = 2;
		if( i == 2 ) {
			iPole = i-2;
			i1 = i-1;
			i2 = i+1;
			i3 = i+2;
		}
		else {
			iPole = i+2;
			i1 = i+1;
			i2 = i-1;
			i3 = i-2;
		}

		// Insert pole
		PAGridIndex gridPole;
		gridPole.push_back(iPole);
		gridPole.push_back(0);
		output.push_back(gridPole);

		for(int xIt=j-jDeviation ; xIt<=j+jDeviation; ++xIt) {
			int x=xIt;
			if(x<0)
				x+=gridsNumberX;
			else
				x=x%gridsNumberX;

			PAGridIndex grid;
			grid.push_back(i);
			grid.push_back(x);

			output.push_back(grid);
		}
		for(int xIt=j-j1Deviation ; xIt<=j+j1Deviation; ++xIt) {
			int x=xIt;
			if(x<0)
				x+=gridsNumberX;
			else
				x=x%gridsNumberX;

			PAGridIndex grid;
			grid.push_back(i1);
			grid.push_back(x);

			output.push_back(grid);
		}
		for(int xIt=j-j2Deviation ; xIt<=j+j2Deviation; ++xIt) {
			int x=xIt;
			if(x<0)
				x+=gridsNumberX;
			else
				x=x%gridsNumberX;

			PAGridIndex grid;
			grid.push_back(i2);
			grid.push_back(x);

			output.push_back(grid);
		}
		for(int xIt=j-j3Deviation ; xIt<=j+j3Deviation; ++xIt) {
			int x=xIt;
			if(x<0)
				x+=gridsNumberX;
			else
				x=x%gridsNumberX;

			PAGridIndex grid;
			grid.push_back(i3);
			grid.push_back(x);

			output.push_back(grid);
		}

		return;
	}
	// The rest of the space
	else {
		int iDeviation = 2;
		int jDeviation = 2;

		for(int yIt=i-iDeviation ; yIt<=i+iDeviation; ++yIt) {
			for(int xIt=j-jDeviation ; xIt<=j+jDeviation; ++xIt) {
				int y=yIt;
				int x=xIt;
				if(y<0)
					y+=gridsNumberY;
				else
					y=y%gridsNumberY;
				if(x<0)
					x+=gridsNumberX;
				else
					x=x%gridsNumberX;

				PAGridIndex grid;
				grid.push_back(y);
				grid.push_back(x);

				output.push_back(grid);
			}
		}
	}
}

void PolarAnglesPartitioner::moveMovableObject(IntelligentObject *movableObject, const PAGridIndex &gridsIndex) {
	// Get in which grid is it right now
	PAGridIndex gridsIndexWoudBe;
	whichGridsIndexWouldBe(movableObject, gridsIndexWoudBe);

	if(gridsIndexWoudBe.empty())
		return;

	// If they are different
	int iOld = gridsIndex[0];
	int jOld = gridsIndex[1];
	int i = gridsIndexWoudBe[0];
	int j = gridsIndexWoudBe[1];
	if( iOld!=i || jOld!=j ) {
		// Remove it from a grid and push it in the new
		space[iOld][jOld].movableObjectSet.erase(movableObject);
		space[i][j].movableObjectSet.insert(movableObject);

		// Change where is the object in the list
		movableObjectsInSpace[movableObject][0] = i;
		movableObjectsInSpace[movableObject][1] = j;

		// Update movable object
		movableObject->setSpatialGrid( gridsIndexWoudBe );
	}
}


bool PolarAnglesPartitioner::isRegistered(Object *staticObject) const {

	StaticObjectsInSpaceMap ::const_iterator it = staticObjectsInSpace.find(staticObject);

	if( it != staticObjectsInSpace.end() )
		return true;
	else
		return false;
}

bool PolarAnglesPartitioner::isRegistered(IntelligentObject *movableObject) const {

	MovableObjectsInSpaceMap ::const_iterator it = movableObjectsInSpace.find(movableObject);

	if( it != movableObjectsInSpace.end() )
		return true;
	else
		return false;
}
