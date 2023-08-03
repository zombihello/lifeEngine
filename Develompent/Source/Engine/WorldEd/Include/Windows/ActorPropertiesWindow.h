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
	 * @brief Object field
	 */
	class CObjectField
	{
	public:
		/**
		 * @brief Constructor
		 * 
		 * @param InField	Field
		 * @param InObjects	Array of objects to edit
		 */
		FORCEINLINE CObjectField( const SFieldDescription* InField, const std::list<CObject*> InObjects )
			: field( InField )
			, objects( InObjects )
		{}

		/**
		 * @brief Method tick interface of a objects property
		 */
		void Tick();

	private:
		/**
		 * @brief Apply value to all objects
		 * 
		 * @param InValue	Value
		 */
		template<typename TType>
		FORCEINLINE void SetObjectValue( const TType& InValue )
		{
			for ( auto it = objects.begin(), itEnd = objects.end(); it != itEnd; ++it )
			{
				*DataMap::DataFieldAccess<TType>( *it, field ) = InValue;
			}
		}

		const SFieldDescription*		field;		/**< Field */
		std::list<CObject*>				objects;	/**< Array of objects to edit */
	};

	/**
	 * @brief Typedef map of object fields separated by category
	 */
	typedef std::unordered_map<std::string, std::list<CObjectField>>		MapObjectFields_t;

	/**
	 * @brief Event called when actors selected or unselected
	 * 
	 * @param InActors	Selected or unselected actors
	 */
	void OnActorsUnSelected( const std::vector<ActorRef_t>& InActors );

	/**
	 * @brief Access to all object fields in data map, include base classes
	 *
	 * @param InDataMap		Data map in the class
	 * @param InObject		Object
	 * @param OutFields		Output array with all fields
	 */
	FORCEINLINE static void GetAllObjectFields( const SDataMap* InDataMap, CObject* InObject, std::unordered_map<const SFieldDescription*, std::list<CObject*>>& OutFields )
	{
		for ( const SDataMap* pDataMap = InDataMap; pDataMap; pDataMap = pDataMap->baseMap )
		{
			for ( uint32 idxField = 0; idxField < pDataMap->numFields; ++idxField )
			{
				// We add to OutFields only valid fields
				const SFieldDescription&	field = pDataMap->fields[idxField];
				if ( field.name )
				{
					OutFields[&field].push_back( InObject );
				}
			}
		}
	}

	MapObjectFields_t											objectFields;				/**< Map of actor fields to edit */
	std::unordered_map<std::string, MapObjectFields_t>			componentsFields;			/**< Map of components fields to edit */
	SEditorDelegates::COnActorsSelected::DelegateType_t*		actorsSelectedDelegate;		/**< Actors selected delegate */
	SEditorDelegates::COnActorsUnselected::DelegateType_t*		actorsUnselectedDelegate;	/**< Actors unselected delegate */
};

#endif // !ACTORPROPERTIESWINDOW_H