/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ACTORPROPERTIESWINDOW_H
#define ACTORPROPERTIESWINDOW_H

#include <list>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include "ImGUI/ImGUIEngine.h"
#include "WorldEd.h"

/**
 * @ingroup WorldEd
 * @brief Window of actor properties
 */
class CActorPropertiesWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InName	Window name
	 */
	CActorPropertiesWindow( const std::wstring& InName );

	/**
	 * @brief Destructor
	 */
	~CActorPropertiesWindow();

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	/**
	 * @brief Object properties
	 */
	class CObjectProperties
	{
	public:
		/**
		 * @brief Method tick interface of a objects property
		 * 
		 * @param InItemWidthSpacing			Item spacing by width
		 * @param InApplySpacingToCategories	Is need apply item spacing to categories
		 */
		void Tick( float InItemWidthSpacing, bool InApplySpacingToCategories = false );

		/**
		 * @brief Clear object properties
		 */
		FORCEINLINE void Clear()
		{
			properties.clear();
			objects.clear();
		}

		/**
		 * @brief Is empty object properties
		 * @return Return TRUE if object properties is empty
		 */
		FORCEINLINE bool IsEmpty() const
		{
			return properties.empty();
		}

		/**
		 * @brief Get array of properties
		 * @return Return array of properties
		 */
		FORCEINLINE std::unordered_map<CName, std::vector<CProperty*>, CName::SHashFunction>& GetProperties()
		{
			return properties;
		}

		/**
		 * @brief Get array of objects
		 * @return Return array of objects
		 */
		FORCEINLINE std::vector<CObject*>& GetObjects()
		{
			return objects;
		}

	private:
		/**
		 * @brief Tick property (draw and handle it's events)
		 * 
		 * @param InItemWidthSpacing	Item spacing by width
		 * @param InProperty			Property
		 */
		void TickProperty( float InItemWidthSpacing, CProperty* InProperty );

		/**
		 * @brief Set property value
		 * 
		 * @param InProperty		Property
		 * @param InPropertyValue	Contains the value that should be copied into all objects
		 */
		void SetPropertyValue( CProperty* InProperty, const UPropertyValue& InPropertyValue );

		std::unordered_map<CName, std::vector<CProperty*>, CName::SHashFunction>		properties;	/**< Array of properties */
		std::vector<CObject*>															objects;	/**< Array of objects */
	};

	/**
	 * @brief Event called when actors selected or unselected
	 * 
	 * @param InActors	Selected or unselected actors
	 */
	void OnActorsUnSelected( const std::vector<ActorRef_t>& InActors );

	/**
	 * @brief Event called when map changed (created or loaded)
	 */
	void OnMapChanged();

	/**
	 * @brief Get all properties from object
	 *
	 * @param InObject					Object
	 * @param OutObjectProperties		Output object properties
	 * @param OutComponentsProperties	Output array of all components' properties in the object
	 */
	void GetAllPropertiesFromObject( CObject* InObject, CObjectProperties& OutObjectProperties, std::unordered_map<CName, CObjectProperties, CName::SHashFunction>& OutComponentsProperties ) const;

	/**
	 * @brief Remove properties that are not in the array InArrayB
	 * 
	 * @param InArrayA			Check array with properties
	 * @param InOutArrayB		An array with properties. After the method is executed, it will contain only those properties that are also in InActorA
	 * @return Return TRUE if there are matches in the array InArrayA, otherwise FALSE
	 */
	bool RemoveMissingProperties( const std::vector<CProperty*>& InArrayA, std::vector<CProperty*>& InOutArrayB ) const;

	/**
	 * @brief Remove properties that are not in the array InArrayB
	 *
	 * @param InArrayA			Check array with properties
	 * @param InOutArrayB		An array with properties. After the method is executed, it will contain only those properties that are also in InActorA
	 * @return Return TRUE if there are matches in the array InArrayA, otherwise FALSE
	 */
	bool RemoveMissingProperties( const std::unordered_map<CName, std::vector<CProperty*>, CName::SHashFunction>& InArrayA, std::unordered_map<CName, std::vector<CProperty*>, CName::SHashFunction>& InOutArrayB ) const;

	CObjectProperties														actorProperties;			/**< Array of actor properties */
	std::unordered_map<CName, CObjectProperties, CName::SHashFunction>		componentsProperties;		/**< Array of components properties */
	SEditorDelegates::COnActorsSelected::DelegateType_t*					actorsSelectedDelegate;		/**< Actors selected delegate */
	SEditorDelegates::COnActorsUnselected::DelegateType_t*					actorsUnselectedDelegate;	/**< Actors unselected delegate */
	SEditorDelegates::COnEditorCreatedNewMap::DelegateType_t*				createdNewMapDelegate;		/**< Created new map delegate */
	SEditorDelegates::COnEditorLoadedMap::DelegateType_t*					loadedMapDelegate;			/**< Loaded map delegate */
};

#endif // !ACTORPROPERTIESWINDOW_H