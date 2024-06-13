/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef OBJECTREDIRECTOR_H
#define OBJECTREDIRECTOR_H

#include "Reflection/Object.h"
#include "System/Delegate.h"

/**
 * @ingroup Core
 * @brief Delegate for called when CObjectRedirector was followed to find the destination object
 * 
 * @param InPackageName         Name of the package that pointed to the redirect
 * @param InObjectRedirector    The CObjectRedirector that was followed
 */
DECLARE_MULTICAST_DELEGATE( COnObjectRedirectorFollowed, const std::wstring& /*InPackageName*/, class CObjectRedirector* /*InObjectRedirector*/ );

/**
 * @ingroup Core
 * @brief This class will redirect an object load to another object
 * 
 * This class will redirect an object load to another object, so if an object is renamed
 * to a different package or group, external references to the object can be found
 */
class CObjectRedirector : public CObject
{
public:
	DECLARE_CLASS_INTRINSIC( CObjectRedirector, CObject, 0, 0, TEXT( "Core" ) )

    /**
     * @brief Constructor
     */
    CObjectRedirector();

    /**
     * @brief Presave function
     * @warning Objects created from within PreSave won't have PreSave called on them
     *
     * Gets called once before an object gets serialized for saving. This function is necessary
     * for save time computation as Serialize gets called three times per object from within CObjectPackage::SavePackage
     */
    virtual void PreSave() override;

    /**
     * @brief Serialize object
     * @param InArchive     Archive for serialize
     */
    virtual void Serialize( class CArchive& InArchive ) override;

    /**
     * @brief Set the destination object
     * @param InDestinationObject       A new destination object for this redirector
     */
    FORCEINLINE void SetDestinationObject( CObject* InDestinationObject )
    {
        MarkPackageDirty();
        destinationObject = InDestinationObject;
    }

    /**
     * @brief Get the destination object
     * @return Return the destination object. If this redirector haven't the one returns NULL
     */
    FORCEINLINE CObject* GetDestinationObject() const
    {
        return destinationObject;
    }

    static COnObjectRedirectorFollowed      onObjectRedirectorFollowed;     /**< Delegate for called when CObjectRedirector was followed to find the destination object */
private:
	CObject*	destinationObject;		/**< Destination object */
};

#endif // !OBJECTREDIRECTOR_H