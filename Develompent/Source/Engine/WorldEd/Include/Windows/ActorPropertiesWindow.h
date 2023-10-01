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
#include "System/ObjectPropertyNode.h"
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
#if 0
	// Forward declarations
	class CObjectProperties;

	/**
	 * @brief Property handler
	 */
	struct PropertyHandler
	{
		/**
		 * @brief Constructor
		 */
		PropertyHandler()
			: property( nullptr )
			, objectProperties( nullptr )
		{}

		/**
		 * @brief Constructor
		 * @param InProperty	Property
		 */
		PropertyHandler( CProperty* InProperty )
			: property( InProperty )
			, objectProperties( nullptr )
		{
			Assert( InProperty );
			if ( InProperty->GetClass()->HasAnyCastFlags( CASTCLASS_CObjectProperty ) )
			{
				objectProperties = new CObjectProperties();
			}
		}

		/**
		 * @brief Destructor
		 */
		~PropertyHandler()
		{
			if ( objectProperties )
			{
				delete objectProperties;
			}
		}

		CProperty*			property;			/**< Property */
		CObjectProperties*	objectProperties;	/**< Object peroperties. It is valid when property is object type */
	};

	/**
	 * @brief Container all of object properties
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
		void Tick( float InItemWidthSpacing = 0.f, bool InApplySpacingToCategories = false );

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
		FORCEINLINE std::unordered_map<CName, std::vector<TSharedPtr<PropertyHandler>>, CName::HashFunction>& GetProperties()
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
		 * @param InItemWidthSpacing			Item spacing by width
		 * @param InApplySpacingToCategories	Is need apply item spacing to categories
		 * @param InPropertyHandler				Property handler
		 */
		void TickProperty( float InItemWidthSpacing, bool InApplySpacingToCategories, const TSharedPtr<PropertyHandler>& InPropertyHandler );

		/**
		 * @brief Set property value
		 * 
		 * @param InProperty		Property
		 * @param InPropertyValue	Contains the value that should be copied into all objects
		 */
		void SetPropertyValue( CProperty* InProperty, const UPropertyValue& InPropertyValue );

		std::unordered_map<CName, std::vector<TSharedPtr<PropertyHandler>>, CName::HashFunction>		properties;	/**< Array of properties */
		std::vector<CObject*>																			objects;	/**< Array of objects */
	};

	class CArrayProperties
	{
	public:
		/**
		 * @brief Method tick interface of a objects property
		 * 
		 * @param InItemWidthSpacing			Item spacing by width
		 * @param InApplySpacingToCategories	Is need apply item spacing to categories
		 */
		//void Tick( float InItemWidthSpacing = 0.f, bool InApplySpacingToCategories = false );
	private:
		//void TickProperty( uint32 index );

		std::vector<TSharedPtr<PropertyHandler>>		properties;
		std::vector<std::vector<byte>*>					addresses;
	};
#endif // 0

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
	 * @brief Destroy property tree
	 */
	void DestroyPropertyTree();

#if 0
	/**
	 * @brief Get all properties from object
	 *
	 * @param InObject					Object
	 * @param OutObjectProperties		Output object properties
	 */
	void GetAllPropertiesFromObject( CObject* InObject, CObjectProperties& OutObjectProperties ) const;

	/**
	 * @brief Remove properties that are not in the array InArrayB
	 * 
	 * @param InArrayA			Check array with properties
	 * @param InOutArrayB		An array with properties. After the method is executed, it will contain only those properties that are also in InActorA and it's objects
	 * @return Return TRUE if there are matches in the array InArrayA, otherwise FALSE
	 */
	bool RemoveMissingProperties( const std::vector<TSharedPtr<PropertyHandler>>& InArrayA, std::vector<TSharedPtr<PropertyHandler>>& InOutArrayB ) const;

	/**
	 * @brief Remove properties that are not in the array InArrayB
	 *
	 * @param InArrayA			Check array with properties
	 * @param InOutArrayB		An array with properties. After the method is executed, it will contain only those properties that are also in InActorA and it's objects
	 * @return Return TRUE if there are matches in the array InArrayA, otherwise FALSE
	 */
	bool RemoveMissingProperties( const std::unordered_map<CName, std::vector<TSharedPtr<PropertyHandler>>, CName::HashFunction>& InArrayA, std::unordered_map<CName, std::vector<TSharedPtr<PropertyHandler>>, CName::HashFunction>& InOutArrayB ) const;

	CObjectProperties														properties;					/**< Array of actor properties */
#endif // 0
	
	CObjectPropertyNode*													rootPropertyNode;			/**< Root property node */
	EditorDelegates::COnActorsSelected::DelegateType_t*						actorsSelectedDelegate;		/**< Actors selected delegate */
	EditorDelegates::COnActorsUnselected::DelegateType_t*					actorsUnselectedDelegate;	/**< Actors unselected delegate */
	EditorDelegates::COnEditorCreatedNewMap::DelegateType_t*				createdNewMapDelegate;		/**< Created new map delegate */
	EditorDelegates::COnEditorLoadedMap::DelegateType_t*					loadedMapDelegate;			/**< Loaded map delegate */
};

#endif // !ACTORPROPERTIESWINDOW_H