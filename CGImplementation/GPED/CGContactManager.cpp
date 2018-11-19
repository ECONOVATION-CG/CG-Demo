#include "CGContactManager.h"
#pragma once

GPED::ContactManager::ContactManager()
{
	m_root = NODE_NULL;

	m_nodeCapacity = 16;
	m_nodeCount = 0;
	m_nodes = new Contact[m_nodeCapacity];

	// Build a linked list for the free list
	m_nodes[0].prev = NODE_NULL;
	m_nodes[0].next = 1;
	for (int i = 1; i < m_nodeCapacity - 1; ++i)
	{
		m_nodes[i].prev = i - 1;
		m_nodes[i].next = i + 1;
	}
	m_nodes[m_nodeCapacity - 1].prev = m_nodeCapacity - 2;
	m_nodes[m_nodeCapacity - 1].next = NODE_NULL;
	
	m_freeList = 0;
}

GPED::ContactManager::ContactManager(int nodeCapacity)
{
	m_root = NODE_NULL;

	m_nodeCapacity = nodeCapacity;
	m_nodeCount = 0;
	m_nodes = new Contact[m_nodeCapacity];

	// Build a linked list for the free list
	m_nodes[0].prev = NODE_NULL;
	m_nodes[0].next = 1;
	for (int i = 1; i < m_nodeCapacity - 1; ++i)
	{
		m_nodes[i].prev = i - 1;
		m_nodes[i].next = i + 1;
	}
	m_nodes[m_nodeCapacity - 1].prev = m_nodeCapacity - 2;
	m_nodes[m_nodeCapacity - 1].next = NODE_NULL;

	m_freeList = 0;
}

GPED::ContactManager::~ContactManager()
{
	delete[] m_nodes;
	m_nodes = nullptr;
}

GPED::Contact* GPED::ContactManager::GetEmptyContactNode()
{
	int nodeId = AllocateNode();
	InsertNode(nodeId);
	return &(m_nodes[nodeId]);
}

void GPED::ContactManager::sortByPenetration()
{
	int lastId = m_root;
	while (lastId && m_nodes[lastId].next)
		lastId = m_nodes[lastId].next;

	// Decreasing Order
	// refer to https://www.geeksforgeeks.org/quicksort-for-linked-list/
	// abou the implemenetation
	QuickSortPenetration(m_root, lastId);
}

void GPED::ContactManager::sortByVelocity()
{
	int lastId = m_root;
	while (lastId && m_nodes[lastId].next)
		lastId = m_nodes[lastId].next;

	// Decreasing Order
	QuickSortVelocity(m_root, lastId);
}

void GPED::ContactManager::AllcalculateInternals(GPED::real duration)
{
	int move = m_root;
	while (move != NODE_NULL)
	{
		m_nodes[move].calculateInternals(duration);
		move = m_nodes[move].next;
	}
}

GPED::Contact * GPED::ContactManager::GetFirstContact()
{
	return &m_nodes[m_root];
}

int GPED::ContactManager::GetNodeCount()
{
	return m_nodeCount;
}

int GPED::ContactManager::AllocateNode()
{
	if (m_freeList == NODE_NULL)
	{
		assert(m_nodeCount == m_nodeCapacity);

		// The free list is empty. Rebuild a bigger pool
		Contact* oldNodes = m_nodes;
		m_nodeCapacity *= 2;
		m_nodes = new Contact[m_nodeCapacity];
		memcpy(m_nodes, oldNodes, m_nodeCount * sizeof(Contact));
		delete[] oldNodes;
		oldNodes = nullptr;

		// Build a linked list for the free list.
		m_nodes[m_nodeCount].prev = NODE_NULL;
		m_nodes[m_nodeCount].next = m_nodeCount + 1;
		for (int i = m_nodeCount + 1; i < m_nodeCapacity - 1; ++i)
		{
			m_nodes[i].prev = NODE_NULL;
			m_nodes[i].next = i + 1;
		}
		m_nodes[m_nodeCapacity - 1].prev = m_nodeCapacity - 2;
		m_nodes[m_nodeCapacity - 1].next = NODE_NULL;
		m_freeList = m_nodeCount;
	}

	// Peel a node off the free list
	int nodeId = m_freeList;
	m_freeList = m_nodes[nodeId].next;
	m_nodes[nodeId].body[0] = nullptr;
	m_nodes[nodeId].body[1] = nullptr;
	++m_nodeCount;
	return nodeId;
}

