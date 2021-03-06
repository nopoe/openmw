/*
  OpenMW - The completely unofficial reimplementation of Morrowind
  Copyright (C) 2008-2010  Nicolay Korslund
  Email: < korslund@gmail.com >
  WWW: http://openmw.sourceforge.net/

  This file (ogre_nif_loader.h) is part of the OpenMW package.

  OpenMW is distributed as free software: you can redistribute it
  and/or modify it under the terms of the GNU General Public License
  version 3, as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  version 3 along with this program. If not, see
  http://www.gnu.org/licenses/ .

 */

#ifndef OPENMW_COMPONENTS_NIFBULLET_BULLETNIFLOADER_HPP
#define OPENMW_COMPONENTS_NIFBULLET_BULLETNIFLOADER_HPP

#include <cassert>
#include <string>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <btBulletDynamicsCommon.h>
#include <openengine/bullet/BulletShapeLoader.h>

// For warning messages
#include <iostream>

namespace Nif
{
    class Node;
    struct Transformation;
    struct NiTriShape;
}

namespace NifBullet
{

// Subclass btBhvTriangleMeshShape to auto-delete the meshInterface
struct TriangleMeshShape : public btBvhTriangleMeshShape
{
    TriangleMeshShape(btStridingMeshInterface* meshInterface, bool useQuantizedAabbCompression)
        : btBvhTriangleMeshShape(meshInterface, useQuantizedAabbCompression)
    {
    }

    virtual ~TriangleMeshShape()
    {
        delete getTriangleInfoMap();
        delete m_meshInterface;
    }
};


/**
*Load bulletShape from NIF files.
*/
class ManualBulletShapeLoader : public OEngine::Physic::BulletShapeLoader
{
public:
    ManualBulletShapeLoader(bool showMarkers=false)
      : mShape(NULL)
      , mStaticMesh(NULL)
      , mCompoundShape(NULL)
      , mBoundingBox(NULL)
      , mShowMarkers(showMarkers)
    {
    }

    virtual ~ManualBulletShapeLoader();

    void warn(const std::string &msg)
    {
        std::cerr << "NIFLoader: Warn:" << msg << "\n";
    }

    void fail(const std::string &msg)
    {
        std::cerr << "NIFLoader: Fail: "<< msg << std::endl;
        abort();
    }

    /**
    *This function should not be called manualy. Use load instead. (this is called by the BulletShapeManager when you use load).
    */
    void loadResource(Ogre::Resource *resource);

    /**
    *This function load a new bulletShape from a NIF file into the BulletShapeManager.
    *When the file is loaded, you can then use BulletShapeManager::getByName() to retrive the bulletShape.
    *Warning: this function will just crash if the resourceGroup doesn't exist!
    */
    void load(const std::string &name,const std::string &group);

private:
    btVector3 getbtVector(Ogre::Vector3 const &v);

    /**
    *Parse a node.
    */
    void handleNode(Nif::Node const *node, int flags, bool isCollisionNode,
                    bool raycasting, bool isAnimated=false);

    /**
    *Helper function
    */
    bool hasAutoGeneratedCollision(const Nif::Node *rootNode);

    /**
    *convert a NiTriShape to a bullet trishape.
    */
    void handleNiTriShape(const Nif::NiTriShape *shape, int flags, const Ogre::Matrix4 &transform, bool raycasting, bool isAnimated);

    std::string mResourceName;

    OEngine::Physic::BulletShape* mShape;//current shape

    btCompoundShape* mCompoundShape;

    btTriangleMesh* mStaticMesh;

    btBoxShape *mBoundingBox;

    std::set<std::string> mControlledNodes;

    bool mShowMarkers;
};


bool getBoundingBox(const std::string& nifFile, Ogre::Vector3& halfExtents, Ogre::Vector3& translation, Ogre::Quaternion& orientation);

bool findBoundingBox(const Nif::Node* node, Ogre::Vector3& halfExtents, Ogre::Vector3& translation, Ogre::Quaternion& orientation);

}

#endif
