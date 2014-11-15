#include "ControllerManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

ControllerManager::ControllerManager()
{
	
}

UIControllerRef ControllerManager::create( DataSourceRef dataSource, app::WindowRef aWindow )
{
	JsonTree empty( "{}" ); // tmp
	JsonTree json( dataSource );
	
	// create a root UIController
	UIControllerRef rootController = shared_ptr<UIController>(new UIController(aWindow, empty));
	
	// loop through the JSON to create any child UIControllers
	if ( json.hasChild( "UIControllers" ) ) {
		for ( auto controllerDefinition : json[ "UIControllers" ].getChildren() ) {
			
			// grab the UIController's params if they exist
			JsonTree controllerParams = controllerDefinition.hasChild( "params" ) ? controllerDefinition[ "params" ] : empty;
			
			// create the child controller, passing in the params
			UIControllerRef childController = shared_ptr<UIController>( new UIController( aWindow, controllerParams ) );

			// add the child controller to the root controller
			rootController->addChild( childController );
			
			// loop through to create any child UIElements
			if ( controllerDefinition.hasChild( "UIElements" ) ) {
				for ( auto elementDefinition : controllerDefinition[ "UIElements" ].getChildren() ) {
					
					// grab the UIElement's params if they exist
					JsonTree elementParams = elementDefinition.hasChild( "params" ) ? elementDefinition[ "params" ] : empty;

					// get the type and id of the element (required)
					string type = elementDefinition[ "type" ].getValue();
					string id = elementDefinition[ "id" ].getValue();
					
					// get any event handlers (optional)
					
					// create and add the element to the controller
					if ( type == "Button" ) {
						UIElementRef buttonRef = Button::create( childController, id, elementParams );
						childController->addElement( buttonRef );
						
					} else if ( type == "Slider" ) {
						
					}
					// etc
				}
			}
		}
	}
	
	return rootController;
}
