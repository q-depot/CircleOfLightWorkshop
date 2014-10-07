#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Light.h"
#include "cinder/Rand.h"
#include "cinder/ObjLoader.h"
#include "cinder/MayaCamUI.h"
#include "cinder/params/Params.h"
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "SpawnModule.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class SpawnObjectApp : public AppNative {
    
public:
    
    void prepareSettings( Settings *settings );
    void setup();
    void update();
    void draw();
    
    void keyDown( KeyEvent event );
    void mouseDown( ci::app::MouseEvent event );
    void mouseDrag( ci::app::MouseEvent event );
    void resize();
    
    
private:
    
    vector<FixtureRef>      mFixtures;
    
    gl::VboMeshRef          mFixtureMesh, mVenueMesh;   // 3d meshes
    
    ci::MayaCamUI           mMayaCam;                   // 3d camera
    
    SpawnModuleRef          mModule;                    // the effects
    
    params::InterfaceGlRef  mParams;
    
    float                   mFadeIn, mFadeOut;
    float                   mSpeed, mRadius;
    int                     mPointsN, mDeg;
    
    gl::Light*              mLight;
};


void SpawnObjectApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1200, 800 );
}


void SpawnObjectApp::setup()
{
    mFixtures		= Fixture::loadFixtures( getAssetPath("fixtures_001.csv") );			// load CSV fixtures file
    
    mFixtureMesh    = Fixture::loadObj( getAssetPath("sphere.obj") );						// load fixture mesh
    mVenueMesh      = Fixture::loadObj( getAssetPath("piano2.obj") );						// load venue mesh
    
    mFadeIn         = 0.5f;
    mFadeOut        = 0.1f;
    mSpeed          = 0.2f;
    mRadius         = 1.5f;
    mPointsN        = 8;
    mDeg            = 3;

    mModule         = SpawnModule::create();                                                // create module
    
    mParams         = params::InterfaceGl::create( "Params", Vec2i( 200, 240 ) );           // Gui
    
    mParams->addParam( "Fade IN",   &mFadeIn    , "min=0.001 max=1.0 step=0.001" );
    mParams->addParam( "Fade OUT",  &mFadeOut   , "min=0.001 max=1.0 step=0.001" );
    mParams->addSeparator();
    
    mParams->addParam( "Speed",     &mSpeed     , "min=0.001 max=10.0 step=0.001" );
    mParams->addParam( "Radius",    &mRadius    , "min=0.1 max=15.0 step=0.1" );
    mParams->addParam( "Points N",  &mPointsN   , "min=2 max=100 step=1" );
    mParams->addParam( "Deg",       &mDeg       , "min=1 max=10 step=1" );
    
    ci::CameraPersp initialCam;                                                             // Initialise camera
    initialCam.setPerspective( 45.0f, ci::app::getWindowAspectRatio(), 0.1, 3000 );
    mMayaCam        = MayaCamUI( initialCam );
    
    // Set up light
    mLight = new gl::Light( gl::Light::DIRECTIONAL, 0 );
    mLight->setDirection( Vec3f( 0.0f, 0.1f, 0.3f ).normalized() );
    mLight->setAmbient( ColorAf::gray( 0.843f ) );
    mLight->setDiffuse( ColorAf( 1.0f, 1.0f, 1.0f, 1.0f ) );
    mLight->enable();
    
    // create a path
    mModule->createPath( mPointsN, mDeg, mRadius );
}


void SpawnObjectApp::update()
{
    mModule->update( mFixtures, mSpeed, mRadius );
    
    for( size_t k=0; k < mFixtures.size(); k++ )        // update fixtures brightness
        mFixtures[k]->update( mFadeIn, mFadeOut );
}


void SpawnObjectApp::draw()
{
    // clear out the window with black
    gl::clear( Color::gray( 0.15f ) );
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    gl::enableAlphaBlending();
    
    gl::enable( GL_TEXTURE_2D );
    
    gl::setMatrices( mMayaCam.getCamera() );                   // set camera matrices
    //    mLight->update( mMayaCam.getCamera() );
    
    Fixture::renderGrid();
    
    gl::enable( GL_LIGHTING );
    
    if ( mVenueMesh )                                           // render venue
        gl::draw( mVenueMesh );
    
    gl::disable( GL_LIGHTING );
    
    if ( mFixtureMesh )                                         // render fixtures
    {
        FixtureRef fix;
        
        for( size_t k=0; k < mFixtures.size(); k++ )
        {
            fix = mFixtures[k];
            
            ci::gl::pushMatrices();                             // store current matrices
            ci::gl::translate( fix->getPos() );                 // translate
            
            ci::gl::color( fix->getValue() * Color::white() );
            
            ci::gl::draw( mFixtureMesh );                       // draw mesh
            ci::gl::popMatrices();                              // restore matrices
        }
        
        ci::gl::color( ci::Color::white() );
    }
    
    if ( mModule )                                              // render effect preview
        mModule->render();
    
    mParams->draw();
}


void SpawnObjectApp::keyDown( KeyEvent event )
{
    char c = event.getChar();
    
    if ( c == 'f' )
        setFullScreen( !isFullScreen() );
    
    else if ( c == ' ' )
        mModule->createPath( mPointsN, mDeg, mRadius );
}


void SpawnObjectApp::mouseDown( ci::app::MouseEvent event )
{
    if( event.isAltDown() )
        mMayaCam.mouseDown( event.getPos() );
}


void SpawnObjectApp::mouseDrag( ci::app::MouseEvent event )
{
    if( event.isAltDown() )
        mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}


void SpawnObjectApp::resize()
{
    ci::CameraPersp cam = mMayaCam.getCamera();
    cam.setPerspective( 45.0f, ci::app::getWindowAspectRatio(), 0.1, 3000 );
    mMayaCam.setCurrentCam( cam );
}


CINDER_APP_NATIVE( SpawnObjectApp, RendererGl )