void GPED::ContactManager::FreeNode(int nodeId)
{
	assert(0 <= nodeId && nodeId < m_nodeCapacity);
	assert(0 < m_nodeCount);
	m_nodes[nodeId].next = m_freeList;
	m_freeList = nodeId;
	--m_nodeCount;
}

void GPED::ContactManager::InsertNode(int nodeId)
{
	if (m_root == NODE_NULL)
	{
		m_root = nodeId;
		m_nodes[m_root].prev = NODE_NULL;
		m_nodes[m_root].next = NODE_NULL;
		return;
	}

	// Insert the node into the first place of doubly linked list
	m_nodes[nodeId].prev = NODE_NULL;
	m_nodes[nodeId].next = m_root;

	m_nodes[m_root].prev = nodeId;

	m_root = nodeId;
}

void GPED::ContactManager::DeleteNode(int nodeId)
{
	int prevId = m_nodes[nodeId].prev;
	int nextId = m_nodes[nodeId].next;

	if (prevId != NODE_NULL)
		m_nodes[prevId].next = nextId;
	else
		m_root = nextId;

	if(nextId != NODE_NULL)
		m_nodes[nextId].prev = prevId;

	FreeNode(nodeId);
}

void GPED::ContactManager::QuickSortPenetration(int left, int right)
{
	if (right != NODE_NULL && left != NODE_NULL && left != m_nodes[right].next)
	{
		int node = partitionPenetrtaion(left, right);
		QuickSortPenetration(left, m_nodes[node].prev);
		QuickSortPenetration(m_nodes[node].next, right);
	}
}

int GPED::ContactManager::partitionPenetrtaion(int left, int right)
{
	GPED::real pivot = m_nodes[right].penetration;

	int i = m_nodes[left].prev;

	for (int j = left; j != right; j = m_nodes[j].next)
	{
		if (m_nodes[j].penetration > pivot)
		{
			i = (i == NODE_NULL) ? left : m_nodes[i].next;
			ContactManager::swap(i, j);
		}
	}

	i = (i == NODE_NULL) ? left : m_nodes[i].next;
	ContactManager::swap(i, right);
	return i;
}

void GPED::ContactManager::QuickSortVelocity(int left, int right)
{
	if (right != NODE_NULL && left != NODE_NULL && left != m_nodes[right].next)
	{
		int node = partitionVelocity(left, right);
		QuickSortVelocity(left, m_nodes[node].prev);
		QuickSortVelocity(m_nodes[node].next, right);
	}
}

int GPED::ContactManager::partitionVelocity(int left, int right)
{
	GPED::real pivot = m_nodes[right].desiredDeltaVelocity;

	int i = m_nodes[left].prev;

	for (int j = left; j != right; j = m_nodes[j].next)
	{
		if (m_nodes[j].desiredDeltaVelocity > pivot)
		{
			i = (i == NODE_NULL) ? left : m_nodes[i].next;
			ContactManager::swap(i, j);
		}
	}

	i = (i == NODE_NULL) ? left : m_nodes[i].next;
	ContactManager::swap(i, right);
	return i;
}

void GPED::ContactManager::swap(int nodeIdA, int nodeIdB)
{
	int Aprev = m_nodes[nodeIdA].prev;
	int Anext = m_nodes[nodeIdA].next;
	int Bprev = m_nodes[nodeIdB].prev;
	int Bnext = m_nodes[nodeIdB].next;

	Contact temp = m_nodes[nodeIdA];
	m_nodes[nodeIdA] = m_nodes[nodeIdB];
	m_nodes[nodeIdA].prev = Aprev;
	m_nodes[nodeIdA].next = Anext;

	m_nodes[nodeIdB] = temp;
	m_nodes[nodeIdB].prev = Bprev;
	m_nodes[nodeIdB].next = Bnext;
}