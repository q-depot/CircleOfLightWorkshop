/*
 *  SpawnModule.h
 *
 *  Created by Andrea Cuius
 *  www.nocte.co.uk
 *
 */

#pragma once
#include "Fixture.h"

class SpawnModule;
typedef std::shared_ptr<SpawnModule>	SpawnModuleRef;


class SpawnModule {
	
public:
    
    static SpawnModuleRef create()
    {
        return SpawnModuleRef( new SpawnModule() );
    }
    
    ~SpawnModule() {}
    
    void update( std::vector<FixtureRef> fixtures, float speed, float radius );
	
	void render();
    
    bool containsPoint( ci::Vec2f pos2f );
    
    void createPath( int n, int deg, float radius );
    
protected:
    
    SpawnModule() {}
    
private:
    
    struct SplinePath {
        ci::BSpline3f   spline;
        float           posNorm;
        float           radius;
        
        ci::Vec3f       getCurrentPos() { return spline.getPosition( posNorm ); }
    };
    
    std::vector<SplinePath>     mPaths;
    
};