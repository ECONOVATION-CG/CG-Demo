#pragma once
#ifndef __CG_CONTACT_MANAGER_H__
#define __CG_CONTACT_MANAGER_H__

#include <GPED/GPED_contacts.h>

namespace GPED
{
	/* 181119 Chanhaeng Lee
	* This class is the similar one as the CollisionData class in GPED.
	* However, I will use this since this class uses memory pool technique
	* and provides the method of sorting the contacts to speed up
	* the contact resolution process.
	* 
	* The overall structure of classes used in this contact hedaer is like..
	* Contact == Node
	* Contact Manager == Doubly linked list managing inserting, removing, allocating, freeing, sorting
	*	                 Contact Manager will provide the function to help speed up the resolution proceess
	*					 based on the GPED book optimization tip.
	* Contact Resolver == Contact Resolver gets the contact to resolve from Contact Manager
	*
	* + the relation with narrow phase detector
	* the narrow phase detector interacts with Contact Manager.
	* For Example, GetEmptyContactNode();
	*/

constexpr int NODE_NULL = -1;
	class ContactManager
	{
	public:
		ContactManager();
		ContactManager(int nodeCapacity);
		~ContactManager();

		Contact* GetEmptyContactNode();

		void sortByPenetration();
		void sortByVelocity();

		real friction;
		real restitution;
		real tolerance;

		void AllcalculateInternals(GPED::real duration);
		Contact* GetFirstContact();

		int GetNodeCount();
	private:
		int AllocateNode();
		void FreeNode(int nodeId);

		void InsertNode(int nodeId);
		void DeleteNode(int nodeId);
		
		void QuickSortPenetration(int left, int right);
		int partitionPenetrtaion(int left, int right);
		void QuickSortVelocity(int left, int right);
		int partitionVelocity(int left, int right);
		void swap(int nodeIdA, int nodeIdB);

		int m_root;

		Contact* m_nodes;
		int m_nodeCount;
		int m_nodeCapacity;

		int m_freeList;
	};
}


#endif